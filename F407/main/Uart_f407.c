#include "All_headers.h"
uint8_t received_char = 0;
int main(void)
{
    set_sysclk_to_hsi();
	  uart2_init(115200);
	  //ASCII ART
    uint8_t send_string[] =
    "  .g8\"\"\"bg      \033[31m`7MMF'\033[0m                    \r\n"
    ".dP'              \033[31mMM\033[0m                      \r\n"
    "dM'               \033[31mMM\033[0m           ,AM        \r\n"
    "MM                \033[31mMM\033[0m          AVMM        \r\n"
    "MM.               \033[31mMM\033[0m        ,W' MM        \r\n"
    "`Mb.     ,'       \033[31mMM\033[0m      ,W'   MM        \r\n"
    "  `\"bmmmd'      \033[31m.JMML.\033[0m   'AmmmmmMMmm     \r\n"
    "                                MM        \r\n"
    "                                MM        \r\n";
    uart2_send_str((uint8_t *)send_string);
	  Initialize_LED_GPIOs_F4();
    while (1)
    {
			  LED_Toggle_F4(LED_PIN_14);
			  delay_ms(1000);
        //received_char = uart2_rcv_char();
        //uart2_send_char(received_char); // Echo back the received character
    }
    return 0;
}

