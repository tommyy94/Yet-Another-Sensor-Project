#include "sys_control.h"


/**
 * @brief Initializes IO.
 * @details Initializes external interrupt too.
 * @param void
 * @return void
 */
extern void GPIO_init(void)
{
    PUEB |= (1 << SYS_RESET_PIN);
    PORTB |= (1 << SYS_RESET_PIN);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);

#ifdef HEARTBEAT
    DDRA |= (1 << DDRA5);
    PORTA &= ~(1 << PINA5);
#endif
}


/**
 * @brief Initializes watchdog timer.
 * @details 128K WDT Oscillator (Cycles) = ~1.0s oscillator.
 * @param void
 * @return void
 */
extern void WDT_init(void)
{
    WDTCSR = (1 << WDP2) | (1 << WDP1);
}


/**
 * @brief Puts the MCU to sleep mode.
 * @details Watchdog Interrupt is enable and interrupt flag is set before
 * sleeping. Disabled and unset after waking up.
 * @param void
 * @return void
 */
extern void power_down(void)
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
 * @brief INT0 vector
 * @details Resets the system.
 */
ISR(INT0_vect)
{
    if(!(PINB & (1 << SYS_RESET_PIN))) {
        software_reset();
    }
}
