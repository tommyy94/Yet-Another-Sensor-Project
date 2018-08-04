#include "serial.h"


/**
 * @brief Initializes UART transmission with 8N1 and defined baud rate.
 * @param void
 * @return void
 */
extern void UART_init(void)
{
    /* set baud rate */
    UBRR = UBRR_VALUE;
    /* enable transmitter */
    UCSRB = (1 << TXEN);
}


/**
 * @brief Transmits 8-bit character.
 * @details Waits for empty transmit buffer.
 * @param data : Byte to transmit
 * @return void
 */
extern void UART_transmit(unsigned char data)
{
    while(!(UCSRA & (1 << UDRE))) {
        ; /* wait for empty transmit buffer */
    }

    UDR = data; /* put data into buffer and send it */
}
