#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <stdbool.h>

#include "LCD_driver/LCD.h"


#define BUFSIZ 254
#define DELAY 10
#define EOT 0x04


volatile char RX_buff[BUFSIZ];
volatile uint8_t buff_index = 0;
volatile bool RX_flag = false;


inline void UART_init(void);


void main(void)
{
    char string[BUFSIZ];

    LCD_init();
    UART_init();
    _delay_ms(DELAY);
    sei();

    while(1) {
        if(RX_flag == true) {
            for(uint8_t i = 0; i <= BUFSIZ - buff_index; i += 2) {
                string[0] = RX_buff[i];
                string[1] = RX_buff[i + 1];
                LCD_write_string(0, 0, (const char *)string);
            }
            RX_flag = false;
        }
        _delay_ms(DELAY);
    }
}


inline void UART_init(void)
{
    /* enable internal pull-up */
    DDRD &= ~(1 << PIND0);
    PORTD |= (1 << PIND0);

    /* set baud rate */
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    /* enable receiver and its interrupt */
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0);
}


ISR(USART_RX_vect)
{
    if(buff_index >= BUFSIZ) {
        buff_index = 0;
    }

    RX_buff[buff_index++] = UDR0;
    RX_flag = true;
}
