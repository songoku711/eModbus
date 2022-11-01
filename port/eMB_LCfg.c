/* 
 * File:   eMB_LCfg.c
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



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

extern bool eMB_WEH_PortEventInit(void);
extern bool eMB_WEH_PortEventPost(eMB_EventType eEvent);
extern bool eMB_WEH_PortEventGet(eMB_EventType *eEvent);

extern void eMB_WEH_PortResourceInit(void);
extern bool eMB_WEH_PortResourceTake(void);
extern void eMB_WEH_PortResourceRelease(void);

extern bool eMB_WEH_PortSerialInit(void);
extern void eMB_WEH_PortSerialSetMode(eMB_PortSerialModeType serialMode);
extern bool eMB_WEH_PortSerialPutByte(uint8_t data);
extern bool eMB_WEH_PortSerialGetByte(uint8_t *data);

extern bool eMB_WEH_PortTimersInit(void);
extern void eMB_WEH_PortTimersEnable(eMB_PortTimerModeType timerMode);
extern void eMB_WEH_PortTimersDisable(void);



/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/

const eMB_ConfigStruct eMB_WehConfig =
{
  .role                       = eMB_ROLE_MASTER,
  .comm                       = eMB_COMM_RTU,
  /* Port event function pointer */
  .pPortEventInit             = eMB_WEH_PortEventInit,
  .pPortEventPost             = eMB_WEH_PortEventPost,
  .pPortEventGet              = eMB_WEH_PortEventGet,
  /* Port resource function pointer */
  .pPortResourceInit          = eMB_WEH_PortResourceInit,
  .pPortResourceTake          = eMB_WEH_PortResourceTake,
  .pPortResourceRelease       = eMB_WEH_PortResourceRelease,
  /* Port serial function pointer */
  .pPortSerialInit            = eMB_WEH_PortSerialInit,
  .pPortSerialSetMode         = eMB_WEH_PortSerialSetMode,
  .pPortSerialGetByte         = eMB_WEH_PortSerialGetByte,
  .pPortSerialPutByte         = eMB_WEH_PortSerialPutByte,
  /* Port timer function pointer */
  .pPortTimersInit            = eMB_WEH_PortTimersInit,
  .pPortTimersEnable          = eMB_WEH_PortTimersEnable,
  .pPortTimersDisable         = eMB_WEH_PortTimersDisable
};




/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/




#ifdef __cplusplus
}
#endif
