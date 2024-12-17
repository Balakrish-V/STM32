#ifndef UART2_CONFIG_H
#define UART2_CONFIG_H

#include "All_headers.h" // Ensure all necessary headers are included only once

// Define common baud rate macros
#define UART_BAUD_9600    9600
#define UART_BAUD_19200   19200
#define UART_BAUD_38400   38400
#define UART_BAUD_57600   57600
#define UART_BAUD_115200  115200
#define UART_BAUD_230400  230400
#define UART_BAUD_460800  460800
#define UART_BAUD_921600  921600

#define ENABLE_TX_INTERRUPT 0 // Set to 1 to enable TX interrupt, 0 to disable
#define ENABLE_RX_INTERRUPT 0 // Set to 1 to enable RX interrupt, 0 to disable

#define USART2_TX_DMA_CHANNEL 4
#define USART2_RX_DMA_CHANNEL 5

#define DMA1_STREAM6_IRQ    DMA1_Stream6_IRQn
#define DMA1_STREAM5_IRQ    DMA1_Stream5_IRQn

// Function prototypes for UART2
void uart2_init(uint32_t baudrate);
void uart2_send_char(uint8_t c);
uint8_t uart2_rcv_char(void);
void uart2_send_str(volatile uint8_t *msg);

// Function prototypes for UART2 with DMA
void uart2_init_dma(uint32_t baudrate);
void uart2_send_char_dma(uint8_t c);
uint8_t uart2_rcv_char_dma(void);
void uart2_send_str_dma(volatile uint8_t *msg);

#endif // UART2_CONFIG_H