#include "All_headers.h"
uint16_t adc_value;
int main() 
{
    // Initialize ADC for GPIOA pin 5 (ADC channel 5)
    ADC_Init(GPIOA, 5, 5);

    while (1) {
        // Start ADC conversion
        ADC_Start();

        // Read ADC value
        adc_value = ADC_Output();

        // Use adc_value (e.g., print or process it)

        // Small delay
        for (volatile int i = 0; i < 100000; i++);
    }
    return 0;
}
