/**
  ******************************************************************************
  * @file    .../mbeHeader/mbeThreadCycleb.h 
  * @author  GVG 
  * @brief   Header for mbeThreadCycleb.c  module
  ******************************************************************************
  *  description
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MBETHREADCYCLEB_H
#define __MBETHREADCYCLEB_H

//#include "Driver_USART.h"
#include "cmsis_os2.h"                   /* ARM::CMSIS:RTOS:Keil RTX */
//#include <stdio.h>
//#include <string.h>

typedef unsigned          char U8;           /* uint8_t;  */
typedef unsigned short    int  U16;          /* uint16_t; */
typedef unsigned          int  U32;          /* uint32_t; */
typedef   signed          int  S32;          /* int32_t;  */

#define iTIM2               2
#define iTIM3               3
#define iTIM4               4
#define TIM_INIT            1
#define TIM_COUNTER         0
#define TIM_START_PRESCALER 1000000

#define cpuHCLK             168000000U        /* 168 Mhz    */


#define PG13_geenLED     0
#define PG14_redLED      1

/* mbeThreadCycleb flags definition */

#define FLAG_TC_COMPLETE     0x01
#define FLAG_TIMER4_SLOT     0x02

void myUSART_callbackU1(uint32_t event);
void myUSART_callbackU5(uint32_t event);
U8 HAL_TIM_Base_MspInitMy(U8 instance, U32 reloadCounter, U8 init, uint32_t prescaler);
void Error_Handler(void);

#endif /* _MBETHREADCYCLEB_H */

