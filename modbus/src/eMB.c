/* 
 * File:   eMB.c
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
#include "eMB_Utils.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/




/*===============================================================================================
*                                          VARIABLES
===============================================================================================*/

static enum
{
  STATE_NOT_INITIALIZED,
  STATE_DISABLED,
  STATE_ENABLED,
} eMB_gState = STATE_NOT_INITIALIZED;

/* Frame callout function pointer which are initialized in eMB_Init(). Depending on the
 * mode (RTU or ASCII) they are set to the correct implementations. */
eMB_FrameStartCallout                             eMB_FrameStartCalloutArr;
eMB_FrameStopCallout                              eMB_FrameStopCalloutArr;
eMB_FrameSendCallout                              eMB_FrameSendCalloutArr;
eMB_FrameReceiveCallout                           eMB_FrameReceiveCalloutArr;

eMB_FrameGetSlaveAddressCallout                   eMB_FrameGetSlaveAddressCalloutArr;
eMB_FrameSetSlaveAddressCallout                   eMB_FrameSetSlaveAddressCalloutArr;

eMB_FrameGetSendPduBufferCallout                  eMB_FrameGetSendPduBufferCalloutArr;
eMB_FrameSetSendPduLengthCallout                  eMB_FrameSetSendPduLengthCalloutArr;
eMB_FrameGetSendPduLengthCallout                  eMB_FrameGetSendPduLengthCalloutArr;

eMB_FrameIsBroadcastCallout                       eMB_FrameIsBroadcastCalloutArr;



extern eMB_ExceptionType eMB_FuncReportSlaveIDHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncReadInputRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncReadHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncWriteMultipleHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncWriteHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncReadWriteMultipleHoldingRegisterHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncReadCoilsHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncWriteSingleCoilHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncWriteMultipleCoilsHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);
extern eMB_ExceptionType eMB_Master_FuncReadDiscreteInputsHandler(uint8_t *recvPduFrame, uint16_t *recvPduLength);

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions. */
static eMB_FuncCallbackStruct eMB_FuncHandlerCfg[eMB_FUNC_HANDLERS_MAX] = 
{
#ifdef eMB_FUNC_OTHER_REP_SLAVEID_ENABLED
  { eMB_FUNC_OTHER_REPORT_SLAVEID,                eMB_FuncReportSlaveIDHandler                                },
#endif
#ifdef eMB_FUNC_READ_INPUT_ENABLED
  { eMB_FUNC_READ_INPUT_REGISTER,                 eMB_Master_FuncReadInputRegisterHandler                     },
#endif
#ifdef eMB_FUNC_READ_HOLDING_ENABLED
  { eMB_FUNC_READ_HOLDING_REGISTER,               eMB_Master_FuncReadHoldingRegisterHandler                   },
#endif
#ifdef eMB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED
  { eMB_FUNC_WRITE_MULTIPLE_REGISTERS,            eMB_Master_FuncWriteMultipleHoldingRegisterHandler          },
#endif
#ifdef eMB_FUNC_WRITE_HOLDING_ENABLED
  { eMB_FUNC_WRITE_REGISTER,                      eMB_Master_FuncWriteHoldingRegisterHandler                  },
#endif
#ifdef eMB_FUNC_READWRITE_HOLDING_ENABLED
  { eMB_FUNC_READWRITE_MULTIPLE_REGISTERS,        eMB_Master_FuncReadWriteMultipleHoldingRegisterHandler      },
#endif
#ifdef eMB_FUNC_READ_COILS_ENABLED
  { eMB_FUNC_READ_COILS,                          eMB_Master_FuncReadCoilsHandler                             },
#endif
#ifdef eMB_FUNC_WRITE_COIL_ENABLED
  { eMB_FUNC_WRITE_SINGLE_COIL,                   eMB_Master_FuncWriteSingleCoilHandler                       },
#endif
#ifdef eMB_FUNC_WRITE_MULTIPLE_COILS_ENABLED
  { eMB_FUNC_WRITE_MULTIPLE_COILS,                eMB_Master_FuncWriteMultipleCoilsHandler                    },
#endif
#ifdef eMB_FUNC_READ_DISCRETE_INPUTS_ENABLED
  { eMB_FUNC_READ_DISCRETE_INPUTS,                eMB_Master_FuncReadDiscreteInputsHandler                    },
#endif
};



/* Pointer to global Modbus configuration */
eMB_ConfigStruct *eMB_gConfigPtr;



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

eMB_ErrorCodeType eMB_Init(const eMB_ConfigStruct *config)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  if (config == NULL)
  {
    errStatus = eMB_EINVAL;
  }
  else
  {
    eMB_gConfigPtr = (eMB_ConfigStruct *)config;

    switch (eMB_gConfigPtr->comm)
    {
#ifdef eMB_MASTER_RTU_ENABLED
      case eMB_COMM_RTU:
      {
        eMB_FrameStartCalloutArr                  = eMB_Master_RTUStart;
        eMB_FrameStopCalloutArr                   = eMB_Master_RTUStop;
        eMB_FrameSendCalloutArr                   = eMB_Master_RTUSend;
        eMB_FrameReceiveCalloutArr                = eMB_Master_RTUReceive;

        eMB_FrameGetSlaveAddressCalloutArr        = eMB_Master_RTUGetSlaveAddress;
        eMB_FrameSetSlaveAddressCalloutArr        = eMB_Master_RTUSetSlaveAddress;

        eMB_FrameGetSendPduBufferCalloutArr       = eMB_Master_RTUGetSendPduBuffer;
        eMB_FrameSetSendPduLengthCalloutArr       = eMB_Master_RTUSetSendPduLength;
        eMB_FrameGetSendPduLengthCalloutArr       = eMB_Master_RTUGetSendPduLength;

        eMB_FrameIsBroadcastCalloutArr            = eMB_Master_RTUIsBroadcast;
        
        errStatus = eMB_Master_RTUInit();

        break;
      }
#endif
#ifdef eMB_MASTER_ASCII_ENABLED
      case eMB_COMM_ASCII:
      {
        eMB_FrameStartCalloutArr                  = eMB_Master_ASCIIStart;
        eMB_FrameStopCalloutArr                   = eMB_Master_ASCIIStop;
        eMB_FrameSendCalloutArr                   = eMB_Master_ASCIISend;
        eMB_FrameReceiveCalloutArr                = eMB_Master_ASCIIReceive;

        eMB_FrameGetSlaveAddressCalloutArr        = eMB_Master_ASCIIGetSlaveAddress;
        eMB_FrameSetSlaveAddressCalloutArr        = eMB_Master_ASCIISetSlaveAddress;

        eMB_FrameGetSendPduBufferCalloutArr       = eMB_Master_ASCIIGetSendPduBuffer;
        eMB_FrameSetSendPduLengthCalloutArr       = eMB_Master_ASCIISetSendPduLength;
        eMB_FrameGetSendPduLengthCalloutArr       = eMB_Master_ASCIIGetSendPduLength;

        eMB_FrameIsBroadcastCalloutArr            = eMB_Master_ASCIIIsBroadcast;

        errStatus = eMB_Master_ASCIIInit();

        break;
      }
#endif
      default:
      {
        errStatus = eMB_EINVAL;

        break;
      }
    }

    if (errStatus == eMB_ENOERR)
    {
      if (eMB_gConfigPtr->pPortEventInit() == false)
      {
        /* port dependent event module initalization failed. */
        errStatus = eMB_EPORTERR;
      }
      else
      {
        eMB_gState = STATE_DISABLED;
      }
      
      /* initialize the OS resource for modbus master. */
      eMB_gConfigPtr->pPortResourceInit();
    }
  }

  return errStatus;
}

eMB_ErrorCodeType eMB_Enable(void)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;

  if (eMB_gState == STATE_DISABLED)
  {
    /* Activate the protocol stack. */
    eMB_FrameStartCalloutArr();

    eMB_gState = STATE_ENABLED;
  }
  else
  {
    errStatus = eMB_EILLSTATE;
  }

  return errStatus;
}

eMB_ErrorCodeType eMB_Disable(void)
{
  eMB_ErrorCodeType errStatus;

  if (eMB_gState == STATE_ENABLED)
  {
    eMB_FrameStopCalloutArr();

    eMB_gState = STATE_DISABLED;
    errStatus = eMB_ENOERR;
  }
  else if (eMB_gState == STATE_DISABLED)
  {
    errStatus = eMB_ENOERR;
  }
  else
  {
    errStatus = eMB_EILLSTATE;
  }

  return errStatus;
}

void eMB_MainFunction(void)
{
  static uint8_t   *pduFrame;
  static uint8_t    slaveAddr;
  static uint8_t    funcCode;
  static uint16_t   pduLength;
  static eMB_ExceptionType exptStatus;

  uint8_t           funcCodeHdlrIdx , j;
  eMB_ErrorCodeType errStatus = eMB_ENOERR;
  eMB_EventType     eEvent;
  eMB_ErrorEventType errorType;

  /* Check if the protocol stack is ready. */
  if (eMB_gState != STATE_ENABLED)
  {
    return;
  }

  /* Check if there is a event available. If not return control to caller.
    * Otherwise we will handle the event. */
  if (eMB_gConfigPtr->pPortEventGet(&eEvent) == true)
  {
    switch (eEvent)
    {
      case eMB_EV_READY:
      {
        break;
      }
      case eMB_EV_FRAME_RECEIVED:
      {
        errStatus = eMB_FrameReceiveCalloutArr(&slaveAddr, &pduFrame, &pduLength);

        /* Check if the frame is for us. If not, send an error process event. */
        if ((errStatus == eMB_ENOERR) && (slaveAddr == eMB_FrameGetSlaveAddressCalloutArr()))
        {
          (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_EXECUTE);
        }
        else
        {
          eMB_Util_SetErrorEvent(eMB_EV_ERROR_RECEIVE_DATA);
          (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_ERROR);
        }

        break;
      }
      case eMB_EV_EXECUTE:
      {
        funcCode = pduFrame[eMB_PDU_FUNC_OFFSET];
        exptStatus = eMB_EX_ILLEGAL_FUNCTION;

        /* If receive frame has exception. The receive function code highest bit is 1. */
        if ((uint8_t)0U != (funcCode >> 7U))
        {
          exptStatus = (eMB_ExceptionType)pduFrame[eMB_PDU_DATA_OFFSET];
        }
        else
        {
          for (funcCodeHdlrIdx = (uint8_t)0U; funcCodeHdlrIdx < eMB_FUNC_HANDLERS_MAX; funcCodeHdlrIdx++)
          {
            /* No more function handlers registered. Abort. */
            if (eMB_FuncHandlerCfg[funcCodeHdlrIdx].funcCode == funcCode)
            {
              /* If master request is broadcast,
                * the master need execute function for all slave. */
              if (eMB_FrameIsBroadcastCalloutArr())
              {
                pduLength = eMB_FrameGetSendPduLengthCalloutArr();

                for (j = 1; j <= eMB_MASTER_TOTAL_SLAVE_NUM; j++)
                {
                  eMB_FrameSetSlaveAddressCalloutArr(j);
                  exptStatus = eMB_FuncHandlerCfg[funcCodeHdlrIdx].pFuncCbk(pduFrame, &pduLength);
                }
              }
              else
              {
                exptStatus = eMB_FuncHandlerCfg[funcCodeHdlrIdx].pFuncCbk(pduFrame, &pduLength);
              }

              break;
            }
            else if (eMB_FuncHandlerCfg[funcCodeHdlrIdx].funcCode == (uint8_t)0U)
            {
              break;
            }
            else
            {
              /* Do nothing. Continue looping */
            }
          }
        }

        /* If master has exception, Master will send error process. Otherwise the Master is idle.*/
        if (exptStatus != eMB_EX_NONE)
        {
          eMB_Util_SetErrorEvent(eMB_EV_ERROR_EXECUTE_FUNCTION);
          (void)eMB_gConfigPtr->pPortEventPost(eMB_EV_ERROR);
        }
        else
        {
          eMB_gConfigPtr->pPortResourceRelease();
        }

        break;
      }
      case eMB_EV_FRAME_SENT:
      {
        /* Master is busy now. */
        eMB_FrameGetSendPduBufferCalloutArr(&pduFrame);
        errStatus = eMB_FrameSendCalloutArr(eMB_FrameGetSlaveAddressCalloutArr(), pduFrame, eMB_FrameGetSendPduLengthCalloutArr());
        
        break;
      }
      case eMB_EV_ERROR:
      {
        /* Execute specified error process callback function. */
        errorType = eMB_Util_GetErrorEvent();

        eMB_gConfigPtr->pPortResourceRelease();

        break;
      }
      default:
        break;
    }
  }
}



#ifdef __cplusplus
}
#endif
