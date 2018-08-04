#ifndef CLOCK_H_
#define CLOCK_H_

#include <avr/io.h>


#define IO_SIGNATURE 0xD8


extern void CLK_init(void);
extern void PRR_init(void);


#endif /* CLOCK_H_ */
