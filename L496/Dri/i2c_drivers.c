#ifndef I2C_C
#define I2C_C

#include "lib_headers.h"

void I2C1_gpio_config(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;      // Enable GPIOB clock

	  // 2) Set GPIO parameters (mode, speed, pull-up) - PB8 SCL, PB9 SDA
		GPIOB->MODER &= ~(3 << (8 * 2));          // Clear mode for PB8
		GPIOB->MODER |= (2 << (8 * 2));           // Alternate function for PB8
		GPIOB->MODER &= ~(3 << (9 * 2));          // Clear mode for PB9
		GPIOB->MODER |= (2 << (9 * 2));           // Alternate function for PB9

		GPIOB->OTYPER |= (1 << 8);                // Open drain for PB8
		GPIOB->OTYPER |= (1 << 9);                // Open drain for PB9

		GPIOB->OSPEEDR |= (3 << (8 * 2));         // High speed PB8
		GPIOB->OSPEEDR |= (3 << (9 * 2));         // High speed PB9

		GPIOB->PUPDR |= (1 << (8 * 2));           // Pull-up PB8
		GPIOB->PUPDR |= (1 << (9 * 2));           // Pull-up PB9

		// Set alternate function for I2C1 (AF4 for PB8 and PB9)
		GPIOB->AFR[1] |= (4 << (0 * 4));          // PB8 AF4 setup
		GPIOB->AFR[1] |= (4 << (1 * 4));          // PB9 AF4 setup
}

void I2C_Config(uint8_t dev_own_addr, uint32_t i2c_speed) 
{
    // 1) Enable clocking in the RCC for I2C1 and GPIOB
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;  // Enable I2C1 clock
    RCC->CCIPR &= ~(3 << RCC_CCIPR_I2C1SEL_Pos); // Clear APB1 clock source for I2C1
		// 3) Set the clock source for I2C
		I2C1->CR1 &= ~I2C_CR1_PE;                 // Disable I2C for configuration
		delay_ms(1);                              // Wait for the disable to take effect

		// 4) Set timing for I2C based on speed
		if (i2c_speed == I2C_SPEED_STANDARD) {
			I2C1->TIMINGR = I2C_TIMING_STANDARD;
		} else if (i2c_speed == I2C_SPEED_FAST) {
			I2C1->TIMINGR = I2C_TIMING_FAST;
		} else if (i2c_speed == I2C_SPEED_FAST_PLUS) {
			I2C1->TIMINGR = I2C_TIMING_FAST_PLUS;
		} else {
    // Invalid speed; you can handle this as needed
		}

		// 5) Set own address	
		I2C1->OAR1 &= ~I2C_OAR1_OA1EN;             // Disable the own address
		I2C1->OAR1 &= ~I2C_OAR1_OA1MODE;           // Choose 7-bit address mode
		I2C1->OAR1 |= (dev_own_addr << 1);          // Set the own address
		I2C1->OAR1 |= I2C_OAR1_OA1EN;               // Enable the own address

		// 6) Clean up the ISR register
		I2C1->ISR |= I2C_ISR_TXIS;                  // Flush the transmit register

		// 7) Enable I2C
		I2C1->CR1 |= I2C_CR1_PE;                    // Enable I2C
		I2C1->CR1 |= I2C_CR1_ANFOFF;                // Enable analog filter
		I2C1->CR1 |= I2C_CR1_NOSTRETCH;             // Enable clock stretching
		delay_ms(1);                                // Wait for setup to take effect
}

// Function to scan for I2C devices
int I2C_Scanner(uint8_t slave_addr) 
{
    uint8_t reply = 0;

    // 1) Reset the transmission control register
    I2C1->CR2 = 0x0;                      // Reset the CR2 register

    // 2) Set the slave address and addressing mode
    I2C1->CR2 = (slave_addr << 1);        // Set slave address
    I2C1->CR2 &= ~(1 << 11);              // 7-bit address mode

    // 3) Set NBYTES to 0
    I2C1->CR2 |= (0 << 16);               // No bytes to send

    // 4) Enable AUTOEND and no RELOAD
    I2C1->CR2 |= (1 << 25);               // Enable AUTOEND
    I2C1->CR2 &= ~(1 << 24);              // Disable RELOAD

    // 5) Enable I2C and set the START bit
    I2C1->CR1 |= (1 << 0);                // Enable I2C
    I2C1->CR2 |= (1 << 13);               // Generate START condition

    // 6) Wait for a reply
    delay_ms(1);                          // Allow time for response

    // 7) Check for NACK
    if ((I2C1->ISR & (1 << 4)) != 0) {    // If NACKF bit is set
        reply = 0;                        // No ACK received
    } else {
        reply = 1;                        // ACK received
    }

    // 8) Reset the bus and all the registers in ISR
    I2C1->CR1 &= ~(1 << 0);               // Disable I2C
    delay_ms(1);                          // Wait after PE is cleared
    return reply;
}

int I2C_Transmit(uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_to_send) 
{
    // 1) Reset and configure CR2 register for transmission
    I2C1->CR2 = (slave_addr << 1) | (number_of_bytes << 16) | I2C_CR2_AUTOEND;

    // 2) Enable I2C peripheral if not already enabled
    if (!(I2C1->CR1 & I2C_CR1_PE)) {
        I2C1->CR1 |= I2C_CR1_PE;
    }

    // 3) Generate START condition
    I2C1->CR2 |= I2C_CR2_START;

    // 4) Transmit each byte
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        // Wait until TXIS flag is set (indicating TXDR is ready for data)
        while (!(I2C1->ISR & I2C_ISR_TXIS)) {}

        // Write data to TXDR
        I2C1->TXDR = bytes_to_send[i];
    }

    // 5) Wait for the STOP condition after the last byte is transmitted
    while (!(I2C1->ISR & I2C_ISR_STOPF)) {}

    // 6) Clear the STOP flag to reset the I2C peripheral for the next transmission
    I2C1->ICR |= I2C_ICR_STOPCF;

    return 1;  // Transmission successful
}

int I2C_Receive(uint8_t slave_addr, uint8_t number_of_bytes, uint8_t *bytes_received) 
{
    // 1) Reset and configure CR2 register for reception
    I2C1->CR2 = (slave_addr << 1) | (1 << 10) | (number_of_bytes << 16) | I2C_CR2_AUTOEND;

    // 2) Enable I2C peripheral if not already enabled
    if (!(I2C1->CR1 & I2C_CR1_PE)) {
        I2C1->CR1 |= I2C_CR1_PE;
    }

    // 3) Generate START condition
    I2C1->CR2 |= I2C_CR2_START;

    // 4) Receive each byte
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        // Wait until RXNE flag is set (indicating data is received)
        while (!(I2C1->ISR & I2C_ISR_RXNE)) {}

        // Read data from RXDR
        bytes_received[i] = I2C1->RXDR;
    }

    // 5) Wait for the STOP condition to confirm the end of reception
    while (!(I2C1->ISR & I2C_ISR_STOPF)) {}

    // 6) Clear the STOP flag to reset the I2C peripheral for the next reception
    I2C1->ICR |= I2C_ICR_STOPCF;

    return 1;  // Reception successful
}

#endif
