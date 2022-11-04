/* 
 * File:   eMB_FuncHolding.c
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
#define eMB_PDU_FUNC_READ_REGCNT_MAX              ( 0x007D )
#define eMB_PDU_FUNC_READ_BYTECNT_OFF             ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_FUNC_READ_VALUES_OFF              ( eMB_PDU_DATA_OFFSET + 1 )
#define eMB_PDU_FUNC_READ_SIZE_MIN                ( 1 )

#define eMB_PDU_REQ_WRITE_ADDR_OFF                ( eMB_PDU_DATA_OFFSET + 0)
#define eMB_PDU_REQ_WRITE_VALUE_OFF               ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_WRITE_SIZE                    ( 4 )
#define eMB_PDU_FUNC_WRITE_ADDR_OFF               ( eMB_PDU_DATA_OFFSET + 0)
#define eMB_PDU_FUNC_WRITE_VALUE_OFF              ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_FUNC_WRITE_SIZE                   ( 4 )

#define eMB_PDU_REQ_WRITE_MUL_ADDR_OFF            ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_REQ_WRITE_MUL_REGCNT_OFF          ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_WRITE_MUL_BYTECNT_OFF         ( eMB_PDU_DATA_OFFSET + 4 )
#define eMB_PDU_REQ_WRITE_MUL_VALUES_OFF          ( eMB_PDU_DATA_OFFSET + 5 )
#define eMB_PDU_REQ_WRITE_MUL_SIZE_MIN            ( 5 )
#define eMB_PDU_REQ_WRITE_MUL_REGCNT_MAX          ( 0x0078 )
#define eMB_PDU_FUNC_WRITE_MUL_ADDR_OFF           ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_FUNC_WRITE_MUL_REGCNT_OFF         ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_FUNC_WRITE_MUL_SIZE               ( 4 )

#define eMB_PDU_REQ_READWRITE_READ_ADDR_OFF       ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_REQ_READWRITE_READ_REGCNT_OFF     ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_READWRITE_WRITE_ADDR_OFF      ( eMB_PDU_DATA_OFFSET + 4 )
#define eMB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF    ( eMB_PDU_DATA_OFFSET + 6 )
#define eMB_PDU_REQ_READWRITE_WRITE_BYTECNT_OFF   ( eMB_PDU_DATA_OFFSET + 8 )
#define eMB_PDU_REQ_READWRITE_WRITE_VALUES_OFF    ( eMB_PDU_DATA_OFFSET + 9 )
#define eMB_PDU_REQ_READWRITE_SIZE_MIN            ( 9 )
#define eMB_PDU_FUNC_READWRITE_READ_BYTECNT_OFF   ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_FUNC_READWRITE_READ_VALUES_OFF    ( eMB_PDU_DATA_OFFSET + 1 )
#define eMB_PDU_FUNC_READWRITE_SIZE_MIN           ( 1 )



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_WRITE_HOLDING_ENABLED
/**
 * This function will request write holding register.
 *
 * @param slaveAddr slave address
 * @param holdingAddr register start address
 * @param holdingData register data to be written
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestWriteHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingAddr,
  uint16_t holdingData
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]             = eMB_FUNC_WRITE_REGISTER;
    pPduBuffer[eMB_PDU_REQ_WRITE_ADDR_OFF]      = (uint8_t)(holdingAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_ADDR_OFF + 1]  = (uint8_t)(holdingAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_WRITE_VALUE_OFF]     = (uint8_t)(holdingData >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_VALUE_OFF + 1] = (uint8_t)(holdingData & (uint16_t)0x00FFU);

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_WRITE_SIZE);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle write holding register response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncWriteHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint16_t holdingAddr;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  if (*recvPduLength == (eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_WRITE_SIZE))
  {
    holdingAddr   = (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_ADDR_OFF] << 8U);
    holdingAddr  |= (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_ADDR_OFF + 1]  );
    holdingAddr++;

    /* Make callback to update the value. */
    errStatus = eMB_Util_FuncHoldingRegisterCallback(holdingAddr, (uint16_t)1U, &recvPduFrame[eMB_PDU_FUNC_WRITE_VALUE_OFF]);

    /* If an error occured convert it into a Modbus exception. */
    if (errStatus != eMB_ENOERR)
    {
      exptStatus = eMB_Util_ErrorToException(errStatus);
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

#ifdef eMB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED
/*=============================================================================================*/
/**
 * This function will request write multiple holding register.
 *
 * @param slaveAddr slave address
 * @param holdingAddr register start address
 * @param holdingNum register total number
 * @param holdingData data to be written
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestWriteMultipleHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingAddr,
  uint16_t holdingNum,
  uint16_t *holdingData
)
{
  uint8_t *pPduBuffer;
  uint16_t usRegIndex = (uint16_t)0U;
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]                  = eMB_FUNC_WRITE_MULTIPLE_REGISTERS;
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_ADDR_OFF]       = (uint8_t)(holdingAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_ADDR_OFF + 1]   = (uint8_t)(holdingAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_REGCNT_OFF]     = (uint8_t)(holdingNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_REGCNT_OFF + 1] = (uint8_t)(holdingNum & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_BYTECNT_OFF]    = (uint8_t)(holdingNum * 2U);

    pPduBuffer += eMB_PDU_REQ_WRITE_MUL_VALUES_OFF;

    while (holdingNum > usRegIndex)
    {
      *pPduBuffer++ = holdingData[usRegIndex] >> 8;
      *pPduBuffer++ = holdingData[usRegIndex++] ;
    }

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_WRITE_MUL_SIZE_MIN + holdingNum * 2U);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle write multiple holding register response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncWriteMultipleHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint8_t *pPduBuffer;
  uint16_t holdingAddr;
  uint16_t holdingNum;
  uint8_t  byteCount;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  /* If this request is broadcast, the *recvPduLength is not need check. */
  if ((*recvPduLength == (uint16_t)(eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_WRITE_MUL_SIZE)) || eMB_FrameIsBroadcastCalloutArr())
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);

    holdingAddr  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_WRITE_MUL_ADDR_OFF] << 8U);
    holdingAddr |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_WRITE_MUL_ADDR_OFF + 1]  );
    holdingAddr++;

    holdingNum  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_WRITE_MUL_REGCNT_OFF] << 8U);
    holdingNum |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_WRITE_MUL_REGCNT_OFF + 1]  );

    byteCount = pPduBuffer[eMB_PDU_REQ_WRITE_MUL_BYTECNT_OFF];

    if (byteCount == (uint8_t)(holdingNum * 2U))
    {
      /* Make callback to update the register values. */
      errStatus = eMB_Util_FuncHoldingRegisterCallback(holdingAddr, holdingNum, &pPduBuffer[eMB_PDU_REQ_WRITE_MUL_VALUES_OFF]);

      /* If an error occured convert it into a Modbus exception. */
      if (errStatus != eMB_ENOERR)
      {
        exptStatus = eMB_Util_ErrorToException(errStatus);
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

#ifdef eMB_FUNC_READ_HOLDING_ENABLED
/*=============================================================================================*/
/**
 * This function will request read holding register.
 *
 * @param slaveAddr slave address
 * @param holdingAddr register start address
 * @param holdingNum register total number
 * @param lTimeOut timeout (-1 will waiting forever)
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestReadHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingAddr,
  uint16_t holdingNum
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]             = eMB_FUNC_READ_HOLDING_REGISTER;
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF]       = (uint8_t)(holdingAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]   = (uint8_t)(holdingAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF]     = (uint8_t)(holdingNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF + 1] = (uint8_t)(holdingNum & (uint16_t)0x00FFU);

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_READ_SIZE);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle read holding register response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncReadHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint8_t *pPduBuffer;
  uint16_t holdingAddr;
  uint16_t holdingNum;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  /* If this request is broadcast, and it's read mode. This request don't need execute. */
  if (eMB_FrameIsBroadcastCalloutArr())
  {
    /* Passed */
  }
  else if (*recvPduLength >= (uint16_t)(eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_READ_SIZE_MIN))
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);

    holdingAddr   = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF] << 8U);
    holdingAddr  |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]  );
    holdingAddr++;

    holdingNum    = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF] << 8U);
    holdingNum   |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_REGCNT_OFF + 1]  );

    /* Check if the number of registers to read is valid. If not
     * return Modbus illegal data value exception. */
    if ((holdingNum > (uint16_t)0U) && ((uint8_t)(holdingNum * 2U) == recvPduFrame[eMB_PDU_FUNC_READ_BYTECNT_OFF]))
    {
      /* Make callback to fill the buffer. */
      errStatus = eMB_Util_FuncHoldingRegisterCallback(holdingAddr, holdingNum, &recvPduFrame[eMB_PDU_FUNC_READ_VALUES_OFF]);

      /* If an error occured convert it into a Modbus exception. */
      if (errStatus != eMB_ENOERR)
      {
        exptStatus = eMB_Util_ErrorToException(errStatus);
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

#ifdef eMB_FUNC_READWRITE_HOLDING_ENABLED
/*=============================================================================================*/
/**
 * This function will request read and write holding register.
 *
 * @param slaveAddr slave address
 * @param holdingReadAddr read register start address
 * @param holdingReadNum read register total number
 * @param holdingData data to be written
 * @param holdingWriteAddr write register start address
 * @param holdingWriteNum write register total number
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestReadWriteMultipleHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingReadAddr,
  uint16_t holdingReadNum,
  uint16_t *holdingData,
  uint16_t holdingWriteAddr,
  uint16_t holdingWriteNum
)
{
  uint8_t *pPduBuffer;
  uint16_t usRegIndex = (uint16_t)0U;
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]                        = eMB_FUNC_READWRITE_MULTIPLE_REGISTERS;
    pPduBuffer[eMB_PDU_REQ_READWRITE_READ_ADDR_OFF]        = (uint8_t)(holdingReadAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_READWRITE_READ_ADDR_OFF + 1]    = (uint8_t)(holdingReadAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READWRITE_READ_REGCNT_OFF]      = (uint8_t)(holdingReadNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_READWRITE_READ_REGCNT_OFF + 1]  = (uint8_t)(holdingReadNum & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_ADDR_OFF]       = (uint8_t)(holdingWriteAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_ADDR_OFF + 1]   = (uint8_t)(holdingWriteAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF]     = (uint8_t)(holdingWriteNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF + 1] = (uint8_t)(holdingWriteNum & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_BYTECNT_OFF]    = (uint8_t)(holdingWriteNum * 2U);

    pPduBuffer += eMB_PDU_REQ_READWRITE_WRITE_VALUES_OFF;
    while (holdingWriteNum > usRegIndex)
    {
      *pPduBuffer++ = holdingData[usRegIndex] >> 8U;
      *pPduBuffer++ = holdingData[usRegIndex++];
    }

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_READWRITE_SIZE_MIN + holdingWriteNum * 2U);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle read write holding register response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncReadWriteMultipleHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint16_t holdingReadAddr;
  uint16_t holdingReadNum;
  uint16_t holdingWriteAddr;
  uint16_t holdingWriteNum;
  uint8_t *pPduBuffer;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  /* If this request is broadcast, and it's read mode. This request don't need execute. */
  if (eMB_FrameIsBroadcastCalloutArr())
  {
    /* Passed */
  }
  else if (*recvPduLength >= (uint16_t)(eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_READWRITE_SIZE_MIN))
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);

    holdingReadAddr  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_READ_ADDR_OFF] << 8U);
    holdingReadAddr |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_READ_ADDR_OFF + 1]  );
    holdingReadAddr++;

    holdingReadNum  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_READ_REGCNT_OFF] << 8U);
    holdingReadNum |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_READ_REGCNT_OFF + 1]  );

    holdingWriteAddr  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_ADDR_OFF] << 8U);
    holdingWriteAddr |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_ADDR_OFF + 1]  );
    holdingWriteAddr++;

    holdingWriteNum  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF] << 8U);
    holdingWriteNum |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF + 1]  );

    if ((uint8_t)(holdingReadNum * 2U) == recvPduFrame[eMB_PDU_FUNC_READWRITE_READ_BYTECNT_OFF])
    {
      /* Make callback to update the register values. */
      errStatus = eMB_Util_FuncHoldingRegisterCallback(holdingWriteAddr, holdingWriteNum,
                                                       &pPduBuffer[eMB_PDU_REQ_READWRITE_WRITE_VALUES_OFF]);

      if (errStatus == eMB_ENOERR)
      {
        /* Make the read callback. */
        errStatus = eMB_Util_FuncHoldingRegisterCallback(holdingReadAddr, holdingReadNum, 
                                                         &recvPduFrame[eMB_PDU_FUNC_READWRITE_READ_VALUES_OFF]);
      }

      if (errStatus != eMB_ENOERR)
      {
        exptStatus = eMB_Util_ErrorToException(errStatus);
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
