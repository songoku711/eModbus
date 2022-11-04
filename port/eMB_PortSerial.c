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

#include "cmsis_os.h"
#include "main.h"
#include "usart.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define eMB_WEH_PORT_SERIAL_INSTANCE              huart4

#define eMB_WEH_PORT_SERIAL_TX_MODE()             { RS485_CS_GPIO_Port->ODR |= (uint32_t)RS485_CS_Pin; }
#define eMB_WEH_PORT_SERIAL_RX_MODE()             { RS485_CS_GPIO_Port->ODR &= (uint32_t)(~((uint32_t)RS485_CS_Pin)); }

/* Serial transmit event */
#define eMB_WEH_PORT_SERIAL_TXEVENT_START         (0x01)



/*===============================================================================================
*                                           VARIABLES
===============================================================================================*/

/* Serial hardware instance */
UART_HandleTypeDef* eMB_WEH_pUartIns;

/* Serial event */
static osEventFlagsId_t eMB_WEH_PortSerialTxEvent;

/* Definitions for eMB_WEH_PortSerialTxTask */
osThreadId_t eMB_WEH_PortSerialTxTaskHandle;
const osThreadAttr_t eMB_WEH_PortSerialTxTask_attributes = {
  .name = "eMBTxTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal7,
};

static uint8_t eMB_recvData;



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
  eMB_WEH_pUartIns = &eMB_WEH_PORT_SERIAL_INSTANCE;

  /* Disable serial interrupt and register interrupt callback */
  __HAL_UART_DISABLE_IT(eMB_WEH_pUartIns, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(eMB_WEH_pUartIns, UART_IT_TC);
  HAL_UART_RegisterCallback(eMB_WEH_pUartIns, HAL_UART_RX_COMPLETE_CB_ID, eMB_WEH_PortSerialRxCpltCallback);

  /* Create master transmitter event */
  eMB_WEH_PortSerialTxEvent = osEventFlagsNew(NULL);
  
  /* Create master transmitter task */
  eMB_WEH_PortSerialTxTaskHandle = osThreadNew(eMB_WEH_PortSerialTxTask, NULL, &eMB_WEH_PortSerialTxTask_attributes);
  
  eMB_WEH_PORT_SERIAL_RX_MODE();
  
  return true;
}

void eMB_WEH_PortSerialSetMode(eMB_PortSerialModeType serialMode)
{
  switch (serialMode)
  {
    case eMB_PORT_SERIAL_TX:
    {
      /* Switch 485 to transmit mode */
      eMB_WEH_PORT_SERIAL_TX_MODE();
      
      /* Disable RX interrupt */
      __HAL_UART_DISABLE_IT(eMB_WEH_pUartIns, UART_IT_RXNE);

      /* Start serial transmit */
      osEventFlagsSet(eMB_WEH_PortSerialTxEvent, eMB_WEH_PORT_SERIAL_TXEVENT_START);

      break;
    }
    case eMB_PORT_SERIAL_RX:
    {
      /* Enable RX interrupt */
      __HAL_UART_ENABLE_IT(eMB_WEH_pUartIns, UART_IT_RXNE);

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
  while (!(eMB_WEH_pUartIns->Instance->SR & UART_FLAG_TXE));
  eMB_WEH_pUartIns->Instance->DR = data;
  while (!(eMB_WEH_pUartIns->Instance->SR & UART_FLAG_TC));

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
  if (eMB_WEH_pUartIns->Instance->SR & UART_FLAG_RXNE)
  {
    eMB_recvData = eMB_WEH_pUartIns->Instance->DR & (uint8_t)0xFFU;

    /* execute modbus callback */
    eMB_Master_RTUFrameByteReceivedCallback();
  }
}



#ifdef __cplusplus
}
#endif
