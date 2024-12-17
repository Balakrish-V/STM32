#include "lib_headers.h"

void delay_timer_init(void) {
    uint32_t clockFrequency = 16000000; // 80 MHz (adjust this if your clock differs)
    SysTick->LOAD = (clockFrequency / 1000) - 1; // Set reload for 1 ms tick
    SysTick->VAL = 0;                            // Clear current value register
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // Use processor clock
                    SysTick_CTRL_ENABLE_Msk;      // Enable SysTick
}

void delay_ms(uint32_t delay) {
    for (uint32_t i = 0; i < delay; i++) {
        // Wait for the COUNTFLAG to be set every 1 ms
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
}

void delay_us(uint32_t delay) {
    uint32_t clockFrequency = 16000000; // 16 MHz system clock
    uint32_t reload_value = (clockFrequency / 1000000) - 1; // Reload value for 1 microsecond

    // Configure SysTick for 1 microsecond ticks
    SysTick->LOAD = reload_value;
    SysTick->VAL = 0; // Clear current value register

    for (uint32_t i = 0; i < delay; i++) {
        // Wait for the COUNTFLAG to be set (this indicates 1 tick has passed)
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
}

void delay_sec(uint32_t delay) {
    delay_ms(delay * 1000);  // Convert seconds to milliseconds
}

// Function to configure a timer for interrupts
void configure_Timer_for_Interrupt(TIM_TypeDef *TIMx, uint32_t timeout_ms) {
    // Enable Timer clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // Assuming TIM2, change as needed

    // Set Timer prescaler
    TIMx->PSC = SystemCoreClock / 1000 - 1; // Prescale to get 1ms ticks

    // Set the timer auto-reload register for the specified timeout period
    TIMx->ARR = timeout_ms;

    // Re-init the counter and update the registers
    TIMx->EGR |= TIM_EGR_UG;

    // Enable Timer interrupt
    TIMx->DIER |= TIM_DIER_UIE;

    // Enable the Timer
    TIMx->CR1 |= TIM_CR1_CEN;

    // Enable and set Timer IRQ priority
    NVIC_SetPriority(TIM2_IRQn, 0x02);
    NVIC_EnableIRQ(TIM2_IRQn);
}
/*
// Timer interrupt handler
void TIM2_IRQHandler(void) {
    // Check if the update interrupt flag is set
    if (TIM2->SR & TIM_SR_UIF) {
        // Clear the interrupt
        TIM2->SR &= ~TIM_SR_UIF;

        // Toggle the GPIO pin (replace GPIOB_ODR_OD3 with the respective pin)
        Blue_LED_Toggle();
    }
}
*/
/*
// Function to initialize the timer
void timer_init(TIM_TypeDef *TIMx) {
    TIMx->CR1 &= ~TIM_CR1_CEN; // Stop the timer if it was running

    if (TIMx == TIM2) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    } else if (TIMx == TIM3) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
    } else if (TIMx == TIM4) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
    }

    TIMx->PSC = 15999; // Prescaler for 1 ms tick
    TIMx->ARR = 0;     // Default value, will be set in delay functions
    TIMx->CNT = 0;
    TIMx->CR1 |= TIM_CR1_CEN; // Enable the timer
}

// Delay function for milliseconds
void delay_ms(TIM_TypeDef *TIMx, uint16_t delay) {
    TIMx->ARR = 1;   // Set ARR for 1 ms delay (1000 counts for 1 ms)
    TIMx->CNT = 0;   // Reset counter

    for (uint16_t i = 0; i < delay; i++) {
        while (!(TIMx->SR & TIM_SR_UIF)); // Wait for update flag
        TIMx->SR &= ~TIM_SR_UIF;          // Clear the update flag
			  TIMx->CNT = 0;
    }
}

// Delay function for seconds
void delay_sec(TIM_TypeDef *TIMx, uint16_t delay) {
    TIMx->ARR = 1000;   // Set ARR for 1 ms delay (1000 counts for 1 ms)
    TIMx->CNT = 0;     // Reset counter

    for (uint16_t i = 0; i < delay  ; i++) { // Convert seconds to milliseconds
        while (!(TIMx->SR & TIM_SR_UIF)); // Wait for update flag
        TIMx->SR &= ~TIM_SR_UIF; // Clear the update flag
			  TIMx->CNT = 0;
    }
}

void USART2_Init(void)
{
    RCC->AHB2ENR |= 1;
    RCC->APB1ENR1 |= 0x20000;
    
    GPIOA->AFR[0] |= 0x7700; //Configure PA2 as USART2 TX and configure PA3 as USART2 RX
    GPIOA->MODER |= 0x00A0; //Enable alternate function for PA2, PA3
    
    USART2->BRR = 0x1A1; //9600 baud @4Mhz
    USART2->CR1 = 0x000C; //Enable Tx, Rx
    USART2->CR1 |= 1; //Enable USART2
    
}

void systickDelayMs(int n)
{
    SysTick->LOAD = 4000; //Reload with number of clocks per millisecond
    SysTick->VAL = 0;     //Clear current value register
    SysTick->CTRL = 0x5;  //Enable Systick
    
    for(int i =0;i<n;i++)
    {
        //Wait until the COUNT flag is set
        while((SysTick->CTRL & 0x10000) ==0){}
    }
    SysTick->CTRL =0;
}

*/
