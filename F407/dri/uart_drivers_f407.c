#include "uart_f407.h"

void uart2_init(uint32_t baudrate)
{
    // Enable USART2 clock, bit 17 on APB1ENR
    RCC->APB1ENR |= (1 << 17);

    // Enable GPIOA clock, bit 0 on AHB1ENR
    RCC->AHB1ENR |= (1 << 0);

    // Set pin modes as alternate mode 7 (pins 2 and 3)
    GPIOA->MODER &= ~(0xFU << 4); // Reset bits 4:5 for PA2 and 6:7 for PA3
    GPIOA->MODER |=  (0xAU << 4); // Set bits 4:5 for PA2 and 6:7 to alternate mode (10)

    // Set pin modes as high speed
    GPIOA->OSPEEDR |= 0x000000A0; // Set pin 2/3 to high speed mode (0b10)

    // Choose AF7 for USART2 in Alternate Function registers
    GPIOA->AFR[0] |= (0x7 << 8);  // For pin A2
    GPIOA->AFR[0] |= (0x7 << 12); // For pin A3

    // USART2 TX enable, TE bit 3
    USART2->CR1 |= (1 << 3);

    // USART2 RX enable, RE bit 2
    USART2->CR1 |= (1 << 2);

    // Calculate and set baud rate
    uint32_t usartdiv = 16000000 / (16 * baudrate);
    USART2->BRR = (usartdiv << 4) | ((16000000 / baudrate) - (usartdiv * 16));

    // Enable USART2 - UE, bit 13
    USART2->CR1 |= (1 << 13);

    // Enable interrupts if defined in macros
    #if ENABLE_TX_INTERRUPT
    USART2->CR1 |= (1 << 7); // Enable TXE interrupt
    #endif

    #if ENABLE_RX_INTERRUPT
    USART2->CR1 |= (1 << 5); // Enable RXNE interrupt
    #endif

    // Enable USART2 // Enable USART2 interrupt in NVIC
    NVIC_EnableIRQ(USART2_IRQn); // Make sure to enable the USART2 interrupt in the NVIC
}

void uart2_send_char(uint8_t c)
{
    // Wait for the transmit data register to be empty (TXE)
    while (!(USART2->SR & (1 << 7)));

    // Send the character
    USART2->DR = c;
}

uint8_t uart2_rcv_char(void)
{
    // Wait for data to be received (RXNE)
    while (!(USART2->SR & (1 << 5)));

    // Return the received character
    return (uint8_t)USART2->DR;
}

void uart2_send_str(volatile uint8_t *msg)
{
    while (*msg != '\0') // Continue until null terminator is found
    {
        uart2_send_char(*msg);
        msg++;
    }
}

// UART2 Initialization with DMA
void uart2_init_dma(uint32_t baudrate) {
    // Enable necessary clocks
    RCC->AHB1ENR |= (1 << 21);  // DMA1 clock enable
    RCC->APB1ENR |= (1 << 17);  // USART2 clock enable
    RCC->AHB1ENR |= (1 << 0);   // GPIOA clock enable

    // Configure GPIOA pins 2 and 3 for USART2 TX and RX
    GPIOA->MODER &= ~(0xF << 4);     // Reset bits 4:5 for PA2 and 6:7 for PA3
    GPIOA->MODER |= (0xA << 4);      // Set alternate function mode (10) for PA2 and PA3
    GPIOA->AFR[0] |= (0x7 << 8);     // Set AF7 for PA2 (USART2_TX)
    GPIOA->AFR[0] |= (0x7 << 12);    // Set AF7 for PA3 (USART2_RX)

    // Configure USART2 settings
    USART2->CR1 &= ~USART_CR1_UE;    // Disable USART to configure
    USART2->CR1 |= USART_CR1_M;      // 8 data bits
    USART2->CR1 |= USART_CR1_PCE;    // Enable parity
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable transmitter and receiver

    // Set baud rate
    uint32_t pclk1 = 42000000;       // Assuming PCLK1 is 42 MHz (may vary)
    uint32_t mantissa = pclk1 / (16 * baudrate);
    uint32_t fraction = ((pclk1 % (16 * baudrate)) * 16) / baudrate;
    USART2->BRR = (mantissa << 4) | (fraction & 0xF); // Set BRR value

    // Enable USART2
    USART2->CR1 |= USART_CR1_UE;

    // Enable DMA for USART2 TX
    USART2->CR3 |= USART_CR3_DMAT;

    // DMA setup for USART2 TX (Channel 4)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;  // Enable DMA1 clock
    DMA1_Stream6->CR &= ~DMA_SxCR_EN;    // Disable DMA stream before configuring

    // Configure DMA for memory-to-peripheral transfer (USART2)
    DMA1_Stream6->CR |= (USART2_TX_DMA_CHANNEL << DMA_SxCR_CHSEL_Pos);  // Select channel 4
    DMA1_Stream6->CR |= DMA_SxCR_DIR_0;  // Memory to peripheral transfer
    DMA1_Stream6->CR |= DMA_SxCR_MINC;   // Enable memory increment
    DMA1_Stream6->CR |= DMA_SxCR_TCIE;   // Enable transfer complete interrupt
    DMA1_Stream6->CR |= (0x2 << 16);     // High priority for the stream

    // Enable DMA interrupt in NVIC
    NVIC_EnableIRQ(DMA1_STREAM6_IRQ);

    // Enable DMA stream
    DMA1_Stream6->CR |= DMA_SxCR_EN;    // Enable the DMA stream
}

// Send a character using DMA
void uart2_send_char_dma(uint8_t c) {
    // Wait until TX buffer is empty (USART_TC bit)
    while (!(USART2->SR & USART_SR_TC));

    // Load the character into the DR register
    USART2->DR = c;
}

// Send a string using DMA
void uart2_send_str_dma(volatile uint8_t *msg) {
    // Wait until TX buffer is empty
    while (!(USART2->SR & USART_SR_TC));

    // Setup DMA to send the string (memory-to-peripheral)
    DMA1_Stream6->M0AR = (uint32_t)msg;  // Set source address
    DMA1_Stream6->NDTR = strlen((const char*)msg); // Set number of bytes to transfer

    // Start the DMA transfer
    DMA1_Stream6->CR |= DMA_SxCR_EN; // Enable the DMA stream
}

// DMA interrupt handler for TX complete
void DMA1_Stream6_IRQHandler(void) {
    if (DMA1->HISR & DMA_HISR_TCIF6) {  // Check if transfer complete interrupt flag is set
        // Clear interrupt flag
        DMA1->HIFCR |= DMA_HIFCR_CTCIF6;

        // Handle any post-processing (e.g., signal completion or update LEDs)
    }
}

// Receive a character using DMA (if needed)
uint8_t uart2_rcv_char_dma(void) {
    // Check if data is available (RXNE bit)
    if (USART2->SR & USART_SR_RXNE) {
        return USART2->DR;  // Read the received byte
    }
    return 0;  // No data received
}


