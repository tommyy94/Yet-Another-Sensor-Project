/*******************************************************************************
*
* 1. NAME
*       LCD.h
*
* 2. DESCRIPTION
*       Driver for generic 2x16 LCD.
*
*       - EN (enable) control line informs the LCD, that data is being sent.
*         Set to HIGH, wait required time, then set LOW to execute instruction.
*       - RS (register select), set LOW to treat data as command/special
*         instruction, HIGH treats data as text.
*       - RW (read/write), LOW to write on LCD, HIGH to read LCD.
*
*******************************************************************************/
#ifndef LCD_H_
#define LCD_H_


/*******************************************************************************
*   HEADER FILES                                                               *
*******************************************************************************/
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>


/*******************************************************************************
*   GLOBAL VARIABLES                                                           *
*******************************************************************************/
/* ports and pins for LCD */
#define RS PINC2
#define RW PINC1
#define EN PINC0
#define DATA_LINES PORTB
#define CONTROL_LINES PORTC
#define DATA_DIRECTION DDRB
#define CONTROL_DIRECTION DDRC

#define ROWS 2
#define COLUMNS 16

#define FIRST_ROW_POSITION 0
#define SECOND_ROW_POSITION 64

#define DECIMAL_SYSTEM 10 /* for converting integer to ascii */

#define MSB 0x80 /* most significant bit */

/* LCD commands */
#define SET_4_BIT 0x28
#define SET_8_BIT 0x38
#define SET_DATA_LINE_MODE SET_8_BIT /* used data line mode */

#define CLEAR_DISPLAY 0x01
#define DISPLAY_ON_CURSOR_OFF 0x0C
#define DISPLAY_ON_CURSOR_ON 0x0E
#define DISPLAY_ON_CURSOR_BLINK 0x0F

/* memory addresses for LCD cursor positions */
#define FIRST_LINE_ADDRESS 0x80
#define SECOND_LINE_ADDRESS 0xC0


#ifndef F_CPU
    #error Define CPU frequency
#else
    /* delays; refer to the manual */
    #define LCD_DELAY_US_AFTER_VDD 500
    #define LCD_DELAY_US_LONG 52
    #define LCD_DELAY_US_SHORT 15
#endif


/*******************************************************************************
*   MACROS                                                                     *
*****************************************************************************///
#ifndef NOP
    #define NOP asm volatile("nop")
#endif


/*******************************************************************************
*   FUNCTION PROTOTYPES                                                        *
*****************************************************************************///


/***************************************************************************//**
@brief Sends string to given position in LCD.
@details Moves LCD cursor to given position. Sends string one character at time.
@param X & Y -coordinates, String to send
@return void
*******************************************************************************/
void LCD_write_string(uint8_t const x, uint8_t const y,
                      char const *string);


/***************************************************************************//**
@brief Sends a hexadecimal command to LCD.
@details Given command is set to port.
Sets write and command mode, executes instruction and clears used port.
@param Hexadecimal command to send
@return void
*******************************************************************************/
void LCD_send_command(uint8_t const command);


/***************************************************************************//**
@brief Moves LCD cursor to given position.
@details Cursor is moved relative to first line.
@param X & Y -coordinates
@return void
*******************************************************************************/
void LCD_move_cursor(uint8_t const x, uint8_t const y);


/***************************************************************************//**
@brief Sends a character when LCD is available.
@param Character to send
@return void
*******************************************************************************/
void LCD_write_character(char const character);


/***************************************************************************//**
@brief Moves cursor to given location, then writes integer to LCD.
@param x, y, integer to write
@return void
*******************************************************************************/
void LCD_write_integer(uint8_t const x, uint8_t const y, int const number);


/***************************************************************************//**
@brief Sets 4/8-bit mode and control lines to desirable port.
@param void
@return void
*******************************************************************************/
void LCD_init(void);


#endif /* LCD_H_ */
