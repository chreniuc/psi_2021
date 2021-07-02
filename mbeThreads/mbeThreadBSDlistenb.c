/*------------------------------------------------------------------------------
 * Descriptin ..........
 *------------------------------------------------------------------------------
 * Name:    mbeThreadBSDListen.c
 * Purpose: implement the DISPATCHER for server request 
 *----------------------------------------------------------------------------*/

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS:Keil RTX5
#include "mbeThreadBSDlistenb.h"

/*------------------------------------------------------------------------------
  mbeThreadCycle - implement mbe cycle and USART communication
 *----------------------------------------------------------------------------*/
__NO_RETURN void mbeThreadBSDListenb (void *argument) {
 
  while(1)
    {
      osDelay (500U); // Delay 500 ms
    }
  }
  
