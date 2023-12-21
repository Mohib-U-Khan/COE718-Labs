
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

// Bit Band Macros used to calculate the alias address at run time
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) 	ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 |(((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))
	
volatile unsigned long * bit;

int a=0; 
int b=1; 
int c=0;

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/
volatile unsigned int memory_access_counter = 0;
volatile unsigned int cpu_mgmt_access_counter = 0;
volatile unsigned int app_interface_counter = 0;
volatile unsigned int device_mgmt_counter = 0;
volatile unsigned int num_of_users = 0;
char logger[100];
osMutexDef(LOGGER_MUTEX);
osMutexId logger_mutex;
  
void Thread1 (void const *argument); // thread function
void Thread2 (void const *argument); // thread function
void Thread3 (void const *argument); // thread function
void Thread4 (void const *argument); // thread function
void Thread5 (void const *argument); // thread function


osThreadId tid_Thread; // thread id
osThreadDef (Thread1, osPriorityAboveNormal, 1, 0);                   // thread object

osThreadId tid2_Thread; // thread id
osThreadDef (Thread2, osPriorityAboveNormal, 1, 0);                   // thread object

osThreadId tid3_Thread; // thread id
osThreadDef (Thread3, osPriorityNormal, 1, 0);                   // thread object

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

void Thread5 (void const *argument) { // User Interface
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(3);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 5      ");
		LED_On(4);
		osMutexRelease(logger_mutex);
		#endif
			num_of_users++;
			osDelay(1);
			osThreadTerminate(tid5_Thread);
}

void Thread4 (void const *argument) { // Device Management
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(2);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 4      ");
		LED_On(3);
		osMutexRelease(logger_mutex);
		#endif
			osSignalWait(0x03, osWaitForever); // Wait for signal from Application Interface
      strcat(logger, "Ending from Device Management.");
      osSignalSet(tid3_Thread, 0x04); // Signal Application Interface
			//osThreadYield();
      device_mgmt_counter++;
      osDelay(1);
      osThreadTerminate(tid4_Thread);
}

void Thread3 (void const *argument) { // Application Interface
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(1);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 3      ");
		LED_On(2);
		osMutexRelease(logger_mutex);
		#endif
			osMutexWait(logger_mutex, osWaitForever);
      sprintf(logger, "Partial message from Application Interface: ");
      osSignalSet(tid4_Thread, 0x03); // Signal Device Management
			//osThreadYield();
      osSignalWait(0x04, osWaitForever); // Wait for signal from Device Management
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(1);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 3      ");
		LED_On(2);
		osMutexRelease(logger_mutex);
		#endif
      app_interface_counter++;
      osDelay(1);
      osThreadTerminate(tid3_Thread);
}

void Thread2 (void const *argument) { // CPU Management
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(0);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 2      ");
		LED_On(1);
		osMutexRelease(logger_mutex);
		#endif
			cpu_mgmt_access_counter++;
			// Conditional Execution and barrel shifting scenario
			while (c <= 3) {
				if (a > 10) {
					a--;
				}
				else {
					a++;
				}
				b = a + (b*2);
				c++;
			}
			c = 0;
      osSignalSet(tid_Thread, 0x02); // Signal Memory Management
      osThreadTerminate(tid2_Thread);
}

void Thread1 (void const *argument) { // Memory Management
		#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(4);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 1      ");
		LED_On(0);
		osMutexRelease(logger_mutex);
		#endif
		 memory_access_counter++;
     bit = &BitBand(&LPC_GPIO2->FIOPIN, 2);
     osSignalWait(0x02, osWaitForever); // Wait for signal from CPU Management
	#ifdef __DEMO
		osMutexWait(logger_mutex, osWaitForever);
		LED_Off(4);
		GLCD_DisplayString(6, 0, __FI, (unsigned char *)"Thread: 1      ");
		LED_On(0);
		osMutexRelease(logger_mutex);
		#endif
     osDelay(1);
     osThreadTerminate(tid_Thread);
}
