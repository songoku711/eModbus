/* 
 * File:   eMB_FuncDisInputs.c
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
#define eMB_PDU_REQ_READ_DISCCNT_OFF              ( eMB_PDU_DATA_OFFSET + 2 )
#define eMB_PDU_REQ_READ_SIZE                     ( 4 )
#define eMB_PDU_FUNC_READ_DISCCNT_OFF             ( eMB_PDU_DATA_OFFSET + 0 )
#define eMB_PDU_FUNC_READ_VALUES_OFF              ( eMB_PDU_DATA_OFFSET + 1 )
#define eMB_PDU_FUNC_READ_SIZE_MIN                ( 1 )



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_READ_DISCRETE_INPUTS_ENABLED
/**
 * This function will request read discrete inputs.
 *
 * @param slaveAddr slave address
 * @param disInputAddr discrete start address
 * @param disInputNum discrete total number
 *
 * @return error code
 */
eMB_ErrorCodeType eMB_Master_RequestReadDiscreteInputs
(
  uint8_t  slaveAddr,
  uint16_t disInputAddr,
  uint16_t disInputNum
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
    pPduBuffer[eMB_PDU_FUNC_OFFSET]              = eMB_FUNC_READ_DISCRETE_INPUTS;
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF]        = (uint8_t)(disInputAddr >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]    = (uint8_t)(disInputAddr & (uint16_t)0x00FFU);
    pPduBuffer[eMB_PDU_REQ_READ_DISCCNT_OFF ]    = (uint8_t)(disInputNum >> 8U);
    pPduBuffer[eMB_PDU_REQ_READ_DISCCNT_OFF + 1] = (uint8_t)(disInputNum & (uint16_t)0x00FFU);

    /* Set PDU buffer length */
    eMB_FrameSetSendPduLengthCalloutArr(eMB_PDU_SIZE_MIN + eMB_PDU_REQ_READ_SIZE);

    (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_FRAME_SENT);
  }

  return errStatus;
}

/*=============================================================================================*/
/**
 * This function will handle read discrete inputs response from slave.
 *
 * @param recvPduFrame received PDU frame pointer
 * @param recvPduLength received PDU length pointer
 *
 * @return error code
 */
eMB_ExceptionType eMB_Master_FuncReadDiscreteInputsHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  uint8_t *pPduBuffer;
  uint16_t disInputAddr;
  uint16_t disInputNum;
  uint8_t  byteCount;
  eMB_ExceptionType exptStatus = eMB_EX_NONE;
  eMB_ErrorCodeType errStatus;

  /* If this request is broadcast, and it's read mode. This request don't need execute. */
  if (eMB_FrameIsBroadcastCalloutArr() == true)
  {
    /* Passed */
  }
  else if (*recvPduLength >= (uint16_t)(eMB_PDU_SIZE_MIN + eMB_PDU_FUNC_READ_SIZE_MIN))
  {
    eMB_FrameGetSendPduBufferCalloutArr(&pPduBuffer);

    disInputAddr  = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF] << 8U);
    disInputAddr |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_ADDR_OFF + 1]  );
    disInputAddr++;

    disInputNum   = (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_DISCCNT_OFF] << 8U);
    disInputNum  |= (uint16_t)(pPduBuffer[eMB_PDU_REQ_READ_DISCCNT_OFF + 1]  );

    /* Test if the quantity of discrete inputs is a multiple of 8. If not last
     * byte is only partially field with unused coils set to zero. */
    if ((disInputNum & (uint16_t)0x0007U) != (uint16_t)0U)
    {
      byteCount = (uint8_t)(disInputNum / 8U + 1);
    }
    else
    {
      byteCount = (uint8_t)(disInputNum / 8U);
    }

    /* Check if the number of registers to read is valid. If not
     * return Modbus illegal data value exception. */
    if ((disInputNum > (uint16_t)0U) && (byteCount == recvPduFrame[eMB_PDU_FUNC_READ_DISCCNT_OFF]))
    {
      /* Make callback to fill the buffer. */
      errStatus = eMB_Util_FuncDiscreteInputsCallback(disInputAddr, disInputNum, &recvPduFrame[eMB_PDU_FUNC_READ_VALUES_OFF]);

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

#endif

#ifdef __cplusplus
}
#endif
