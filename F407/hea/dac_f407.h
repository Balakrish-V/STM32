#ifndef DAC_H
#define DAC_H

#include "stm32f4xx.h"

// Function prototypes
void DAC_Init(void);
void DAC_SetValue(uint32_t value);
void DAC_SetFrequency(float frequency);

#endif // DAC_H
