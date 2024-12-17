#ifndef SPI_H
#define SPI_H

#include "stm32f4xx.h"

// SPI Mode Selection Macros
#define SPI_MODE_MASTER   1
#define SPI_MODE_SLAVE    0

// Define symbolic constants for SPI clock polarity and phase modes
#define SPI_MODE_0 0  // CPOL = 0, CPHA = 0
#define SPI_MODE_1 1  // CPOL = 0, CPHA = 1
#define SPI_MODE_2 2  // CPOL = 1, CPHA = 0
#define SPI_MODE_3 3  // CPOL = 1, CPHA = 1

// Define common baud rates
#define SPI_BAUDRATE_1MBPS 1000000
#define SPI_BAUDRATE_2MBPS 2000000

#define SPI_LSB_FIRST 0
#define SPI_MSB_FIRST 1

void SPI_Init(SPI_TypeDef *SPIx, uint8_t mode, uint32_t speed, uint8_t bitOrder);

// Chip Select control functions
void CS_Enable(void);
void CS_Disable(void);

// SPI Enable/Disable
void SPI_Disable(SPI_TypeDef *SPIx);
void SPI_Enable(SPI_TypeDef *SPIx);

// SPI Transmit and Receive functions
void SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *data, int size);
void SPI_Receive(SPI_TypeDef *SPIx, uint8_t *data, int size);

#endif