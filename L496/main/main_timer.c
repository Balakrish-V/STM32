#include "lib_headers.h"
int main(void) 
{
		// init_clock(); - by default 80mhz. so error
    set_sysclk_to_hsi(); 
  	Initialize_LED_GPIOs();
	  // delay_timer_init();
		configure_Timer_for_Interrupt(TIM2,100);  //toggle function is wrapped
    while (1) {
			/*
			  Blue_LED_On();
			  delay_sec(1);
			  Blue_LED_Off();
			  delay_ms(1000);
			*/
    }
    return 0;
}

/*
//	timer_init(TIM3); // Initialize the timer
//	delay_ms(TIM3,1000);
//	delay_sec(TIM1,1);
*/
