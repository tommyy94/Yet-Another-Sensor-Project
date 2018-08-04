#ifndef SYS_CONTROL_H_
#define SYS_CONTROL_H_

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define HEARTBEAT
#define SYS_RESET_PIN PINB1


/**
 * @brief Reset the software by using Watchdog Timer.
 */
#define software_reset()                        \
    do {                                        \
        wdt_enable(WDTO_15MS);                  \
        while(1) {                              \
            ; /* wait until timer runs out */   \
        }                                       \
    } while(0)


extern void GPIO_init(void);
extern void power_down(void);
extern void WDT_init(void)
__attribute__((naked))
__attribute__((section(".init3")));


#endif /* SYS_CONTROL_H_ */
