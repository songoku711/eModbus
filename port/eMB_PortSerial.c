/* 
 * File:   eMB_PortSerial.c
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

#include "usart.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define eMB_WEH_PORT_SERIAL_INSTANCE              &huart1

#define eMB_WEH_PORT_SERIAL_TX_MODE()             { RS485_CS_GPIO_Port->ODR |= (uint32_t)RS485_CS_Pin; }
#define eMB_WEH_PORT_SERIAL_RX_MODE()             { RS485_CS_GPIO_Port->ODR &= (uint32_t)(~((uint32_t)RS485_CS_Pin)); }

/* Serial transmit event */
#define eMB_WEH_PORT_SERIAL_TXEVENT_START         (0x01)



/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/

/* software simulation serial transmit IRQ handler thread */
static TaskHandle_t eMB_WEH_PortSerialTxTaskHandle = NULL;
/* serial event */
static osEventFlagsId_t eMB_WEH_PortSerialTxEvent;



/*===============================================================================================
*                                       FUNCTION PROTOTYPES
===============================================================================================*/

static void eMB_WEH_PortSerialTxTask(void *parameter);
static void eMB_WEH_PortSerialRxCpltCallback(struct __UART_HandleTypeDef *huart);



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

bool eMB_WEH_PortSerialInit(void)
{
  /* Disable serial interrupt and register interrupt callback */
  __HAL_UART_DISABLE_IT(eMB_WEH_PORT_SERIAL_INSTANCE, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(eMB_WEH_PORT_SERIAL_INSTANCE, UART_IT_TC);
  HAL_UART_RegisterCallback(eMB_WEH_PORT_SERIAL_INSTANCE, HAL_UART_RX_COMPLETE_CB_ID, eMB_WEH_PortSerialRxCpltCallback);

  /* Create master transmitter event */
  eMB_WEH_PortSerialTxEvent = osEventFlagsNew(NULL);
  
  /* Create master transmitter task */
  BaseType_t xReturn = xTaskCreate((TaskFunction_t)eMB_WEH_PortSerialTxTask,              /* Task entry function */
                                   (const char *)"WEH TX",                                /* Task name */
                                   (uint16_t)512,                                         /* Task stack size */
                                   (void *)NULL,                                          /* Task entry function parameters */
                                   (UBaseType_t)40,                                       /* Priority of task */
                                   (TaskHandle_t *)&eMB_WEH_PortSerialTxTaskHandle);      /* Task control block pointer */

  eMB_WEH_PORT_SERIAL_RX_MODE();
  
  return true;
}

void eMB_WEH_PortSerialSetMode(eMB_PortSerialModeType serialMode)
{
  switch (serialMode)
  {
    case eMB_WEH_PORT_SERIAL_TX:
    {
      /* Switch 485 to transmit mode */
      eMB_WEH_PORT_SERIAL_TX_MODE();
      
      /* Disable RX interrupt */
      __HAL_UART_DISABLE_IT(serial, UART_IT_RXNE);

      /* Start serial transmit */
      osEventFlagsSet(eMB_WEH_PortSerialTxEvent, eMB_WEH_PORT_SERIAL_TXEVENT_START);

      break;
    }
    case eMB_WEH_PORT_SERIAL_RX:
    {
      /* Enable RX interrupt */
      __HAL_UART_ENABLE_IT(serial, UART_IT_RXNE);

      /* Switch 485 to receive mode */
      eMB_WEH_PORT_SERIAL_RX_MODE();

      /* Stop serial transmit */
      osEventFlagsClear(eMB_WEH_PortSerialTxEvent, eMB_WEH_PORT_SERIAL_TXEVENT_START);

      break;
    }
    default:
      break;
  }
}

bool eMB_WEH_PortSerialPutByte(uint8_t data)
{
  while (!(serial->Instance->SR & UART_FLAG_TXE));
  serial->Instance->DR = data;
  while (!(serial->Instance->SR & UART_FLAG_TC));

  return true;
}

bool eMB_WEH_PortSerialGetByte(uint8_t *data)
{
  *data = eMB_recvData;

  return true;
}



/**
 * Software simulation serial transmit IRQ handler.
 *
 * @param parameter parameter
 */
static void eMB_WEH_PortSerialTxTask(void* parameter)
{
  while (1)
  {
    /* waiting for serial transmit start */
    osEventFlagsWait(eMB_WEH_PortSerialTxEvent, eMB_WEH_PORT_SERIAL_TXEVENT_START, osFlagsWaitAny | osFlagsNoClear, osWaitForever);

    /* execute modbus callback */
    eMB_Master_RTUFrameTransmitterEmptyCallback();
  }
}



/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void eMB_WEH_PortSerialRxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t ch;
  
  if (serial->Instance->SR & UART_FLAG_RXNE)
  {
    eMB_recvData = serial->Instance->DR & (uint8_t)0xFFU;

    /* execute modbus callback */
    eMB_Master_RTUFrameByteReceivedCallback();
  }
}



#ifdef __cplusplus
}
#endif
