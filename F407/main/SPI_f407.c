#include "All_headers.h"

uint8_t data_to_send[5] = {0x01, 0x02, 0x03, 0x04, 0x05}; // Example data to send
uint8_t received_data[5];                                 // Buffer to store received data

// Main function to demonstrate SPI communication with the slave device
int main(void) 
{
    Initialize_LED_GPIOs_F4();    // for debug
	
    set_sysclk_to_hsi();  
	
    // Initialize the SPI with desired parameters
    SPI_Init(SPI1, 0, 1000000, 0); // Example: SPI1, Mode 0, 1 MHz, MSB first
	
    // Transmit data to the slave
    SPI_Transmit(SPI1, data_to_send, 3);

    // Receive data from the slave
    SPI_Receive(SPI1, received_data, 3);

    while (1) 
    {
        // Main loop can include further processing or handling of received data
    }

    return 0;
}
