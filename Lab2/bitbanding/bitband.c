#include "LPC17xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"

#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0										/* Uncomment to use the LCD */

//------- ITM Stimulus Port definitions for printf ------------------- //
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}

volatile unsigned long msTicks = 0; // Variable for SysTick Interrupt

// SysTick Handler (Interrupt Service Routine for the SysTick interrupt)
void SysTick_Handler(void) {
    msTicks++;
}

// Initialize SysTick
void SysTick_Initialize(unsigned long ticks) {
    SysTick->LOAD = (ticks & 0xFFFFFF) - 1;  // Set reload register
    SysTick->VAL   = 0; 										// Reset the SysTick counter value
    SysTick->CTRL  = 0x07; 									// Enable SysTick IRQ and SysTick Timer
}

// Delay using SysTick timer
void delay_ms(unsigned long delayTicks) {
    unsigned long currentTicks;
    currentTicks = msTicks; // Save current ticks
    while ((msTicks - currentTicks) < delayTicks); // Wait until required time passes
}
//------------------------------------------------------------------- //

// Bit Band Macros used to calculate the alias address at run time
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) 	ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 |(((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))

volatile unsigned long * bit1;
volatile unsigned long * bit2;
//#define ADCR_Bit24   (*((volatile unsigned long *)0x42680060))
#define FIOPIN_Bit28 (*((volatile unsigned long *)0x233806F0))
#define FIOPIN_Bit02 (*((volatile unsigned long *)0x23380A88))
	
#define BASE_DELAY 			1000
#define BASE_ADD 				500
int currentShift = 0;
int shiftDirection = 1;
char buffer[10];
char buffer1[10];
char barshift[15] = "1000 + 500x";

int main(void){
	uint32_t joystick_val;
	SysTick_Initialize(100000); 
	LED_Init();
	KBD_Init();
	
#ifdef __USE_LCD
  GLCD_Init();                               /* Initialize graphical LCD (if enabled */

  GLCD_Clear(White);                         /* Clear graphical LCD display   */
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, "     COE718 Demo    ");
	GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, "       bitband.c     ");
  //GLCD_DisplayString(2, 0, __FI, "  Turn pot for LEDs ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  //GLCD_DisplayString(6, 0, __FI, "AD value:            ");
	GLCD_DisplayString(3, 0, __FI, "Delay (ms):        ");
	GLCD_DisplayString(4, 0, __FI, "1000 + 500x0        ");
	char base_buffer[10];
	snprintf(base_buffer, sizeof(base_buffer), "%d", BASE_DELAY);
	GLCD_DisplayString(3, 11, __FI, (unsigned char *)base_buffer);
	GLCD_DisplayString(6, 0, __FI, "Mode:        ");
	//GLCD_DisplayString(6, 10, __FI, (unsigned char *)"CENTER    ");
#endif
	bool led_switch = false;

while (1) {
	joystick_val = get_button();
//mask mode
	//printf("\nMask mode\n");
  //LPC_ADC->ADCR |=  ( 1 << 24);              // start conversion    
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);
  //LPC_ADC->ADCR &= ~( 7 << 24);              // stop conversion     
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);
	printf("\nMask Method\n");	
#ifdef __USE_LCD
	GLCD_DisplayString(6, 6, __FI, (unsigned char *)"MASK              ");
#endif
	//'s' suffix conditional execution method
	if (led_switch) {
		LPC_GPIO1->FIOPIN &=  ~(1UL<< 28);
		LPC_GPIO2->FIOPIN &=  ~(1UL<< 2);	
		led_switch = false;
		delay_ms(BASE_DELAY + (BASE_ADD*currentShift));  // 1 second delay
	}
	else {
		LPC_GPIO1->FIOPIN |=  (1UL<< 28);
		LPC_GPIO2->FIOPIN |=  (1UL<< 2);	
		led_switch = true;		
		delay_ms(BASE_DELAY + (BASE_ADD*currentShift));  // 1 second delay
	}
		
	if (joystick_val != KBD_MASK) {	
		switch(joystick_val){
				case KBD_UP:
					if (shiftDirection < 0) {
						shiftDirection *= -1;
					}
					currentShift += shiftDirection;
					snprintf(buffer, sizeof(buffer), "%d", BASE_DELAY + (BASE_ADD*currentShift));
					snprintf(buffer1, sizeof(buffer1), "%d", currentShift);
					GLCD_DisplayString(3, 11, __FI, (unsigned char *)buffer);
					GLCD_DisplayString(4, 11, __FI, (unsigned char *)strcat(buffer1, " "));
					break;
				case KBD_DOWN:
					if (shiftDirection > 0) {
						shiftDirection *= -1;
					}
					if (currentShift > -1 ) {
					currentShift += shiftDirection;
					}
					snprintf(buffer, sizeof(buffer), "%d", BASE_DELAY + (BASE_ADD*currentShift));
					snprintf(buffer1, sizeof(buffer1), "%d", currentShift);
					GLCD_DisplayString(3, 11, __FI, (unsigned char *)strcat(buffer, " "));
					GLCD_DisplayString(4, 11, __FI, (unsigned char *)strcat(buffer1, " "));
					break;
			}
		}
	joystick_val = get_button();
	//function mode	
	//printf("\nfunction mode\n");
  //bit = &BitBand(&LPC_ADC->ADCR, 24);		
 //*bit = 0;
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);	
 //*bit = 1;
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);
 //*bit = 0;
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);
	printf("\nFunction Method\n");
#ifdef __USE_LCD
	GLCD_DisplayString(6, 6, __FI, (unsigned char *)"Function    ");
#endif
  bit1 = &BitBand(&LPC_GPIO1->FIOPIN, 28);
	bit2 = &BitBand(&LPC_GPIO2->FIOPIN, 2);
	// ITE conditional execution method
	led_switch ? (*bit1 = 0) : (*bit1 = 1);
	led_switch ? (*bit2 = 0) : (*bit2 = 1);
	led_switch ? (led_switch = false) : (led_switch = true);
	delay_ms(BASE_DELAY + (BASE_ADD*currentShift));
	
	if (joystick_val != KBD_MASK) {	
		switch(joystick_val){
				case KBD_UP:
					if (shiftDirection < 0) {
						shiftDirection *= -1;
					}
					currentShift += shiftDirection;
					snprintf(buffer, sizeof(buffer), "%d", BASE_DELAY + (BASE_ADD*currentShift));
					snprintf(buffer1, sizeof(buffer1), "%d", currentShift);
					GLCD_DisplayString(3, 11, __FI, (unsigned char *)buffer);
					GLCD_DisplayString(4, 11, __FI, (unsigned char *)strcat(buffer1, " "));
					break;
				case KBD_DOWN:
					if (shiftDirection > 0) {
						shiftDirection *= -1;
					}
					if (currentShift > -1 ) {
					currentShift += shiftDirection;
					}
					snprintf(buffer, sizeof(buffer), "%d", BASE_DELAY + (BASE_ADD*currentShift));
					snprintf(buffer1, sizeof(buffer1), "%d", currentShift);
					GLCD_DisplayString(3, 11, __FI, (unsigned char *)strcat(buffer, " "));
					GLCD_DisplayString(4, 11, __FI, (unsigned char *)strcat(buffer1, " "));
					break;
			}
		}
	joystick_val = get_button();
	//bit band mode
	//printf("\nBit Masking mode\n");
	//ADCR_Bit24 = 1;
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);
	//ADCR_Bit24  = 0;
	//printf("Addr: 0x%08X \n", LPC_ADC->ADCR);
	printf("\nDirect Bit Banding Method\n");
#ifdef __USE_LCD
	GLCD_DisplayString(6, 6, __FI, (unsigned char *)"Bit Banding    ");
#endif
	//'s' suffix conditional execution method
	if (led_switch) {
		FIOPIN_Bit28 = 0;
		FIOPIN_Bit02 = 0;
		led_switch = false;
		delay_ms(BASE_DELAY + (BASE_ADD*currentShift));  // 1 second delay
	}
	else {
		FIOPIN_Bit28 = 1;
		FIOPIN_Bit02 = 1;
		led_switch = true;
		delay_ms(BASE_DELAY + (BASE_ADD*currentShift));  // 1 second delay
	}
	
	if (joystick_val != KBD_MASK) {	
		switch(joystick_val){
				case KBD_UP:
					if (shiftDirection < 0) {
						shiftDirection *= -1;
					}
					currentShift += shiftDirection;
					snprintf(buffer, sizeof(buffer), "%d", BASE_DELAY + (BASE_ADD*currentShift));
					snprintf(buffer1, sizeof(buffer1), "%d", currentShift);
					GLCD_DisplayString(3, 11, __FI, (unsigned char *)buffer);
					GLCD_DisplayString(4, 11, __FI, (unsigned char *)strcat(buffer1, " "));
					break;
				case KBD_DOWN:
					if (shiftDirection > 0) {
						shiftDirection *= -1;
					}
					if (currentShift > -1 ) {
					currentShift += shiftDirection;
					}
					snprintf(buffer, sizeof(buffer), "%d", BASE_DELAY + (BASE_ADD*currentShift));
					snprintf(buffer1, sizeof(buffer1), "%d", currentShift);
					GLCD_DisplayString(3, 11, __FI, (unsigned char *)strcat(buffer, " "));
					GLCD_DisplayString(4, 11, __FI, (unsigned char *)strcat(buffer1, " "));
					break;
			}
		}
	
	}
}

