#ifndef UART_C
#define UART_C

#include "lib_headers.h"

void uart2_config(uint32_t baud_rate)
{
    // Step 1: Enable GPIOD clock (for PD5 and PD6)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;

    // Step 2: Set PD5 (TX) and PD6 (RX) to alternate function mode
    GPIOD->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6); // Clear MODER5 and MODER6 bits
    GPIOD->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1); // Set to Alternate function (10)

    // Step 3: Select the correct alternate function (AF7 for USART2) for PD5 and PD6
    GPIOD->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL6);
    GPIOD->AFR[0] |= (7 << GPIO_AFRL_AFSEL5_Pos) | (7 << GPIO_AFRL_AFSEL6_Pos);

    // Step 4: Configure PD5 and PD6 as high-speed output
    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6);

    // Step 5: Configure PD5 and PD6 as push-pull outputs (clear open-drain)
    GPIOD->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6);

    // Step 6: Configure PD5 and PD6 to use pull-up resistors
    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6); // Clear pull-up/pull-down configuration
    GPIOD->PUPDR |= (GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD6_0); // Set to pull-up

    // Step 7: Enable USART2 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Enable USART2 clock

    // Step 8: Configure the baud rate dynamically (for HSE 16 MHz)
    uint32_t pclk1 = SystemCoreClock; // Assuming PCLK1 is equal to SystemCoreClock (HSE at 16 MHz)
    uint32_t uartdiv = (pclk1 + (baud_rate / 2U)) / baud_rate; // Proper rounding for baud rate
    USART2->BRR = uartdiv; // Set baud rate in BRR register

    // Step 9: Enable USART2 and configure TX, RX
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; // Enable RX, TX, and USART
}

void uart2_send_character(char data)
{
    while (!(USART2->ISR & USART_ISR_TXE)); // Wait until TXE is set
    USART2->TDR = data; // Transmit data
    while (!(USART2->ISR & USART_ISR_TC)); // Wait until TC is set
}

char uart2_receive_character(void)
{
    while (!(USART2->ISR & USART_ISR_RXNE)); // Wait until RXNE is set
    return (uint8_t)(USART2->RDR); // Read received data
}

// Function to send an integer value via UART
void uart2_send_integer(int32_t value) {
    char buffer[12]; // Buffer to hold the string representation of the integer
    int index = 0;
    bool is_negative = false;

    // Handle negative values
    if (value < 0) {
        is_negative = true;
        value = -value; // Make the value positive for conversion
    }

    // Convert integer to string
    do {
        buffer[index++] = (value % 10) + '0'; // Get last digit and convert to char
        value /= 10; // Remove last digit
    } while (value > 0);

    if (is_negative) {
        buffer[index++] = '-'; // Add minus sign for negative numbers
    }

    // Reverse the string in the buffer
    for (int i = 0; i < index / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[index - 1 - i];
        buffer[index - 1 - i] = temp;
    }

    buffer[index] = '\0'; // Null-terminate the string

    // Send each character in the buffer
    for (int i = 0; i < index; i++) {
        uart2_send_character(buffer[i]);
    }
}

void uart2_send_float(float value, uint8_t decimal_places) {
    // Prepare to send integer part
    int32_t int_part = (int32_t)value;
    uart2_send_integer(int_part); // Send integer part

    // Send decimal point
    uart2_send_character('.');

    // Prepare to send fractional part
    float frac_part = value - int_part; // Get fractional part

    // Send up to the specified number of decimal places
    for (uint8_t i = 0; i < decimal_places; i++) {
        frac_part *= 10.0f; // Shift left by one decimal place
        int32_t frac_digit = (int32_t)frac_part; // Get the next digit
        uart2_send_character(frac_digit + '0'); // Send the digit
        frac_part -= frac_digit; // Remove the digit from the fractional part
    }
}

// Function to send a string via UART with a specified number of characters
void uart2_send_string(const char *send_string, uint32_t max_length) {
    uint32_t i;
    for (i = 0; i < max_length; i++) {
        // Check for null terminator
        if (send_string[i] == '\0') {
            break; // Stop if the string is already null-terminated
        }
        uart2_send_character(send_string[i]); // Send each character
    }
    
    // If the null terminator wasn't found within max_length, add it
    if (i == max_length) {
        uart2_send_character('\0'); // Send explicit null terminator
    }
}

// Function to receive a string via UART up to a specified number of characters
void uart2_receive_string(char *rcv_string, uint32_t max_length) {
    char ch;
    uint32_t i = 0;

    while (1) {  // Loop until we explicitly break
        ch = uart2_receive_character();  // Receive character via UART

        // Break the loop on carriage return or newline
        if (ch == '\r' || ch == '\n') {
            break;
        }

        // Check for buffer overflow
        if (i >= max_length - 1) {
            // Send overflow message via UART using uart2_send_string
            uart2_send_string(BUFFER_OVERFLOW_MSG_UART, strlen(BUFFER_OVERFLOW_MSG_UART));
            rcv_string[i] = '\0';  // Null-terminate the string
            return;  // Exit the function
        }

        rcv_string[i++] = ch;  // Store received character
        uart2_send_character(ch);  // Echo the received character
    }
    rcv_string[i] = '\0';  // Null-terminate the string
}

// Function to verify the password
bool uart2_password_verification(const char *actual_password) {
    char entered_password[20];  
    char ch;
    uint8_t i = 0;

    // Receive the password input from the user
    while (1) {
        ch = uart2_receive_character();  // Receive character via UART
        
        if (ch == '\r' || ch == '\n') {  // Check for Enter key (carriage return or newline)
            entered_password[i] = '\0';  // Null-terminate the entered password
            break;
        } 
        else if (ch == '\b') {  // Check for backspace
            if (i > 0) {
                uart2_send_character('\b');  // Move cursor back
                uart2_send_character(' ');   // Erase the last character
                uart2_send_character('\b');  // Move cursor back again
                i--;  // Adjust index to overwrite previous character
            }
        } else if (i < sizeof(entered_password) - 1) {  // Prevent buffer overflow
            entered_password[i++] = ch;  // Store character in entered_password
            uart2_send_character('*');   // Send '*' to mask the input
        }
    }

    // Compare the entered password with the provided actual password
    if (strcmp(entered_password, actual_password) == 0) {
        // Send success message in green
        uart2_send_string(PASSWORD_CORRECT_MSG_UART, strlen(PASSWORD_CORRECT_MSG_UART));
        return true;  // Passwords match
    } else {
        // Send error message in red
        uart2_send_string(PASSWORD_MISMATCH_MSG_UART, strlen(PASSWORD_MISMATCH_MSG_UART));
        return false;  // Passwords do not match
    }
}

#endif