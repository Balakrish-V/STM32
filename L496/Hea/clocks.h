#ifndef CLOCKS_H
#define CLOCKS_H

#include "lib_headers.h"

#define FREQ_1MHZ   1
#define FREQ_2MHZ   2
#define FREQ_4MHZ   4
#define FREQ_8MHZ   8
#define FREQ_10MHZ  10
#define FREQ_16MHZ  16
#define FREQ_20MHZ  20
#define FREQ_40MHZ  40
#define FREQ_60MHZ  60
#define FREQ_80MHZ  80

// Define HSE and HSI values if not defined
#ifndef HSE_VALUE
#define HSE_VALUE    ((uint32_t)8000000) // HSE frequency (8 MHz) - STM32CubeIDE ClockDiagram
#endif

#ifndef HSI_VALUE
#define HSI_VALUE 16000000 // HSI frequency (16 MHz)
#endif

#define MSI_100KHZ  RCC_CR_MSIRANGE_0
#define MSI_200KHZ  RCC_CR_MSIRANGE_1
#define MSI_400KHZ  RCC_CR_MSIRANGE_2
#define MSI_800KHZ  RCC_CR_MSIRANGE_3
#define MSI_1MHZ    RCC_CR_MSIRANGE_4
#define MSI_2MHZ    RCC_CR_MSIRANGE_5
#define MSI_4MHZ    RCC_CR_MSIRANGE_6
#define MSI_8MHZ    RCC_CR_MSIRANGE_7
#define MSI_16MHZ   RCC_CR_MSIRANGE_8
#define MSI_24MHZ   RCC_CR_MSIRANGE_9
#define MSI_32MHZ   RCC_CR_MSIRANGE_10
#define MSI_48MHZ   RCC_CR_MSIRANGE_11

// Function Prototypes
void set_sysclk_to_hse(void);
void set_sysclk_to_hsi(void);
void set_sysclk_to_msi(uint32_t frequency_khz);
void enable_lsi(void);
void init_clock(void); // 80MHz
void set_sysclk_to_frequency(uint32_t frequency);

#endif
