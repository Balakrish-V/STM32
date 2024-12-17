#include "All_headers.h"
/*
// WWDG Initialization function for STM32F407VG
void WWDG_Init(uint32_t timeout_value, char unit) 
{
    RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;  // Enable WWDG clock
    uint32_t wwdg_counter_value;
    if (unit == 'm') { 
        wwdg_counter_value = WWDG_TIMEOUT_MS(timeout_value);
    } else if (unit == 's') { 
        wwdg_counter_value = WWDG_TIMEOUT_SEC(timeout_value);
    } else {
        wwdg_counter_value = WWDG_TIMEOUT_SEC(1); // Default to 1 second
    }
    if (wwdg_counter_value > WWDG_MAX_VALUE) {
        wwdg_counter_value = WWDG_MAX_VALUE; 
    }
    WWDG->CFR = (WWDG_WINDOW_VALUE << WWDG_CFR_W_Pos) | WWDG_CFR_EWI; 
    WWDG->CR = (wwdg_counter_value & WWDG_CR_T); 
    WWDG->CR |= WWDG_CR_WDGA; 
}

void WWDG_Refresh(uint32_t wwdg_counter_value) 
{
    WWDG->CR = WWDG_CR_WDGA | wwdg_counter_value; 
}
*/