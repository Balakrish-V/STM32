#include "lib_headers.h"

#define SPI1EN          (1U << 12)
#define GPIOAEN         (1U << 0)
#define SR_TXE          (1U << 1)
#define SR_RXNE         (1U << 0)
#define SR_BSY          (1U << 7)

const uint8_t write_data[] = {'1','2','3','4'};  // Data to write (4 bytes)

uint8_t read_data[4] = {0};           // Buffer to store received data (4 bytes)
uint8_t reg_address = 0x01;           // Example register address

void delay(volatile uint32_t count) 
{
    while (count--);
}

void spi1_transmit(uint8_t *data, uint32_t size) 
{
    for (uint32_t i = 0; i < size; i++) 
    {
        // Wait until TXE is set before writing data
        while (!(SPI1->SR & SR_TXE)) {}
        
        // Send the data as a 16-bit value (shift the byte to the higher byte of DR)
        *(__IO uint16_t *)(&SPI1->DR) = data[i]<<8; // Send high byte first
        
        // Wait for the transmission to complete (BSY bit should be cleared)
        while (SPI1->SR & SR_BSY) {}
        
        // Clear the data register by reading it (dummy read)
        volatile uint16_t dummy = *(__IO uint16_t *)(&SPI1->DR);
        (void)dummy; // Prevent unused variable warning
    }
}

void spi1_receive(uint8_t *data, uint32_t size) 
{
    for (uint32_t i = 0; i < size; i++) 
	  {
        // Wait until TXE is set before sending a dummy byte
        while (!(SPI1->SR & SR_TXE)) {}
        
        // Send a dummy byte to generate clock
        *(__IO uint16_t *)(&SPI1->DR) = 0xFFFF; // Send dummy 0xFFFF for clock
        
        // Wait until RXNE is set to read the received data
        while (!(SPI1->SR & SR_RXNE)) {}
        
        // Read the received data
        data[i] = (*(__IO uint16_t *)(&SPI1->DR) & 0x00FF); // Read lower byte
    }
    
    // Ensure all transmissions are complete
    while (SPI1->SR & SR_BSY) {}
    
    // Clear the data register (optional, to prevent unused variable warning)
    volatile uint16_t dummy = *(__IO uint16_t *)(&SPI1->DR);
    (void)dummy; // Prevent unused variable warning
}

int main(void) 
{
    set_sysclk_to_hsi();
    uart2_config(9600);
    Initialize_LED_GPIOs();
    
    RCC->AHB2ENR |= GPIOAEN | (1 << 1); // Enable GPIOA and GPIOB
    RCC->APB2ENR |= SPI1EN; // SPI1 clock enable
    
    // Configure SPI1 pins for SCK, MISO, MOSI, and NSS
    GPIOA->MODER &= ~((3 << 10) | (3 << 12) | (3 << 14)); // Clear modes
    GPIOA->MODER |= (2 << 10) | (2 << 12) | (2 << 14);    // Set alternate mode
    GPIOA->AFR[0] |= (5 << 20) | (5 << 24) | (5 << 28);   // AF5 for SPI1
    
    GPIOA->MODER &= ~(3 << 8);   // Clear mode for PA4
    GPIOA->MODER |= (2 << 8);    // Set PA4 to alternate function mode
    GPIOA->AFR[0] |= (5 << 16);  // AF5 for SPI1_NSS
    
    Red_LED_Toggle();
    delay(400000);
    Red_LED_Toggle();
    
    // SPI1 configuration
    SPI1->CR1 = 0;                         // Clear all settings
    SPI1->CR1 |= (0 << 0) | (0 << 1);      // CPOL = 0, CPHA = 0
    SPI1->CR1 |= (3 << 3);                 // Baud rate (fPCLK/16) for 1 MHz
    SPI1->CR1 |= (0 << 10);                // Full duplex communication
    SPI1->CR1 |= (0 << 13);                // CRC disabled
    SPI1->CR1 &= ~(1U << 10);              // Enable full duplex
    SPI1->CR1 &= ~(1U << 7);               // Set MSB first
    SPI1->CR1 |= (1U << 2);                // Set mode to MASTER
    SPI1->CR1 &= ~(3U << 11);              // Set 8-bit data mode
    
    SPI1->CR2 = 0;
    SPI1->CR2 |= (1 << 2);                 // SSOE = 1 (SS output enable)
    SPI1->CR2 |= (0 << 3) | (0 << 4);      // Motorola frame format
    SPI1->CR2 &= ~(0xF << 8);              // Clear DS[3:0]
    SPI1->CR2 |= ( 7 << 8);                // Data size to 8 bits
    
    SPI1->CR1 |= (1 << 6); // Enable SPI
    
    // Transmit data
    spi1_transmit((uint8_t *)write_data, 4);
    
    // Receive data
    spi1_receive((uint8_t *)read_data, 4);
    
    // Send received data over UART
    uart2_send_string((char *)read_data, 4);
		
    uart2_send_string("\r\n", 2);
    
    // Toggle blue LED for testing
    while (1) {
        Blue_LED_Toggle();
        delay(200000);
    }
    
    return 0;
}

