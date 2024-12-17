#include "lib_headers.h"

// Sampling rate macros (samples per second)
#define ADC_SAMPLES_PER_SECOND_100000  (16000000 / (2 + 12.5))  // ˜ 100000 samples/sec
#define ADC_SAMPLES_PER_SECOND_50000   (16000000 / (6 + 12.5))  // ˜ 50000 samples/sec
#define ADC_SAMPLES_PER_SECOND_10000   (16000000 / (12 + 12.5)) // ˜ 10000 samples/sec
#define ADC_SAMPLES_PER_SECOND_5000    (16000000 / (24 + 12.5)) // ˜ 5000 samples/sec
#define ADC_SAMPLES_PER_SECOND_1000    (16000000 / (92 + 12.5)) // ˜ 1000 samples/sec

// Sampling time definitions
#define ADC_SAMPLE_TIME_2CYCLES    0x0
#define ADC_SAMPLE_TIME_6CYCLES    0x1
#define ADC_SAMPLE_TIME_12CYCLES   0x2
#define ADC_SAMPLE_TIME_24CYCLES   0x3
#define ADC_SAMPLE_TIME_47CYCLES   0x4
#define ADC_SAMPLE_TIME_92CYCLES   0x5
#define ADC_SAMPLE_TIME_247CYCLES  0x6
#define ADC_SAMPLE_TIME_640CYCLES  0x7

// Helper macro to map desired sampling rate to ADC sampling time
#define ADC_SAMPLING_TIME_FROM_RATE(rate) \
    ((rate) >= ADC_SAMPLES_PER_SECOND_100000 ? ADC_SAMPLE_TIME_2CYCLES : \
     (rate) >= ADC_SAMPLES_PER_SECOND_50000  ? ADC_SAMPLE_TIME_6CYCLES : \
     (rate) >= ADC_SAMPLES_PER_SECOND_10000  ? ADC_SAMPLE_TIME_12CYCLES : \
     (rate) >= ADC_SAMPLES_PER_SECOND_5000   ? ADC_SAMPLE_TIME_24CYCLES : \
     (rate) >= ADC_SAMPLES_PER_SECOND_1000   ? ADC_SAMPLE_TIME_92CYCLES : \
                                               ADC_SAMPLE_TIME_640CYCLES)

uint16_t ADC_VALUES[2] = {0}; // Array to hold ADC values

void ADC_SetChannelRank(ADC_TypeDef *ADCx, uint8_t channel, uint8_t rank) {
    if (rank <= 4) {
        ADCx->SQR1 &= ~(0x1F << ((rank - 1) * 6)); // Clear the previous channel setting
        ADCx->SQR1 |= (channel << ((rank - 1) * 6)); // Set the new channel
    } else if (rank <= 9) {
        ADCx->SQR2 &= ~(0x1F << ((rank - 5) * 6)); // Clear the previous channel setting
        ADCx->SQR2 |= (channel << ((rank - 5) * 6)); // Set the new channel
    } else if (rank <= 14) {
        ADCx->SQR3 &= ~(0x1F << ((rank - 10) * 6)); // Clear the previous channel setting
        ADCx->SQR3 |= (channel << ((rank - 10) * 6)); // Set the new channel
    } else {
        ADCx->SQR4 &= ~(0x1F << ((rank - 15) * 6)); // Clear the previous channel setting
        ADCx->SQR4 |= (channel << ((rank - 15) * 6)); // Set the new channel
    }
}

void ADc_Init(ADC_TypeDef *ADCx, ADC_Channel *channels, uint8_t num_channels, uint32_t sampling_rate) {
    uint32_t sampling_time = ADC_SAMPLING_TIME_FROM_RATE(sampling_rate);

    // Enable GPIO clocks for all channels
    for (int i = 0; i < num_channels; i++) {
        GPIO_TypeDef *GPIOx = channels[i].GPIOx;
        if (GPIOx == GPIOA) 
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
        else if (GPIOx == GPIOC) 
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
        else if (GPIOx == GPIOF) 
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;

        // Configure GPIO pin as analog
        GPIOx->MODER |= (3U << (channels[i].pin * 2));  // Analog mode
        GPIOx->PUPDR &= ~(3U << (channels[i].pin * 2)); // No pull-up/pull-down
    }

    // Enable ADC clock
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

    // Configure ADC clock and voltage regulator
    ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0; // Synchronous clock mode (HCLK/1)
    ADCx->CR &= ~ADC_CR_DEEPPWD;            // Disable deep power-down
    ADCx->CR |= ADC_CR_ADVREGEN;            // Enable voltage regulator
    for (int i = 0; i < 1600000; i++); // Delay for regulator stabilization

    // Calibrate ADC
    ADCx->CR |= ADC_CR_ADCAL; // Start calibration
    while (ADCx->CR & ADC_CR_ADCAL); // Wait for calibration to finish

    // Configure sampling time for each channel
    for (int i = 0; i < num_channels; i++) {
        uint8_t channel = channels[i].channel;
        if (channel < 10) {
            ADCx->SMPR1 &= ~(0x7 << (channel * 3));
            ADCx->SMPR1 |= (sampling_time << (channel * 3));
        } else {
            ADCx->SMPR2 &= ~(0x7 << ((channel - 10) * 3));
            ADCx->SMPR2 |= (sampling_time << ((channel - 10) * 3));
        }
    }

    // Set channels and ranks based on index
    for (int i = 0; i < num_channels; i++) {
        ADC_SetChannelRank(ADCx, channels[i].channel, i + 1); // Set channel rank based on index
    }

    // Set continuous conversion mode
    ADCx->CFGR |= ADC_CFGR_CONT;

    // Enable ADC
    ADCx->CR |= ADC_CR_ADEN;
    while (!(ADCx->ISR & ADC_ISR_ADRDY)); // Wait until ADC is ready
}

void ADC_Start(ADC_TypeDef *ADCx) {
    ADCx->CR |= ADC_CR_ADSTART; // Start ADC conversions
}

void ADC_WaitForReady(ADC_TypeDef *ADCx) {
		ADC1->ISR |= ADC_ISR_EOC;     // Clear EOC flag
    while (!(ADCx->ISR & ADC_ISR_EOC)); // Wait for end of conversion
}

void ADC_Stop(ADC_TypeDef *ADCx) {
    ADCx->CR &= ~ADC_CR_ADEN; // Disable ADC
}

void ADC_Read(ADC_TypeDef *ADCx, uint8_t num_channels) {
    for (int i = 0; i < num_channels; i++) {
        ADC_WaitForReady(ADCx); // Wait for ADC to be ready for each channel
        ADC_VALUES[i] = ADCx->DR; // Read ADC value
			  ADCx->ISR |= ADC_ISR_EOC; // Raise EOC flag after reading all channels
    }
}

int main(void) {
    set_sysclk_to_hsi();
    Initialize_LED_GPIOs();
    Green_LED_On();
    ADC_Channel channels[] = {ADC1_CHANNEL_PA0_CH5, ADC1_CHANNEL_PB0_CH15};
    uint8_t num_channels = sizeof(channels) / sizeof(channels[0]);
    ADc_Init(ADC1, channels, num_channels, ADC_SAMPLES_PER_SECOND_1000); // Initialize ADC1
	  ADC_Start(ADC1);              // Start ADC conversions
    while (1) {
        ADC_Read(ADC1, num_channels); // Read ADC values
        Blue_LED_Toggle();
        for(int i = 0; i < 100000; i++); // Delay
    }
} 
