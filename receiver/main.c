#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <stdbool.h>
#include <stdlib.h>

#include "LCD_driver/LCD.h"


#define BUFSIZ 254
#define DELAY 10


volatile char RX_buff[BUFSIZ];
volatile uint8_t buff_index = 0;
volatile bool RX_flag = false;


inline void UART_init(void);
uint16_t get_multiplier(uint8_t value);


/**
 * @brief Main entry point.
 * @param void
 * @return void
 */
void main(void)
{
    char string[BUFSIZ];
    uint8_t adc_val_h;
    uint8_t adc_val_l;
    uint16_t adc_val;
    uint16_t mul;

    LCD_init();
    UART_init();
    _delay_ms(DELAY);
    sei();

    while(1) {
        if(RX_flag == true) {
            for(uint8_t i = 0; i <= BUFSIZ - buff_index; i += 2) {
                adc_val_h = RX_buff[i];
                adc_val_l =  RX_buff[i + 1];
                mul = get_multiplier(adc_val_l);
                adc_val = (adc_val_h * mul) + adc_val_l;
                itoa(adc_val, string, 10);
                LCD_write_string(0, 0, (const char *)string);
            }
            RX_flag = false;
        }
        _delay_ms(DELAY);
    }
}


/**
 * @brief Initializes UART with 8N1 and defined baud rate.
 * @param void
 * @return void
 */
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


/**
 * @brief Determines multiplier based on number of digits.
 * @param value : value to get multiplier of
 * @return mul : multiplier
 */
uint16_t get_multiplier(uint8_t value)
{
    uint16_t mul;

    if(value >= 100) {
        mul = 1000;
    } else if(value < 100 && (value >= 10)) {
        mul = 100;
    } else {
        mul = 10;
    }

    return mul;
}


/**
 * @brief USART vector
 */
ISR(USART_RX_vect)
{
    if(buff_index >= BUFSIZ) {
        buff_index = 0;
    }

    RX_buff[buff_index++] = UDR0;
    RX_flag = true;
}
