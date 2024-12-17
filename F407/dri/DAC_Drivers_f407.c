#include "All_headers.h"

// Global variable for DAC delay (used for controlling frequency)
volatile uint32_t dac_delay = 100; // Default delay, can be adjusted by frequency

// Function to initialize the DAC
void DAC_Init(void) {
    // Enable GPIOA clock for PA4
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
    
    // Set PA4 to analog mode for DAC output
    GPIOA->MODER |= (3 << (4 * 2)); // Analog mode for PA4
    
    // Enable DAC clock
    RCC->APB1ENR |= RCC_APB1ENR_DACEN; // Enable DAC1 clock
    
    // Enable DAC Channel 1
    DAC->CR |= DAC_CR_EN1; // Enable DAC Channel 1
}

// Function to set DAC output value (12-bit resolution)
void DAC_SetValue(uint32_t value) {
    // Ensure the value does not exceed 12-bit maximum (4095)
    if (value > 4095) {
        value = 4095; // Cap value at 4095
    }
    
    // Set the output value for DAC channel 1 (12-bit DAC)
    DAC->DHR12R1 = value;
}

// Function to set DAC output frequency
void DAC_SetFrequency(float frequency) {
    // Calculate the time delay in microseconds for the desired frequency
    if (frequency > 0) {
        float delay_time = (1.0f / (frequency * 4096.0f)) * 1000000.0f; // Time delay in microseconds
        dac_delay = (uint32_t)(delay_time + 0.5f); // Round to the nearest integer
    }
}
