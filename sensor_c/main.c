#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/setbaud.h>


#define IO_SIGNATURE 0xD8
#define HEARTBEAT

#define SYS_RESET_PIN PINB1
#define ADC_CHANNEL PINA0


/**
 * @brief Reset the software by using Watchdog Timer.
 */
#define software_reset() /* resets the system */    \
    do {                                            \
        wdt_enable(WDTO_15MS);                      \
        while(1) {                                  \
            ; /* wait until timer runs out */       \
        }                                           \
    } while(0)


void WDT_init(void) __attribute__((naked))__attribute__((section(".init3")));
inline void CLK_init(void);
inline void UART_init(void);
inline void ADC_init(void);
inline void PRR_init(void);
inline void GPIO_init(void);
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
    GPIO_init();
    CLK_init();
    PRR_init();
    UART_init();
    ADC_init();
    sei();

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
inline void CLK_init(void)
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
void WDT_init(void)
{
    WDTCSR = (1 << WDP2) | (1 << WDP1);
}


/**
 * @brief Initializes UART transmission with 8N1 and defined baud rate.
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
 * interrupt on completed conversion. Disables digital input buffer on ADC pin.
 * @param void
 * @return void
 */
inline void ADC_init(void)
{
    ADMUX = (1 << REFS0) | ADC_CHANNEL;
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 = ADC_CHANNEL;
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
 * @brief Initializes IO.
 * @details Initializes external interrupt too.
 * @param void
 * @return void
 */
inline void GPIO_init(void)
{
    PUEB |= (1 << SYS_RESET_PIN);
    PORTB |= (1 << SYS_RESET_PIN);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);

    DDRA |= (1 << DDRA5);
    PORTA &= ~(1 << PINA5);
}


/**
 * @brief Transmits 8-bit character.
 * @details Waits for empty transmit buffer.
 * @param data : Byte to transmit
 * @return void
 */
void UART_transmit(unsigned char data)
{
    while(!(UCSRA & (1 << UDRE))) {
        ; /* wait for empty transmit buffer */
    }

    UDR = data; /* put data into buffer and send it */
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
 * @details ADC is powered on only during conversion to save power.
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
 * @details Used to wake up from sleep mode. Pin toggling indicates heartbeat.
 */
ISR(WDT_vect)
{
#ifdef HEARTBEAT
    PORTA ^= (1 << PORTA5);
#endif
}


/**
 * @brief ADC vector
 */
ISR(ADC_vect)
{
    ;
}


/**
 * @brief INT0 vector
 * @details Resets the system.
 */
ISR(INT0_vect)
{
    if(!(PINB & (1 << SYS_RESET_PIN))) {
        software_reset();
    }
}
