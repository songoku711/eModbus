/* 
 * File:   eMB_PortTimer.c
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

#include "tim.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define eMB_WEH_PORT_TIMER_INSTANCE               &htim7



/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/




/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/

static void eMB_WEH_PortTimerElapsedCallback(TIM_HandleTypeDef *xTimer);



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

/* By default we acknowledged that timer frequency is 10kHz (period is 100us) */
bool eMB_WEH_PortTimersInit(void)
{
  __HAL_TIM_SetAutoreload(eMB_WEH_PORT_TIMER_INSTANCE, (eMB_MASTER_DELAY_MS_T35 - 1));
  HAL_TIM_RegisterCallback(eMB_WEH_PORT_TIMER_INSTANCE, HAL_TIM_PERIOD_ELAPSED_CB_ID, eMB_WEH_PortTimerElapsedCallback);
  
  return true;
}

void eMB_WEH_PortTimersEnable(eMB_PortTimerModeType timerMode)
{
  uint16_t delay = (uint16_t)0U;

  switch (timerMode)
  {
    case eMB_PORT_TIMER_T35:
    {
      delay = (uint16_t)eMB_MASTER_DELAY_MS_T35 - 1U;

      break;
    }
    case eMB_PORT_TIMER_RESPOND_TIMEOUT:
    {
      delay = (uint16_t)eMB_MASTER_DELAY_MS_CONVERT - 1U;

      break;
    }
    case eMB_PORT_TIMER_CONVERT_DELAY:
    {
      delay = (uint16_t)eMB_MASTER_TIMEOUT_MS_RESPOND - 1U;

      break;
    }
    default:
      break;
  }

  __HAL_TIM_SetAutoreload(eMB_WEH_PORT_TIMER_INSTANCE, delay);
  __HAL_TIM_SetCounter(eMB_WEH_PORT_TIMER_INSTANCE, 0);
  
  HAL_TIM_Base_Start_IT(eMB_WEH_PORT_TIMER_INSTANCE);
}

void eMB_WEH_PortTimersDisable(void)
{
  HAL_TIM_Base_Stop_IT(eMB_WEH_PORT_TIMER_INSTANCE);
}





static void eMB_WEH_PortTimerElapsedCallback(TIM_HandleTypeDef *xTimer)
{
  HAL_TIM_Base_Stop_IT(eMB_WEH_PORT_TIMER_INSTANCE);

  (void)eMB_Master_RTUTimerExpiredCallback();
}



#ifdef __cplusplus
}
#endif
