#ifndef ADC_H_
#define ADC_H_

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define ADC_CHANNEL PINA0


extern void ADC_init(void);
extern void ADC_start_conversion(void);


#endif /* ADC_H_ */
