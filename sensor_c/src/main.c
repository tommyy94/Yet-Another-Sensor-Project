#include "main.h"

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
