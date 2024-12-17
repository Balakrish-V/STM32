#include "All_headers.h"

uint32_t SPI_CalculatePrescaler(uint32_t speed, uint32_t refClock) 
{
    uint32_t prescaler = 0; // Initialize the prescaler value to 0

    // Loop to find the appropriate prescaler value
    while (speed < (refClock / (2 << prescaler)) && prescaler < 8) {
        prescaler++; // Increment the prescaler value
    }

    // Return the calculated prescaler value
    return (prescaler << SPI_CR1_BR_Pos); 
}

void CS_Enable(void) {
    GPIOC->BSRR = (1 << 5) << 16;  // Pull CS low
}

void CS_Disable(void) {
    GPIOC->BSRR = (1 << 5);  // Pull CS high
}

void SPI_Init(SPI_TypeDef *SPIx, uint8_t mode, uint32_t speed, uint8_t bitOrder) 
{
    uint32_t refClock;

    if (SPIx == SPI1) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;    
    } 

    refClock = 16000000; // 16 MHz HSI

    // Configure GPIO for SPI
    GPIOA->MODER &= ~(0x3F << 10);   
    GPIOA->MODER |= (0x2A << 10);    
    GPIOA->AFR[0] |= (0x555 << 20);  

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;  
    GPIOC->MODER &= ~(0x3 << (5 * 2));    
    GPIOC->MODER |= (0x1 << (5 * 2));     
    GPIOC->OTYPER &= ~(0x1 << 5);         
    GPIOC->OSPEEDR |= (0x3 << (5 * 2));   
    GPIOC->PUPDR &= ~(0x3 << (5 * 2));    
    GPIOC->ODR |= (0x1 << 5); // Pull CS high initially

    SPIx->CR1 = 0;                               
    SPIx->CR1 |= SPI_CalculatePrescaler(speed, refClock);  
    SPIx->CR1 &= ~SPI_CR1_DFF;                   
    SPIx->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;      
    SPIx->CR1 |= (1 << SPI_CR1_MSTR_Pos);        

    switch (mode) {
        case 0: 
            SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
            break;
        case 1: 
            SPIx->CR1 &= ~SPI_CR1_CPOL;
            SPIx->CR1 |= SPI_CR1_CPHA;
            break;
        case 2: 
            SPIx->CR1 |= SPI_CR1_CPOL;
            SPIx->CR1 &= ~SPI_CR1_CPHA;
            break;
        case 3: 
            SPIx->CR1 |= (SPI_CR1_CPOL | SPI_CR1_CPHA);
            break;
        default:
            break;
    }
  
    if (bitOrder == 0) {
        SPIx->CR1 &= ~SPI_CR1_LSBFIRST;          
    } else {
        SPIx->CR1 |= SPI_CR1_LSBFIRST;           
    }
    SPIx->CR1 |= SPI_CR1_SPE;                    
}

void SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *data, int size) 
{
    CS_Enable(); // Pull CS low before transmission
	  LED_On_F4(LED_PIN_15);
    for (int i = 0; i < size; i++) 
    {
        while (!(SPIx->SR & SPI_SR_TXE));  // Wait until TXE is set
        SPIx->DR = data[i];                // Transmit data

        // Wait until the transmission is complete
        while (SPIx->SR & SPI_SR_BSY); 

    }
    CS_Disable(); // Pull CS high after transmission
		LED_Off_F4(LED_PIN_15);
}

void SPI_Receive(SPI_TypeDef *SPIx, uint8_t *data, int size) 
{
    CS_Enable(); // Pull CS low before reception
	  LED_On_F4(LED_PIN_14);
    for (int i = 0; i < size; i++) 
    {
        while (!(SPIx->SR & SPI_SR_TXE));  // Wait until TXE is set
        SPIx->DR = 0xFF;                   // Send dummy byte to generate clock
        
        while (!(SPIx->SR & SPI_SR_RXNE)); // Wait until RXNE is set
        data[i] = SPIx->DR;                // Read received data
    }
    while (SPIx->SR & SPI_SR_BSY); // Wait until SPI is not busy
    CS_Disable(); // Pull CS high after reception
		LED_Off_F4(LED_PIN_14);
}