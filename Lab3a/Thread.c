
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "osObjects.h"                      									  // RTOS object definitions
#include <stdio.h>
#include "LPC17xx.h"
#include "LED.h"
#include "GLCD.h"

#define __DEMO 0
#define __FI 1
osMutexDef(LOGGER_MUTEX);
osMutexId logger_mutex;

/*----------------------------------------------------------------------------
 *      Sample threads
 *---------------------------------------------------------------------------*/
unsigned int valueA = 1; // For incrementation
unsigned int valueB = 2; // For prime number generation
unsigned int valueC = 1; // For pattern-based counting
int directionC = 1;      // 1 for increasing, -1 for decreasing
unsigned int i;
unsigned int j;

void delay(int time) {
	int x = 0;
	while (x++ < (time*10000)) {}
}

int is_prime(unsigned int num) {
    if (num <= 1) return 0;
    for (i = 2; i <= num / 2; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}
  
void Thread1 (void const *argument); // thread function
void Thread2 (void const *argument); // thread function
void Thread3 (void const *argument); // thread function


osThreadId tid_Thread; // thread id
osThreadDef (Thread1, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid2_Thread; // thread id
osThreadDef (Thread2, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid3_Thread; // thread id
osThreadDef (Thread3, osPriorityNormal, 1, 0);                   // thread object

int Init_Thread (void) {
	logger_mutex = osMutexCreate(osMutex(LOGGER_MUTEX));
  tid_Thread = osThreadCreate (osThread(Thread1), NULL);
	tid2_Thread = osThreadCreate (osThread(Thread2), NULL);
	tid3_Thread = osThreadCreate (osThread(Thread3), NULL);
  if(!tid_Thread) return(-1);
  if(!tid2_Thread) return(-1);
	if(!tid3_Thread) return(-1);
  return(0);
}

void Thread3 (void const *argument) {
	  for(j = 0; j <500; j++) {
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(1);
		GLCD_DisplayString(6, 0, __FI, "Thread:   3    ");
		LED_On(2);
		osMutexRelease(logger_mutex);
		#endif
    valueA++;
		delay(50);
  }
}

void Thread2 (void const *argument) {
	  for(j = 0; j <500; j++) {
			#ifdef __DEMO
			osMutexWait(logger_mutex, osWaitForever);
			LED_Off(0);
			GLCD_DisplayString(6, 0, __FI, "Thread:  2     ");
			LED_On(1);
			osMutexRelease(logger_mutex);
			#endif
    valueB++;
        while(!is_prime(valueB)) {
            valueB++;
        }
		delay(50);
  }
}

void Thread1 (void const *argument) {
	 for(j = 0; j <500; j++) {
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(2);
		GLCD_DisplayString(6, 0, __FI, "Thread: 1      ");
		LED_On(0);
		osMutexRelease(logger_mutex);
		#endif
   if (valueC > 10 || valueC < 1) {
            directionC = -directionC;
        }
        valueC += directionC;
				delay(50);
  } 
}
