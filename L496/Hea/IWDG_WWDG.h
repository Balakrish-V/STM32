#ifndef WDG_H
#define WDG_H

#include "lib_headers.h"

//void IWDG_Init(void);
void IWDG_Init(uint32_t timeout_ms);
void IWDG_Refresh(void);
void WWDG_Init(uint32_t timeout_value, char unit);
void WWDG_Refresh(uint32_t wwdg_counter_value);
void IWDG_Demo(void);
void WWDG_Demo(void);

// WWDG CR register bit definitions
#define WWDG_CR_T_Pos           (0U)
#define WWDG_CR_T_Msk           (0x7FUL << WWDG_CR_T_Pos)                      
#define WWDG_CR_T               WWDG_CR_T_Msk                                  
#define WWDG_CR_WDGA_Pos        (7U)
#define WWDG_CR_WDGA_Msk        (0x1UL << WWDG_CR_WDGA_Pos)                    

// WWDG CFR register bit definitions
#define WWDG_CFR_W_Pos          (0U)
#define WWDG_CFR_W_Msk          (0x7FUL << WWDG_CFR_W_Pos)                     
#define WWDG_CFR_W              WWDG_CFR_W_Msk                                
                             
#define WWDG_MAX_VALUE 0x7F   // Maximum value for the WWDG counter
#define WWDG_WINDOW_VALUE 0x50 // Window value (must be < 0x7F)

#define IWDG_PRESCALER_4   ((uint32_t)0x00000000)
#define IWDG_PRESCALER_8   ((uint32_t)0x00000001)
#define IWDG_PRESCALER_16  ((uint32_t)0x00000002)
#define IWDG_PRESCALER_32  ((uint32_t)0x00000003)
#define IWDG_PRESCALER_64  ((uint32_t)0x00000004)
#define IWDG_PRESCALER_128 ((uint32_t)0x00000005)
#define IWDG_PRESCALER_256 ((uint32_t)0x00000006)

#define LSI_CLOCK 32000U  // Typical LSI clock frequency in Hz for STM32L496

// Macro definitions for common timeouts (in milliseconds)
#define IWDG_TIMEOUT_500MS  500
#define IWDG_TIMEOUT_1S    1000
#define IWDG_TIMEOUT_2S    2000
#define IWDG_TIMEOUT_5S    5000

#define HSI_FREQ 16000000 
#define WWDG_MAX_VALUE 0x7F 
#define WWDG_WINDOW_VALUE 0x50 

// Macros for WWDG timeout calculations
#define WWDG_TIMEOUT_MS(x)   ((x) * (SystemCoreClock / 8000) - 1) // Adjust based on your clock
#define WWDG_TIMEOUT_SEC(x)  (WWDG_TIMEOUT_MS((x) * 1000)) // Convert seconds to milliseconds

// Timeout macros in milliseconds
#define WWDG_TIMEOUT_MS_1   1
#define WWDG_TIMEOUT_MS_2   2
#define WWDG_TIMEOUT_MS_5   5

// Timeout macros in seconds (convert to milliseconds)
#define WWDG_TIMEOUT_SEC_1   (1000 * WWDG_TIMEOUT_MS_1) // 1000 ms
#define WWDG_TIMEOUT_SEC_2   (1000 * WWDG_TIMEOUT_MS_2) // 2000 ms
#define WWDG_TIMEOUT_SEC_5   (1000 * WWDG_TIMEOUT_MS_5) // 5000 ms

#endif