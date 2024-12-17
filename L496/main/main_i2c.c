#include "lib_headers.h"

uint8_t data_to_send[] = {0xAA, 0xBB, 0xCC};  // Example data (3 bytes)
uint8_t data_received[3];         // Buffer for received data
uint8_t my_own_address = 0x10;               // own address (7-bit format)
uint8_t found_device = 0;

int main(void)
{
	  set_sysclk_to_msi(MSI_16MHZ);   //16MHz
	  // init_clock();                //80MHz
	  Initialize_LED_GPIOs();
	  delay_timer_init();
	  I2C1_gpio_config();
	  I2C_Config(my_own_address, I2C_SPEED_FAST_PLUS);  // Example to set own address and speed to Fast (400 kHz)

    for (uint8_t addr = 0; addr < 127; addr++) 
	  {
        if (I2C_Scanner(addr)) {          // Scan for I2C devices
            found_device = addr;          // Store found device address
            break;                        // Stop after finding the first device
        }
    }    
		while (1) {
			  I2C_Transmit(found_device, sizeof(data_to_send), data_to_send); // Transmit data
			  I2C_Receive(found_device, sizeof(data_received), data_received);
			  delay_ms(1000);
    }
}
