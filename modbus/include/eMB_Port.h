/* 
 * File:   eMB_Port.h
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_PORT_H
#define EMB_PORT_H

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

/*! \ingroup modbus
 *  \brief Serial mode for port.
 */
typedef enum
{
  eMB_PORT_SERIAL_TX,                             /*!< Port serial for transmitter enable. */
  eMB_PORT_SERIAL_RX                              /*!< Port serial for receiver enable. */
} eMB_PortSerialModeType;

/*! \ingroup modbus
 *  \brief Timer mode for port.
 */
typedef enum
{
  eMB_PORT_TIMER_T35,                             /*!< Port receive frame T3.5 timeout. */
  eMB_PORT_TIMER_RESPOND_TIMEOUT,                 /*!< Port waits respond for slave. */
  eMB_PORT_TIMER_CONVERT_DELAY                    /*!< Port sends broadcast, then delay sometime. */
} eMB_PortTimerModeType;



/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/

void eMB_PortEnterCriticalSection(void);
void eMB_PortExitCriticalSection(void);

bool (*eMB_PortEventInit)(void);
bool (*eMB_PortEventPost)(eMB_EventType eEvent);
bool (*eMB_PortEventGet)(eMB_EventType* eEvent);

void (*eMB_PortResourceInit)(void);
bool (*eMB_PortResourceTake)(void);
void (*eMB_PortResourceRelease)(void);

bool (*eMB_PortSerialInit)(void);
void (*eMB_PortSerialSetMode)(eMB_PortSerialModeType serialMode);
bool (*eMB_PortSerialGetByte)(uint8_t *data);
bool (*eMB_PortSerialPutByte)(uint8_t data);

bool (*eMB_PortTimersInit)(void);
void (*eMB_PortTimersEnable)(eMB_PortTimerModeType timerMode);
void (*eMB_PortTimersDisable)(void);



#ifdef __cplusplus
}
#endif

#endif /* EMB_PORT_H */
