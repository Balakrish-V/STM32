#ifndef CLOCKS_C
#define CLOCKS_C

#include "lib_headers.h"

// global variable for the system core clock frequency
extern uint32_t SystemCoreClock;

// function to set system clock to HSI
void set_sysclk_to_hsi(void) {
    // Enable HSI
    RCC->CR |= RCC_CR_HSION; // Set HSION bit to enable HSI

    // Wait until HSI is ready
    int timeout = 0;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0 && (timeout++ < 20000)); // Wait for HSI ready, with a timeout

    if (timeout >= 20000) {
        // HSI failed to start, handle error (e.g., log the error)
        return; // Consider falling back to another clock source
    }

    // Configure flash latency for 16 MHz (typically 0 WS is okay)
    FLASH->ACR &= ~FLASH_ACR_LATENCY; // Clear latency bits
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_0WS; // 0 WS for HSI 16 MHz
		
		// Set APB1, APB2, and AHB prescalers to 1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;  // Clear AHB prescaler bits (set AHB prescaler to 1)
    RCC->CFGR &= ~RCC_CFGR_PPRE1; // Clear APB1 prescaler bits (set APB1 prescaler to 1)
    RCC->CFGR &= ~RCC_CFGR_PPRE2; // Clear APB2 prescaler bits (set APB2 prescaler to 1)

    // Set HSI as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW; // Clear SW bits
    RCC->CFGR |= RCC_CFGR_SW_HSI; // Set HSI as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // Wait for HSI to be used

    // Update SystemCoreClock to reflect HSI
    SystemCoreClock = HSI_VALUE; // Ensure SystemCoreClock reflects HSI frequency
    SystemCoreClockUpdate(); // Call this to ensure any other configurations are taken into account
}

void set_sysclk_to_hse(void) {
    // Enable HSE
    RCC->CR |= RCC_CR_HSEON;

    // Wait until HSE is ready (add timeout to avoid infinite loop)
    int timeout = 0;
    while (!(RCC->CR & RCC_CR_HSERDY) && (timeout++ < 20000));

    if (timeout >= 20000) {
        // HSE failed to start, handle error (e.g., fallback to HSI)
        // You can log the error or switch to HSI here if needed
			  Red_LED_On();
        return; // Consider adding error logging here
    }

    // Configure flash latency for HSE frequency (adjust based on actual HSE frequency)
    FLASH->ACR &= ~FLASH_ACR_LATENCY; // Clear latency bits
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_0WS; // 0 WS for up to 16 MHz
		
		// Set APB1, APB2, and AHB prescalers to 1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;  // Clear AHB prescaler bits (set AHB prescaler to 1)
    RCC->CFGR &= ~RCC_CFGR_PPRE1; // Clear APB1 prescaler bits (set APB1 prescaler to 1)
    RCC->CFGR &= ~RCC_CFGR_PPRE2; // Clear APB2 prescaler bits (set APB2 prescaler to 1)

    // Set HSE as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW; // Clear SW bits
    RCC->CFGR |= RCC_CFGR_SW_HSE; // Set HSE as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE); // Wait for HSE to be used

    // Update SystemCoreClock to reflect HSE
    SystemCoreClock = HSE_VALUE; // Ensure SystemCoreClock reflects HSE frequency
    SystemCoreClockUpdate(); // Call this to ensure any other configurations are taken into account
}

void set_sysclk_to_msi(uint32_t frequency_khz) {
    uint32_t msirange = 0;

    // Select the MSI range based on the requested frequency using macros
    switch (frequency_khz) {
        case 100:
            msirange = RCC_CR_MSIRANGE_0;  // MSI = 100 kHz
            break;
        case 200:
            msirange = RCC_CR_MSIRANGE_1;  // MSI = 200 kHz
            break;
        case 400:
            msirange = RCC_CR_MSIRANGE_2;  // MSI = 400 kHz
            break;
        case 800:
            msirange = RCC_CR_MSIRANGE_3;  // MSI = 800 kHz
            break;
        case 1000:
            msirange = RCC_CR_MSIRANGE_4;  // MSI = 1 MHz
            break;
        case 2000:
            msirange = RCC_CR_MSIRANGE_5;  // MSI = 2 MHz
            break;
        case 4000:
            msirange = RCC_CR_MSIRANGE_6;  // MSI = 4 MHz
            break;
        case 8000:
            msirange = RCC_CR_MSIRANGE_7;  // MSI = 8 MHz
            break;
        case 16000:
            msirange = RCC_CR_MSIRANGE_8;  // MSI = 16 MHz
            break;
        case 24000:
            msirange = RCC_CR_MSIRANGE_9;  // MSI = 24 MHz
            break;
        case 32000:
            msirange = RCC_CR_MSIRANGE_10; // MSI = 32 MHz
            break;
        case 48000:
            msirange = RCC_CR_MSIRANGE_11; // MSI = 48 MHz
            break;
        default:
            return;  // Invalid frequency, exit function
    }

    // Enable MSI and wait until it's ready
    RCC->CR |= RCC_CR_MSION;
    while ((RCC->CR & RCC_CR_MSIRDY) == 0);  // Wait until MSI is ready

    // Clear the MSI range bits and set the new MSI range
    RCC->CR &= ~RCC_CR_MSIRANGE;
    RCC->CR |= msirange;
    RCC->CR |= RCC_CR_MSIRGSEL;  // Set MSI range from RCC_CR register

    // Wait until MSI is ready again
    while ((RCC->CR & RCC_CR_MSIRDY) == 0);

    // Set APB1, APB2, and AHB prescalers to 1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;  // Clear AHB prescaler bits
    RCC->CFGR &= ~RCC_CFGR_PPRE1; // Clear APB1 prescaler bits
    RCC->CFGR &= ~RCC_CFGR_PPRE2; // Clear APB2 prescaler bits

    // Set MSI as the system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_MSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);  // Wait for MSI to be used

    // Update SystemCoreClock based on the selected MSI frequency
    SystemCoreClock = frequency_khz * 1000;  // Convert kHz to Hz
}

void enable_lsi(void) {
    // enable LSI
    RCC->CSR |= RCC_CSR_LSION; 
    while (!(RCC->CSR & RCC_CSR_LSIRDY)); // wait for LSI to be ready
}

void set_system_clock_frequency(uint32_t frequency) {
    // Reset PLL configuration
    RCC->PLLCFGR = 0;  // Reset PLL configuration

    // Check and set PLLM, PLLN, and PLLR based on the desired frequency
    uint32_t pllM = 1;  
    uint32_t pllN = 0;
    uint32_t pllR = 0;

    switch (frequency) {
			/*
			
			still have to view the AHB Prescaler, HCLK value, APB2 and APB1 Prescaler, ADC Clock Mux values
			
			In testing
			
        case FREQ_1MHZ:
            pllN = 8;  
            pllR = 8;  
            break;
        case FREQ_2MHZ:
            pllN = 8;  
            pllR = 4;  
            break;
        case FREQ_4MHZ:
            pllN = 8;  
            pllR = 2;  
            break;
				case FREQ_8MHZ:
					  pllM = 4;  // 16 / 4 = 4
            pllN = 16; // 4 * 16 = 64
            pllR = 8;  // 64 / 8 = 8
            break;
        case FREQ_10MHZ:
					  pllM = 2;
            pllN = 10;  
            pllR = 8;  
            break;
        case FREQ_16MHZ:
            pllN = 8;  
            pllR = 8;  
            break;
        case FREQ_20MHZ:
            pllN = 10;  
            pllR = 8; 
            break;
						
			*/
        case FREQ_40MHZ:
            pllN = 10;  // For 40 MHz, VCO output will be 80 MHz
            pllR = 4;  // SYSCLK = 80 MHz / 2 = 40 MHz
            break;
        case FREQ_60MHZ:
            pllN = 15;  // For 60 MHz, VCO output will be 240 MHz
            pllR = 4;  // SYSCLK = 240 MHz / 4 = 60 MHz
            break;
        case FREQ_80MHZ:
            pllN = 40;  // For 80 MHz, VCO output will be 80 MHz
            pllR = 1;  // SYSCLK = 80 MHz / 1 = 80 MHz
            break;
        default:
            return;  // Invalid frequency
    }

    // Set PLL configuration
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI | 
                    (pllM << RCC_PLLCFGR_PLLM_Pos) |
                    (pllN << RCC_PLLCFGR_PLLN_Pos) |
                    (pllR << RCC_PLLCFGR_PLLR_Pos) | 
                    RCC_PLLCFGR_PLLREN;

    // Enable the PLL
    RCC->CR |= RCC_CR_PLLON;  // Enable PLL
    while (!(RCC->CR & RCC_CR_PLLRDY));  // Wait until PLL is ready

    // Set PLL as the system clock source
    RCC->CFGR &= ~(3 << 0);  // Clear SW bits
    RCC->CFGR |= (3 << 0);   // Select PLL as system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait for PLL to be used

    // Check if the system clock exceeds 80 MHz
    if (frequency > 80) {
        //Red_LED_On();
    }

    // Update SystemCoreClock variable
    SystemCoreClockUpdate();  // Update the SystemCoreClock variable with the new clock value
}

void init_clock(void)
{
    //set Flash Latency to 4 wait states for 80 MHz operation
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;  // Set latency for 80 MHz operation

    //configure AHB prescaler to divide by 1 (HCLK = SYSCLK)
    RCC->CFGR &= ~(0xF << 4);  // Clear HPRE bits
    RCC->CFGR |= (0 << 4);  

    RCC->CR |= RCC_CR_HSION;  // Enable HSI
    while (!(RCC->CR & RCC_CR_HSIRDY));  // Wait until HSI is ready

    set_system_clock_frequency(FREQ_80MHZ);  //--> correct
	  //set_system_clock_frequency(FREQ_60MHZ); --> correct
	  //set_system_clock_frequency(FREQ_40MHZ); --> correct
	
	  // set_system_clock_frequency(FREQ_8MHZ);  --> in testing
}

#endif