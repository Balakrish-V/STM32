#ifndef LED_CONTROL_F4_H
#define LED_CONTROL_F4_H

#include "stm32f4xx.h"  // Ensure this includes all necessary STM32 HAL definitions

// Define LED Ports and Pins
#define LED_PORT        GPIOD
#define LED_PIN_12      (1U << 12)   // PD12 - Green
#define LED_PIN_13      (1U << 13)   // PD13 - Orange
#define LED_PIN_14      (1U << 14)   // PD14 - Red
#define LED_PIN_15      (1U << 15)   // PD15 - Blue

// Function Prototypes for STM32F4
void Initialize_LED_GPIOs_F4(void);
void LED_On_F4(uint16_t pin);
void LED_Off_F4(uint16_t pin);
void LED_Toggle_F4(uint16_t pin);
void All_LEDs_On_F4(void);
void All_LEDs_Off_F4(void);
void All_LEDs_Toggle_F4(void);

#endif

