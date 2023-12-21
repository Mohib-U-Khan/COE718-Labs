/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
 #include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include "RTE_Components.h"             // Component selection

int a, b, c;
  
/*----------------------------------------------------------------------------
	CMSIS RTX Priority Inversion Example
	Priority Inversion = leave commented lines commented
	Priority Elevation = uncomment the 2 commented lines
	Anita Tino
*----------------------------------------------------------------------------*/

void P1 (void const *argument);
void P2 (void const *argument);
void P3 (void const *argument);

osThreadDef(P1, osPriorityHigh, 1, 0);
osThreadDef(P2, osPriorityNormal, 1, 0);
osThreadDef(P3, osPriorityBelowNormal, 1, 0);

osThreadId t_main,t_P1,t_P2,t_P3;

void delay(){ 
	long k, count = 0;
	for(k = 0; k < 100000; k++){
					count++;
			}
	}

void P1 (void const *argument) {
	for (;;) 
	{
		for (a = 0 ; a < 10; a++) {
			delay();
		}
		osThreadSetPriority(t_P3, osPriorityHigh); //**solution uncomment
	
		osSignalSet(t_P3,0x01);						//call P3 to finish the task
		osSignalWait(0x02,osWaitForever);			//Error => priority inversion, P2 will run instead
		
		osThreadSetPriority(t_P3,osPriorityBelowNormal); //**solution uncomment
	}
}

void P2 (void const *argument) {
	for (;;) 	{
		for (b = 0 ; b < 20; b++) {
			delay();
		}
	}
}

void P3 (void const *argument) {
	for (;;) 	{
		for (c = 0; c < 10 ; c++){
			delay(); //do something
		}
		osSignalWait(0x01,osWaitForever);  
		osSignalSet(t_P1,0x02);	 
	}
}

int main(void)
{    
	osKernelInitialize (); 
	t_main = osThreadGetId ();
	osThreadSetPriority(t_main,osPriorityHigh);
	t_P3 = osThreadCreate(osThread(P3), NULL);
	
	 osDelay(500);

	t_P2 = osThreadCreate(osThread(P2), NULL);
	
	osDelay(100);
	t_P1 = osThreadCreate(osThread(P1), NULL);
	
	osThreadTerminate(t_main);
	 osKernelStart ();    
	for (;;) {}
}

