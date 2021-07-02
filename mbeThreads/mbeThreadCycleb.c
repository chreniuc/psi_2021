/*------------------------------------------------------------------------------
 * Descriptin ..........
 *------------------------------------------------------------------------------
 * Name:    mbeThreadCycle.c
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
#include "Board_LED.h"

extern ARM_DRIVER_USART Driver_USART1;
extern osThreadId_t tid_mbeThreadCycleb;

uint32_t retValue;
TIM_HandleTypeDef TimHandle;
U16 TickCounter[6] = {1000,1200,1600,1400,1200,1100}; 

/*------------------------------------------------------------------------------
  mbeThreadCycle - implement mbe cycle and USART communication
 *----------------------------------------------------------------------------*/
__NO_RETURN void mbeThreadCycleb(void *argument) 
{

 static ARM_DRIVER_USART* USARTdrv = &Driver_USART1;
 ARM_DRIVER_VERSION     version;
 ARM_USART_CAPABILITIES drv_capabilities;
 char                   cmd [256];
 extern U32 startReceive;
 
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
/*Initialize the TIMER driver */
  
  __TIM2_CLK_ENABLE();
  __TIM3_CLK_ENABLE();	
	__TIM4_CLK_ENABLE();
  
/* Configure the NVIC for TIMx, Set Interrupt Group Priority */ 
 HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);  // was 4,0 ??????????????????????????????????????????
 HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);  // was 4,0 ??????????????????????????????????????????
 HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);  // was 4,0 ??????????????????????????????????????????
/* init TIMER2 for timestamp */	
 HAL_TIM_Base_MspInitMy(4, 10, TIM_INIT, (uint32_t) ((cpuHCLK /1) / TIM_START_PRESCALER) - 1);
 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
/* init TIMER4 with lenght in time of the slot */
 HAL_TIM_Base_MspInitMy(2, 0xffff, TIM_INIT, (uint32_t) ((cpuHCLK /1) / 1000000) - 1);
 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
 /* prepare T3 for gernerate an interrpt in 3,5 char */  	 
 HAL_TIM_Base_MspInitMy(3, 100, 0, 0);
 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
 
 LED_Initialize ();
 LED_Off(PG13_geenLED);
	
/* Enable the TIMx global Interrupt */
 HAL_NVIC_EnableIRQ(TIM2_IRQn);
 HAL_NVIC_EnableIRQ(TIM3_IRQn);
 HAL_NVIC_EnableIRQ(TIM4_IRQn);	

  
  
/*Initialize the USART driver */
 retValue = USARTdrv->Initialize(myUSART_callbackU1);
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

 //osDelay (500);
 while (startReceive == 0)

{ osDelay (10);}
 //test:
 retValue = USARTdrv->Send("\nPress Enter to receive a message", 34);
 //retValue = osThreadFlagsWait (FLAG_TC_COMPLETE, osFlagsWaitAny,osWaitForever);

/* == TIMER4 slot time  */
 TimHandle.Instance = TIM4;
 TimHandle.Instance -> ARR = TickCounter[0];
 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
 HAL_TIM_Base_Start_IT(&TimHandle); /* Starts the TIM Base generation in interrupt mode */
 
 
    //osSignalWait(0x01, osWaitForever);
     
 //   goto test;
    while (1)
    {
        USARTdrv->Receive(&cmd, 12);          /* Get byte from UART */
        retValue = osThreadFlagsWait (FLAG_TIMER4_SLOT, osFlagsWaitAny,osWaitForever);
        if ((retValue & FLAG_TIMER4_SLOT)!=0) /* CR, send greeting  */
        {
          USARTdrv->Send("\nHello World!", 12);
          //osSignalWait(0x01, osWaitForever);
        }
 
    }
  }
  

 
/* USART Driver */

 
 
void myUSART_callbackU1(uint32_t event)
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
    retValue = osThreadFlagsSet(tid_mbeThreadCycleb, FLAG_TC_COMPLETE);
  }    
  
  if (event & ARM_USART_EVENT_RX_TIMEOUT) {
    //__breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
  if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) {
    //__breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
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
  
U8 HAL_TIM_Base_MspInitMy(U8 instance, uint32_t reloadCounter, U8 init, uint32_t prescaler)
{ 
 // TIM_HandleTypeDef TimHandle; 
	/* Set TIMx instance */
	switch (instance) {
		case 2:
	   TimHandle.Instance = TIM2;
		 break;
		case 3:
		 TimHandle.Instance = TIM3;
		 break;
		case 4:
		 TimHandle.Instance = TIM4;
		 break;
		default:
			return HAL_ERROR;
	}		
   
  /* Initialize TIMx peripheral as follow:
       + Period = 10000 - 1
       + Prescaler = ((SystemCoreClock/2)/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
if (init == TIM_INIT) {	
  TimHandle.Init.Period = reloadCounter - 1;
	switch(instance) {
	case 2:
	   TimHandle.Init.Prescaler = prescaler;
		 break;
		case 3:
		 TimHandle.Instance = TIM3;
		 TimHandle.Init.Prescaler = prescaler;
		 break;
		case 4:
		 TimHandle.Instance = TIM4;
		 TimHandle.Init.Prescaler = prescaler;
		 break;
		default:
			return HAL_ERROR;
	}
  //TimHandle.Init.Prescaler = (uint32_t) ((cpuHCLK /2) / 1000000) - 1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
} else {
 TimHandle.Init.Period = reloadCounter - 1;
} 

  if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
		return HAL_ERROR;
  }
//	HAL_TIM_Base_Start_IT(&TimHandle);
  return HAL_OK;
 }

/// \brief TIM4_IRQHandler - interrupt for signlingb end of slot 
///        - reload counter with time for next slot
///        - clear the pending interrupt
///        - switch the RS485 driver direction to transmission if necessary
///          (32F746 can switch automatic but not and F407)            
/// \param[in]    NONE
/// \return[out]  NONE

void TIM4_IRQHandler(void)
{
 uint32_t retVal;
 static U16 i;
 LED_On(PG13_geenLED);
 i++;
 if (i > 5) i=0;
/* An instance of TIM4  stucture          */
 TIM_HandleTypeDef TimHandle;
 TimHandle.Instance = TIM4;
 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
 //TimHandle.Instance = TIM4;
 TimHandle.Instance -> ARR = TickCounter [i];  
 retVal = osThreadFlagsSet(tid_mbeThreadCycleb, FLAG_TIMER4_SLOT);
 LED_Off(PG13_geenLED);
}

  
 