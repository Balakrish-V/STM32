#ifndef SPI_C
#define SPI_C

#include "lib_headers.h"  // Include the appropriate header for STM32L4

// SPI1 Prescaler Calculation
uint32_t SPI_CalculatePrescaler(uint32_t spi_target_speed) 
{
    uint32_t prescaler = 0;
    if (SPI_REF_CLOCK / 2 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_0;                     // Prescaler = 2
    } else if (SPI_REF_CLOCK / 4 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_1;                     // Prescaler = 4
    } else if (SPI_REF_CLOCK / 8 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_1 | SPI_CR1_BR_0;      // Prescaler = 8
    } else if (SPI_REF_CLOCK / 16 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_2;                     // Prescaler = 16
    } else if (SPI_REF_CLOCK / 32 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_2 | SPI_CR1_BR_0;      // Prescaler = 32
    } else if (SPI_REF_CLOCK / 64 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_2 | SPI_CR1_BR_1;      // Prescaler = 64
    } else if (SPI_REF_CLOCK / 128 <= spi_target_speed) {
        prescaler = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;  // Prescaler = 128
    } else {
        prescaler = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;  // Prescaler = 256
    }
    return prescaler;
}

// Configure GPIO for SPI1
void GPIOConfig(void) 
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  // Enable GPIOA Clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;  // Enable GPIOB Clock

    // Configure PA5, PA6, PA7 for SPI1
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk);
    GPIOA->MODER |= (2 << GPIO_MODER_MODE5_Pos) | (2 << GPIO_MODER_MODE6_Pos) | (2 << GPIO_MODER_MODE7_Pos);  // Alternate function mode

    // Set alternate function for PA5, PA6, PA7 to SPI1 (AF5)
    GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL6_Pos) | (5 << GPIO_AFRL_AFSEL7_Pos);

    // Configure PB6 for CS
    GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;        // Clear mode
    GPIOB->MODER |= (1 << GPIO_MODER_MODE6_Pos);  // Output mode for CS

    // Set speed for SPI pins
    GPIOA->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED5_Pos) | (3 << GPIO_OSPEEDR_OSPEED6_Pos) | (3 << GPIO_OSPEEDR_OSPEED7_Pos);  // High speed for PA5, PA6, PA7
    GPIOB->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED6_Pos);  // High speed for PB6
}

// SPI1 Configuration
void SPIConfig(uint32_t mode, uint32_t speed, uint32_t role) 
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;          // Enable SPI1 Clock
    SPI1->CR1 = 0;                               // Reset CR1 to clear previous settings
    SPI1->CR1 |= mode;                           // Set SPI Mode (CPOL, CPHA)
    SPI1->CR1 |= role;                           // Set SPI Role (Master or Slave)
    SPI1->CR1 |= SPI_CalculatePrescaler(speed);  // Set Baud Rate Prescaler based on speed
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;              // Set MSB First
    SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI );    // Enable Hardware Slave Management
    SPI1->CR1 &= ~ SPI_CR1_RXONLY;                // Full-Duplex Mode
	  SPI1->CR2 = 0;                                // Reset CR2
    SPI1->CR2 &= ~SPI_CR2_DS_Msk;                // Clear Data Size bits
    SPI1->CR2 &= ~SPI_CR2_DS_Msk;                // Clear Data Size bits
    SPI1->CR2 |= SPI_CR2_DS_2 ;                   // Set Data Size to 16 bits
    SPI1->CR1 |= SPI_CR1_SPE;                    // Enable SPI1 (turn on the peripheral)
}

// Enable and Disable Chip Select (CS)
void CS_Low(void) {
    GPIOB->ODR &= ~(1 << 6);  // CS Low
}

void CS_High(void) {
    GPIOB->ODR |= (1 << 6);  // CS High
}

// SPI1 Transmit
void SPI_Transmit_byte(uint8_t *data) 
{
   // while (!(SPI1->SR & SPI_SR_TXE)){}       // Wait until TX buffer is empty
		CS_Low();                                // Pull CS low
	  *(__IO uint16_t *)(&SPI1->DR) = *data;
    while (SPI1->SR & SPI_SR_BSY);           // Wait until SPI is not busy
    CS_High();                               //CS high
    *data = (uint8_t)*data;			
}

// SPI1 Receive
uint8_t SPI_Receive_byte(void) 
{
    CS_Low();               //CS low                          // Pull CS low
    while (!(SPI1->SR & SPI_SR_RXNE));    // Wait until RX buffer is not empty
    uint8_t received_data = SPI1->DR;     // Read the received data
    while (SPI1->SR & SPI_SR_BSY);        // Wait until SPI is not busy
    CS_High();              //CS high	                          // CS remains low, since we need it to stay low during the entire string reception
    return received_data;
}

// SPI1 Transmit String
void SPI_TransmitString(uint8_t *data, uint16_t size) 
{
	  CS_Low();              //CS low  
    for (uint16_t i = 0; i < size; i++) 
    {
			 while (!(SPI1->SR & SPI_SR_TXE)){}  // Wait until TX buffer is empty
	     *(__IO uint16_t *)(&SPI1->DR) = data[i]; 
			 SPI1->CR1 &= ~SPI_CR1_SPE;         // Disable SPI	 
		   uint8_t dummy = *(__IO uint16_t *)(&SPI1->DR);          // Read the DR to clear it
       (void)dummy;                       // Prevent compiler warnings for unused variable
       SPI1->CR1 |= SPI_CR1_SPE;          // Re-enable SPI
    }
		CS_High();              //CS high	 
}

// SPI1 Receive String
void SPI_ReceiveString(uint8_t *data, uint16_t size) 
{
	  CS_Low();               //CS low  
    for (uint16_t i = 0; i < size; i++) 
    {
         while (!(SPI1->SR & 2)) {}      // wait until Transfer buffer Empty  
         *(__IO uint16_t *)(&SPI1->DR) = 0xFFFF;
         while (SPI1->SR & 0x80) {}      // wait for transmission done
         // Wait until RXNE flag indicates that data has been received
         while (!(SPI1->SR & (1 << 0))); 
         // Read the received data and store it in the buffer
         data[i] = *(__IO uint16_t *)(&SPI1->DR); 
				 data[i] = (uint8_t)data[i];	 
    }
    CS_High();              //CS high	  
}

#endif

/*
void SPI_TransmitReceiveString(const char *tx_data, char *rx_data, uint16_t length) 
{
    for (uint16_t i = 0; i < length; i += 2) 
    {
        // Wait until the transmit buffer is empty
        while (!(SPI1->SR & SPI_SR_TXE)) {}

        // Combine two bytes into a 16-bit word for transmission
        uint16_t tx_word = (tx_data[i] << 8) | tx_data[i + 1];
        SPI1->DR = tx_word;

        // Wait until a word is received
        while (!(SPI1->SR & SPI_SR_RXNE)) {}

        // Read the received 16-bit word
        uint16_t rx_word = SPI1->DR;

        // Store the received bytes in the correct order
        rx_data[i] = (uint8_t)(rx_word >> 8);      // High byte
        rx_data[i + 1] = (uint8_t)(rx_word & 0xFF); // Low byte

        // Ensure SPI transaction completes
        while (SPI1->SR & SPI_SR_BSY) {}
    }

    // Pull CS high after the transaction
    CS_High();

    // Null-terminate the received string for safety
    rx_data[length] = '\0';
}
*/
