/* 
 * File:   eMB_FuncOther.c
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




/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/

#ifdef eMB_FUNC_OTHER_REP_SLAVEID_ENABLED
static uint8_t eMB_SlaveIDBuf[eMB_FUNC_OTHER_REP_SLAVEID_BUF];
static uint16_t eMB_SlaveIDLength;
#endif

/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#ifdef eMB_FUNC_OTHER_REP_SLAVEID_ENABLED
eMB_ErrorCodeType eMB_SetSlaveID
(
  uint8_t slaveID,
  bool isRunning,
  uint8_t const *pucAdditional,
  uint16_t usAdditionalLen
)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  /* The first byte and second byte in the buffer is reserved for
   * the parameter slaveID and the running flag. The rest of
   * the buffer is available for additional data. */
  if (usAdditionalLen + 2U < (uint16_t)eMB_FUNC_OTHER_REP_SLAVEID_BUF)
  {
    eMB_SlaveIDLength = (uint16_t)0U;

    eMB_SlaveIDBuf[eMB_SlaveIDLength++] = slaveID;
    eMB_SlaveIDBuf[eMB_SlaveIDLength++] = (uint8_t)(isRunning ? 0xFF : 0x00);

    if (usAdditionalLen > (uint16_t)0U)
    {
      memcpy(&eMB_SlaveIDBuf[eMB_SlaveIDLength], pucAdditional, (size_t)usAdditionalLen);
      eMB_SlaveIDLength += usAdditionalLen;
    }
  }
  else
  {
    errStatus = eMB_ENORES;
  }

  return errStatus;
}

eMB_ExceptionType eMB_FuncReportSlaveIDHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength)
{
  memcpy(&recvPduFrame[eMB_PDU_DATA_OFFSET], &eMB_SlaveIDBuf[0], (size_t)eMB_SlaveIDLength);
  *recvPduLength = (uint16_t)(eMB_PDU_DATA_OFFSET + eMB_SlaveIDLength);

  return eMB_EX_NONE;
}
#endif
