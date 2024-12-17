#include "All_headers.h"

// Function to initialize Timer 2
void Timer2_Init_Basic(void)
{
    // Enable TIM2 clock (bit0)
    RCC->APB1ENR |= (1 << 0);

    // Set prescaler to 15999
    // 16 MHz / (15999 + 1) = 1 kHz timer clock speed
    TIM2->PSC = 15999;

    // Enable Timer 2 module (CEN, bit0)
    TIM2->CR1 |= (1 << 0);
}

// Function to create a delay in seconds
void delay_s(uint32_t seconds)
{
    // Set the auto-reload value to 1000 for 1 second delay
    TIM2->ARR = 1000;

    for (uint32_t i = 0; i < seconds; i++)
    {
        // Reset the counter
        TIM2->CNT = 0;

        // Wait until the update event flag is set
        while (!(TIM2->SR & TIM_SR_UIF));

        // Clear the update event flag
        TIM2->SR &= ~TIM_SR_UIF;
    }
}

// Function to create a delay in milliseconds
void delay_ms(uint32_t milliseconds)
{
    // Set the auto-reload value to 1 for 1 millisecond delay
    TIM2->ARR = 1;

    for (uint32_t i = 0; i < milliseconds; i++)
    {
        // Reset the counter
        TIM2->CNT = 0;

        // Wait until the update event flag is set
        while (!(TIM2->SR & TIM_SR_UIF));

        // Clear the update event flag
        TIM2->SR &= ~TIM_SR_UIF;
    }
}

// Function to create a delay in microseconds
void delay_us(uint32_t microseconds)
{
    for (uint32_t i = 0; i < microseconds; i++)
    {
        // Reset the counter
        TIM2->CNT = 0;

        // Wait until the update event flag is set (indicating that 1 microsecond has passed)
        while (!(TIM2->SR & TIM_SR_UIF));

        // Clear the update event flag
        TIM2->SR &= ~TIM_SR_UIF;
    }
}

void TIM2_IRQHandler(void)
{
    // Clear interrupt status
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;  // Clear the update interrupt flag

        // Toggle LEDs
        All_LEDs_Toggle_F4();
    }
}

void Timer2_Init_IRQ(void)
{
    // Enable TIM2 clock (bit0)
    RCC->APB1ENR |= (1 << 0);

    // Set prescaler to 15999
    // 16 MHz / (15999 + 1) = 1 kHz timer clock speed
    TIM2->PSC = 15999;

    // Set the auto-reload value to 1000
    // 1 kHz / 1000 = 1 second timer period
    TIM2->ARR = 1000;

    // Enable Update Interrupt
    TIM2->DIER |= (1 << 0);

    // Set the interrupt priority level to 2
    NVIC_SetPriority(TIM2_IRQn, 2);

    // Enable TIM2 IRQ from NVIC
    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable Timer 2 module (CEN, bit0)
    TIM2->CR1 |= (1 << 0);
}



