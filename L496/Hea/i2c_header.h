#ifndef I2C_H
#define I2C_H

#include "lib_headers.h"

// I2C Speed Macros
#define I2C_SPEED_STANDARD   100000  // 100 kHz
#define I2C_SPEED_FAST       400000  // 400 kHz
#define I2C_SPEED_FAST_PLUS  1000000 // 1 MHz

// Timing configuration for different speeds
#define I2C_TIMING_STANDARD  ((19 << I2C_TIMINGR_SCLL_Pos) | \
                              (15 << I2C_TIMINGR_SCLH_Pos) | \
                              (2 << I2C_TIMINGR_SDADEL_Pos) | \
                              (4 << I2C_TIMINGR_SCLDEL_Pos) | \
                              (1 << I2C_TIMINGR_PRESC_Pos)) 

#define I2C_TIMING_FAST      ((8 << I2C_TIMINGR_SCLL_Pos) | \
                              (9 << I2C_TIMINGR_SCLH_Pos) | \
                              (2 << I2C_TIMINGR_SDADEL_Pos) | \
                              (4 << I2C_TIMINGR_SCLDEL_Pos) | \
                              (0 << I2C_TIMINGR_PRESC_Pos))

#define I2C_TIMING_FAST_PLUS ((3 << I2C_TIMINGR_SCLL_Pos) | \
                              (4 << I2C_TIMINGR_SCLH_Pos) | \
                              (1 << I2C_TIMINGR_SDADEL_Pos) | \
                              (2 << I2C_TIMINGR_SCLDEL_Pos) | \
                              (0 << I2C_TIMINGR_PRESC_Pos))

void I2C_Config(uint8_t dev_own_addr, uint32_t i2c_speed); 
int I2C_Scanner(uint8_t slave_addr);
int I2C_Transmit(uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_to_send);
int I2C_Receive(uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_received);
void I2C1_gpio_config(void);

#endif