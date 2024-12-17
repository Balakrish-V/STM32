#include "All_headers.h"

void I2C_Init(I2C_TypeDef *I2Cx, uint8_t mode, uint32_t speed, uint8_t slave_addr) 
{
    // Enable the respective I2C and GPIO clocks
    if (I2Cx == I2C1) {
        RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
        GPIOB->MODER &= ~(3U << (6 * 2) | 3U << (9 * 2));
        GPIOB->MODER |= (2 << (6 * 2)) | (2 << (9 * 2)); // Alternate function for PB6 and PB9
        GPIOB->OTYPER |= (1 << 6) | (1 << 9);            // Open-drain
        GPIOB->AFR[0] |= (4 << (6 * 4));                 // AF4 for PB6
        GPIOB->AFR[1] |= (4 << ((9 - 8) * 4));           // AF4 for PB9
    }
    // Add cases for I2C2 and I2C3 as needed
    I2Cx->CR1 = I2C_CR1_SWRST; // Reset I2C peripheral
    I2Cx->CR1 = 0;

    // Set peripheral clock frequency (assuming system clock is 84 MHz)
    uint32_t pclk1 = 16000000;      // Example for STM32F4, update this as per your system
    I2Cx->CR2 = (pclk1 / 1000000U); // Peripheral clock frequency in MHz

    // Configure speed settings based on desired frequency
    if (speed <= 100000) {
        // Standard mode (100 kHz)
        I2Cx->CCR = (pclk1 / (2 * speed)); // CCR formula for standard mode
        I2Cx->TRISE = 11;                  // Maximum rise time for 100 kHz
    } else {
        // Fast mode (400 kHz)
        I2Cx->CCR = (pclk1 / (3 * speed)); // CCR formula for fast mode
        I2Cx->TRISE = 3;                   // Maximum rise time for 400 kHz
    }

    // Configure the I2C mode (Master/Slave)
    if (mode == I2C_MODE_MASTER) {
        I2Cx->OAR1 = 0; // Own address not used in master mode
    } else {
        I2Cx->OAR1 = (slave_addr << 1) | (1 << 14); // Set own address for slave mode
    }

    // Enable I2C peripheral
    I2Cx->CR1 |= I2C_CR1_PE;
}

#include <stdio.h>

#define I2C_TIMEOUT 1000 // Define a timeout value (in iterations)

static int wait_for_flag(volatile uint32_t *reg, uint32_t flag) {
    for (int i = 0; i < I2C_TIMEOUT; i++) {
        if (*reg & flag) {
            return 1; // Flag is set
        }
    }
    return 0; // Timeout
}

int I2C_Transmit(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_to_send) 
{
    // 1) Send start condition
    I2Cx->CR1 |= I2C_CR1_START;
    if (!wait_for_flag(&I2Cx->SR1, I2C_SR1_SB)) {
        printf("Start condition failed\n");
        return -1; // Timeout or failure
    }

    // 2) Send slave address in write mode
    I2Cx->DR = (slave_addr << 1); // Slave address + write bit
    if (!wait_for_flag(&I2Cx->SR1, I2C_SR1_ADDR)) {
        printf("Address ACK failure\n");
        return -1; // Timeout or failure
    }
    (void)I2Cx->SR2; // Clear ADDR flag

    // 3) Transmit data bytes
    for (int i = 0; i < number_of_bytes; i++) {
        I2Cx->DR = bytes_to_send[i]; // Write data to DR
        if (!wait_for_flag(&I2Cx->SR1, I2C_SR1_BTF)) {
            printf("Data ACK failure at byte %d\n", i);
            return -1; // Timeout or failure
        }
    }

    // 4) Send stop condition
    I2Cx->CR1 |= I2C_CR1_STOP;
    // Optionally wait a bit or check for stop condition if needed

    printf("All bytes sent successfully\n");
    return 0; // Success
}

#include <stdio.h>

int I2C_Receive(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_received) 
{
    // 1) Send start condition
    I2Cx->CR1 |= I2C_CR1_START;
    if (!wait_for_flag(&I2Cx->SR1, I2C_SR1_SB)) {
        printf("Start condition failed\n");
        return -1; // Timeout or failure
    }

    // 2) Send slave address in read mode
    I2Cx->DR = (slave_addr << 1) | 0x01; // Slave address + read bit
    if (!wait_for_flag(&I2Cx->SR1, I2C_SR1_ADDR)) {
        printf("Address ACK failure\n");
        return -1; // Timeout or failure
    }
    (void)I2Cx->SR2; // Clear ADDR flag

    // 3) Receive data bytes
    for (int i = 0; i < number_of_bytes; i++) {
        if (i == (number_of_bytes - 1)) {
            // Last byte: Disable ACK, generate STOP condition
            I2Cx->CR1 &= ~I2C_CR1_ACK; // Disable ACK
            I2Cx->CR1 |= I2C_CR1_STOP; // Generate STOP condition
        }

        // Wait until data is received
        if (!wait_for_flag(&I2Cx->SR1, I2C_SR1_RXNE)) {
            printf("Data ACK failure at byte %d\n", i);
            return -1; // Timeout or failure
        }

        bytes_received[i] = I2Cx->DR; // Read received byte from DR
    }

    // 4) Re-enable ACK for next communications (if needed)
    I2Cx->CR1 |= I2C_CR1_ACK;

    // Optional: Wait for the STOP condition to be sent
    // This is not always necessary but may help in some cases
    // wait_for_flag(&I2Cx->SR1, I2C_SR1_STOPF);

    printf("All bytes received successfully\n");
    return 0; // Success
}