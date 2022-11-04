/* 
 * File:   eMB_Frame.h
 * Author: Long
 *
 * Constants which defines the format of a modbus frame. The example is
 * shown for a Modbus RTU/ASCII frame. Note that the Modbus PDU is not
 * dependent on the underlying transport.
 *
 * <------------------------ MODBUS SERIAL LINE PDU (1) ------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+----------------------------+-------------+
 *  | Address   | Function Code | Data                       | CRC/LRC     |
 *  +-----------+---------------+----------------------------+-------------+
 *  |           |               |                                   |
 * (2)        (3/2')           (3')                                (4)
 *
 * (1)  ... eMB_SDU_SIZE_MAX    = 256
 * (2)  ... eMB_SDU_ADDR_OFFSET = 0
 * (3)  ... eMB_SDU_FUNC_OFFSET = 1
 * (4)  ... eMB_SDU_CRC_SIZE    = 2
 *
 * (1') ... eMB_PDU_SIZE_MAX    = 253
 * (2') ... eMB_PDU_FUNC_OFFSET = 0
 * (3') ... eMB_PDU_DATA_OFFSET = 1
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef EMB_FRAME_H
#define EMB_FRAME_H

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

/*! \brief Maximum size of a SDU. */
#define eMB_SDU_SIZE_MAX                          (256 )

/*! \brief Minimum size of a SDU. */
#define eMB_SDU_SIZE_MIN                          (  4 )

/*! \brief Offset of address in SDU. */
#define eMB_SDU_ADDR_OFFSET                       (  0 )

/*! \brief Offset of function code in SDU. */
#define eMB_SDU_FUNC_OFFSET                       (  1 )

/*! \brief Size for CRC/LRC in SDU. */
#define eMB_SDU_CRC_SIZE                          (  2 )



/*! \brief Maximum size of a PDU. */
#define eMB_PDU_SIZE_MAX                          (253 )

/*! \brief Minimum size of a PDU. */
#define eMB_PDU_SIZE_MIN                          (  1 )

/*! \brief Offset of function code in PDU. */
#define eMB_PDU_FUNC_OFFSET                       (  0 )

/*! \brief Offset for response data in PDU. */
#define eMB_PDU_DATA_OFFSET                       (  1 )



/* Modbus address */
#define eMB_ADDRESS_BROADCAST                     (  0 )              /*! Modbus broadcast address. */
#define eMB_ADDRESS_MIN                           (  1 )              /*! Smallest possible slave address. */
#define eMB_ADDRESS_MAX                           (247 )              /*! Biggest possible slave address. */



/*===============================================================================================
*                                   FUNCTION PROTOTYPES
===============================================================================================*/

typedef void (*eMB_FrameStartCallout)(void);
typedef void (*eMB_FrameStopCallout)(void);
typedef eMB_ErrorCodeType (*eMB_FrameSendCallout)(uint8_t ucSlaveAddress, const uint8_t *pucFrame, uint16_t usLength);
typedef eMB_ErrorCodeType (*eMB_FrameReceiveCallout)(uint8_t *pucRcvAddress, uint8_t **pucFrame, uint16_t *pusLength);

typedef uint8_t (*eMB_FrameGetSlaveAddressCallout)(void);
typedef void (*eMB_FrameSetSlaveAddressCallout)(uint8_t address);

typedef void (*eMB_FrameGetSendPduBufferCallout)(uint8_t **pucFrame);
typedef void (*eMB_FrameSetSendPduLengthCallout)(uint16_t length);
typedef uint16_t (*eMB_FrameGetSendPduLengthCallout)(void);

typedef bool (*eMB_FrameIsBroadcastCallout)(void);



#ifdef __cplusplus
}
#endif

#endif /* EMB_FRAME_H */
