// Standard library includes.
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32l4xx.h"

extern uint32_t SystemCoreClock;
char rxb;

/**
 * Function to send data over UART2.
 * @param data: Pointer to the data buffer to send.
 * @param size: Number of bytes to send.
 * @return Number of bytes sent.
 */
int uart2_send(const char *data, int size) 
{
  int count = size;
  while (count--) {
    while (!(USART2->ISR & USART_ISR_TXE)) {}; // Wait until TX ready.
    USART2->TDR = *data++;
  }
  return size;
}

/**
 * Main program.
 */
int main(void) 
{
  // Set the core system clock speed.
  RCC->CR |= (RCC_CR_HSION);
  while (!(RCC->CR & RCC_CR_HSIRDY)) {};
  RCC->CFGR &= ~(RCC_CFGR_SW);
  RCC->CFGR |= (RCC_CFGR_SW_HSI);
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {};
  SystemCoreClock = 16000000;

  // Enable peripheral clocks and set up GPIO pins.
  RCC->APB1ENR1 |= (RCC_APB1ENR1_USART2EN);
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
  GPIOA->MODER &= ~((0x3 << (2 * 2)) | (0x3 << (15 * 2)));
  GPIOA->MODER |= ((0x2 << (2 * 2)) | (0x2 << (15 * 2)));
  GPIOA->OTYPER &= ~((0x1 << 2) | (0x1 << 15));
  GPIOA->OSPEEDR &= ~((0x3 << (2 * 2)) | (0x3 << (15 * 2)));
  GPIOA->OSPEEDR |= ((0x2 << (2 * 2)) | (0x2 << (15 * 2)));
  GPIOA->AFR[0] &= ~(0xF << (2 * 4));
  GPIOA->AFR[0] |= (0x7 << (2 * 4));
  GPIOA->AFR[1] &= ~(0xF << ((15 - 8) * 4));
  GPIOA->AFR[1] |= (0x3 << ((15 - 8) * 4));

  // Setup the NVIC to enable interrupts.
  NVIC_SetPriorityGrouping(0);
  uint32_t uart_pri_encoding = NVIC_EncodePriority(0, 1, 0);
  NVIC_SetPriority(USART2_IRQn, uart_pri_encoding);
  NVIC_EnableIRQ(USART2_IRQn);

  // Set the baud rate to 9600.
  uint16_t uartdiv = SystemCoreClock / 9600;
  USART2->BRR = uartdiv;

  // Enable the USART peripheral with RX and RX timeout interrupts.
  USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE);

  // Main loop: wait for a new byte, then echo it back.
  const char *msg = "Received: ";
  while (1) 
	{
    __WFI();
    uart2_send(msg, strlen(msg));
    uart2_send(&rxb, 1);
  }
}

// USART2 interrupt handler
void USART2_IRQHandler(void) 
{
  if (USART2->ISR & USART_ISR_RXNE) {
    rxb = USART2->RDR;
  }
}
