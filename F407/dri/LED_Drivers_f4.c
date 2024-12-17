#include "LEDs_f4.h"

void Initialize_LED_GPIOs_F4(void) {
    // Enable the GPIO clock for Port D
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    // Set the mode to output for the LED pins (PD12, PD13, PD14, PD15)
    GPIOD->MODER |= (1 << (12 * 2)) | (1 << (13 * 2)) | (1 << (14 * 2)) | (1 << (15 * 2)); // Set PD12, PD13, PD14, PD15 to output mode

    // Set the output type to push-pull
    GPIOD->OTYPER &= ~((1 << 12) | (1 << 13) | (1 << 14) | (1 << 15)); // Set PD12, PD13, PD14, PD15 to push-pull

    // Set the speed to high
    GPIOD->OSPEEDR |= (3 << (12 * 2)) | (3 << (13 * 2)) | (3 << (14 * 2)) | (3 << (15 * 2)); // Set PD12, PD13, PD14, PD15 to high speed

    // Set pull-up/pull-down resistors to none
    GPIOD->PUPDR &= ~((3 << (12 * 2)) | (3 << (13 * 2)) | (3 << (14 * 2)) | (3 << (15 * 2))); // No pull-up, no pull-down
}

void LED_On_F4(uint16_t pin) {
    LED_PORT->ODR |= pin;
}

void LED_Off_F4(uint16_t pin) {
    LED_PORT->ODR &= ~pin;
}

void LED_Toggle_F4(uint16_t pin) {
    LED_PORT->ODR ^= pin;
}

void All_LEDs_On_F4(void) {
    LED_PORT->ODR |= (LED_PIN_12 | LED_PIN_13 | LED_PIN_14 | LED_PIN_15);
}

void All_LEDs_Off_F4(void) {
    LED_PORT->ODR &= ~(LED_PIN_12 | LED_PIN_13 | LED_PIN_14 | LED_PIN_15);
}

void All_LEDs_Toggle_F4(void) {
    LED_PORT->ODR ^= (LED_PIN_12 | LED_PIN_13 | LED_PIN_14 | LED_PIN_15);
}
