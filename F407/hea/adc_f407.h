#ifndef ADC_H
#define ADC_H

#include "All_headers.h"

void ADC_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t adc_channel);

void ADC_Start(void);

uint16_t ADC_Output(void);

void ADC_Stop(void);

#endif // ADC_H