#include "All_headers.h"
int main(void)
{
    set_sysclk_to_hsi();            // Configure system clock
    Initialize_LED_GPIOs_F4();      // Initialize LEDs
    LED_On_F4(LED_PIN_13);          // Turn ON Orange LED 
	  LED_On_F4(LED_PIN_14);          // Turn ON Red LED 
	  LED_On_F4(LED_PIN_12);          // Turn ON Green LED 
    Timer2_Init_Basic();                  // Initialize Timer 2
    while (1)
    {
        // Toggle Blue LED
        LED_On_F4(LED_PIN_15);    
        delay_ms(200);                 // 1 second delay
        LED_Off_F4(LED_PIN_15);   
		  	//delay_s(1); 
        delay_ms(200);             // 1 second delay in milliseconds
    }
    return 0;
}