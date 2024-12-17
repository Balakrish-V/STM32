#include "lib_headers.h" // Ensure this header is for the STM32L4 series

void ADC1_Init(void);
uint16_t ADC_Read(void); // Function to read from the ADC

uint16_t adc_value;

int main(void) 
{
    set_sysclk_to_msi(MSI_16MHZ); // Set system clock to MSI 16 MHz
    ADC1_Init();                   // Initialize ADC1
    while (1) {
        adc_value = ADC_Read();    // Read ADC value from ADC1
        // Process adc_value here as needed, e.g., send via UART or update an LED
    }
}

void ADC1_Init(void) 
{
    // Step 1: Enable clock for ADC and GPIO
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;   // Enable ADC1 clock

    // Step 2: Configure the ADC
    ADC1->CR = 0; // Reset control register
    ADC1->CFGR = 0; // Reset configuration register

    // Step 3: Enable ADC voltage regulator
    ADC1->CR |= ADC_CR_ADVREGEN; // Enable ADC voltage regulator
    for (volatile int i = 0; i < 10000; i++); // Increase delay for regulator stabilization

    // Step 4: Set the clock prescaler (this can be adjusted as needed)
    RCC->CCIPR &= ~RCC_CCIPR_ADCSEL; // Clear ADC clock selection bits
    RCC->CCIPR |= RCC_CCIPR_ADCSEL_0; // Select system clock (MSI or HSI) as ADC clock source

    // Step 5: Set the scan mode and resolution
    ADC1->CR |= ADC_CR_ADEN; // Enable ADC

    // Step 6: Wait for ADC to be ready
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait for ADRDY flag

    // Step 7: Configure ADC settings
    ADC1->CFGR |= ADC_CFGR_CONT; // Enable continuous conversion mode
    ADC1->CFGR |= (0 << 30); // Set 12-bit resolution (0: 12-bit)

    // Step 8: Configure GPIO for ADC channel (e.g., PA1 for ADC Channel 1)
    GPIOA->MODER |= (3 << (1 * 2)); // Set PA1 to Analog mode (11)

    // Step 9: Set the sampling time for channel 1 to 12.5 cycles (adjust if needed)
    ADC1->SMPR2 |= (0 << 12); // Sampling time of 12.5 cycles for channel 1

    // Step 10: Set the Regular channel sequence length in SQR1
    ADC1->SQR1 &= ~ADC_SQR1_L; // Clear the regular sequence length bits
    ADC1->SQR1 |= (0 << 20); // Set SQR1_L = 0 for 1 conversion (one channel)

    // Optional: Enable ADC interrupts if needed
    ADC1->IER |= ADC_IER_EOCIE; // Enable ADC end-of-conversion interrupt
}

uint16_t ADC_Read(void)
{
    // Start the ADC conversion
    ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion

    // Wait for conversion to complete
    while (!(ADC1->ISR & ADC_ISR_EOC)); // Wait for EOC flag to be set
    
    // Read the converted value
    uint16_t value = ADC1->DR; // Read and return the data register value

    // Optionally check EOC flag
    if (ADC1->ISR & ADC_ISR_EOC) {
        ADC1->ISR |= ADC_ISR_EOC; // Clear EOC flag (optional)
    }

    return value; // Return the ADC value
}
