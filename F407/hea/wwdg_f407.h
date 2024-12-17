#ifndef WWDG_H
#define WWDG_H

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

// Macros for STM32F407VG
#define HSI_FREQ 16000000 
#define WWDG_MAX_VALUE 0x7F 
#define WWDG_WINDOW_VALUE 0x50 

// Macros for WWDG timeout calculations
#define WWDG_TIMEOUT_MS(x)   ((x) * (SystemCoreClock / 16000) - 1) // Adjust based on your clock
#define WWDG_TIMEOUT_SEC(x)  (WWDG_TIMEOUT_MS((x) * 1000)) // Convert seconds to milliseconds

// Timeout macros in milliseconds
#define WWDG_TIMEOUT_MS_1   1
#define WWDG_TIMEOUT_MS_2   2
#define WWDG_TIMEOUT_MS_5   5

/*
void WWDG_Init(uint32_t timeout_value, char unit); 
void WWDG_Refresh(uint32_t wwdg_counter_value);
void dummy_function(volatile uint32_t s);
*/
#endif
