/* 
 * File:   eMB_FuncInput.c
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

#define eMB_PDU_REQ_READ_ADDR_OFF                 ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_REQ_READ_REGCNT_OFF               ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_READ_SIZE                     ( 4 )
#define eMB_PDU_FUNC_READ_BYTECNT_OFF             ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_FUNC_READ_VALUES_OFF              ( eMB_PDU_DATA_OFFSET + 1 )
#define eMB_PDU_FUNC_READ_SIZE_MIN                ( 1 )

#define eMB_PDU_FUNC_READ_RSP_BYTECNT_OFF         ( eMB_PDU_DATA_OFFSET )



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_READ_INPUT_ENABLED
/**
 * This function will request read input register.
 *
 * @param slaveAddr slave address
 * @param inputAddr register start address
 * @param inputNum register total number
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestReadInputRegister
(
  uint8_t  slaveAddr,
  uint16_t inputAddr,
  uint16_t inputNum
)
{
  uint8_t *pPduBuffer;
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  /* Check if slave address exceeds number of slave addresses in network */
  if (slaveAddr > eMB_MASTER_TOTAL_SLAVE_NUM)
  {
    errStatus = eMB_EINVAL;
  }
  /* Get resource control */
  else if (eMB_gConfigPtr->pPortResourceTake() == false)
  {
    errStatus = eMB_EBUSY;
  }
  else
  {
    /* Get PDU buffer pointer to store transmit data */
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);

    /* Set current slave address */
    eMB_FrameSetSlaveAddressCalloutArr(slaveAddr);

    /* Fill payload data */
    pPduBuffer[eMB_PDU_FUNC_OFFSET]             = eMB_FUNC_READ_INPUT_REGISTER;
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF]       = (uint8_t)(inputAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]   = (uint8_t)(inputAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF]     = (uint8_t)(inputNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF + 1] = (uint8_t)(inputNum & (uint16_t)0x00FFU);

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_READ_SIZE);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle read input register response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncReadInputRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint8_t *pPduBuffer;
  uint16_t inputAddr;
  uint16_t inputNum;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType eRegStatus;

  /* If this request is broadcast, and it's read mode. This request don't need execute. */
  if (eMB_FrameIsBroadcastCalloutArr())
  {
    /* Passed */
  }
  else if(*recvPduLength >= (uint16_t)(eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_READ_SIZE_MIN))
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);

    inputAddr   = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF] << 8U);
    inputAddr  |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]  );
    inputAddr++;

    inputNum    = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF] << 8U);
    inputNum   |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF + 1]  );

    /* Check if the number of registers to read is valid. If not
     * return Modbus illegal data value exception. */
    if ((inputNum > (uint16_t)0U) && ((uint8_t)(inputNum * 2U) == recvPduFrame[eMB_PDU_FUNC_READ_BYTECNT_OFF]))
    {
      /* Make callback to fill the buffer. */
      eRegStatus = eMB_Util_FuncInputRegisterCallback(inputAddr, inputNum, &recvPduFrame[eMB_PDU_FUNC_READ_VALUES_OFF]);

      /* If an error occurred convert it into a Modbus exception. */
      if (eRegStatus != eMB_ENOERR)
      {
        exptStatus = eMB_Util_ErrorToException(eRegStatus);
      }
    }
    else
    {
        exptStatus = eMB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    /* Can't be a valid request because the length is incorrect. */
    exptStatus = eMB_EX_ILLEGAL_DATA_VALUE;
  }

  return exptStatus;
}
#endif

#endif

#ifdef __cplusplus
}
#endif
