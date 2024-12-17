#ifndef TIMER_H
#define TIMER_H

#include "lib_headers.h"
/*
// Function prototypes
void timer_init(TIM_TypeDef *TIMx);
void delay_ms(TIM_TypeDef *TIMx, uint16_t delay);     // Update to uint16_t
void delay_sec(TIM_TypeDef *TIMx, uint16_t delay);     // Update to uint16_t
*/

void delay_timer_init(void);
void delay_us(uint32_t delay);
void delay_ms(uint32_t delay);
void delay_sec(uint32_t delay);
void configure_Timer_for_Interrupt(TIM_TypeDef *TIMx, uint32_t timeout_ms);

#endif // TIMER_H
