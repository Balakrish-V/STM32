#include "All_headers.h"

void delay_ms(volatile uint32_t ms)
{
    // Delay in milliseconds
    uint32_t count = (SystemCoreClock / 1000) * ms; // Convert ms to clock cycles
    for (; count > 0; count--)
    {
        __NOP(); // No operation, just burn cycles
    }
}

int main(void)
{
    set_sysclk_to_84(); // Start with 84 MHz
    SystemCoreClockUpdate(); // Ensure the system clock is updated
    Initialize_LED_GPIOs_F4();
    All_LEDs_On_F4();
    volatile uint32_t i = 0;
    while (1)
    {
        if (i > 40) 
            i = 0;
        else 
            i++;

        switch (i)
				{
				case 0: 
					set_sysclk_to_hse(); 
					SystemCoreClockUpdate();  // Update system clock after switch
					break; 
				case 20: 
					set_sysclk_to_hsi(); 
					SystemCoreClockUpdate();  // Update system clock after switch
					break;
				case 35: 
					set_sysclk_to_84(); 
					SystemCoreClockUpdate();  // Update system clock after switch
					break; 
				default: 
					break;
				}
        delay_ms(10); 
        All_LEDs_Toggle_F4();
    }
    return 0;
}

