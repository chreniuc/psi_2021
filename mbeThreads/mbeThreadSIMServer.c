/*------------------------------------------------------------------------------
 * Descriptin ..........
 *------------------------------------------------------------------------------
 * Name:    mbeThreadSIMServer.c
 * Purpose: implment cycle and USART communication for Modbus Extension - mbe
 *----------------------------------------------------------------------------*/

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS:Keil RTX5
#include "mbeThreadCycleb.h"
#include "Driver_USART.h"
//#include "cmsis_os.h"                   /* ARM::CMSIS:RTOS:Keil RTX */
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

extern ARM_DRIVER_USART Driver_USART5;
extern osThreadId_t tid_mbeThreadSIMServer;

TIM_HandleTypeDef TimHandle3;
uint32_t retValueSIM;
U32 startReceive = 0; 

/*------------------------------------------------------------------------------
  mbeThreadSIMServer - implement mbe cycle SIM SERVER 
 *----------------------------------------------------------------------------*/
 
__NO_RETURN void mbeThreadSIMServer(void *argument) 
{

 static ARM_DRIVER_USART* USARTdrv = &Driver_USART5;
 ARM_DRIVER_VERSION     version;
 ARM_USART_CAPABILITIES drv_capabilities;
 char                   cmd1 [256];
 
/*Initialize the USART driver */
  retValueSIM = USARTdrv->Initialize(myUSART_callbackU5);
/*Power up the USART peripheral */
 USARTdrv->PowerControl(ARM_POWER_FULL);
/*Configure the USART to 4800 Bits/sec */
 USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                         ARM_USART_DATA_BITS_8 |
                         ARM_USART_PARITY_NONE |
                         ARM_USART_STOP_BITS_1 |
                         ARM_USART_FLOW_CONTROL_NONE, 38400);
     
/* Enable Receiver and Transmitter lines */
 USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
 USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
  
 //TimHandle3.Instance = TIM4;
 //__HAL_TIM_CLEAR_IT(&TimHandle3, TIM_IT_UPDATE);
/* init TIM3 interrupt at 3,5 char */	
 //HAL_TIM_Base_Start_IT(&TimHandle3); 
  
  #ifdef DEBUG
    version = USARTdrv->GetVersion();
    if (version.api < 0x200)   /* requires at minimum API version 2.00 or higher */
    {                          /* error handling */
        return;
    }
    drv_capabilities = USARTdrv->GetCapabilities();
    if (drv_capabilities.event_tx_complete == 0)
    {                          /* error handling */
        return;
    }
  #endif
 
//retValueSIM = USARTdrv->Send("\nPress Enter to receive a message", 34);
//retValueSIM = osThreadFlagsWait (FLAG_TC_COMPLETE, osFlagsWaitAny,osWaitForever);
    startReceive = 1;  
    while (1)
    {
        //cmd1[0] = 0xAA;
        USARTdrv->Receive(&cmd1[0], 34);          /* Get byte from UART */
        osDelay (4);
        //retValueSIM = osThreadFlagsWait (FLAG_TIMER4_SLOT, osFlagsWaitAny,osWaitForever);
        if (cmd1[1] == 10)                       /* CR, send greeting  */
        {
          USARTdrv->Send("\nHello World!", 12);
          //osSignalWait(0x01, osWaitForever);
        }
        
 
    }
  }
  

 /* USART Driver */

 
void myUSART_callbackU5(uint32_t event)
{
  uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
  if (event & mask) {
    /* Success: Wakeup Thread */
    //osSignalSet(tid_myUART_Thread, 0x01);
  }
  if (event & ARM_USART_EVENT_TX_COMPLETE)
  {
    retValueSIM = osThreadFlagsSet(tid_mbeThreadSIMServer, FLAG_TC_COMPLETE);
  }    
  
  if (event & ARM_USART_EVENT_RX_TIMEOUT) {
 //   __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
  if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) {
//    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
}
  /**
  * @brief TIM MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param htim: TIM handle pointer
  * @retval None
  */
  


/// \brief TIM4_IRQHandler - interrupt for signlingb end of slot 
///        - reload counter with time for next slot
///        - clear the pending interrupt
///        - switch the RS485 driver direction to transmission if necessary
///          (32F746 can switch automatic but not and F407)            
/// \param[in]    NONE
/// \return[out]  NONE

void TIM3_IRQHandler(void)
{
  TimHandle3.Instance = TIM3;
  __HAL_TIM_CLEAR_IT(&TimHandle3, TIM_IT_UPDATE); /* achita intr.*/
  //HAL_TIM_Base_Stop_IT(&TimHandle3);
}

  
 