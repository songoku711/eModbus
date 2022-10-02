/* 
 * File:   mbconfig.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef MB_CONFIG_H
#define MB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "mbtypes.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/*! \defgroup modbus_cfg Modbus Configuration
 *
 * Most modules in the protocol stack are completly optional and can be
 * excluded. This is specially important if target resources are very small
 * and program memory space should be saved.<br>
 *
 * All of these settings are available in the file <code>mbconfig.h</code>
 */
/*! \addtogroup modbus_cfg
 *  @{
 */
/*! \brief If Modbus Master ASCII support is enabled. */
// #define MB_MASTER_ASCII_ENABLED
/*! \brief If Modbus Master RTU support is enabled. */
#define MB_MASTER_RTU_ENABLED
/*! \brief If Modbus Master TCP support is enabled. */
// #define MB_MASTER_TCP_ENABLED

/*! \brief If Modbus Slave ASCII support is enabled. */
// #define MB_SLAVE_ASCII_ENABLED
/*! \brief If Modbus Slave RTU support is enabled. */
// #define MB_SLAVE_RTU_ENABLED
/*! \brief If Modbus Slave TCP support is enabled. */
// #define MB_SLAVE_TCP_ENABLED



/*! \brief If the <em>Report Slave ID</em> function should be enabled. */
#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED
/*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MB_FUNC_READ_INPUT_ENABLED
/*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MB_FUNC_READ_HOLDING_ENABLED
/*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MB_FUNC_WRITE_HOLDING_ENABLED
/*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED
/*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MB_FUNC_READ_COILS_ENABLED
/*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_COIL_ENABLED
/*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED
/*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED
/*! \brief If the <em>Read/Write Multiple Registers</em> function should be enabled. */
#define MB_FUNC_READWRITE_HOLDING_ENABLED
/*! @} */



#if (defined MB_MASTER_ASCII_ENABLED) || (defined MB_SLAVE_ASCII_ENABLED)
/*! \brief The character timeout value for Modbus ASCII.
 *
 * The character timeout value is not fixed for Modbus ASCII and is therefore
 * a configuration option. It should be set to the maximum expected delay
 * time of the network.
 */
#define MB_ASCII_TIMEOUT_SEC                                          (  1 )
#endif



/*! \brief Maximum number of Modbus functions codes the protocol stack
 *    should support.
 *
 * The maximum number of supported Modbus functions must be greater than
 * the sum of all enabled functions in this file and custom function
 * handlers. If set to small adding more functions will fail.
 */
#define MB_FUNC_HANDLERS_MAX                                          ( 16 )



/*! \brief Number of bytes which should be allocated for the <em>Report Slave ID
 *    </em>command.
 *
 * This number limits the maximum size of the additional segment in the
 * report slave id function. See eMBSetSlaveID(  ) for more information on
 * how to set this value. It is only used if MB_FUNC_OTHER_REP_SLAVEID_ENABLED
 * is set to <code>1</code>.
 */
#define MB_FUNC_OTHER_REP_SLAVEID_BUF                                 ( 32 )



#if (defined MB_MASTER_RTU_ENABLED) || (defined MB_MASTER_ASCII_ENABLED)
/*! \brief If master send a broadcast frame,the master will wait time of convert to delay,
 * then master can send other frame */
#define MB_MASTER_DELAY_MS_CONVERT                                    (200 )

/*! \brief If master send a frame which is not broadcast,the master will wait sometime for slave.
 * And if slave is not respond in this time,the master will process this timeout error.
 * Then master can send other frame */
#define MB_MASTER_TIMEOUT_MS_RESPOND                                  (100 )

/*! \brief The total slaves in Modbus Master system. Default 16.
 * \note : The slave ID must be continuous from 1.*/
#define MB_MASTER_TOTAL_SLAVE_NUM                                     ( 16 )
#endif



#if (defined MB_MASTER_TCP_ENABLED)
/*! \brief Use the default Modbus Master TCP port (502) */
#define MB_MASTER_TCP_PORT_USE_DEFAULT                                (  0 )
#endif



#ifdef __cplusplus
}
#endif

#endif /* MB_CONFIG_H */
