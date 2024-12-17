#include "All_headers.h"

void ADC_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t adc_channel) {
    // 1. Enable the GPIO clock
    if (GPIOx == GPIOA) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (GPIOx == GPIOB) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    else if (GPIOx == GPIOC) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // 2. Configure the pin as an analog input
    GPIOx->MODER |= (0x3 << (GPIO_Pin * 2));  // Analog mode (11)
    GPIOx->PUPDR &= ~(0x3 << (GPIO_Pin * 2)); // No pull-up, no pull-down

    // 3. Enable the ADC clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 4. Configure the ADC
    ADC1->CR1 = 0;  // 12-bit resolution (default)
    ADC1->CR2 = ADC_CR2_ADON;  // Enable ADC, single conversion mode
    ADC1->SQR3 = adc_channel;  // Set ADC channel
}

void ADC_Start() {
    ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion
}

uint16_t ADC_Output() {
    while (!(ADC1->SR & ADC_SR_EOC)); // Wait for conversion to complete
    return (uint16_t)(ADC1->DR);      // Read the converted value
}

void ADC_Stop() {
    ADC1->CR2 &= ~ADC_CR2_ADON; // Disable ADC
}
