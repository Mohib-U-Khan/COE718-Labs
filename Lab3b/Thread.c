
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <math.h>
#include "osObjects.h"                      									  // RTOS object definitions
#include <stdio.h>
#include <string.h>
#include "LPC17xx.h"
#include "LED.h"
#include "GLCD.h"

//#define __DEMO 0
#define __FI 1

/*----------------------------------------------------------------------------
 *      Sample threads
 *---------------------------------------------------------------------------*/
unsigned int a=0;
unsigned int resulta=0;
unsigned int b=0;
double resultb=0;
unsigned int c=0;
double resultc=0;
unsigned int d=0;
double resultd=0;
double power=0;
unsigned int e=0;
double resulte=0;

osMutexDef(LOGGER_MUTEX);
osMutexId logger_mutex;

double factorial(int n) {
    if (n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}
  
void Thread1 (void const *argument); // thread function
void Thread2 (void const *argument); // thread function
void Thread3 (void const *argument); // thread function
void Thread4 (void const *argument); // thread function
void Thread5 (void const *argument); // thread function


osThreadId tid_Thread; // thread id
osThreadDef (Thread1, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid2_Thread; // thread id
osThreadDef (Thread2, osPriorityBelowNormal, 1, 0);                   // thread object

osThreadId tid3_Thread; // thread id
osThreadDef (Thread3, osPriorityAboveNormal, 1, 0);                   // thread object

osThreadId tid4_Thread; // thread id
osThreadDef (Thread4, osPriorityNormal, 1, 0);                   // thread object

osThreadId tid5_Thread; // thread id
osThreadDef (Thread5, osPriorityBelowNormal, 1, 0);                   // thread object

int Init_Thread (void) {
	logger_mutex = osMutexCreate(osMutex(LOGGER_MUTEX));
  tid_Thread = osThreadCreate (osThread(Thread1), NULL);
	tid2_Thread = osThreadCreate (osThread(Thread2), NULL);
	tid3_Thread = osThreadCreate (osThread(Thread3), NULL);
	tid4_Thread = osThreadCreate (osThread(Thread4), NULL);
	tid5_Thread = osThreadCreate (osThread(Thread5), NULL);
  if(!tid_Thread) return(-1);
	if(!tid2_Thread) return(-1);
	if(!tid3_Thread) return(-1);
	if(!tid4_Thread) return(-1);
	if(!tid5_Thread) return(-1);
  
  return(0);
}


void Thread5 (void const *argument) {
	const double PI = 3.14159265358979323846;
	const double gr = (1 + sqrt(5)) / 2; // Golden ratio
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(3);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 5      ");
		LED_On(4);
		osMutexRelease(logger_mutex);
		#endif
	for (d = 1 ; d < 13; d++) {
		resulte += d * PI * gr * gr;
	}
}

void Thread4 (void const *argument) {
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(2);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 4      ");
		LED_On(3);
		osMutexRelease(logger_mutex);
		#endif
	resultd = 1;
	power = 5;
	for (d = 1 ; d < 6 ; d++) {
		resultd += power / factorial(d);
		power *= 5;
	}
}

void Thread3 (void const *argument) {
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(1);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 3      ");
		LED_On(2);
		osMutexRelease(logger_mutex);
		#endif
	  for(c = 1; c < 17; c++) {
			resultc += (double)(c+1) / c;
  }
}

void Thread2 (void const *argument) {
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(0);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 2      ");
		LED_On(1);
		osMutexRelease(logger_mutex);
		#endif
	  for(b = 1; b < 17; b++) {
			resultb += (1 << b) / factorial(b);
  }
}

void Thread1 (void const *argument) {
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(4);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 1      ");
		LED_On(0);
		osMutexRelease(logger_mutex);
		#endif
	 for(a = 0; a < 257; a++) {
		resulta += (a + (a+2));	
  }                                          
}
