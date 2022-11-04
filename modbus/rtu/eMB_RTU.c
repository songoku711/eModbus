/* 
 * File:   eMB_RTU.c
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "eMB.h"
#include "eMB_Utils.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#ifdef eMB_MASTER_RTU_ENABLED
typedef enum _eMB_RTU_RecvStateType
{
  eMB_RTU_RECV_STATE_INIT,                        /*!< Receiver is in initial state. */
  eMB_RTU_RECV_STATE_IDLE,                        /*!< Receiver is in idle state. */
  eMB_RTU_RECV_STATE_RCV,                         /*!< Frame is being received. */
  eMB_RTU_RECV_STATE_ERROR,                       /*!< If the frame is invalid. */
} eMB_RTU_RecvStateType;

typedef enum _eMB_RTU_SendStateType
{
  eMB_RTU_SEND_STATE_IDLE,                        /*!< Transmitter is in idle state. */
  eMB_RTU_SEND_STATE_XMIT,                        /*!< Transmitter is in transfer state. */
  eMB_RTU_SEND_STATE_DONE,                        /*!< Transmitter is in transfer finish and wait receive state. */
} eMB_RTU_SendStateType;
#endif



/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/

#ifdef eMB_MASTER_RTU_ENABLED
static volatile eMB_RTU_SendStateType eMB_RTU_SendState;
static volatile eMB_RTU_RecvStateType eMB_RTU_RecvState;

static volatile uint8_t  eMB_RTU_SlaveAddr;

static volatile uint8_t  eMB_RTU_SendBuf[eMB_PDU_SIZE_MAX];
static volatile uint8_t *eMB_RTU_SendBufPos;
static volatile uint16_t eMB_RTU_SendLength;
static volatile uint16_t eMB_RTU_SendCount;

static volatile uint8_t  eMB_RTU_RecvBuf[eMB_SDU_SIZE_MAX];
static volatile uint16_t eMB_RTU_RecvLength;

static volatile bool eMB_RTU_FrameIsBroadcast = false;
#endif



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#ifdef eMB_MASTER_RTU_ENABLED
eMB_ErrorCodeType eMB_Master_RTUInit(void)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  eMB_PortEnterCriticalSection();

  /* Initialize Modbus serial. */
  if (eMB_gConfigPtr->pPortSerialInit() != true)
  {
    errStatus = eMB_EPORTERR;
  }
  else
  {
    /* Initialize Modbus timer. */
    if (eMB_gConfigPtr->pPortTimersInit() != true)
    {
      errStatus = eMB_EPORTERR;
    }
  }

  eMB_PortExitCriticalSection();

  return errStatus;
}

void eMB_Master_RTUStart(void)
{
  eMB_PortEnterCriticalSection();
  
  /* Initially the receiver is in the state eMB_RTU_RECV_STATE_INIT. we start
   * the timer and if no character is received within t3.5 we change
   * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
   * modbus protocol stack until the bus is free. */
  eMB_RTU_SendState = eMB_RTU_SEND_STATE_IDLE;
  eMB_RTU_RecvState = eMB_RTU_RECV_STATE_INIT;

  eMB_gConfigPtr->pPortSerialSetMode(eMB_PORT_SERIAL_RX);
  eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_T35);

  eMB_PortExitCriticalSection();
}

void eMB_Master_RTUStop(void)
{
  eMB_PortEnterCriticalSection();

  eMB_gConfigPtr->pPortSerialSetMode(eMB_PORT_SERIAL_RX);
  eMB_gConfigPtr->pPortTimersDisable();

  eMB_PortExitCriticalSection();
}

eMB_ErrorCodeType eMB_Master_RTUReceive(uint8_t *pucRcvAddress, uint8_t **pucFrame, uint16_t *pusLength)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  eMB_PortEnterCriticalSection();

  /* Length and CRC check */
  if ((eMB_RTU_RecvLength >= eMB_SDU_SIZE_MIN) &&
      (eMB_GetCRC((uint8_t *)eMB_RTU_RecvBuf, eMB_RTU_RecvLength) == (uint16_t)0U))
  {
    /* Save the address field. All frames are passed to the upper layer
     * and the decision if a frame is used is done there. */
    *pucRcvAddress = eMB_RTU_RecvBuf[eMB_SDU_ADDR_OFFSET];

    /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
     * size of address field and CRC checksum. */
    *pusLength = (uint16_t)(eMB_RTU_RecvLength - eMB_SDU_FUNC_OFFSET - eMB_SDU_CRC_SIZE);

    /* Return the start of the Modbus PDU to the caller. */
    *pucFrame = (uint8_t *)&eMB_RTU_RecvBuf[eMB_SDU_FUNC_OFFSET];
  }
  else
  {
    errStatus = eMB_EIO;
  }

  eMB_PortExitCriticalSection();

  return errStatus;
}

eMB_ErrorCodeType eMB_Master_RTUSend(uint8_t ucSlaveAddress, const uint8_t *pucFrame, uint16_t usLength)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;
  uint16_t          crcVal;

  if (ucSlaveAddress > eMB_MASTER_TOTAL_SLAVE_NUM)
    return eMB_EINVAL;

  eMB_PortEnterCriticalSection();

  /* Check if the receiver is still in idle state. If not we where to
   * slow with processing the received frame and the master sent another
   * frame on the network. We have to abort sending the frame. */
  if (eMB_RTU_RecvState == eMB_RTU_RECV_STATE_IDLE)
  {
    /* First byte before the Modbus-PDU is the slave address. */
    eMB_RTU_SendBufPos = (uint8_t *)pucFrame - 1U;
    eMB_RTU_SendCount = 1;

    /* Now copy the Modbus-PDU into the Modbus-SDU. */
    eMB_RTU_SendBufPos[eMB_SDU_ADDR_OFFSET] = ucSlaveAddress;
    eMB_RTU_SendCount += usLength;

    /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
    crcVal = eMB_GetCRC((uint8_t *)eMB_RTU_SendBufPos, eMB_RTU_SendCount);

    eMB_RTU_SendBuf[eMB_RTU_SendCount++] = (uint8_t)(crcVal & 0xFF);
    eMB_RTU_SendBuf[eMB_RTU_SendCount++] = (uint8_t)(crcVal >> 8U);

    /* Activate the transmitter. */
    eMB_RTU_SendState = eMB_RTU_SEND_STATE_XMIT;
    eMB_gConfigPtr->pPortSerialSetMode(eMB_PORT_SERIAL_TX);
  }
  else
  {
    errStatus = eMB_EIO;
  }

  eMB_PortExitCriticalSection();

  return errStatus;
}





bool eMB_Master_RTUFrameByteReceivedCallback(void)
{
  uint8_t recvByte;

  /* Always read the character. */
  (void) eMB_gConfigPtr->pPortSerialGetByte(&recvByte);

  switch (eMB_RTU_RecvState)
  {
    /* If we have received a character in the init state we have to
     * wait until the frame is finished. */
    case eMB_RTU_RECV_STATE_INIT:
    {
      eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_T35);
      break;
    }
    /* In the error state we wait until all characters in the
     * damaged frame are transmitted. */
    case eMB_RTU_RECV_STATE_ERROR:
    {
      eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_T35);
      break;
    }
    /* In the idle state we wait for a new character. If a character
     * is received the t3.5 timers are started and the receiver
     * is in the state STATE_RX_RECEIVCE and disable early the timer
     * of respond timeout. */
    case eMB_RTU_RECV_STATE_IDLE:
    {
      /* In time of respond timeout, the receiver receive a frame.
       * Disable timer of respond timeout and change the transmiter state to idle. */
      eMB_gConfigPtr->pPortTimersDisable();
      eMB_RTU_SendState = eMB_RTU_SEND_STATE_IDLE;

      eMB_RTU_RecvLength = 0;
      eMB_RTU_RecvBuf[eMB_RTU_RecvLength++] = recvByte;

      eMB_RTU_RecvState = eMB_RTU_RECV_STATE_RCV;

      /* Enable t3.5 timers. */
      eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_T35);

      break;
    }
    /* We are currently receiving a frame. Reset the timer after
     * every character received. If more than the maximum possible
     * number of bytes in a modbus frame is received the frame is
     * ignored. */
    case eMB_RTU_RECV_STATE_RCV:
    {
      if (eMB_RTU_RecvLength < eMB_SDU_SIZE_MAX)
      {
        eMB_RTU_RecvBuf[eMB_RTU_RecvLength++] = recvByte;
      }
      else
      {
        eMB_RTU_RecvState = eMB_RTU_RECV_STATE_ERROR;
      }

      /* Restart t3.5 timers. */
      eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_T35);

      break;
    }
    default:
      break;
  }

  return true;
}

bool eMB_Master_RTUFrameTransmitterEmptyCallback(void)
{
  switch (eMB_RTU_SendState)
  {
    /* We should not get a transmitter event if the transmitter is in idle state.  */
    case eMB_RTU_SEND_STATE_IDLE:
    {
      /* Enable receiver, disable transmitter. */
      eMB_gConfigPtr->pPortSerialSetMode(eMB_PORT_SERIAL_RX);

      break;
    }
    case eMB_RTU_SEND_STATE_XMIT:
    {
      /* Check if there is data to be transmitted. */
      if (eMB_RTU_SendCount != 0)
      {
        eMB_gConfigPtr->pPortSerialPutByte(*eMB_RTU_SendBufPos);

        eMB_RTU_SendBufPos++;  /* next byte in sendbuffer. */
        eMB_RTU_SendCount--;
      }
      else
      {
        eMB_RTU_FrameIsBroadcast = (eMB_RTU_SendBuf[eMB_SDU_ADDR_OFFSET] == eMB_ADDRESS_BROADCAST) ? true : false;

        /* Disable transmitter. This prevents another transmit buffer empty interrupt. */
        eMB_gConfigPtr->pPortSerialSetMode(eMB_PORT_SERIAL_RX);

        eMB_RTU_SendState = eMB_RTU_SEND_STATE_DONE;

        /* If the frame is broadcast, master will enable timer of convert delay,
         * else master will enable timer of respond timeout. */
        if (eMB_RTU_FrameIsBroadcast == true)
        {
          eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_CONVERT_DELAY);
        }
        else
        {
          eMB_gConfigPtr->pPortTimersEnable(eMB_PORT_TIMER_RESPOND_TIMEOUT);
        }
      }
      break;
    }
    default:
      break;
  }

  return true;
}

bool eMB_Master_RTUTimerExpiredCallback(void)
{
  switch (eMB_RTU_RecvState)
  {
    /* Timer t35 expired. Startup phase is finished. */
    case eMB_RTU_RECV_STATE_INIT:
    {
      eMB_gConfigPtr->pPortEventPost(eMB_EV_READY);

      break;
    }
    /* A frame was received and t35 expired. Notify the listener that
     * a new frame was received. */
    case eMB_RTU_RECV_STATE_RCV:
    {
      eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_RECEIVED);

      break;
    }
    /* An error occured while receiving the frame. */
    case eMB_RTU_RECV_STATE_ERROR:
    {
      eMB_Util_SetErrorEvent(eMB_EV_ERROR_RECEIVE_DATA);
      eMB_gConfigPtr->pPortEventPost(eMB_EV_ERROR);

      break;
    }
    /* Function called in an illegal state. */
    default:
      break;
  }

  eMB_RTU_RecvState = eMB_RTU_RECV_STATE_IDLE;

  switch (eMB_RTU_SendState)
  {
    /* A frame was send finish and convert delay or respond timeout expired.
     * If the frame is broadcast, the master will idle, and if the frame is not
     * broadcast, then notify the listener process error. */
    case eMB_RTU_SEND_STATE_DONE:
    {
      if (eMB_RTU_FrameIsBroadcast == false)
      {
        eMB_Util_SetErrorEvent(eMB_EV_ERROR_RESPOND_TIMEOUT);
        eMB_gConfigPtr->pPortEventPost(eMB_EV_ERROR);
      }

      break;
    }
    /* Function called in an illegal state. */
    default:
      break;
  }

  eMB_RTU_SendState = eMB_RTU_SEND_STATE_IDLE;

  eMB_gConfigPtr->pPortTimersDisable();

  /* If timer mode is convert delay, the master event then turns eMB_EV_EXECUTE status. */
  // if (eMB_RTU_TimerMode == eMB_PORT_TIMER_CONVERT_DELAY)
  // {
  //   eMB_gConfigPtr->pPortEventPost(eMB_EV_EXECUTE);
  // }

  return true;
}





/* Get Modbus Master send destination address. */
uint8_t eMB_Master_RTUGetSlaveAddress(void)
{
  return eMB_RTU_SlaveAddr;
}

/* Set Modbus Master send destination address. */
void eMB_Master_RTUSetSlaveAddress(uint8_t address)
{
  eMB_RTU_SlaveAddr = address;
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void eMB_Master_RTUGetSendPduBuffer(uint8_t **pucFrame)
{
  *pucFrame = (uint8_t *)&eMB_RTU_SendBuf[eMB_SDU_FUNC_OFFSET];
}

/* Set Modbus Master send PDU's buffer length.*/
void eMB_Master_RTUSetSendPduLength(uint16_t length)
{
  eMB_RTU_SendLength = length;
}

/* Get Modbus Master send PDU's buffer length.*/
uint16_t eMB_Master_RTUGetSendPduLength(void)
{
  return eMB_RTU_SendLength;
}

/* The master request is broadcast? */
bool eMB_Master_RTUIsBroadcast(void)
{
  return eMB_RTU_FrameIsBroadcast;
}
#endif



#ifdef __cplusplus
}
#endif
