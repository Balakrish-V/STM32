#ifndef WDG_C
#define WDG_C

#include "lib_headers.h"

void IWDG_Init(uint32_t timeout_ms) 
{
    uint32_t reload_value = 0;
    uint8_t prescaler = 0;

    IWDG->KR = 0x5555;

    if (timeout_ms <= 512) {
        prescaler = IWDG_PRESCALER_4; 
        reload_value = (timeout_ms * (LSI_CLOCK / 4)) / 1000 - 1;
    } else if (timeout_ms <= 1024) {
        prescaler = IWDG_PRESCALER_8;  
        reload_value = (timeout_ms * (LSI_CLOCK / 8)) / 1000 - 1;
    } else if (timeout_ms <= 2048) {
        prescaler = IWDG_PRESCALER_16;  
        reload_value = (timeout_ms * (LSI_CLOCK / 16)) / 1000 - 1;
    } else if (timeout_ms <= 4096) {
        prescaler = IWDG_PRESCALER_32;  
        reload_value = (timeout_ms * (LSI_CLOCK / 32)) / 1000 - 1;
    } else if (timeout_ms <= 8192) {
        prescaler = IWDG_PRESCALER_64;  
        reload_value = (timeout_ms * (LSI_CLOCK / 64)) / 1000 - 1;
    } else if (timeout_ms <= 16384) {
        prescaler = IWDG_PRESCALER_128;  
        reload_value = (timeout_ms * (LSI_CLOCK / 128)) / 1000 - 1;
    } else {
        prescaler = IWDG_PRESCALER_256;  
        reload_value = (timeout_ms * (LSI_CLOCK / 256)) / 1000 - 1;
    }

    if (reload_value > 0xFFF) {
        reload_value = 0xFFF;
    }

    IWDG->PR = prescaler;        
    IWDG->RLR = reload_value;    

    IWDG->KR = 0xAAAA;  // Reload
    IWDG->KR = 0xCCCC;  // Start IWDG
}

void IWDG_Refresh(void) {
    IWDG->KR = 0xAAAA;  // Reload the IWDG counter
}

void IWDG_Demo(void) {
    set_sysclk_to_hse();
    Initialize_LED_GPIOs();
    delay_timer_init();
    IWDG_Init(IWDG_TIMEOUT_2S);  

    Green_LED_On();
    delay_ms(1000);  
    Green_LED_Off();

    while (1) {
       Blue_LED_Toggle();  
       // Scenario 1:
       // delay_ms(900);  // (still under 2 seconds)

       // Scenario 2:
       delay_ms(1100);    // (exceeds 2 seconds timeout)
       
			 IWDG_Refresh();
    }
}

/*
example:- 
Prescaler = 64 (IWDG->PR = 0x06)
Reload value = 0x0FFF (4095 in decimal)
LSI Frequency = 32,000 Hz
Calculating the timeout period:
Timeout = (4095×64)/32 = 8.19 sec
the IWDG will timeout after approximately 8.19 seconds, 
meaning you must refresh the IWDG within this period to prevent a reset.

LSI Clock = 32,000/64 = 500 Hz
Timeout = 500/255 = 0.51 seconds (approx.)
*/

void WWDG_Init(uint32_t timeout_value, char unit) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_WWDGEN;  // Enable WWDG clock

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
    WWDG->CR = (wwdg_counter_value & WWDG_CR_T); // Set counter value
    WWDG->CR |= WWDG_CR_WDGA; // Enable WWDG
}

void WWDG_Refresh(uint32_t wwdg_counter_value) {
    // Refresh WWDG to prevent reset
    WWDG->CR = WWDG_CR_WDGA | wwdg_counter_value; // Use the appropriate counter value
}

void WWDG_Demo(void) 
{
    set_sysclk_to_hse();        
    Initialize_LED_GPIOs();      
    delay_timer_init();          

    // Initialize WWDG with a timeout of 2 seconds
    WWDG_Init(2000, 's'); // Adjust this timeout as needed

    Green_LED_On();         
    delay_ms(3000); // Wait for 3 seconds
    Green_LED_Off();        
    
    while (1) 
    {
        Red_LED_Toggle();    
        delay_ms(500); // Keep this less than the WWDG timeout
        // WWDG_Refresh(WWDG_MAX_VALUE); // Commented for testing
    }
}

/*
PCLK1 Frequency: You can calculate or check it, but typically, it’s around 80 MHz on STM32L4 series devices.
Prescaler = 8 (WWDG->CFR)
T[6:0] (the counter value) is typically initialized to 0x7F (127 in decimal).
Calculating the timeout period:
Timeout = (( 127 + 1 )× 8)/(80*10^6) = 0.000128 seconds = 128 microseconds
the WWDG requires much more frequent refreshes compared to the IWDG. 
You must refresh the WWDG within 128 microseconds
*/

#endif