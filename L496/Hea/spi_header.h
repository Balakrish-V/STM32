#ifndef SPI_H
#define SPI_H

#include "lib_headers.h"

// SPI Mode Macros
#define SPI_MODE_0 (0)
#define SPI_MODE_1 (1 << SPI_CR1_CPHA_Pos)
#define SPI_MODE_2 (1 << SPI_CR1_CPOL_Pos)
#define SPI_MODE_3 ((1 << SPI_CR1_CPOL_Pos) | (1 << SPI_CR1_CPHA_Pos))

// SPI Speed Macros (assuming 80MHz system clock)
#define SPI_SPEED_2MHZ   (5 << SPI_CR1_BR_Pos)   // fPCLK/32 (80MHz/32 = 2.5MHz)
#define SPI_SPEED_1MHZ   (6 << SPI_CR1_BR_Pos)   // fPCLK/64 (80MHz/64 = 1.25MHz)
#define SPI_SPEED_500KHZ (7 << SPI_CR1_BR_Pos)   // fPCLK/128 (80MHz/128 = 625KHz)
#define SPI_SPEED_250KHZ (8 << SPI_CR1_BR_Pos)   // fPCLK/256 (80MHz/256 = 312.5KHz)
#define SPI_SPEED_125KHZ (9 << SPI_CR1_BR_Pos)   // fPCLK/512 (80MHz/512 = 156.25KHz)

// SPI Role Macros
#define SPI_MASTER_MODE (SPI_CR1_MSTR)
#define SPI_SLAVE_MODE  (0)  // No bits set for slave mode

#define SPI_CR1_DFF_Pos         (11U)
#define SPI_CR1_DFF_Msk         (0x1UL << SPI_CR1_DFF_Pos)                    /*!< 0x00000800 */
#define SPI_CR1_DFF             SPI_CR1_DFF_Msk                               /*!< Data Frame Format */

// Define the possible clock sources
#define SPI_HSE_CLOCK    16000000U  // 16 MHz
#define SPI_SYS_CLOCK    80000000U  // 80 MHz

// Macro to select the reference clock source for SPI
#define SPI_REF_CLOCK    SPI_SYS_CLOCK  // or HSE_CLOCK, depending on your configuration 

// Define the desired SPI speed (e.g., 1 MHz)
#define SPI_TARGET_SPEED 1000000U

// Function prototypes
void SPIConfig(uint32_t mode, uint32_t speed, uint32_t role);
uint32_t SPI_CalculatePrescaler(uint32_t spi_target_speed);
void GPIOConfig(void);
void SPI_Enable(void);
void SPI_Disable(void);
// Enable and Disable Chip Select (CS)
void CS_Low(void);
void CS_High(void);
void SPI_Transmit_byte(uint8_t *data);
uint8_t SPI_Receive_byte(void); 
void SPI_TransmitString(uint8_t *data, uint16_t size);
void SPI_ReceiveString(uint8_t *buffer, uint16_t size);

#endif