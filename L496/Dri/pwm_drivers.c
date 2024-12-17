#ifndef PWM_C
#define PWM_C

#include "lib_headers.h"

// Define the pwm_channels array (only here, not in the header)
PWM_Channel pwm_channels[] = {
    {GPIOA, 6}, // TIM4_CH1 on PA6
    {GPIOB, 7}, // TIM4_CH2 on PB7
    {GPIOB, 8}, // TIM4_CH3 on PB8
    {GPIOB, 9}  // TIM4_CH4 on PB9
};

// Initialize GPIO pins for PWM
void init_gpio_for_pwm(PWM_Channel channel) 
{
    // Enable clock for GPIO port
    if (channel.port == GPIOA) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable clock for GPIOA
    } else if (channel.port == GPIOB) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // Enable clock for GPIOB
    }

    // Set GPIO pin to alternate function mode
    channel.port->MODER &= ~(0x3 << (channel.pin * 2)); // Clear mode bits
    channel.port->MODER |= (0x2 << (channel.pin * 2));  // Set to AF mode

    // Set alternate function to AF2 (TIM4) for the corresponding pin
    if (channel.pin < 8) { // Pins 0-7
        channel.port->AFR[0] &= ~(0xF << (channel.pin * 4)); // Clear AF bits
        channel.port->AFR[0] |= (2 << (channel.pin * 4));    // Set AF2 for TIM4
    } else { // Pins 8-15
        channel.port->AFR[1] &= ~(0xF << ((channel.pin - 8) * 4)); // Clear AF bits
        channel.port->AFR[1] |= (2 << ((channel.pin - 8) * 4));    // Set AF2 for TIM4
    }
}

// Initialize Timer 4 for PWM on the specified channel
void init_timer_4_for_pwm(uint8_t channel) 
{
    // Enable Timer 4 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

    // Set Timer 4 prescaler to achieve 1 MHz (1 us timer)
    TIM4->PSC = (SystemCoreClock / 1000000) - 1;

    // Ensure valid channel (1-4)
    if (channel > 0 && channel <= 4) {
        // Initialize GPIO for the specified channel
        init_gpio_for_pwm(pwm_channels[channel - 1]);

        // Configure Timer 4 for PWM based on the specified channel
        switch (channel) {
            case 1: // TIM4_CH1
                TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk | TIM_CCMR1_OC1M_Msk);
                TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // Set PWM mode 1 for CH1
                TIM4->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC1E;    // Enable capture/compare
                TIM4->CCR1 = 0;                 // Initialize duty cycle
                break;

            case 2: // TIM4_CH2
                TIM4->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk | TIM_CCMR1_OC2M_Msk);
                TIM4->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // Set PWM mode 1 for CH2
                TIM4->CCMR1 |= TIM_CCMR1_OC2PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC2E;    // Enable capture/compare
                TIM4->CCR2 = 0;                 // Initialize duty cycle
                break;

            case 3: // TIM4_CH3
                TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S_Msk | TIM_CCMR2_OC3M_Msk);
                TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; // Set PWM mode 1 for CH3
                TIM4->CCMR2 |= TIM_CCMR2_OC3PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC3E;    // Enable capture/compare
                TIM4->CCR3 = 0;                 // Initialize duty cycle
                break;

            case 4: // TIM4_CH4
                TIM4->CCMR2 &= ~(TIM_CCMR2_CC4S_Msk | TIM_CCMR2_OC4M_Msk);
                TIM4->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2; // Set PWM mode 1 for CH4
                TIM4->CCMR2 |= TIM_CCMR2_OC4PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC4E;    // Enable capture/compare
                TIM4->CCR4 = 0;                 // Initialize duty cycle
                break;

            default:
                // Handle invalid channel
                break;
        }

        // Update and enable the timer
        TIM4->EGR |= TIM_EGR_UG;   // Update registers
        TIM4->CR1 |= TIM_CR1_CEN;  // Enable Timer 4
    }
}

// Set PWM frequency by adjusting the auto-reload value
void set_pwm_frequency(uint32_t frequency) 
{
    if (frequency > 0) {
        TIM4->ARR = (SystemCoreClock / (TIM4->PSC + 1)) / frequency - 1;
    }
}

// Set PWM duty cycle (0 to 1000) for TIM4_CH2
void set_pwm_duty_cycle(uint32_t duty_cycle) 
{
    if (duty_cycle <= 1000) {
        TIM4->CCR2 = duty_cycle;
    }
}

// Fade LED with gradual brightness change
void fade_led(void) 
{
    while (1) {
        // Gradually increase brightness
        for (uint32_t brightness = 0; brightness <= 500; brightness++) {
            set_pwm_duty_cycle(brightness); // Set duty cycle
            delay_ms(1);                    // Delay to see brightness change
        }
        // Gradually decrease brightness
        for (uint32_t brightness = 500; brightness > 0; brightness--) {
            set_pwm_duty_cycle(brightness); // Set duty cycle
            delay_ms(1);                    // Delay to see brightness change
        }
    }
}

#endif

