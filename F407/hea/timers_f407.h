#ifndef TIMER2_CONFIG_H
#define TIMER2_CONFIG_H

#include "stm32f4xx.h"

// Function prototypes for Timer 2 configuration
void Timer2_Init_Basic(void);
void Timer2_Init_IRQ(void);
void TIM2_IRQHandler(void);
void delay_s(uint32_t seconds);
void delay_ms(uint32_t milliseconds);

#endif // TIMER2_CONFIG_H