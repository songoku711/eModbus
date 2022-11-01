/* 
 * File:   eMB_PortUtils.c
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




/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/




/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

void eMB_PortEnterCriticalSection(void)
{
  taskENTER_CRITICAL();
}

void eMB_PortExitCriticalSection(void)
{
  taskEXIT_CRITICAL();
}



#ifdef __cplusplus
}
#endif
