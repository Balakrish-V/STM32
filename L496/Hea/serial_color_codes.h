#ifndef COLORS_H
#define COLORS_H

// ANSI Escape Code Macros
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"

// Macro for formatted overflow message
#define BUFFER_OVERFLOW_MSG_UART   "\r\n" ANSI_RED "Buffer overflow" ANSI_RESET "\r\n"

#define PASSWORD_CORRECT_MSG_UART   "\r\n" ANSI_GREEN "Password correct!" ANSI_RESET "\r\n"
#define PASSWORD_MISMATCH_MSG_UART   "\r\n" ANSI_RED "Password mismatch!" ANSI_RESET "\r\n"

#endif