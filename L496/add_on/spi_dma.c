#include "stm32l4xx.h"
#include <stdio.h>
#include <string.h>

// define chip select (CS) pin and port
#define CS_PIN         (1 << 9)
#define CS_GPIO_PORT   GPIOC

// SPI3 configuration macros
#define SPI_BAUDRATE_4000Kbps   (SPI_CR1_BR_1)   
#define SPI_MASTER_MODE         (SPI_CR1_MSTR)
#define SPI_MSB_FIRST           (0x0)
#define SPI_MODE_0              (0x0)

// example data to send
volatile uint16_t txBuffer[5] = {0x1234, 0x5678, 0xABCD, 0xEF12, 0x3456};
volatile uint16_t rxBuffer[5];
volatile uint16_t txSize = 5;
volatile uint16_t rxSize = 5;

// function prototypes
void GPIO_Init(void);
void SPI3_Init(uint32_t mode, uint32_t baudrate, uint32_t dataOrder, uint32_t role);
void DMA_Init(void);
void SPI3_Transmit_DMA(void);
void SPI3_Receive_DMA(void);
void print_buffer(const char* prefix, volatile uint16_t* buffer, uint16_t size);

void SPI3_Transmit_DMA(void)
{
    CS_GPIO_PORT->BRR = CS_PIN;       // set CS low (select the slave)
    SPI3->CR1 |= SPI_CR1_SPE;         // enable SPI
    DMA2_Channel2->CCR |= DMA_CCR_EN; // enable DMA for TX
}

void SPI3_Receive_DMA(void)
{
    CS_GPIO_PORT->BRR = CS_PIN;       // set CS low (select the slave)
    SPI3->CR1 |= SPI_CR1_SPE;         // enable SPI
    DMA2_Channel1->CCR |= DMA_CCR_EN; // enable DMA for RX
}

void DMA2_Channel1_IRQHandler(void)
{
    // check if the RX transfer is complete
    if (DMA2->ISR & DMA_ISR_TCIF1)
    {
        DMA2->IFCR |= DMA_IFCR_CTCIF1;  // clear transfer complete flag
        DMA2_Channel1->CCR &= ~DMA_CCR_EN;  // disable DMA channel
        CS_GPIO_PORT->BSRR = CS_PIN;  // set CS high (deselect the slave)

        // print received buffer data
        printf("DMA RX Transfer Complete\n");
        print_buffer("RX Buffer: ", rxBuffer, rxSize);

        // re-enable continuous RX DMA
        SPI3_Receive_DMA();
    }
}

void DMA2_Channel2_IRQHandler(void)
{
    // check if the TX transfer is complete
    if (DMA2->ISR & DMA_ISR_TCIF2)
    {
        DMA2->IFCR |= DMA_IFCR_CTCIF2;  // clear transfer complete flag
        DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // disable DMA channel
        CS_GPIO_PORT->BSRR = CS_PIN;  // set CS high (deselect the slave)

        // print transmitted buffer data
        printf("DMA TX Transfer Complete\n");
        print_buffer("TX Buffer: ", txBuffer, txSize);

        // re-enable continuous TX DMA
        SPI3_Transmit_DMA();
    }
}

int main(void)
{
    // system initialization
    SystemInit();

    // initialize GPIO, SPI3, and DMA
    GPIO_Init();
    SPI3_Init(SPI_MODE_0, SPI_BAUDRATE_4000Kbps, SPI_MSB_FIRST, SPI_MASTER_MODE);
    DMA_Init();

    // start continuous transmit and receive in DMA mode
    SPI3_Transmit_DMA();
    SPI3_Receive_DMA();

    // loop forever
    while (1) {}
}

void GPIO_Init(void)
{
    // enable clock for GPIOC and GPIOB
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // configure GPIO pins for SPI3 (PC10: SCK, PC11: MISO, PC12: MOSI)
    GPIOC->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);
    GPIOC->MODER |= (GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1);
    GPIOC->AFR[1] |= (0x6 << (4 * (10 - 8))) | (0x6 << (4 * (11 - 8))) | (0x6 << (4 * (12 - 8)));

    // configure CS pin (PC9)
    GPIOC->MODER &= ~(GPIO_MODER_MODE9);      // clear mode bits
    GPIOC->MODER |= GPIO_MODER_MODE9_0;       // output mode
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_9);     // push-pull
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED9;   // high speed
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD9);      // no pull-up/pull-down
}

void SPI3_Init(uint32_t mode, uint32_t baudrate, uint32_t dataOrder, uint32_t role)
{
    // enable clock for SPI3
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;

    // set CS high (deselect the slave)
    CS_GPIO_PORT->BSRR = CS_PIN;

    // configure SPI3
    SPI3->CR1 = role | baudrate | dataOrder | mode | SPI_CR1_SSI | SPI_CR1_SSM; // master/slave, baudrate, data order, mode, software slave management

    // set data frame size to 16 bits and enable DMA
    SPI3->CR2 = (0xF << SPI_CR2_DS_Pos) | SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;

    // enable SPI
    SPI3->CR1 |= SPI_CR1_SPE;
}


void DMA_Init(void)
{
    // enable clock for DMA2
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // configure DMA2 Channel 1 for SPI3_RX
    DMA2_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_PL_1 | DMA_CCR_PSIZE_1 | DMA_CCR_MSIZE_1 | DMA_CCR_TCIE;
    DMA2_Channel1->CPAR = (uint32_t)&SPI3->DR; // peripheral address (SPI3 data register)
    DMA2_Channel1->CMAR = (uint32_t)rxBuffer;  // memory address (receive buffer)
    DMA2_Channel1->CNDTR = rxSize;             // number of data to transfer

    // configure DMA2 Channel 2 for SPI3_TX
    DMA2_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_PL_1 | DMA_CCR_PSIZE_1 | DMA_CCR_MSIZE_1 | DMA_CCR_DIR | DMA_CCR_TCIE;
    DMA2_Channel2->CPAR = (uint32_t)&SPI3->DR; // peripheral address (SPI3 data register)
    DMA2_Channel2->CMAR = (uint32_t)txBuffer;  // memory address (transmit buffer)
    DMA2_Channel2->CNDTR = txSize;             // number of data to transfer

    // enable DMA2 Channel 1 and Channel 2 interrupts in NVIC
    NVIC_EnableIRQ(DMA2_Channel1_IRQn);
    NVIC_EnableIRQ(DMA2_Channel2_IRQn);
}

// helper function to print buffer data using printf
void print_buffer(const char* prefix, volatile uint16_t* buffer, uint16_t size)
{
    printf("%s", prefix);
    for (int i = 0; i < size; i++) {
        printf("0x%04X ", buffer[i]);
    }
    printf("\n");
}
