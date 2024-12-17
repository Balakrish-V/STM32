#include "All_headers.h"

void USART2_IRQHandler(void)
{
    if (USART2->SR & (1 << 5)) // RXNE flag
    {
        uint8_t received_data = USART2->DR; // Read received data
        // Echo the received character back
        USART2->DR = received_data;
        // Wait for the transmission to complete before exiting the ISR
        while (!(USART2->SR & (1 << 6))); // Wait for TC (Transmission Complete)
    }
}

int main(void)
{
    set_sysclk_to_hsi();
    uart2_init(UART_BAUD_115200);
    // Enable the receive interrupt for USART2
    USART2->CR1 |= (1 << 5); 

    while (1)
    {
        // Main loop can be empty or have other processing
    }
    return 0;
}
