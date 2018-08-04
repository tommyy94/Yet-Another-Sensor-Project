#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <util/setbaud.h>


extern void UART_init(void);
extern void UART_transmit(unsigned char data);


#endif /* UART_H_ */
