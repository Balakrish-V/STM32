#ifndef SYSCLK_CONFIG_H
#define SYSCLK_CONFIG_H

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

// Function prototypes for system clock configuration
void set_sysclk_to_hse(void);
void set_sysclk_to_hsi(void);
void set_sysclk_to_84(void);

// Define HSE and HSI values if not defined
#ifndef HSE_VALUE
#define HSE_VALUE    ((uint32_t)8000000) // HSE frequency (8 MHz) - STM32CubeIDE ClockDiagram
#endif

#ifndef HSI_VALUE
#define HSI_VALUE    ((uint32_t)16000000) // HSI frequency (16 MHz)
#endif

#endif // SYSCLK_CONFIG_H