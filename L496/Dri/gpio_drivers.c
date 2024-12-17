#ifndef GPIO_CONTROL_C
#define GPIO_CONTROL_C

#include "lib_headers.h"

void Enable_GPIO_Clock(GPIO_Port port) {
    switch (port) {
        case GPIO_PORT_A: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; break;
        case GPIO_PORT_B: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; break;
        case GPIO_PORT_C: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; break;
        case GPIO_PORT_D: RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN; break;
        case GPIO_PORT_E: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN; break;
        case GPIO_PORT_F: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN; break;
        case GPIO_PORT_G: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN; break;
        case GPIO_PORT_H: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN; break;
        case GPIO_PORT_I: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOIEN; break;
        default: break; // Handle invalid port if necessary
    }
}

void GPIO_Pin_Init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t pull) {
    MY_GPIO_TypeDef *GPIO = NULL;

    // enable clock and set GPIO pointer based on the port
    switch (port) {
        case 'A':
            ENABLE_GPIO_CLOCK(GPIOA, 0);
            break;
        case 'B':
            ENABLE_GPIO_CLOCK(GPIOB, 1);
            break;
        case 'C':
            ENABLE_GPIO_CLOCK(GPIOC, 2);
            break;
        case 'D':
            ENABLE_GPIO_CLOCK(GPIOD, 3);
            break;
        case 'E':
            ENABLE_GPIO_CLOCK(GPIOE, 4);
            break;
        case 'F':
            ENABLE_GPIO_CLOCK(GPIOF, 5);
            break;
        case 'G':
            ENABLE_GPIO_CLOCK(GPIOG, 6);
            break;
        case 'H':
            ENABLE_GPIO_CLOCK(GPIOH, 7);
            break;
        case 'I':
            ENABLE_GPIO_CLOCK(GPIOI, 8);
            break;
        default:
            return;  // invalid port
    }

    // configure pin mode
    GPIO->MODER &= ~(3 << (pin * 2));    // clear mode bits
    GPIO->MODER |= (mode << (pin * 2));  // set mode

    // if output mode, set push-pull type
    if (mode == GPIO_MODE_OUTPUT) {
        GPIO->OTYPER &= ~(1 << pin);     // push-pull type
    }

    // set low speed (default)
    GPIO->OSPEEDR &= ~(3 << (pin * 2));

    // configure pull-up/pull-down resistors
    GPIO->PUPDR &= ~(3 << (pin * 2));    // clear pull bits
    GPIO->PUPDR |= (pull << (pin * 2));  // set pull-up/down
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t PinState) {
    if (PinState == 1) {
        GPIOx->BSRR = GPIO_Pin; // Set the pin
    } else {
        GPIOx->BRR = GPIO_Pin; // Reset the pin
    }
}

/*

For most applications, it is recommended to use the first implementation that utilizes the BSRR and BRR registers. 
This approach is more efficient, clearer in intent, and reduces the risk of issues related to interrupt handling.

void GPIO_WritePin(GPIO_TypeDef* port, uint8_t pin, uint8_t state) {
    if (state)
        port->ODR |= (1 << pin);  // set pin
    else
        port->ODR &= ~(1 << pin); // clear pin
}

*/

void EXTI_Init(GPIO_TypeDef *port, uint32_t pin, uint8_t interrupt_type) {
	// Enable SYSCFG clock for EXTI configuration
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Map GPIO port to the EXTI line
	uint8_t exti_index = pin % 4;
	uint32_t exticr_value = ((port == GPIOA) ? 0 :
	                         (port == GPIOB) ? 1 :
	                         (port == GPIOC) ? 2 : 0);
	SYSCFG->EXTICR[pin / 4] &= ~(0xF << (exti_index * 4));
	SYSCFG->EXTICR[pin / 4] |= (exticr_value << (exti_index * 4));

	// Unmask the interrupt on the given EXTI line
	EXTI->IMR1 |= (1 << pin);

	// Set trigger type based on interrupt_type
	if (interrupt_type == INTERRUPT_RISING_EDGE) {
		EXTI->RTSR1 |= (1 << pin); // Enable rising trigger
		EXTI->FTSR1 &= ~(1 << pin); // Disable falling trigger
	} else if (interrupt_type == INTERRUPT_FALLING_EDGE) {
		EXTI->RTSR1 &= ~(1 << pin); // Disable rising trigger
		EXTI->FTSR1 |= (1 << pin);  // Enable falling trigger
	} else if (interrupt_type == INTERRUPT_RISING_FALLING) {
		EXTI->RTSR1 |= (1 << pin);  // Enable rising trigger
		EXTI->FTSR1 |= (1 << pin);  // Enable falling trigger
	}

	// Set the interrupt priority and enable it in NVIC
	NVIC_SetPriority(EXTI15_10_IRQn, 0x02);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*
void EXTI15_10_IRQHandler(void) {
	// Check if line interrupt occurred on the correct EXTI (PC13)
	if (EXTI->PR1 & EXTI_PR1_PIF13) {
		All_LEDs_Toggle();
		// Reset interrupt flag by setting '1' in PR1 register
		EXTI->PR1 |= EXTI_PR1_PIF13;
	}
}
*/

volatile uint8_t button_pressed = 0; // Flag to track button press status

void EXTI15_10_IRQHandler(void) {
    // Check if the interrupt is for PC13
    if (EXTI->PR1 & EXTI_PR1_PIF13) {
        // Handle debounce
        if (DebounceButton(50)) {
            // Only toggle LED if button is confirmed pressed
            All_LEDs_Toggle();
        }
        // Clear the interrupt flag
        EXTI->PR1 |= EXTI_PR1_PIF13;
    }
}

uint8_t DebounceButton(uint8_t max_deb_delay) {
    // Small delay to allow for debounce
    delay_ms(max_deb_delay);

    // Check if button is still pressed after delay
    if ((GPIOC->IDR & GPIO_IDR_ID13) == 0) { // Assuming button is active low
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

#endif
