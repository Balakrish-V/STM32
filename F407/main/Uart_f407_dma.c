#include "All_headers.h"

volatile uint8_t msg[] = "UART_DMA\n\r"; // Example message to be sent

int main(void)
{
    // Initialize system (HAL, Clocks, etc.)
    set_sysclk_to_hsi();
    
    Initialize_LED_GPIOs_F4();
	
    // Initialize UART2 with DMA for a baud rate of 115200
    uart2_init_dma(UART_BAUD_115200);

    // Send a string using DMA
    uart2_send_str_dma((volatile uint8_t *)msg);  // Sending predefined message

    // Main loop
    while (1)
    {
        uint8_t received_char = uart2_rcv_char_dma();
        if (received_char != 0) {
            // Handle received data (e.g., process or echo back)
            uart2_send_char_dma(received_char);  // Echo received char back via DMA
        }
    }
}
