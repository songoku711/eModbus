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

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"

#include "string.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

typedef enum _eMB_EventType
{
  eMB_EV_READY                                    = (0x01),           /*!< Startup finished. */
  eMB_EV_FRAME_RECEIVED                           = (0x02),           /*!< Frame received. */
  eMB_EV_EXECUTE                                  = (0x04),           /*!< Execute function. */
  eMB_EV_FRAME_SENT                               = (0x08),           /*!< Frame sent. */
  eMB_EV_ERROR                                    = (0x80),           /*!< Frame error. */
  eMB_EV_ALL                                      = (0xFF)            /*!< All events. */
} eMB_EventType;

typedef enum _eMB_ErrorEventType
{
  eMB_EV_ERROR_RESPOND_TIMEOUT,                                       /*!< Slave respond timeout. */
  eMB_EV_ERROR_RECEIVE_DATA,                                          /*!< Receive frame data error. */
  eMB_EV_ERROR_EXECUTE_FUNCTION,                                      /*!< Execute function error. */
} eMB_ErrorEventType;

/*! \ingroup modbus
 * \brief Error codes used by all function in the protocol stack.
 */
typedef enum _eMB_ErrorCodeType
{
  eMB_ENOERR,                                                         /*!< no error. */
  eMB_ENOREG,                                                         /*!< illegal register address. */
  eMB_EINVAL,                                                         /*!< illegal argument. */
  eMB_EPORTERR,                                                       /*!< porting layer error. */
  eMB_ENORES,                                                         /*!< insufficient resources. */
  eMB_EIO,                                                            /*!< I/O error. */
  eMB_EILLSTATE,                                                      /*!< protocol stack in illegal state. */
  eMB_EBUSY,                                                          /*!< busy. */
  eMB_ETIMEDOUT                                                       /*!< timeout error occurred. */
} eMB_ErrorCodeType;

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
