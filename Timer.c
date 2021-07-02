#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "cmsis_os.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Timer: Sample timer functions
 *---------------------------------------------------------------------------*/

// S&C
#include <stdio.h>
#include <stdlib.h>

extern osThreadId_t tid_DataLoggerThread;

struct clock {
  unsigned char hour;
  unsigned char min;
  unsigned char sec;
  unsigned short msec;
};
static struct clock time;
 
/*----- Periodic Timer Example -----*/
osTimerId_t tim_id2;                            // timer id
static uint32_t exec2;                          // argument for the timer call back function
 
// Periodic Timer Function
static void Timer2_Callback (void const *arg) {

  // Incrementeaza timpul de cand a pornit programul.	
	if (++time.msec == 100) {
  time.msec = 0;
  if (++time.sec == 60) {
    time.sec = 0;
    if (++time.min == 60) {
      time.min = 0;
      if (++time.hour == 24) {
        time.hour = 0;
      }
    }
   }
  }
	
	// trimite semnal catre thread
	osSignalSet(tid_DataLoggerThread,0x01);
}
 
// Example: Create and Start timers
int Init_Timers (void) {
  osStatus_t status;                            // function return status
 
  // Create periodic timer, every 10 milliseconds
  exec2 = 2U;
  tim_id2 = osTimerNew((osTimerFunc_t)&Timer2_Callback, osTimerPeriodic, &exec2, NULL);
  if (tim_id2 != NULL) {  // Periodic timer created
    // start timer with periodic 10ms interval
    status = osTimerStart(tim_id2, 10);            
    if (status != osOK) {
      return -1;
    }
  }
  return NULL;
}

// Timpul de cand a pornit aplicatia
void getTimestamp(char* timestamp) {
  sprintf(timestamp, "[%02d:%02d:%02d.%02d]", time.hour, time.min, time.sec, time.msec);
}
