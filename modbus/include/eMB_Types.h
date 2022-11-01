/* 
 * File:   eMB_Types.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_TYPES_H
#define EMB_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

typedef enum _eMB_ExceptionType
{
  eMB_EX_NONE                                     = 0x00,
  eMB_EX_ILLEGAL_FUNCTION                         = 0x01,
  eMB_EX_ILLEGAL_DATA_ADDRESS                     = 0x02,
  eMB_EX_ILLEGAL_DATA_VALUE                       = 0x03,
  eMB_EX_SLAVE_DEVICE_FAILURE                     = 0x04,
  eMB_EX_ACKNOWLEDGE                              = 0x05,
  eMB_EX_SLAVE_BUSY                               = 0x06,
  eMB_EX_MEMORY_PARITY_ERROR                      = 0x08,
  eMB_EX_GATEWAY_PATH_FAILED                      = 0x0A,
  eMB_EX_GATEWAY_TGT_FAILED                       = 0x0B
} eMB_ExceptionType;



#ifdef __cplusplus
}
#endif

#endif /* EMB_TYPES_H */
