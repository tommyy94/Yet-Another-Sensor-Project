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
inline void PRR_init(void);
void UART_transmit(unsigned char data);
void power_down(void);
void ADC_start_conversion(void);


/**
 * @brief Main entry point.
 * @param void
 * @return 0
 */
int main(void)
{
    clk_init();
    PRR_init();
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


/**
 * @brief Initializes system clock.
 * @details 1 MHz Clock frequency
 * @param void
 * @return void
 */
inline void clk_init(void)
{
    CCP = IO_SIGNATURE;
    CLKPSR = (1 << CLKPS1) | (1 << CLKPS0);
}


/**
 * @brief Initializes watchdog timer.
 * @details 128K WDT Oscillator (Cycles) = ~1.0s oscillator.
 * @param void
 * @return void
 */
inline void WDT_init(void)
{
    WDTCSR = (1 << WDP2) | (1 << WDP1);
}


/**
 * @brief Initializes UART transmission with defined baud rate.
 * @param void
 * @return void
 */
inline void UART_init(void)
{
    /* set baud rate */
    UBRR = UBRR_VALUE;
    /* enable transmitter */
    UCSRB = (1 << TXEN);
}


/**
 * @brief Initializes ADC on channel 0.
 * @details Set ADC prescaler to 8 resulting in 125 kHz resolution. Also enables
 * interrupt on completed conversion.
 * @param void
 * @return void
 */
inline void ADC_init(void)
{
    ADMUX = (1 << REFS0) | PINA0;
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 = PINA0;
}


/**
 * @brief Ensures that Timer 0 is disabled to save power.
 * @param void
 * @return void
 */
inline void PRR_init(void)
{
    PRR |= (1 << PRTIM0);
}


/**
 * @brief Transmits 8-bit character.
 * @details Waits for empty transmit buffer. Interrupts are disabled during
 * transmission.
 * @param data : Byte to transmit
 * @return void
 */
void UART_transmit(unsigned char data)
{
    cli();

    while(!(UCSRA & (1 << UDRE))) {
        ; /* wait for empty transmit buffer */
    }

    UDR = data; /* put data into buffer and send it */
    sei();
}


/**
 * @brief Puts the MCU to sleep mode.
 * @details Watchdog Interrupt is enable and interrupt flag is set before
 * sleeping. Disabled and unset after waking up.
 * @param void
 * @return void
 */
void power_down(void)
{
    WDTCSR |= (1 << WDIE);                  /* enable watchdog interrupt */
    _delay_ms(5);                           /* wait until transmission done */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    /* mode has to be set every time */
    sleep_enable();
    sei();                                  /* interrupt is used to wake up */
    sleep_cpu();                            /* power-down */
    sleep_disable();
    WDTCSR &= ~(1 << WDIE);                 /* disable watchdog interrupt */
}


/**
 * @brief Starts Analog-to-digital conversion by turning off the CPU.
 * @details ADC is power on only during conversion to save power.
 * @param void
 * @return void
 */
void ADC_start_conversion(void)
{
    PRR &= ~(1 << PRADC);           /* power on ADC */
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sei();                          /* interrupt is used to wake up */
    sleep_cpu();                    /* ADC Noise Reduction Mode */
    sleep_disable();
    PRR |= (1 << PRADC);            /* power off ADC */
}


/**
 * @brief Watchdog vector
 * @param void
 * @return void
 */
ISR(WDT_vect)
{
    ;
}


/**
 * @brief ADC vector
 * @param void
 * @return void
 */
ISR(ADC_vect)
{
    ;
}
