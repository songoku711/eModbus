/* 
 * File:   eMB_Utils.h
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_UTILS_H
#define EMB_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "eMB_Types.h"
#include "eMB_Cfg.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/




/*===============================================================================================
*                                          VARIABLES
===============================================================================================*/

extern eMB_FrameStartCallout                      eMB_FrameStartCalloutArr;
extern eMB_FrameStopCallout                       eMB_FrameStopCalloutArr;
extern eMB_FrameSendCallout                       eMB_FrameSendCalloutArr;
extern eMB_FrameReceiveCallout                    eMB_FrameReceiveCalloutArr;

extern eMB_FrameGetSlaveAddressCallout            eMB_FrameGetSlaveAddressCalloutArr;
extern eMB_FrameSetSlaveAddressCallout            eMB_FrameSetSlaveAddressCalloutArr;

extern eMB_FrameGetSendPduBufferCallout           eMB_FrameGetSendPduBufferCalloutArr;
extern eMB_FrameSetSendPduLengthCallout           eMB_FrameSetSendPduLengthCalloutArr;
extern eMB_FrameGetSendPduLengthCallout           eMB_FrameGetSendPduLengthCalloutArr;

extern eMB_FrameIsBroadcastCallout                eMB_FrameIsBroadcastCalloutArr;



extern eMB_ConfigStruct *eMB_gConfigPtr;



/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/

eMB_ErrorCodeType eMB_Util_FuncCoilsCallback(uint16_t coilAddr, uint16_t coilNum, uint8_t *recvPduFrame);

eMB_ErrorCodeType eMB_Util_FuncDiscreteInputsCallback(uint16_t disInputAddr, uint16_t disInputNum, uint8_t *recvPduFrame);

eMB_ErrorCodeType eMB_Util_FuncHoldingRegisterCallback(uint16_t holdingAddr, uint16_t holdingNum, uint8_t *recvPduFrame);

eMB_ErrorCodeType eMB_Util_FuncInputRegisterCallback(uint16_t inputAddr, uint16_t inputNum, uint8_t *recvPduFrame);

eMB_ExceptionType eMB_Util_ErrorToException(eMB_ErrorCodeType errorCode);

eMB_ErrorEventType eMB_Util_GetErrorEvent(void);
void eMB_Util_SetErrorEvent(eMB_ErrorEventType errorType);



#ifdef __cplusplus
}
#endif

#endif /* EMB_UTILS_H */
