#include "lib_headers.h"
uint8_t rx_buffer = 0;
uint32_t send_int = 8000000;
char *snd_string = "Test";  
char RCV_STRNG[30] = {0};
const char *password = "boss";
float send = 123.45678;
int main(void) 
{
	  Initialize_LED_GPIOs();
	  delay_timer_init();
    //set_sysclk_to_hse();  // set to 8Mhz in clocks header
	  set_sysclk_to_msi(16000);  // Set to 16 MHz
    uart2_config(115200); // Configure UART2
//	  bool var = uart2_password_verification(password);
//	  if(var)
//			Green_LED_On();
//		else
//			Red_LED_On();
    while (1) {
			  //rx_buffer = uart2_receive_character();
			  //uart2_send_character(rx_buffer);
				//uart2_send_integer(&send_int);
			  //uart2_send_string(snd_string);
			  //uart2_receive_string((char*)&RCV_STRNG,10);
			  //uart2_send_string((char*)&RCV_STRNG,strlen(RCV_STRNG));
			  uart2_send_integer(-800000);
			  uart2_send_character('\r');
			  uart2_send_character('\n');
				uart2_send_float(send,3);
			  uart2_send_character('\r');
			  uart2_send_character('\n');
				delay_ms(1000);
    }
}
