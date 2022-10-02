/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2022
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MB_PORT_H
#define _MB_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------- Defines ------------------------------------------*/
#include "mbtypes.h"



/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    EV_READY                                    = (0x01),   /*!< Startup finished. */
    EV_FRAME_RECEIVED                           = (0x02),   /*!< Frame received. */
    EV_EXECUTE                                  = (0x04),   /*!< Execute function. */
    EV_FRAME_SENT                               = (0x08),   /*!< Frame sent. */
} eMB_EventType;

typedef enum
{
    EV_ERROR_RESPOND_TIMEOUT,                               /*!< Slave respond timeout. */
    EV_ERROR_RECEIVE_DATA,                                  /*!< Receive frame data error. */
    EV_ERROR_EXECUTE_FUNCTION,                              /*!< Execute function error. */
} eMB_ErrorEventType;

/*! \ingroup modbus
 * \brief Data bits used for characters in serial mode.
 *
 * The data bits which should be applied to the characters sent over the serial
 * link. Please note that this values are actually passed to the porting
 * layer and therefore not all data bits modes might be available.
 */
typedef enum
{
    MB_DATA_7_BITS,                                         /*!< Data byte has 7 bits. */
    MB_DATA_8_BITS,                                         /*!< Data byte has 8 bits. */
    MB_DATA_9_BITS                                          /*!< Data byte has 9 bits. */
} eMB_DataBitsType;

/*! \ingroup modbus
 * \brief Parity used for characters in serial mode.
 *
 * The parity which should be applied to the characters sent over the serial
 * link. Please note that this values are actually passed to the porting
 * layer and therefore not all parity modes might be available.
 */
typedef enum
{
    MB_PAR_NONE,                                            /*!< No parity. */
    MB_PAR_ODD,                                             /*!< Odd parity. */
    MB_PAR_EVEN                                             /*!< Even parity. */
} eMB_ParityType;

typedef enum
{
    MB_DIR_TX,                                              /*!< Serial transmit direction. */
    MB_DIR_RX,                                              /*!< Serial receive direction. */
} eMB_PortSerialDirType;

/*! \ingroup modbus
 * \brief Structure for serial configuration.
 */
typedef struct
{
    (void *) hwInstance;                                    /*!< Pointer to external hw instance config structure. */
    uint32_t u32Baudrate;                                   /*!< Serial baudrate. */
    eMB_DataBitsType eDataBits;                             /*!< Serial data bits. */
    eMB_ParityType eParity;                                 /*!< Serial parity. */
} eMB_PortSerialCfgType;

/*! \ingroup modbus
 *  \brief TimerMode for Master.
 */
typedef enum
{
    MB_TMODE_T35,                                           /*!< Master receive frame T3.5 timeout. */
    MB_TMODE_RESPOND_TIMEOUT,                               /*!< Master waits respond for slave. */
    MB_TMODE_CONVERT_DELAY                                  /*!< Master sends broadcast, then delay sometime.*/
} eMB_PortTimerModeType;



/* ----------------------- Function prototypes ------------------------------*/
#if (defined MB_SLAVE_RTU_ENABLED) || (defined MB_SLAVE_ASCII_ENABLED)
bool eMB_PortEventInit(void);
bool eMB_PortEventPost(eMB_EventType eEvent);
bool eMB_PortEventGet(eMB_EventType* eEvent);

bool eMB_PortSerialInit(eMB_PortSerialCfgType *pserialCfg);
void eMB_PortSerialEnable(eMB_PortSerialDirType eDir);
void eMB_PortSerialDisable(void);
bool eMB_PortSerialGetByte(uint8_t *pu8Byte);
bool eMB_PortSerialPutByte(uint8_t u8Byte);

bool eMB_PortTimersInit(uint16_t usTimeOut50us);
void eMB_PortTimersEnable(void);
void eMB_PortTimersDisable(void);
#endif

#if (defined MB_MASTER_RTU_ENABLED) || (defined MB_MASTER_ASCII_ENABLED)
bool eMB_Master_PortEventInit(void);
bool eMB_Master_PortEventPost(eMB_EventType eEvent);
bool eMB_Master_PortEventGet(eMB_EventType* eEvent);

void eMB_Master_PortSemphrInit(void);
bool eMB_Master_PortSemphrTake(void);
void eMB_Master_PortSemphrRelease(void);

bool eMB_Master_PortSerialInit(eMB_PortSerialCfgType *pserialCfg);
void eMB_Master_PortSerialEnable(eMB_PortSerialDirType eDir);
void eMB_Master_PortSerialDisable(void);
bool eMB_Master_PortSerialGetByte(uint8_t *pucByte);
bool eMB_Master_PortSerialPutByte(uint8_t ucByte);

bool eMB_Master_PortTimersInit(uint16_t usTimeOut50us);
void eMB_Master_PortTimersEnable(eMB_PortTimerModeType timerMode);
void eMB_Master_PortTimersDisable(void);
#endif



/* ----------------- Callback for the master error process ------------------*/
void eMB_Master_RequestSuccessCbk(void);
void eMB_Master_RequestErrorCbk(void);



/* ----------------------- Callback for the protocol stack ------------------*/

/*!
 * \brief Callback function for the porting layer when a new byte is
 *   available.
 *
 * Depending upon the mode this callback function is used by the RTU or
 * ASCII transmission layers. In any case a call to xMBPortSerialGetByte()
 * must immediately return a new character.
 *
 * \return <code>TRUE</code> if a event was posted to the queue because
 *   a new byte was received. The port implementation should wake up the
 *   tasks which are currently blocked on the eventqueue.
 */
extern bool (*pxMBFrameCBByteReceived)(void);

extern bool (*pxMBFrameCBTransmitterEmpty)(void);

extern bool (*pxMBPortCBTimerExpired)(void);

extern bool (*pxMBMasterFrameCBByteReceived)(void);

extern bool (*pxMBMasterFrameCBTransmitterEmpty)(void);

extern bool (*pxMBMasterPortCBTimerExpired)(void);

/* ----------------------- TCP port functions -------------------------------*/
bool xMBTCPPortInit(USHORT usTCPPort);

void vMBTCPPortClose(void);

void vMBTCPPortDisable(void);

bool xMBTCPPortGetRequest(UCHAR **ppucMBTCPFrame, USHORT * usTCPLength);

bool xMBTCPPortSendResponse(const UCHAR *pucMBTCPFrame, USHORT usTCPLength);

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
