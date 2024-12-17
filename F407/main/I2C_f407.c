#include "All_headers.h"
// Data to send
uint8_t data_to_send[4] = {0xAA, 0xBB, 0xCC, 0xDD};
uint8_t data_received[3]; // Array to store received data

int main(void) 
{
	  printf("HI\r\n");
	  set_sysclk_to_hsi();
	  Initialize_LED_GPIOs_F4();
    // Parameters for I2C initialization
    uint32_t i2c_speed = 100000;         // I2C speed set to 100 kHz (standard mode)
    uint8_t i2c_mode = I2C_MODE_MASTER;  // Set as master mode
    uint8_t slave_address = 0x0A;        // Example slave address

    // Initialize I2C1
    I2C_Init(I2C1, i2c_mode, i2c_speed, slave_address);

    // Transmit data to the slave
    if (I2C_Transmit(I2C1, slave_address, 4, data_to_send) == 0) {
    } else {
			LED_On_F4(LED_PIN_12);
			LED_On_F4(LED_PIN_14);
    }

    // Receive data from the slave
    if (I2C_Receive(I2C1, slave_address, 3, data_received) == 0) {
    } else {
			LED_On_F4(LED_PIN_13);
			LED_On_F4(LED_PIN_15);
    }

    // Main loop
    while (1) {
			
    }
    return 0;
}
