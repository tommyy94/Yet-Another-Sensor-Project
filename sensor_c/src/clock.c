#include "clock.h"


/**
 * @brief Initializes system clock.
 * @details 1 MHz Clock frequency
 * @param void
 * @return void
 */
extern void CLK_init(void)
{
    CCP = IO_SIGNATURE;
    CLKPSR = (1 << CLKPS1) | (1 << CLKPS0);
}


/**
 * @brief Ensures that Timer 0 is disabled to save power.
 * @param void
 * @return void
 */
extern void PRR_init(void)
{
    PRR |= (1 << PRTIM0);
}
