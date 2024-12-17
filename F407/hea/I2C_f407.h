#ifndef I2C_H
#define I2C_H

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

// Define macro for master/slave mode
#define I2C_MODE_MASTER 0
#define I2C_MODE_SLAVE  1

// Function prototypes
void I2C_Init(I2C_TypeDef *I2Cx, uint8_t mode, uint32_t speed, uint8_t slave_addr);
int I2C_Transmit(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_to_send);
int I2C_Receive(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_received);

#endif