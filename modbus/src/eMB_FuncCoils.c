/* 
 * File:   eMB_FuncCoils.c
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
#define eMB_PDU_REQ_READ_COILCNT_OFF              ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_READ_SIZE                     ( 4 )
#define eMB_PDU_FUNC_READ_COILCNT_OFF             ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_FUNC_READ_VALUES_OFF              ( eMB_PDU_DATA_OFFSET + 1 )
#define eMB_PDU_FUNC_READ_SIZE_MIN                ( 1 )

#define eMB_PDU_REQ_WRITE_ADDR_OFF                ( eMB_PDU_DATA_OFFSET )
#define eMB_PDU_REQ_WRITE_VALUE_OFF               ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_WRITE_SIZE                    ( 4 )
#define eMB_PDU_FUNC_WRITE_ADDR_OFF               ( eMB_PDU_DATA_OFFSET )
#define eMB_PDU_FUNC_WRITE_VALUE_OFF              ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_FUNC_WRITE_SIZE                   ( 4 )

#define eMB_PDU_REQ_WRITE_MUL_ADDR_OFF            ( eMB_PDU_DATA_OFFSET )
#define eMB_PDU_REQ_WRITE_MUL_COILCNT_OFF         ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_WRITE_MUL_BYTECNT_OFF         ( eMB_PDU_DATA_OFFSET + 4 )
#define eMB_PDU_REQ_WRITE_MUL_VALUES_OFF          ( eMB_PDU_DATA_OFFSET + 5 )
#define eMB_PDU_REQ_WRITE_MUL_SIZE_MIN            ( 5 )
#define eMB_PDU_REQ_WRITE_MUL_COILCNT_MAX         ( 0x07B0 )
#define eMB_PDU_FUNC_WRITE_MUL_ADDR_OFF           ( eMB_PDU_DATA_OFFSET )
#define eMB_PDU_FUNC_WRITE_MUL_COILCNT_OFF        ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_FUNC_WRITE_MUL_SIZE               ( 5 )



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_READ_COILS_ENABLED
/**
 * This function will request read coil.
 *
 * @param slaveAddr slave address
 * @param coilAddr coil start address
 * @param coilNum coil total number
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestReadCoils
(
  uint8_t  slaveAddr,
  uint16_t coilAddr,
  uint16_t coilNum
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]               = eMB_FUNC_READ_COILS;
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF]         = (uint8_t)(coilAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]     = (uint8_t)(coilAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READ_COILCNT_OFF ]     = (uint8_t)(coilNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_COILCNT_OFF + 1]  = (uint8_t)(coilNum & (uint16_t)0x00FFU);

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_READ_SIZE);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle read coil response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncReadCoilsHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint8_t *pPduBuffer;
  uint16_t coilAddr;
  uint16_t coilNum;
  uint8_t  byteCount;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  /* If this request is broadcast, and it's read mode. This request don't need execute. */
  if (eMB_FrameIsBroadcastCalloutArr() == true)
  {
    /* Passed */
  }
  /* Check received payload data length */
  else if (*recvPduLength >= (uint16_t)(eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_READ_SIZE_MIN))
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);
    
    coilAddr    = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF] << 8U);
    coilAddr   |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]  );
    coilAddr++;

    coilNum     = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_COILCNT_OFF] << 8U);
    coilNum    |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_COILCNT_OFF + 1]  );

    /* Test if the quantity of coils is a multiple of 8. If not last
     * byte is only partially field with unused coils set to zero. */
    if ((coilNum & (uint16_t)0x0007U) != (uint16_t)0U)
    {
      byteCount = (uint8_t)(coilNum / 8U + 1);
    }
    else
    {
      byteCount = (uint8_t)(coilNum / 8U);
    }

    /* Check if the number of registers to read is valid. If not
     * return Modbus illegal data value exception. */
    if ((coilNum > (uint16_t)0U) && (byteCount == recvPduFrame[eMB_PDU_FUNC_READ_COILCNT_OFF]))
    {
      /* Make callback to fill the buffer. */
      errStatus = eMB_Util_FuncCoilsCallback(coilAddr, coilNum, &recvPduFrame[eMB_PDU_FUNC_READ_VALUES_OFF]);

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
    /* Can't be a valid read coil register request because the length
     * is incorrect. */
    exptStatus = eMB_EX_ILLEGAL_DATA_VALUE;
  }

  return exptStatus;
}
#endif



#ifdef eMB_FUNC_WRITE_COIL_ENABLED
/*=============================================================================================*/
/**
 * This function will request write one coil.
 *
 * @param slaveAddr slave address
 * @param coilAddr coil start address
 * @param coilData data to be written
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestWriteSingleCoil
(
  uint8_t  slaveAddr,
  uint16_t coilAddr,
  uint16_t coilData
)
{
  uint8_t *pPduBuffer;
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  /* Check if slave address exceeds number of slave addresses in network */
  if (slaveAddr > eMB_MASTER_TOTAL_SLAVE_NUM)
  {
    errStatus = eMB_EINVAL;
  }
  /* Check if coil data is valid - 0xFF00 (ON) or 0x0000 (OFF) */
  else if ((coilData != 0xFF00) && (coilData != 0x0000))
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]             = eMB_FUNC_WRITE_SINGLE_COIL;
    pPduBuffer[eMB_PDU_REQ_WRITE_ADDR_OFF]      = (uint8_t)(coilAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_ADDR_OFF + 1]  = (uint8_t)(coilAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_WRITE_VALUE_OFF ]    = (uint8_t)(coilData >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_VALUE_OFF + 1] = (uint8_t)(coilData & (uint16_t)0x00FFU);

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_WRITE_SIZE);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle write single coil response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncWriteSingleCoilHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint16_t coilAddr;
  uint8_t  ucBuf[2];
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  if (*recvPduLength == (uint16_t)(eMB_PDU_FUNC_WRITE_SIZE + eMB_PDU_SIZE_MIN))
  {
    coilAddr  = (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_ADDR_OFF] << 8U);
    coilAddr |= (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_ADDR_OFF + 1]  );
    coilAddr++;

    if (
        (recvPduFrame[eMB_PDU_FUNC_WRITE_VALUE_OFF + 1] == 0x00) &&
        (
          (recvPduFrame[eMB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF) ||
          (recvPduFrame[eMB_PDU_FUNC_WRITE_VALUE_OFF] == 0x00)
        )
       )
    {
      ucBuf[1] = 0;

      if (recvPduFrame[eMB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF)
      {
        ucBuf[0] = 1;
      }
      else
      {
        ucBuf[0] = 0;
      }

      errStatus = eMB_Util_FuncCoilsCallback(coilAddr, (uint16_t)1U, ucBuf);

      /* If an error occurred convert it into a Modbus exception. */
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
    /* Can't be a valid write coil register request because the length is incorrect. */
    exptStatus = eMB_EX_ILLEGAL_DATA_VALUE;
  }

  return exptStatus;
}
#endif



#ifdef eMB_FUNC_WRITE_MULTIPLE_COILS_ENABLED
/*=============================================================================================*/
/**
 * This function will request write multiple coils.
 *
 * @param slaveAddr salve address
 * @param coilAddr coil start address
 * @param coilNum coil total number
 * @param coilData data to be written
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestWriteMultipleCoils
(
  uint8_t  slaveAddr,
  uint16_t coilAddr,
  uint16_t coilNum,
  uint8_t *coilData
)
{
  uint8_t *pPduBuffer;
  uint16_t usRegIndex = 0;
  uint8_t  byteCount;
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  /* Check if slave address exceeds number of slave addresses in network */
  if (slaveAddr > eMB_MASTER_TOTAL_SLAVE_NUM)
  {
    errStatus = eMB_EINVAL;
  }
  /* Check if number of write coil data exceeds maximum number */
  else if (coilNum > eMB_PDU_REQ_WRITE_MUL_COILCNT_MAX)
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]                   = eMB_FUNC_WRITE_MULTIPLE_COILS;
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_ADDR_OFF]        = (uint8_t)(coilAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_ADDR_OFF + 1]    = (uint8_t)(coilAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_COILCNT_OFF]     = (uint8_t)(coilNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_COILCNT_OFF + 1] = (uint8_t)(coilNum & (uint16_t)0x00FFU) ;

    /* Test if the quantity of coils is a multiple of 8. If not last
     * byte is only partially field with unused coils set to zero. */
    if ((coilNum & (uint16_t)0x0007U) != (uint16_t)0U)
    {
      byteCount = (uint8_t)(coilNum / 8U + 1);
    }
    else
    {
      byteCount = (uint8_t)(coilNum / 8U);
    }

    pPduBuffer[eMB_PDU_REQ_WRITE_MUL_BYTECNT_OFF]     = byteCount;
    pPduBuffer += eMB_PDU_REQ_WRITE_MUL_VALUES_OFF;

    /* Fill payload data */
    while (byteCount > usRegIndex)
    {
      *pPduBuffer++ = coilData[usRegIndex++];
    }

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_WRITE_MUL_SIZE_MIN + byteCount);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle write multiple coils response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncWriteMultipleCoilsHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint16_t coilAddr;
  uint16_t coilNum;
  uint8_t  byteCount;
  uint8_t  byteCountVerify;
  uint8_t *pPduBuffer;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  /* If this request is broadcast, the *recvPduLength is not need check. */
  if ((*recvPduLength == eMB_PDU_FUNC_WRITE_MUL_SIZE) || eMB_FrameIsBroadcastCalloutArr())
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);
    
    coilAddr  = (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8U);
    coilAddr |= (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1]  );
    coilAddr++;

    coilNum  = (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_MUL_COILCNT_OFF] << 8U);
    coilNum |= (uint16_t)(recvPduFrame[eMB_PDU_FUNC_WRITE_MUL_COILCNT_OFF + 1]  );

    byteCount = pPduBuffer[eMB_PDU_REQ_WRITE_MUL_BYTECNT_OFF];

    /* Compute the number of expected bytes in the request. */
    if ((coilNum & (uint16_t)0x0007U) != (uint16_t)0U)
    {
      byteCount = (uint8_t)(coilNum / 8U + 1);
    }
    else
    {
      byteCount = (uint8_t)(coilNum / 8U);
    }

    if ((coilNum > (uint16_t)0U) && (byteCountVerify == byteCount))
    {
      errStatus = eMB_Util_FuncCoilsCallback(coilAddr, coilNum, &pPduBuffer[eMB_PDU_REQ_WRITE_MUL_VALUES_OFF]);

      /* If an error occurred convert it into a Modbus exception. */
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
    /* Can't be a valid write coil register request because the length is incorrect. */
    exptStatus = eMB_EX_ILLEGAL_DATA_VALUE;
  }

  return exptStatus;
}
#endif

#endif

#ifdef __cplusplus
}
#endif
