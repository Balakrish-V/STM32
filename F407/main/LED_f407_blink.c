#include "All_headers.h"  

#define LED_DELAY 100000  // Adjust this value based on your system clock for a 1-second delay

void delay(volatile uint32_t s)
{
    for (; s > 0; s--); 
}

int main(void)
{
    Initialize_LED_GPIOs_F4();
    while (1)
    {
        All_LEDs_Toggle_F4();
        delay(LED_DELAY);
    }
    return 0;
}

