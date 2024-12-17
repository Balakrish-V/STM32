#ifndef ADC_H
#define ADC_H

#include "lib_headers.h"

// Structure to represent an ADC channel with its GPIO port
typedef struct {
    GPIO_TypeDef *GPIOx; // GPIO Port
    uint8_t pin;         // GPIO Pin number
    uint8_t channel;     // ADC Channel number
} ADC_Channel;

// Macros for ADC Channels and Pins
// Macros for ADC Channels, GPIO Ports, and Pins
#define ADC1_CHANNEL_PC0_CH1  {GPIOC, 0, 1}  // GPIOC, Pin 0 - ADC1 Channel 1
#define ADC1_CHANNEL_PC3_CH4  {GPIOC, 3, 4}  // GPIOC, Pin 3 - ADC1 Channel 4
#define ADC1_CHANNEL_PA0_CH5  {GPIOA, 0, 5}  // GPIOA, Pin 0 - ADC1 Channel 5
#define ADC1_CHANNEL_PA3_CH8  {GPIOA, 3, 8}  // GPIOA, Pin 3 - ADC1 Channel 8
#define ADC1_CHANNEL_PB0_CH15 {GPIOB, 0, 15}// GPIOA, Pin 3 - ADC1 Channel 8
#define ADC3_CHANNEL_PF5_CH8  {GPIOF, 5, 8}  // GPIOF, Pin 5 - ADC3 Channel 8
#define ADC3_CHANNEL_PF7_CH10 {GPIOF, 7, 10} // GPIOF, Pin 7 - ADC3 Channel 10
#define ADC3_CHANNEL_PF8_CH11 {GPIOF, 8, 11} // GPIOF, Pin 8 - ADC3 Channel 11
#define ADC3_CHANNEL_PF9_CH12 {GPIOF, 9, 12} // GPIOF, Pin 9 - ADC3 Channel 12

// Define ADC instances for better readability
#define ADC_INSTANCE_1 ADC1
#define ADC_INSTANCE_3 ADC3

// Function to initialize ADC for a specific channel and GPIO pin
// void ADc_Init(ADC_TypeDef *ADCx, ADC_Channel *channels, uint8_t num_channels);

// Function to read ADC value
// void ADC_Read(ADC_TypeDef *ADCx, uint8_t num_channels);

#endif