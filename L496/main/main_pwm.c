#include "lib_headers.h"
int main(void) 
{
    // Set system clock and initialize delays
    set_sysclk_to_msi(MSI_16MHZ);
    delay_timer_init();

    // Initialize GPIO for PWM on PB7 (TIM4_CH2)
    init_gpio_for_pwm(pwm_channels[1]); 
    // Initialize Timer 4 for PWM on Channel 2 (corresponding to PB7)
    init_timer_4_for_pwm(2);

    // Set a standard PWM frequency (1 kHz)
    set_pwm_frequency(1000);

    // Start fading the LED
    fade_led();

    return 0;
}


