/* 
 * File:   eMB_CRC.h
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_CRC_H
#define EMB_CRC_H

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
*                                       FUNCTION PROTOTYPES
===============================================================================================*/

uint16_t eMB_GetCRC(uint8_t *pucFrame, uint16_t usLen);



#ifdef __cplusplus
}
#endif

#endif
