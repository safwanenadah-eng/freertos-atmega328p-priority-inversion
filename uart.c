#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "uart.h"

void UART_Init(uint32_t baudrate)
{
    uint16_t ubrr_value = (F_CPU / (16UL * baudrate)) - 1;

    UBRR0H = (uint8_t)(ubrr_value >> 8);
    UBRR0L = (uint8_t)ubrr_value;

    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_SendChar(char c)
{
    while (!(UCSR0A & (1 << UDRE0))) { ; }
    UDR0 = c;
}

void UART_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART_SendChar(*str);
        str++;
    }
}