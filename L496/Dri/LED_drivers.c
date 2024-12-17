#ifndef LED_CONTROL_C
#define LED_CONTROL_C

#include "lib_headers.h"

// Red LED control functions
void Red_LED_On(void) {
    LED_ON(RED_LED_PORT, RED_LED_PIN);
}

void Red_LED_Off(void) {
    LED_OFF(RED_LED_PORT, RED_LED_PIN);
}

void Red_LED_Toggle(void) {
    LED_TOGGLE(RED_LED_PORT, RED_LED_PIN);
}

// Blue LED control functions
void Blue_LED_On(void) {
    LED_ON(BLUE_LED_PORT, BLUE_LED_PIN);
}

void Blue_LED_Off(void) {
    LED_OFF(BLUE_LED_PORT, BLUE_LED_PIN);
}

void Blue_LED_Toggle(void) {
    LED_TOGGLE(BLUE_LED_PORT, BLUE_LED_PIN);
}

// Green LED control functions
void Green_LED_On(void) {
    LED_ON(GREEN_LED_PORT, GREEN_LED_PIN);
}

void Green_LED_Off(void) {
    LED_OFF(GREEN_LED_PORT, GREEN_LED_PIN);
}

void Green_LED_Toggle(void) {
    LED_TOGGLE(GREEN_LED_PORT, GREEN_LED_PIN);
}

// Control all LEDs simultaneously
void All_LEDs_On(void) {
    Red_LED_On();
    Blue_LED_On();
    Green_LED_On();
}

void All_LEDs_Off(void) {
    Red_LED_Off();
    Blue_LED_Off();
    Green_LED_Off();
}

void All_LEDs_Toggle(void) {
    Red_LED_Toggle();
    Blue_LED_Toggle();
    Green_LED_Toggle();
}

// Example function to control LEDs based on switch state
void Control_LEDs_using_switch(void) {
    if (!IS_SWITCH_PRESSED(GPIOC, 13)) {
        All_LEDs_On();  // Turn on all LEDs
    } else {
        All_LEDs_Off(); // Turn off all LEDs
    }
}

// initialize GPIO pins for LEDs and switches
void Initialize_LED_GPIOs(void) {
    GPIO_Pin_Init('B', 14, GPIO_MODE_OUTPUT, GPIO_NO_PULL); // PB14 as output
    GPIO_Pin_Init('B', 7, GPIO_MODE_OUTPUT, GPIO_NO_PULL);  // PB7 as output
    GPIO_Pin_Init('C', 7, GPIO_MODE_OUTPUT, GPIO_NO_PULL);  // PC7 as output
    GPIO_Pin_Init('C', 13, GPIO_MODE_INPUT, GPIO_NO_PULL);  // PC13 as input
	  // GPIO_Pin_Init('A', 5, GPIO_MODE_OUTPUT, GPIO_NO_PULL); // PB14 as output - L476 - 64pin
	  // LED_ON(GPIOA,5);
}


#endif