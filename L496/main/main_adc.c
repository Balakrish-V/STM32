#include "lib_headers.h"
uint32_t adcValue = 0;
int main(void)
{
	  //set_sysclk_to_msi(16000);
	  init_clock();
	  uart2_config(115200);
	  delay_timer_init();
    //ADC_Init(CHANNEL_8, GPIO_PORT_A, 3);   // initialize ADC1
	  adcInit();
    while (1) 
		{
        //adcValue = ADC1_Read();       // read ADC value
        adcValue = adcRead();
			  uart2_send_integer(adcValue);
			  uart2_send_string("\r\n",2);
    }
}
