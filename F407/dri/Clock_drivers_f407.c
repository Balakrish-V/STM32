#include "All_headers.h"

extern uint32_t SystemCoreClock;

// Set system clock to HSE (8 MHz external crystal)
void set_sysclk_to_hse(void)
{
    SystemInit();

    RCC->CR |= (1 << 16); // Enable HSE
    while (!(RCC->CR & (1 << 17))); // Wait for HSE to be ready

    FLASH->ACR = (1 << 8) | (1 << 9) | (1 << 10); // Enable Flash cache

    RCC->CFGR &= ~(3U << 0); // Clear SYSCLK source
    RCC->CFGR |= (1 << 0); // Set SYSCLK to HSE
    while (!(RCC->CFGR & (1 << 2))); // Wait for HSE to be used as SYSCLK

    SystemCoreClock = HSE_VALUE;
}

// Set system clock to HSI (16 MHz internal oscillator)
void set_sysclk_to_hsi(void)
{
    // Initialize the system (reset RCC, configure system peripherals)
    SystemInit();

    // Enable the HSI oscillator (RCC_CR: bit 0)
    RCC->CR |= RCC_CR_HSION;

    // Wait until the HSI oscillator is ready (RCC_CR: bit 1)
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);

    // Configure the Flash prefetch and cache settings (enable prefetch, instruction cache, data cache)
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    // Select the HSI oscillator as the system clock source (RCC_CFGR: bits 1:0)
    RCC->CFGR &= ~RCC_CFGR_SW;  // Clear the current system clock source
    RCC->CFGR |= RCC_CFGR_SW_HSI;  // Set the system clock to HSI

    // Wait until the system clock source is switched to HSI (RCC_CFGR: bits 3:2)
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    // Update SystemCoreClock variable
    SystemCoreClock = HSI_VALUE;
}

void set_sysclk_to_84(void)
{
    SystemInit();

    uint32_t PLL_M = 8;  // HSE frequency is 8 MHz, divide by 8
    uint32_t PLL_N = 336; // Multiply by 336 to get 336 MHz
    uint32_t PLL_P = 4;   // Output divider by 4, resulting in 84 MHz
    uint32_t PLL_Q = 7;   // USB/SDIO clock output divider (can be set to 7)

    /* Enable HSE (CR: bit 16) */
    RCC->CR |= (1 << 16);

    /* Wait till HSE is ready (CR: bit 17) */
    while (!(RCC->CR & (1 << 17)));

    /* Enable power interface clock (APB1ENR: bit 28) */
    RCC->APB1ENR |= (1 << 28);

    /* Set voltage scale to 1 for max frequency (PWR_CR: bit 14) */
    PWR->CR |= (1 << 14);

    /* Set AHB prescaler to /1 (CFGR: bits 7:4) */
    RCC->CFGR |= (0 << 4);

    /* Set APB1 prescaler to /4 (CFGR: bits 12:10) */
    RCC->CFGR |= (5 << 10);

    /* Set APB2 prescaler to /2 (CFGR: bits 15:13) */
    RCC->CFGR |= (4 << 13);

    /* Set M, N, P, and Q PLL dividers */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) | (PLL_Q << 24) | (1 << 22);

    /* Enable the main PLL (CR: bit 24) */
    RCC->CR |= (1 << 24);

    /* Wait till the main PLL is ready (CR: bit 25) */
    while (!(RCC->CR & (1 << 25)));

    /* Configure Flash:
     * Enable prefetch, instruction and data cache.
     * Set latency to 2 wait states for 84 MHz.
     */
    FLASH->ACR = (1 << 8) | (1 << 9) | (1 << 10) | (2 << 0);

    /* Select the main PLL as system clock source (CFGR: bits 1:0) */
    RCC->CFGR &= ~(3U << 0);  // Clear previous clock source
    RCC->CFGR |= (2 << 0);     // Select PLL as system clock source

    /* Wait until PLL is used as system clock source (CFGR: bits 3:2) */
    while (!(RCC->CFGR & (2 << 2)));

    /* Update SystemCoreClock variable */
    SystemCoreClock = 84000000;  // Set the system clock to 84 MHz
}

