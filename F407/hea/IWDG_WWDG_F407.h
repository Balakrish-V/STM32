#ifndef WDG_H
#define WDG_H

#include "stm32f4xx.h" // Include STM32F4-specific header

#define IWDG_PRESCALER_4    0x00
#define IWDG_PRESCALER_8    0x01
#define IWDG_PRESCALER_16   0x02
#define IWDG_PRESCALER_32   0x03
#define IWDG_PRESCALER_64   0x04
#define IWDG_PRESCALER_128  0x05
#define IWDG_PRESCALER_256  0x06

#define WWDG_MAX_VALUE      0x7F
#define WWDG_WINDOW_VALUE   0x50  // Example value, adjust as per requirement

#define WWDG_TIMEOUT_MS(ms) ((ms * LSI_CLOCK) / 1000)
#define WWDG_TIMEOUT_SEC(s) (s * LSI_CLOCK)

#define LSI_CLOCK 32000 // Typical LSI clock frequency

// Function Prototypes
void IWDG_Init(uint32_t timeout_ms);
void IWDG_Refresh(void);
void IWDG_Demo(void);

void WWDG_Init(uint32_t timeout_value, char unit);
void WWDG_Refresh(uint32_t wwdg_counter_value);
void WWDG_Demo(void);

#endif // WDG_H
