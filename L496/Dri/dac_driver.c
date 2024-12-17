#include "lib_headers.h"

// Function prototypes
void DAC_Init(void);
void DAC_SetValue(uint32_t value);
void DAC_SetFrequency(float frequency); // Function to set DAC frequency

volatile uint32_t dac_delay = 100; // Default delay (can be adjusted by frequency)

void DAC_Init(void) 
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  // Enable GPIOA clock
    GPIOA->MODER |= (3 << (4 * 2));       // Set PA4 to analog mode
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; // Enable DAC1 clock
    DAC1->CR |= DAC_CR_EN1;               // Enable DAC Channel 1
}

void DAC_SetValue(uint32_t value) 
{
    if (value > 4095) {
        value = 4095; // Cap the value at 4095
    }
    DAC1->DHR12R1 = value; // Set the output value (12-bit DAC)
}

void DAC_SetFrequency(float frequency) 
{
    // Calculate the time delay in microseconds for the desired frequency
    if (frequency > 0) {
        float delay_time = (1.0f / (frequency * 4096.0f)) * 1000000.0f; // In microseconds
        dac_delay = (uint32_t)(delay_time + 0.5f); // Round to nearest integer
    }
}

int main(void) 
{
    set_sysclk_to_hsi(); // Assuming this function sets the system clock to HSI (16 MHz)
	  delay_timer_init();
    DAC_Init();
    float desired_frequency = 500.0f; // Set desired frequency in Hz (1 Hz for example)
	
//	100f - > 14.5Hz
//  1000f - > 24.1Hz
//  400f  - > 18.1Hz	
//  500f - >	18.1Hz
    DAC_SetFrequency(desired_frequency); // Set DAC output frequency
    // Main loop
    while (1) {
        for (uint32_t value = 0; value <= 4095; value++) {
            DAC_SetValue(value); // Set the DAC output value
            delay_us(dac_delay); // Delay based on the set frequency
        }

        for (uint32_t value = 4095; value > 0; value--) {
            DAC_SetValue(value); // Set the DAC output value
            delay_us(dac_delay); // Delay based on the set frequency
        }
    }
}
