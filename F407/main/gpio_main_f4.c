#include "All_headers.h"

// Define some basic settings for the GPIO pins
#define BUTTON_PIN    0     // PA0 for the button
#define BUTTON_PORT   GPIOA

int main(void) 
{
    set_sysclk_to_hse();
    Enable_GPIO_Clock(GPIO_PORT_A);
    GPIO_Pin_Init(GPIO_PORT_A, BUTTON_PIN, GPIO_MODE_INPUT, GPIO_PULL_UP);
		Timer2_Init_Basic();
    Initialize_LED_GPIOs_F4();
//    EXTI_Init(GPIOA, BUTTON_PIN, INTERRUPT_FALLING_EDGE);  //only once the interrupt detected. LED's doesn't toggle
//    EXTI_Init(GPIOA, BUTTON_PIN, INTERRUPT_RISING_EDGE);   //randomly interrupt triggered and LED doesn't toggle for every switch press
    EXTI_Init(GPIOA, BUTTON_PIN, INTERRUPT_RISING_FALLING);  //LED toggles for every instance of switch press
    while (1) {
    }
}
