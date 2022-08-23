#include <stdint.h>

static volatile unsigned int * const UART0DR = ( unsigned int * ) ( uintptr_t * ) 0x9000000;

int putchar(int c)
{
    *UART0DR = c; /* Transmit char */
     return c;
}

void putchar_uart0( int c )
{
    *UART0DR = c; /* Transmit char */
}

void putc_uart0( int c )
{
    *UART0DR = c; /* Transmit char */
}

void print_uart0( const char * s )
{
    while( *s != '\0' )                     /* Loop until end of string */
    {
        *UART0DR = ( unsigned int ) ( *s ); /* Transmit char */
        s++;                                /* Next char */
    }
}

void puts_uart0( const char * s )
{
    while( *s != '\0' )                     /* Loop until end of string */
    {
        *UART0DR = ( unsigned int ) ( *s ); /* Transmit char */
        if (*s == '\n') {
           *UART0DR = ( unsigned int ) ( '\r' );
        }
        s++;                                /* Next char */
    }
}
