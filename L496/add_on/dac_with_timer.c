#include "lib_headers.h"

// Function prototypes
void DAC_Init(void);
void TIM2_Init(uint32_t frequency);
void Load_Tri_Waveform(uint16_t *waveform, uint32_t size);

// Globals
#define WAVEFORM_SIZE 100
uint16_t tri_wave[WAVEFORM_SIZE];  // Precomputed triangle wave for DAC Channel 2
volatile uint32_t waveform_index = 0; // Index for the triangle wave array

void DAC_Init(void) 
{
    // Enable GPIOA clock for PA4 (DAC Channel 1) and PA5 (DAC Channel 2)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER |= (3 << (4 * 2)) | (3 << (5 * 2)); // Set PA4 and PA5 to analog mode

    // Enable DAC1 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
    
    // Configure DAC Channel 2 (PA5)
    DAC1->CR &= ~DAC_CR_EN2;             // Disable DAC Channel 2 for configuration
    DAC1->CR |= DAC_CR_TEN2;             // Enable trigger for DAC Channel 2
    DAC1->CR |= (6 << DAC_CR_TSEL2_Pos); // Select TIM2 TRGO as trigger for DAC Channel 2
    DAC1->CR |= DAC_CR_EN2;              // Enable DAC Channel 2
}

void TIM2_Init(uint32_t frequency) 
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;   // Enable TIM2 clock
    // Configure TIM2
    TIM2->PSC = 16 - 1;                    // Prescaler: Divide HSI (16 MHz) by 16 => 1 MHz timer clock
    TIM2->ARR = (1000000 / frequency) - 1; // Auto-reload value for desired frequency (in Hz)
    TIM2->CR1 &= ~TIM_CR1_DIR;             // Set Timer in Upcounting mode
    TIM2->CR1 |= TIM_CR1_ARPE;             // Enable auto-reload preload
    TIM2->CR2 &= ~TIM_CR2_MMS;             // Clear MMS bits
    TIM2->CR2 |= TIM_CR2_MMS_1;            // Set MMS to 010 for TRGO on update event
    TIM2->CR1 |= TIM_CR1_CEN;              // Enable the timer
    TIM2->DIER |= TIM_DIER_UIE;            // Enable update interrupt
    NVIC_EnableIRQ(TIM2_IRQn);             // Enable Timer 2 interrupt in NVIC
    NVIC_SetPriority(TIM2_IRQn, 1);        // Set interrupt priority
}

void Load_Tri_Waveform(uint16_t *waveform, uint32_t size) 
{
    for (uint32_t i = 0; i < size / 2; i++) {
        waveform[i] = (4095 * i) / (size / 2); // Ramp up
    }
    for (uint32_t i = size / 2; i < size; i++) {
        waveform[i] = 4095 - (4095 * (i - size / 2)) / (size / 2); // Ramp down
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {          // Check update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF;          // Clear the interrupt flag

        // Update DAC Channel 2 output with precomputed triangle waveform value
        DAC1->DHR12R2 = tri_wave[waveform_index];
        waveform_index = (waveform_index + 1) % WAVEFORM_SIZE;  // Update waveform index
			  Blue_LED_On();
    }
}

int main(void) 
{
    set_sysclk_to_hsi();                        // Assuming this configures HSI (16 MHz clock)
    Initialize_LED_GPIOs();
    delay_timer_init();
    TIM2_Init(100);                             // Set desired frequency (e.g., 100 Hz for TIM2)
    DAC_Init();                                 // Initialize DAC for both channels
    Load_Tri_Waveform(tri_wave, WAVEFORM_SIZE); // Load triangle waveform into array
    Green_LED_On();
    while (1) {
        // Main loop can handle other tasks or remain idle
    }
}