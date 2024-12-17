#include "All_headers.h"

// Define LED Ports and Pins
#define LED_PORT        GPIOD
#define LED_PIN_12      (1U << 12)   // PD12 - Green  - TIM4 CH1 
#define LED_PIN_13      (1U << 13)   // PD13 - Orange - TIM4 CH2 
#define LED_PIN_14      (1U << 14)   // PD14 - Red    - TIM4 CH3 
#define LED_PIN_15      (1U << 15)   // PD15 - Blue   - TIM4 CH4 

// PWM Channel structure with the updated LED pins
PWM_Channel pwm_channels[] = {
    {GPIOD, 12}, // TIM4_CH1 on PD12 (Green)
    {GPIOD, 13}, // TIM4_CH2 on PD13 (Orange)
    {GPIOD, 14}, // TIM4_CH3 on PD14 (Red)
    {GPIOD, 15}  // TIM4_CH4 on PD15 (Blue)
};

// Initialize GPIO pins for PWM
void init_gpio_for_pwm(PWM_Channel channel) 
{
    // Enable clock for GPIO port
    if (channel.port == GPIOA) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable clock for GPIOA
    } else if (channel.port == GPIOB) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable clock for GPIOB
    } else if (channel.port == GPIOC) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable clock for GPIOC
    } else if (channel.port == GPIOD) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable clock for GPIOD
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
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Use APB1 for Timer 4 on STM32F407VGT6

    // Set Timer 4 prescaler to achieve 1 MHz (1 us timer)
    TIM4->PSC = (SystemCoreClock / 1000000) - 1;

    // Ensure valid channel (1-4)
    if (channel > 0 && channel <= 4) {
        // Initialize GPIO for the specified channel
        init_gpio_for_pwm(pwm_channels[channel - 1]);

        // Configure Timer 4 for PWM based on the specified channel
        switch (channel) {
            case 1: // TIM4_CH1 (Green)
                TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk | TIM_CCMR1_OC1M_Msk);
                TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // Set PWM mode 1 for CH1
                TIM4->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC1E;    // Enable capture/compare
                TIM4->CCR1 = 0;                 // Initialize duty cycle
                break;

            case 2: // TIM4_CH2 (Orange)
                TIM4->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk | TIM_CCMR1_OC2M_Msk);
                TIM4->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // Set PWM mode 1 for CH2
                TIM4->CCMR1 |= TIM_CCMR1_OC2PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC2E;    // Enable capture/compare
                TIM4->CCR2 = 0;                 // Initialize duty cycle
                break;

            case 3: // TIM4_CH3 (Red)
                TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S_Msk | TIM_CCMR2_OC3M_Msk);
                TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; // Set PWM mode 1 for CH3
                TIM4->CCMR2 |= TIM_CCMR2_OC3PE; // Enable preload
                TIM4->CCER |= TIM_CCER_CC3E;    // Enable capture/compare
                TIM4->CCR3 = 0;                 // Initialize duty cycle
                break;

            case 4: // TIM4_CH4 (Blue)
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

// Set PWM duty cycle (0 to 1000) for TIM4_CH2 (Orange)
void set_pwm_duty_cycle(uint32_t duty_cycle) 
{
    if (duty_cycle <= 1000) {
			  TIM4->CCR1 = duty_cycle; // Update for TIM4_CH1 (Green)
        TIM4->CCR2 = duty_cycle; // Update for TIM4_CH2 (Orange)
			  TIM4->CCR3 = duty_cycle; // Update for TIM4_CH2 (Orange)
			  TIM4->CCR4 = duty_cycle; // Update for TIM4_CH2 (Orange)
    }
}

// Fade the LEDs by adjusting brightness
void fade_led(void) 
{
    while (1) {
        // Gradually increase brightness
        for (uint32_t brightness = 0; brightness <= 400; brightness++) {
            set_pwm_duty_cycle(brightness); // Set duty cycle for Orange LED
            delay_ms(1);                    // Delay to see brightness change
        }
        // Gradually decrease brightness
        for (uint32_t brightness = 400; brightness > 0; brightness--) {
            set_pwm_duty_cycle(brightness); // Set duty cycle for Orange LED
            delay_ms(1);                    // Delay to see brightness change
        }
    }
}
