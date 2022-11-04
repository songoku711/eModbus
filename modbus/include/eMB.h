/* 
 * File:   eMB.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_H
#define EMB_H

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "eMB_Cfg.h"

#include "eMB_Frame.h"
#include "eMB_Func.h"
#include "eMB_Port.h"
#include "eMB_CRC.h"

#ifdef eMB_MASTER_RTU_ENABLED
#include "eMB_RTU.h"
#endif

#ifdef eMB_MASTER_ASCII_ENABLED
#include "eMB_ASCII.h"
#endif

#ifdef eMB_MASTER_TCP_ENABLED
#include "eMB_TCP.h"
#endif



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/*! \ingroup modbus
 * \brief Modbus role types (Master/Slave).
 *
 */
typedef enum _eMB_RoleType
{
  eMB_ROLE_MASTER,                                /*!< RTU transmission mode. */
  eMB_ROLE_SLAVE,                                 /*!< ASCII transmission mode. */
} eMB_RoleType;

/*! \ingroup modbus
 * \brief Modbus communication types (TCP/RTU/ASCII).
 *
 * Modbus supports 3 transmission types. Either ASCII or RTU, or TCP. RTU
 * is faster but has more hardware requirements and requires a network with
 * a low jitter. ASCII is slower and more reliable on slower links (E.g. modems).
 * TCP for Ethernet connection and currently not support.
 */
typedef enum _eMB_CommType
{
  eMB_COMM_RTU,                                   /*!< RTU transmission mode. */
  eMB_COMM_ASCII,                                 /*!< ASCII transmission mode. */
  eMB_COMM_TCP                                    /*!< TCP mode. */
} eMB_CommType;



typedef struct _eMB_ConfigStruct
{
  eMB_RoleType                role;
  eMB_CommType                comm;
  /* Port event function pointer */
  eMB_PortEventInit           pPortEventInit;
  eMB_PortEventPost           pPortEventPost;
  eMB_PortEventGet            pPortEventGet;
  /* Port resource function pointer */
  eMB_PortResourceInit        pPortResourceInit;
  eMB_PortResourceTake        pPortResourceTake;
  eMB_PortResourceRelease     pPortResourceRelease;
  /* Port serial function pointer */
  eMB_PortSerialInit          pPortSerialInit;
  eMB_PortSerialSetMode       pPortSerialSetMode;
  eMB_PortSerialGetByte       pPortSerialGetByte;
  eMB_PortSerialPutByte       pPortSerialPutByte;
  /* Port timer function pointer */
  eMB_PortTimersInit          pPortTimersInit;
  eMB_PortTimersEnable        pPortTimersEnable;
  eMB_PortTimersDisable       pPortTimersDisable;
} eMB_ConfigStruct;



/*===============================================================================================
*                                          VARIABLES
===============================================================================================*/




/*===============================================================================================
*                                   FUNCTION PROTOTYPES
===============================================================================================*/

/*! \ingroup modbus
 * \brief Initialize the Modbus Master protocol stack.
 *
 * This functions initializes the ASCII or RTU module and calls the
 * init functions of the porting layer to prepare the hardware. Please
 * note that the receiver is still disabled and no Modbus frames are
 * processed until eMBMasterEnable( ) has been called.
 *
 * \param config  Pointer to Modbus configuration structure
 *
 * \return If no error occurs the function returns eMB_ErrorCodeType::MB_ENOERR.
 *   The protocol is then in the disabled state and ready for activation
 *   by calling eMBMasterEnable( ). Otherwise one of the following error codes
 *   is returned:
 *    - eMB_ErrorCodeType::MB_EPORTERR IF the porting layer returned an error.
 */
eMB_ErrorCodeType eMB_Init(const eMB_ConfigStruct *config);

/*! \ingroup modbus
 * \brief Enable the Modbus Master protocol stack.
 *
 * This function enables processing of Modbus Master frames. Enabling the protocol
 * stack is only possible if it is in the disabled state.
 *
 * \return If the protocol stack is now in the state enabled it returns
 *   eMB_ErrorCodeType::MB_ENOERR. If it was not in the disabled state it
 *   return eMB_ErrorCodeType::MB_EILLSTATE.
 */
eMB_ErrorCodeType eMB_Enable(void);

/*! \ingroup modbus
 * \brief Disable the Modbus Master protocol stack.
 *
 * This function disables processing of Modbus frames.
 *
 * \return If the protocol stack has been disabled it returns
 *  eMB_ErrorCodeType::MB_ENOERR. If it was not in the enabled state it returns
 *  eMB_ErrorCodeType::MB_EILLSTATE.
 */
eMB_ErrorCodeType eMB_Disable(void);

/*! \ingroup modbus
 * \brief The main pooling loop of the Modbus Master protocol stack.
 *
 * This function must be called periodically. The timer interval required
 * is given by the application dependent Modbus slave timeout. Internally the
 * function calls xMBMasterPortEventGet() and waits for an event from the receiver or
 * transmitter state machines.
 *
 * \return If the protocol stack is not in the enabled state the function
 *   returns eMB_ErrorCodeType::MB_EILLSTATE. Otherwise it returns
 *   eMB_ErrorCodeType::MB_ENOERR.
 */
void eMB_MainFunction(void);



/*! \ingroup modbus
 *\brief These Modbus functions are called for user when Modbus run in Master Mode.
 */
#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_READ_COILS_ENABLED
eMB_ErrorCodeType eMB_Master_RequestReadCoils
(
  uint8_t  slaveAddr,
  uint16_t coilAddr,
  uint16_t coilNum
);

eMB_ErrorCodeType eMB_Master_RequestWriteSingleCoil
(
  uint8_t  slaveAddr,
  uint16_t coilAddr,
  uint16_t coilData
);

eMB_ErrorCodeType eMB_Master_RequestWriteMultipleCoils
(
  uint8_t  slaveAddr,
  uint16_t coilAddr,
  uint16_t coilNum,
  uint8_t *coilData
);
#endif
#endif

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_READ_DISCRETE_INPUTS_ENABLED
eMB_ErrorCodeType eMB_Master_RequestReadDiscreteInputs
(
  uint8_t  slaveAddr,
  uint16_t disInputAddr,
  uint16_t disInputNum
);
#endif
#endif

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_WRITE_HOLDING_ENABLED
eMB_ErrorCodeType eMB_Master_RequestWriteHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingAddr,
  uint16_t holdingData
);

eMB_ErrorCodeType eMB_Master_RequestWriteMultipleHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingAddr,
  uint16_t holdingNum,
  uint16_t *holdingData
);

eMB_ErrorCodeType eMB_Master_RequestReadHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingAddr,
  uint16_t holdingNum
);

eMB_ErrorCodeType eMB_Master_RequestReadWriteMultipleHoldingRegister
(
  uint8_t  slaveAddr,
  uint16_t holdingReadAddr,
  uint16_t holdingReadNum,
  uint16_t *holdingData,
  uint16_t holdingWriteAddr,
  uint16_t holdingWriteNum
);
#endif
#endif

#if (defined eMB_MASTER_RTU_ENABLED) || (defined eMB_MASTER_ASCII_ENABLED)
#ifdef eMB_FUNC_READ_INPUT_ENABLED
eMB_ErrorCodeType eMB_Master_RequestReadInputRegister
(
  uint8_t  slaveAddr,
  uint16_t inputAddr,
  uint16_t inputNum
);
#endif
#endif



#ifdef __cplusplus
}
#endif

#endif
