#include "lib_headers.h"

int main(){
RCC->AHB1ENR |= (1 << 1); //GPIOB power clock enable
RCC->APB1ENR |= (1 << 14); //Enable SPI2 clock, bit 12 in APB2ENR
	//Alternate function ports
GPIOB->MODER &= ~(0b11111111U << (12 << 1)); //Reset bits 15:12
GPIOB->MODER |= (0b01 << (12 << 1)); //PB12 (CS) as output
GPIOB->MODER |= (0b101010U << (13 << 1)); //Set bits 15:13 to 0b101010 for alternate function (AF5)
GPIOB->OSPEEDR |= (0b11111111 << (12 << 1)); //Speed vy hi PB15:PB12

//Set AF5 (0x05) for SPI2 in AF registers (PB13:PB15)
GPIOB->AFR[1] |= (0x05 << (20)); //PB13
GPIOB->AFR[1] |= (0x05 << (24)); //PB14
GPIOB->AFR[1] |= (0x05 << (28)); //PB15
	
	//Set SPI2 properties
SPI2->CR1 |= (1 << 2); //Master mode
SPI2->CR1 |= SPI_CR1_SSM; //Software slave management enabled
SPI2->CR1 |= SPI_CR1_SSI; //Internal slave select The value of this bit is 
//forced onto the NSS pin and the IO value of the NSS pin is ignored.
SPI2->CR1 |= SPI_CR1_SPE; //SPI2 enable
	
}