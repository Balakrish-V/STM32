#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "lib_headers.h"

// Macros for switch press detection (active low)
#define IS_SWITCH_PRESSED(port, pin)  (!(port->IDR & (1 << (pin))))

// Macros for controlling LEDs
#define LED_ON(port, pin)   ((port)->ODR |= (1 << (pin)))  // Turn on LED
#define LED_OFF(port, pin)  ((port)->ODR &= ~(1 << (pin))) // Turn off LED
#define LED_TOGGLE(port, pin)  ((port)->ODR ^= (1 << (pin))) // Toggle LED

// LED Pins
#define RED_LED_PORT   GPIOB
#define RED_LED_PIN    14

#define BLUE_LED_PORT  GPIOB
#define BLUE_LED_PIN   7

#define GREEN_LED_PORT GPIOC
#define GREEN_LED_PIN  7

// Function Prototypes
void Initialize_LED_GPIOs(void);

void Red_LED_On(void);
void Red_LED_Off(void);
void Red_LED_Toggle(void);

void Blue_LED_On(void);
void Blue_LED_Off(void);
void Blue_LED_Toggle(void);

void Green_LED_On(void);
void Green_LED_Off(void);
void Green_LED_Toggle(void);

// Combined functions
void All_LEDs_On(void);
void All_LEDs_Off(void);
void All_LEDs_Toggle(void);

void Control_LEDs_using_switch(void);
#endif // LED_CONTROL_H
