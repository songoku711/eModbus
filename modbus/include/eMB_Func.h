/* 
 * File:   eMB_Func.h
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_FUNC_H
#define EMB_FUNC_H

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

/* Modbus function code */
#define eMB_FUNC_NONE                             (  0 )
#define eMB_FUNC_READ_COILS                       (  1 )
#define eMB_FUNC_READ_DISCRETE_INPUTS             (  2 )
#define eMB_FUNC_WRITE_SINGLE_COIL                (  5 )
#define eMB_FUNC_WRITE_MULTIPLE_COILS             ( 15 )
#define eMB_FUNC_READ_HOLDING_REGISTER            (  3 )
#define eMB_FUNC_READ_INPUT_REGISTER              (  4 )
#define eMB_FUNC_WRITE_REGISTER                   (  6 )
#define eMB_FUNC_WRITE_MULTIPLE_REGISTERS         ( 16 )
#define eMB_FUNC_READWRITE_MULTIPLE_REGISTERS     ( 23 )
#define eMB_FUNC_DIAG_READ_EXCEPTION              (  7 )
#define eMB_FUNC_DIAG_DIAGNOSTIC                  (  8 )
#define eMB_FUNC_DIAG_GET_COM_EVENT_CNT           ( 11 )
#define eMB_FUNC_DIAG_GET_COM_EVENT_LOG           ( 12 )
#define eMB_FUNC_OTHER_REPORT_SLAVEID             ( 17 )
#define eMB_FUNC_ERROR                            (128 )



/* Modbus function Handler when received PDU from other nodes */
typedef eMB_ExceptionType (*eMB_FuncCallback)(uint8_t* data, uint16_t* length);

typedef struct _eMB_FuncCallbackStruct
{
  uint8_t                     funcCode;
  eMB_FuncCallback            pFuncCbk;
} eMB_FuncCallbackStruct;



/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/



#ifdef __cplusplus
}
#endif

#endif /* EMB_FUNC_H */
