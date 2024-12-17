#ifndef PWM_H
#define PWM_H

#include "All_headers.h"

// Define GPIO port and pin mappings for each TIM4 channel
typedef struct {
    GPIO_TypeDef *port;  // Pointer to the GPIO port
    uint8_t pin;         // Pin number
} PWM_Channel;

// Declare pwm_channels array (defined in a .c file)
extern PWM_Channel pwm_channels[];

// Function declarations
void init_gpio_for_pwm(PWM_Channel channel);
void init_timer_4_for_pwm(uint8_t channel); // Accept channel as an argument
void set_pwm_frequency(uint32_t frequency);
void set_pwm_duty_cycle(uint32_t duty_cycle);
void fade_led(void);

#endif