/* 
 * File:   eMB_RTU.h
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_RTU_H
#define EMB_RTU_H

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
*                                   FUNCTION PROTOTYPES
===============================================================================================*/

#ifdef eMB_SLAVE_RTU_ENABLED
eMB_ErrorCodeType   eMB_Slave_RTUInit(void);
void                eMB_Slave_RTUStart(void);
void                eMB_Slave_RTUStop(void);
eMB_ErrorCodeType   eMB_Slave_RTUReceive(uint8_t *pucRcvAddress, uint8_t **pucFrame, uint16_t *pusLength);
eMB_ErrorCodeType   eMB_Slave_RTUSend(uint8_t slaveAddress, const uint8_t *pucFrame, uint16_t usLength);

bool                eMB_Slave_RTUFrameByteReceivedCallback( void );
bool                eMB_Slave_RTUFrameTransmitterEmptyCallback( void );
bool                eMB_Slave_RTUTimerExpiredCallback( void );
#endif

#ifdef eMB_MASTER_RTU_ENABLED
eMB_ErrorCodeType   eMB_Master_RTUInit(void);
void                eMB_Master_RTUStart(void);
void                eMB_Master_RTUStop(void);
eMB_ErrorCodeType   eMB_Master_RTUReceive(uint8_t *pucRcvAddress, uint8_t **pucFrame, uint16_t *pusLength);
eMB_ErrorCodeType   eMB_Master_RTUSend(uint8_t slaveAddress, const uint8_t *pucFrame, uint16_t usLength);

bool                eMB_Master_RTUFrameByteReceivedCallback(void);
bool                eMB_Master_RTUFrameTransmitterEmptyCallback(void);
bool                eMB_Master_RTUTimerExpiredCallback(void);

uint8_t             eMB_Master_RTUGetSlaveAddress(void);
void                eMB_Master_RTUSetSlaveAddress(uint8_t address);

void                eMB_Master_RTUGetSendPduBuffer(uint8_t **pucFrame);
void                eMB_Master_RTUSetSendPduLength(uint16_t length);
uint16_t            eMB_Master_RTUGetSendPduLength(void);

bool                eMB_Master_RTUIsBroadcast(void);
#endif



#ifdef __cplusplus
}
#endif

#endif /* EMB_RTU_H */
