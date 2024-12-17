#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include "stm32l496xx.h"

// Clock bit macros for GPIO ports
#define GPIOA_CLK_BIT 0
#define GPIOB_CLK_BIT 1
#define GPIOC_CLK_BIT 2
#define GPIOD_CLK_BIT 3
#define GPIOE_CLK_BIT 4
#define GPIOF_CLK_BIT 5
#define GPIOG_CLK_BIT 6
#define GPIOH_CLK_BIT 7
#define GPIOI_CLK_BIT 8

typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
    GPIO_PORT_D = 3,
    GPIO_PORT_E = 4,
    GPIO_PORT_F = 5,
    GPIO_PORT_G = 6,
    GPIO_PORT_H = 7,
    GPIO_PORT_I = 8
} GPIO_Port;

// Custom GPIO and RCC type definitions
typedef struct {
    volatile uint32_t MODER;   // Mode register
    volatile uint32_t OTYPER;  // Output type register
    volatile uint32_t OSPEEDR; // Output speed register
    volatile uint32_t PUPDR;   // Pull-up/pull-down register
    volatile uint32_t IDR;     // Input data register
    volatile uint32_t ODR;     // Output data register
    volatile uint32_t BSRR;    // Bit Set/Reset Register
    volatile uint32_t BRR;     // Bit Reset Register
    volatile uint32_t LCKR;    // Configuration lock register
} MY_GPIO_TypeDef;

typedef struct {
    volatile uint32_t AHB2ENR;
    volatile uint32_t CR;
    volatile uint32_t CFGR;
} MY_RCC_TypeDef;

// GPIO mode and pull-up/down macros
#define GPIO_MODE_INPUT          0x00
#define GPIO_MODE_OUTPUT         0x01
#define GPIO_NO_PULL             0x00
#define GPIO_PULL_UP             0x01
#define GPIO_PULL_DOWN           0x02

#define ENABLE_GPIO_CLOCK(port, clk_bit) \
    (RCC->AHB2ENR |= (1 << (clk_bit))); \
    GPIO = (MY_GPIO_TypeDef *)(port##_BASE)


// Function prototypes
void GPIO_Pin_Init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t pull);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t PinState); 
void Enable_GPIO_Clock(GPIO_Port port);
void EXTI_Init(GPIO_TypeDef *port, uint32_t pin, uint8_t interrupt_type);
void GPIO_I2C_Init(void);
uint8_t DebounceButton(uint8_t max_deb_delay);

// Macros for interrupt type
#define INTERRUPT_RISING_EDGE      0x01
#define INTERRUPT_FALLING_EDGE     0x02
#define INTERRUPT_RISING_FALLING   0x03

#endif // GPIO_CONTROL_H
