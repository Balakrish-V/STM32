#include "lib_headers.h"
// Main function
int main(void) 
{
    init_clock();
    Initialize_LED_GPIOs();     // Initialize GPIO pins
	  delay_timer_init();
	  //All_LEDs_On();
   	//delay_ms(20);
	  //delay_sec(2);
	  //All_LEDs_Off();	
	  EXTI_Init(GPIOC, 13, INTERRUPT_RISING_FALLING);
	  while (1) 
		{
			  // Control_LEDs_using_switch();
        // Main loop does nothing; LED control is handled by interrupts
    }
}
