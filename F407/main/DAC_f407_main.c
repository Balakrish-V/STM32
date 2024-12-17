#include "All_headers.h"
int main(void) 
{
    // Set system clock to HSI (16 MHz)
    set_sysclk_to_hsi();
    Timer2_Init_Basic();
    DAC_Init();
    // Set the desired frequency for the DAC output (e.g., 50 Hz)
    float desired_frequency = 50.0f;
    DAC_SetFrequency(desired_frequency);
    while (1) {
        // Ramp up the DAC output from 0 to 4095
        for (uint32_t value = 0; value <= 4095; value++) {
            DAC_SetValue(value); // Set the DAC output value
            delay_ms(1);
        }

        // Ramp down the DAC output from 4095 to 0
        for (uint32_t value = 4095; value > 0; value--) {
            DAC_SetValue(value); // Set the DAC output value
            delay_ms(1); 
        }
    }
}
