#ifndef UART_H
#define UART_H

#include "lib_headers.h"

void uart2_config(uint32_t baud_rate);

void uart2_send_character(char ch);
char uart2_receive_character(void);

void uart2_send_string(const char *send_string, uint32_t max_length);
void uart2_receive_string(char *rcv_string, uint32_t max_length);
bool uart2_password_verification(const char *actual_password);

void uart2_send_integer(int32_t value);
void uart2_send_float(float value, uint8_t decimal_places);

#endif // UART_H
