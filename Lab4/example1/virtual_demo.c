/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
 #include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include "LED.h"                  // ::Board Support:LED

  
void led_Thread1 (void const *argument); // thread function
void led_Thread2 (void const *argument); // thread function
void led_Thread3 (void const *argument);


osThreadDef (led_Thread1, osPriorityBelowNormal, 1, 0);                   // thread object
osThreadDef (led_Thread2, osPriorityNormal, 1, 0);                   // thread object
osThreadDef (led_Thread3, osPriorityAboveNormal, 1, 0);  

void delay(long k){ 
	int count = 0;
	for(k = 0; k < 100000; k++){
					count++;
			}
	}

/*###########################################################
	Virtual Timer declaration and call back method
############################################################*/


osThreadId T_led_ID1;
osThreadId T_led_ID2;	
osThreadId T_led_ID3;	

// Toggle the LED associated with the timer
void callback(void const *param){
	switch( (uint32_t) param){
		case 0:
			LED_On(5); LED_Off(6); LED_Off(7);
			osSignalSet	(T_led_ID1,0x01);
			delay(1000);
		  LED_Off(5); LED_Off(6); LED_Off(7);
			break;
		case 1:
			LED_Off(5); LED_On(6); LED_Off(7);
			osSignalSet	(T_led_ID2,0x02);
		  delay(1000);
		  LED_Off(5); LED_Off(6); LED_Off(7);
		  break;
		case 2:
			LED_Off(5); LED_Off(6); LED_On(7);
			osSignalSet	(T_led_ID3,0x03);
		  delay(1000);
		  LED_Off(5); LED_Off(6); LED_Off(7);
		  break;
	}
}
osTimerDef(timer0_handle, callback);
osTimerDef(timer1_handle, callback);
osTimerDef(timer2_handle, callback);

//#############################################################


/*#############################################################
  Flash LED 0, signal to thread 2, wait for 3 to finish
*#############################################################*/
void led_Thread1 (void const *argument) {
	for (;;) {
		osSignalWait (0x01,osWaitForever);
		LED_Off(0); LED_Off(1); LED_Off(2);	
		LED_On(0);
		int x = 0;
		while (x < 260) {
			delay(300000);
			x++;
		}
		LED_Off(0); 
	}
}

/*################################################################
  Flash LED 2, signal to thread 3, wait for thread 1 to finish
*################################################################*/
void led_Thread2 (void const *argument) {
	for (;;) 	{
		osSignalWait (0x02,osWaitForever);
		LED_Off(0); LED_Off(1); LED_Off(2);	
		LED_On(1);
    int x = 0;
		while (x < 130) {
			delay(300000);
			x++;
		}	
		LED_Off(1);
	}
}


/*################################################################
  Flash LED 4, signal to thread 1, wait for thread 2 to finish
*################################################################*/
void led_Thread3 (void const *argument){
	for (;;) 	{
		osSignalWait (0x03,osWaitForever);
		LED_Off(0); LED_Off(1); LED_Off(2);	
		LED_On(2); 
		int x = 0;
		while (x < 65) {
			delay(300000);
			x++;
		}
		LED_Off(2);
		
	}
}


/*###################################################################
  Create and start threads
 *###################################################################*/
int main (void) {
		osKernelInitialize (); 
	//Virtual timer create and start
	osTimerId timer_0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)0);	
	osTimerId timer_1 = osTimerCreate(osTimer(timer1_handle), osTimerPeriodic, (void *)1);	
	osTimerId timer_2 = osTimerCreate(osTimer(timer2_handle), osTimerPeriodic, (void *)2);
	
  LED_Init ();	
	osTimerStart(timer_0, 40000);	
	osTimerStart(timer_1, 40000);	
	osTimerStart(timer_2, 20000);
	
	//Signal and wait threads
	T_led_ID1 = osThreadCreate(osThread(led_Thread1), NULL);
	T_led_ID2 = osThreadCreate(osThread(led_Thread2), NULL);
  T_led_ID3 = osThreadCreate(osThread(led_Thread3), NULL);
	 osKernelStart ();      
	osDelay(osWaitForever);               						
	
	for (;;);
}
