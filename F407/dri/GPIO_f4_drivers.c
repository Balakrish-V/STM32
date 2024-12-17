#ifndef GPIO_CONTROL_C
#define GPIO_CONTROL_C

#include "All_headers.h"

// STM32F4 has GPIO registers under AHB1 peripheral clock
void Enable_GPIO_Clock(GPIO_Port port) {
    switch (port) {
        case GPIO_PORT_A: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; 
				break;
        case GPIO_PORT_B: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 
				break;
        case GPIO_PORT_C: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; 
				break;
        case GPIO_PORT_D:
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 
				break;
        case GPIO_PORT_E: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; 
				break;
        case GPIO_PORT_F: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; 
				break;
        case GPIO_PORT_G: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; 
				break;
        case GPIO_PORT_H: 
					RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; break;
        case GPIO_PORT_I: RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN; 
				break;
        default: 
					break; // Handle invalid port if necessary
    }
}

// For STM32F4, use the GPIO registers for configuration
void GPIO_Pin_Init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t pull) {
    MY_GPIO_TypeDef *GPIO = NULL;

    // enable clock and set GPIO pointer based on the port
    switch (port) {
        case 'A': 
					ENABLE_GPIO_CLOCK(GPIOA, 0); break;
        case 'B': 
					ENABLE_GPIO_CLOCK(GPIOB, 1); break;
        case 'C': 
					ENABLE_GPIO_CLOCK(GPIOC, 2); break;
        case 'D': 
					ENABLE_GPIO_CLOCK(GPIOD, 3); break;
        case 'E': 
					ENABLE_GPIO_CLOCK(GPIOE, 4); break;
        case 'F': 
					ENABLE_GPIO_CLOCK(GPIOF, 5); break;
        case 'G': 
					ENABLE_GPIO_CLOCK(GPIOG, 6); break;
        case 'H': 
					ENABLE_GPIO_CLOCK(GPIOH, 7); break;
        case 'I': 
					ENABLE_GPIO_CLOCK(GPIOI, 8); break;
        default: 
					return; // invalid port
    }

    // configure pin mode
    GPIO->MODER &= ~(3 << (pin * 2)); // clear mode bits
    GPIO->MODER |= (mode << (pin * 2)); // set mode

    // if output mode, set push-pull type
    if (mode == GPIO_MODE_OUTPUT) {
        GPIO->OTYPER &= ~(1 << pin); // push-pull type
    }

    // set low speed (default)
    GPIO->OSPEEDR &= ~(3 << (pin * 2));

    // configure pull-up/pull-down resistors
    GPIO->PUPDR &= ~(3 << (pin * 2)); // clear pull bits
    GPIO->PUPDR |= (pull << (pin * 2)); // set pull-up/down
}

// Function to write a pin (STM32F4 version)
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t PinState) {
    if (PinState == 1) {
        GPIOx->BSRR = GPIO_Pin; // Set the pin (GPIO_Pin is written directly)
    } else {
        GPIOx->BSRR = (uint32_t)GPIO_Pin << 16; // Reset the pin (GPIO_Pin << 16 to reset)
    }
}

// Function to initialize EXTI for PA0
void EXTI_Init(GPIO_TypeDef *port, uint32_t pin, uint8_t interrupt_type) {
    // Enable SYSCFG and GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock

    // Configure the EXTI line for the specified pin (PA0)
    uint32_t exticr_value = (port == GPIOA) ? 0 : 0; // Only PA0 supported in this case
    SYSCFG->EXTICR[pin / 4] &= ~(0xF << (pin % 4) * 4); // Clear previous configuration
    SYSCFG->EXTICR[pin / 4] |= (exticr_value << (pin % 4) * 4); // Set EXTI line for PA0

    // Unmask the interrupt on the given EXTI line (EXTI0 for PA0)
    EXTI->IMR |= (1 << pin); // Unmask interrupt

    // Set trigger type based on interrupt_type
    if (interrupt_type == INTERRUPT_RISING_EDGE) {
        EXTI->RTSR |= (1 << pin);  // Enable rising edge trigger
        EXTI->FTSR &= ~(1 << pin); // Disable falling edge trigger
    } else if (interrupt_type == INTERRUPT_FALLING_EDGE) {
        EXTI->RTSR &= ~(1 << pin); // Disable rising edge trigger
        EXTI->FTSR |= (1 << pin);  // Enable falling edge trigger
    } else if (interrupt_type == INTERRUPT_RISING_FALLING) {
        EXTI->RTSR |= (1 << pin);  // Enable rising edge trigger
        EXTI->FTSR |= (1 << pin);  // Enable falling edge trigger
    }

    // Set the interrupt priority and enable it in NVIC
    NVIC_SetPriority(EXTI0_IRQn, 0x01); // Set priority (lower value = higher priority)
    NVIC_EnableIRQ(EXTI0_IRQn);          // Enable EXTI0 interrupt in NVIC
}

// Interrupt Handler for EXTI0 (PA0)
volatile uint8_t button_pressed = 0; // Flag to track button press status

void EXTI0_IRQHandler(void) {
    // Check if the interrupt is for PA0 (EXTI0)
    if (EXTI->PR & EXTI_PR_PR0) {
        // Handle debounce
        if (DebounceButton(10)) {
            // Only toggle LED if button is confirmed pressed
            All_LEDs_Toggle_F4();
        }
        // Clear the interrupt flag
        EXTI->PR |= EXTI_PR_PR0;
    }
}

// Debounce function to avoid false triggers
uint8_t DebounceButton(uint8_t max_deb_delay) {
    // Small delay to allow for debounce
    delay_ms(max_deb_delay);

    // Check if button is still pressed after delay
    if ((GPIOA->IDR & GPIO_IDR_ID0) == 0) { // Assuming button is active low
        // Button is confirmed as pressed
        if (!button_pressed) {
            button_pressed = 1; // Set flag to indicate button press detected
            return 1; // Valid button press
        }
    } else {
        // Button is not pressed
        button_pressed = 0; // Reset the flag
    }
    return 0; // No valid button press detected
}

#endif // GPIO_CONTROL_C
