#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/setbaud.h>


#define IO_SIGNATURE 0xD8


inline void clk_init(void);
inline void WDT_init(void);
inline void UART_init(void);
inline void ADC_init(void);
void UART_transmit(unsigned char data);
void power_down(void);
void ADC_start_conversion(void);


int main(void)
{
    clk_init();
    WDT_init();
    UART_init();
    ADC_init();

    while(1) {
        ADC_start_conversion();
        UART_transmit(ADCH);
        UART_transmit(ADCL);
        power_down();
    }
    return 0;
}


inline void clk_init(void)
{
    CCP = IO_SIGNATURE;
    CLKPSR = (1 << CLKPS1) | (1 << CLKPS0);
}


inline void WDT_init(void)
{
    WDTCSR = (1 << WDP2) | (1 << WDP1) ;
}


inline void UART_init(void)
{
    /* set baud rate */
    UBRR = UBRR_VALUE;
    /* enable transmitter */
    UCSRB = (1 << TXEN);
}


inline void ADC_init(void)
{
    ADMUX = (1 << REFS0) | PINA0;
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 = PINA0;
}


void UART_transmit(unsigned char data)
{
    cli();

    while(!(UCSRA & (1 << UDRE))) {
        ; /* wait for empty transmit buffer */
    }

    UDR = data; /* put data into buffer and send it */
    sei();
}


void power_down(void)
{
    WDTCSR |= (1 << WDIE);  /* enable watchdog interrupt */
    _delay_ms(5);           /* wait until transmission done */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei();                  /* interrupt is used to wake up */
    sleep_cpu();            /* actual sleep happens here */
    sleep_disable();        /* disable sleep to avoid accidental sleep after wakeup */
    WDTCSR &= ~(1 << WDIE); /* disable watchdog interrupt */
}


void ADC_start_conversion(void)
{
    PRR &= ~(1 << PRADC);   /* power on ADC */

    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sei();                  /* interrupt is used to wake up */
    sleep_cpu();            /* actual sleep happens here */
    sleep_disable();        /* disable sleep to avoid accidental sleep after wakeup */

    PRR |= (1 << PRADC);    /* power off ADC */
}


ISR(WDT_vect)
{
    ;
}


ISR(ADC_vect)
{
    ;
}
