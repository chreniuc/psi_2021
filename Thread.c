#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "cmsis_os.h"
#include "Board_Buttons.h"
#include "Board_LED.h"                  // ::Board Support:LED

#include <stdio.h>
#include <stdlib.h>


/*----------------------------------------------------------------------------
 *      Thread 1 'DataLoggerthread': Sample thread
 *---------------------------------------------------------------------------*/
 
extern void getTimestamp(char* timestamp);
 
osThreadId_t tid_DataLoggerThread;                        // thread id
 
void DataLoggerThread (void *argument);                   // thread function
 

void logButtonState(uint8_t button, uint8_t state) {
  FILE * f;
  const char ButName[][7] = {
    "1",
    "2"
  };
  const char ButState[][15] = {
    "Lasat liber",
    "Apasat"
  };
  f = fopen("buttonLog.txt", "a");
  if (f != NULL) {
		char* timestamp = (char*)malloc(sizeof(char) * 20);
		getTimestamp(timestamp);
    fprintf(f, "%s -Button %s %s\n", timestamp, ButName[button], ButState[state]);
    fclose(f);
		free(timestamp);
  }
} 
 

int Init_DataLoggerThread (void) {
 
  tid_DataLoggerThread = osThreadNew(DataLoggerThread, NULL, NULL);
  if (tid_DataLoggerThread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void DataLoggerThread (void *argument) {
  uint8_t but_current;
  uint8_t but_last;
  uint8_t but_changed;
  uint8_t but_num;
	
	while(1)
	{
		// Starea curenta a butoanelor
		but_current = (Buttons_GetState());
		// Care butoane s-au schimbat? XOr, raman cu 1 doar alea care sunt diferite
		but_changed = but_current ^ but_last;
		// Facem copie, deoarece vom modifica acest but_current, ca sa iteram prin butoane
		but_last = but_current;
		but_num = 0;
		while (but_changed) { // cat timp avem o stare diferita fata de cea anterioara
			if (but_changed & 1) {
				// S-a schimbat starea, logam evenimentul
				logButtonState(but_num, but_current & 1);
			}
			// mergem la urmatorul buton, facem o deplasare pe biti
			but_num++; 
			but_changed >>= 1;
			but_current >>= 1;
		}
		// It will suspend the execution of this thread until the specified signal flag is set.
		// Acest semnal va fi generate de catre timerul periodic.
		osSignalWait(0x01,osWaitForever);
	}
}
