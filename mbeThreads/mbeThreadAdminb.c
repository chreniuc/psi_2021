/*------------------------------------------------------------------------------
 * Descriptin ..........
 *------------------------------------------------------------------------------
 * Name:    mbeThreadAdminb.c
 * Purpose: implement the DISPATCHER for server request 
 *----------------------------------------------------------------------------*/

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS:Keil RTX5
#include "mbeThreadAdminb.h"

/*------------------------------------------------------------------------------
  mbeThreadCycle - implement mbe cycle and USART communication
 *----------------------------------------------------------------------------*/
__NO_RETURN void mbeThreadAdminb(void *argument) {
 
  while(1)
    {
      osDelay (500U); // Delay 500 ms
    }
}

