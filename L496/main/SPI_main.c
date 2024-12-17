#include "lib_headers.h"                       // Include the appropriate header for STM32L4

uint8_t txData[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};  // Data to transmit
uint8_t rxData[6] = {0};                             // Buffer for received data

void SPI_TransmitReceive_byte(uint8_t *wdata,uint8_t *rdata) 
{
    CS_Low();                                  // Pull CS low
    while (!(SPI1->SR & SPI_SR_TXE)) {}        // Wait until TX buffer is empty
    SPI1->DR = *wdata;                         // Write the 8-bit data to the DR
    while (!(SPI1->SR & SPI_SR_RXNE)) {}       // Wait until RX buffer is not empty
    *rdata = SPI1->DR;                         // Read received data
    while (SPI1->SR & SPI_SR_BSY) {}           // Wait until SPI is not busy
    CS_High();                                 // Pull CS high
}

void SPI_TransmitReceiveString(const char *tx_data, char *rx_data, uint16_t length) 
{
    for (uint16_t i = 0; i < length; i += 2) 
    {
        // Wait until the transmit buffer is empty
        while (!(SPI1->SR & SPI_SR_TXE)) {}

        // Combine two bytes into a 16-bit word for transmission
        uint16_t tx_word = (tx_data[i+1] << 8) | tx_data[i];
        SPI1->DR = tx_word;

        // Wait until a word is received
        while (!(SPI1->SR & SPI_SR_RXNE)) {}

        // Read the received 16-bit word
        uint16_t rx_word = SPI1->DR;

        // Store the received bytes in the correct order
        rx_data[i + 1] = (uint8_t)(rx_word >> 8);      // High byte
        rx_data[i] = (uint8_t)(rx_word & 0xFF); // Low byte

        // Ensure SPI transaction completes
        while (SPI1->SR & SPI_SR_BSY) {}
    }

    // Pull CS high after the transaction
    CS_High();

    // Null-terminate the received string for safety
    rx_data[length] = '\0';
}

int main(void) 
{
    set_sysclk_to_hsi();  // Have connection with SPI_TARGET_SPEED
    Initialize_LED_GPIOs();
    delay_timer_init();
    GPIOConfig();
    SPIConfig(SPI_MODE_0, SPI_TARGET_SPEED, SPI_MASTER_MODE);  // Configure SPI1 as Master, Mode 0, at 1MHz speed
	  Green_LED_On();
	  /*
    SPI_TransmitString(txData, 4);
	  delay_ms(500);
    SPI_ReceiveString(rxData, 4);
	
    SPI_TransmitReceive_byte(&tr,&wr);  
	  */
	  //SPI_TransmitReceiveString((const char *)txData,(char *)rxData,4);
	  SPI_ReceiveString((char *)rxData,4);
	  while (1) 
		{
			Blue_LED_Toggle();
			delay_sec(1);
        // Main loop
    }
}
                                                    