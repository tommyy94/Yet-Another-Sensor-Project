#include "analog.h"


/**
 * @brief Initializes ADC on channel 0.
 * @details Set ADC prescaler to 8 resulting in 125 kHz resolution. Also enables
 * interrupt on completed conversion. Disables digital input buffer on ADC pin.
 * @param void
 * @return void
 */
extern void ADC_init(void)
{
    ADMUX = (1 << REFS0) | ADC_CHANNEL;
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 = ADC_CHANNEL;
}


/**
 * @brief Starts Analog-to-digital conversion by turning off the CPU.
 * @details ADC is powered on only during conversion to save power.
 * @param void
 * @return void
 */
extern void ADC_start_conversion(void)
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
 * @brief ADC vector
 */
ISR(ADC_vect)
{
    ;
}
