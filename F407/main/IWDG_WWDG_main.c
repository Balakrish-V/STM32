#include "All_headers.h"
int main(void) {
    Initialize_LED_GPIOs_F4();
    Timer2_Init_Basic();  //delay based on timer 2
    IWDG_Init(60); 
    LED_On_F4(LED_PIN_12);
    delay_ms(100);  // reseting
    LED_Off_F4(LED_PIN_12);
    while (1) {
        // Toggle the LED
        LED_Toggle_F4(LED_PIN_13);
        delay_ms(2000); 
        IWDG_Refresh();
    }
}

/*
#include "All_headers.h"
int main(void) 
{
    Initialize_LED_GPIOs_F4();
    Timer2_Init_Basic();
    // Initialize WWDG with a timeout of 2 seconds in seconds unit
    WWDG_Init(200, 'm'); 
    // Indicate the system has started
    LED_On_F4(LED_PIN_14);
    delay_ms(300);
    LED_Off_F4(LED_PIN_14);
    while (1) {
        // Toggle the LED
        LED_Toggle_F4(LED_PIN_15);
        delay_ms(900); // Less than the WWDG timeout
        WWDG_Refresh(0x7F); // Assuming 0x7F is the max counter value
    }
}
*/

