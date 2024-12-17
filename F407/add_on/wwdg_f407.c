#include "All_headers.h"

#define WWDG_WINDOW_VALUE   0x50  // Window value (must be less than timeout value)
#define WWDG_TIMEOUT_VALUE  0x7F  // Max counter value
#define SIMPLE_DELAY        100000   // Reduced delay for testing

void WWDG_Config(uint32_t timeout_value) 
{
    RCC->APB1ENR |= (1 << 11);  // Enable WWDG clock
    WWDG->CFR = (WWDG_WINDOW_VALUE << WWDG_CFR_W_Pos); // Set window value
    
    if (timeout_value > 0x7F) {
        timeout_value = 0x7F;  // Maximum allowable value
    }
    WWDG->CR = WWDG_CR_WDGA | timeout_value;  // Enable WWDG and set counter
}

void WWDG_Refresh(uint32_t counter_value) 
{
    // Refresh the watchdog counter to avoid reset
    WWDG->CR = WWDG_CR_WDGA | (counter_value & 0x7F);
}

void simple_delay(uint32_t delay) 
{
    for (volatile uint32_t i = 0; i < delay; i++);
}

int main(void) 
{
    set_sysclk_to_hsi();            // Configure system clock
    Initialize_LED_GPIOs_F4();      // Initialize LEDs

    LED_On_F4(LED_PIN_13);          // Turn ON Orange LED to indicate start
    WWDG_Config(WWDG_TIMEOUT_VALUE); // Configure WWDG

    // Main loop
    while (1) 
    {
        // Refresh the WWDG to prevent reset
        // WWDG_Refresh(WWDG_TIMEOUT_VALUE);  

        // Toggle Blue LED
        LED_On_F4(LED_PIN_15);    
        simple_delay(SIMPLE_DELAY);  
        LED_Off_F4(LED_PIN_15);   
        simple_delay(SIMPLE_DELAY);  
    }
    return 0;
}