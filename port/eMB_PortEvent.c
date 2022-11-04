/* 
 * File:   eMB_PortEvent.c
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

#include "cmsis_os.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/




/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
static osSemaphoreId_t eMB_WEH_OsResource;
static osEventFlagsId_t eMB_WEH_OsEvent;
#endif



/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
bool eMB_WEH_PortEventInit(void)
{
  eMB_WEH_OsEvent = osEventFlagsNew(NULL);

  if (eMB_WEH_OsEvent == NULL)
  {
    return false;
  }
  
  return true;
}

bool eMB_WEH_PortEventPost(eMB_EventType eEvent)
{
  osEventFlagsSet(eMB_WEH_OsEvent, eEvent);
  
  return true;
}

bool eMB_WEH_PortEventGet(eMB_EventType *eEvent)
{
  uint32_t recvEvent;
  
  /* Getting OS event */
  recvEvent = osEventFlagsWait(eMB_WEH_OsEvent, eMB_EV_ALL, osFlagsWaitAny, (uint32_t)0U);

  if (recvEvent == (recvEvent & (uint32_t)eMB_EV_ALL))
  {
    *eEvent = (eMB_EventType)recvEvent;
  
    return true;
  }

  return false;
}

/**
 * This function is initialize the OS resource for modbus master.
 * Note: The resource is define by OS. If you not use OS this function can be empty.
 *
 */
void eMB_WEH_PortResourceInit(void)
{
  eMB_WEH_OsResource = osSemaphoreNew(1U, 1U, NULL);
}

/**
 * This function will take Modbus Master running resource.
 * Note: The resource is define by OS. If you not use OS this function can be just return true.
 *
 * @return resource taked result
 */
bool eMB_WEH_PortResourceTake(void)
{
  osStatus_t status;
  
  /* Take resource */
  status = osSemaphoreAcquire(eMB_WEH_OsResource, (uint32_t)0U);
  
  if (status == osOK)
  {
    return true;
  }

  return false;
}

/**
 * This function is release Mobus Master running resource.
 * Note: The resource is define by OS. If you not use OS this function can be empty.
 *
 */
void eMB_WEH_PortResourceRelease(void)
{
  /* Release resource */
  (void)osSemaphoreRelease(eMB_WEH_OsResource);
}
#endif



#ifdef __cplusplus
}
#endif
