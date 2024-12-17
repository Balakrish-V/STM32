#include "All_headers.h"

#define SLAVE_ADDRESS 0x0A

volatile uint8_t DeviceAddr = (SLAVE_ADDRESS << 1);

void GPIO_Init(void);
void I2C_Init(void);
void CS43L22_Reset(void);
uint8_t i2c_read(uint8_t regaddr);
void i2c_write(uint8_t regaddr, uint8_t data);
void i2c_send_bytes(uint8_t *data, uint8_t length);
void i2c_receive_bytes(uint8_t *buffer, uint8_t length);
void Error_Handler(void);

static inline void __i2c_start(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

static inline void __i2c_stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP);
}

static inline void __i2c_wait_busy(void) {
    while (I2C1->SR2 & I2C_SR2_BUSY);
}

void GPIO_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    // Configure LEDs on GPIOD (Pins 12, 13, 14, 15)
    GPIOD->MODER &= ~(0xFFU << 24);
    GPIOD->MODER |= (0x55 << 24);
    GPIOD->ODR = 0x0000;

    // Configure CS43L22 Reset pin (PD4)
    GPIOD->MODER &= ~(3U << 4 * 2);
    GPIOD->MODER |= (1 << 4 * 2);
}

void I2C_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Configure I2C pins (PB6, PB9) as Alternate Function Open-Drain
    GPIOB->MODER &= ~((3U << 6 * 2) | (3U << 9 * 2));
    GPIOB->MODER |= (2 << 6 * 2) | (2 << 9 * 2);
    GPIOB->OTYPER |= (1 << 6) | (1 << 9);

    // Set Alternate Function 4 for I2C1
    GPIOB->AFR[0] |= (4 << 6 * 4);
    GPIOB->AFR[1] |= (4 << (9 - 8) * 4);

    // Reset and initialize I2C1
    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0;
    I2C1->CR2 |= (42 << 0); // 42 MHz APB Clock
    I2C1->CCR |= (210 << 0); // Standard Mode 100kHz (CCR = F_APB1 / (2 * SCL))
    I2C1->TRISE |= (43 << 0); // Max rise time
    I2C1->OAR1 |= (0x00 << 1) | (1 << 14);
    I2C1->CR1 |= I2C_CR1_PE;

    // Enable I2C error interrupt
    NVIC_SetPriority(I2C1_ER_IRQn, 1);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void CS43L22_Reset(void) {
    GPIOD->ODR |= (1 << 4); // Activate CS43L22
}

uint8_t i2c_read(uint8_t regaddr) {
    uint8_t reg;

    __i2c_wait_busy();
    __i2c_start();

    I2C1->DR = DeviceAddr;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    I2C1->DR = regaddr;
    while (!(I2C1->SR1 & I2C_SR1_BTF));

    __i2c_start();
    I2C1->DR = DeviceAddr | 0x01;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    reg = (uint8_t)I2C1->DR;

    __i2c_stop();
    return reg;
}

void i2c_write(uint8_t regaddr, uint8_t data) {
    __i2c_wait_busy();
    __i2c_start();

    I2C1->DR = DeviceAddr;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    I2C1->DR = regaddr;
    while (!(I2C1->SR1 & I2C_SR1_BTF));

    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));

    __i2c_stop();
}

void i2c_send_bytes(uint8_t *data, uint8_t length) {
    __i2c_wait_busy();
    __i2c_start();

    I2C1->DR = DeviceAddr; // Send the device address for writing
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
        if (I2C1->SR1 & I2C_SR1_AF) {
            printf("Address ACK failure\n");
            __i2c_stop();
            return;
        }
    }
    (void)I2C1->SR2; // Clear the address flag

    for (uint8_t i = 0; i < length; i++) {
        I2C1->DR = data[i]; // Send data byte
        while (!(I2C1->SR1 & I2C_SR1_BTF)) {
            if (I2C1->SR1 & I2C_SR1_AF) {
                printf("Data ACK failure at byte %d\n", i);
                __i2c_stop();
                return;
            }
        }
    }
    __i2c_stop(); // Send stop condition
    printf("All bytes sent successfully\n");
}

void i2c_receive_bytes(uint8_t *buffer, uint8_t length) {
    __i2c_wait_busy();
    __i2c_start();

    I2C1->DR = DeviceAddr | 0x01;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
        if (I2C1->SR1 & I2C_SR1_AF) {
            printf("Address ACK failure\n");
            __i2c_stop();
            return;
        }
    }
    (void)I2C1->SR2;

    for (uint8_t i = 0; i < length; i++) {
        while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
            if (I2C1->SR1 & I2C_SR1_AF) {
                printf("Data ACK failure at byte %d\n", i);
                __i2c_stop();
                return;
            }
        }
        buffer[i] = (uint8_t)I2C1->DR;
        if (i == length - 1) {
            I2C1->CR1 &= ~I2C_CR1_ACK; // Send NACK for the last byte
        }
    }

    __i2c_stop();
    printf("All bytes received successfully\n");
}

void Error_Handler(void) {
    GPIOD->ODR |= (1 << 14); // Red LED
}

void I2C1_ER_IRQHandler(void) {
    Error_Handler();
}

int main(void) 
{
    set_sysclk_to_hsi();
    GPIO_Init();
    I2C_Init();
    CS43L22_Reset();

    uint8_t data_to_send[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t received_data[3];
    printf("hi\n");
    i2c_send_bytes(data_to_send, 5);
    i2c_receive_bytes(received_data, 3);

    while (1) {
        GPIOD->ODR ^= (1 << 12); // Toggle Green LED on completion
    }
}