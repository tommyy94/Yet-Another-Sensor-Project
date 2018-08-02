/*******************************************************************************
*
* 1. NAME
*       LCD.c
*
* 2. DESCRIPTION
*       Source file for LCD driver.
*
*******************************************************************************/

/*******************************************************************************
*   HEADER FILES                                                               *
*******************************************************************************/
#include "lcd.h"


/*******************************************************************************
*   FUNCTION PROTOTYPES                                                        *
*****************************************************************************///

/***************************************************************************//**
@brief Set EN HIGH for required time, then set EN LOW to execute instructions.
@details Instruction is executed when EN is set LOW.
@param void
@return void
*******************************************************************************/
static void LCD_execute_instruction(void);


/***************************************************************************//**
@brief Determine if the last given instruction is finished.
@details Set data lines to read mode, use command and read mode.
Execute instruction, then set data lines to write mode.
@param void
@return void
*******************************************************************************/
static void LCD_wait_if_busy(void);


/*******************************************************************************
*   FUNCTION DEFINITIONS                                                       *
*******************************************************************************/
static void LCD_execute_instruction(void)
{
    CONTROL_LINES |= (1 << EN);
    NOP;
    NOP;
    CONTROL_LINES &= ~(1 << EN);
}


static void LCD_wait_if_busy(void)
{
    DATA_DIRECTION &= ~0xFF;        /* read data lines */
    CONTROL_LINES &= ~(1 << RS);    /* command mode */
    CONTROL_LINES |= (1 << RW);     /* read mode */

    while(DATA_LINES & MSB) {       /* LCD is busy if MSB is set */
        LCD_execute_instruction();
    }

    DATA_DIRECTION |= 0xFF;         /* write to data lines */
    _delay_us(LCD_DELAY_US_LONG);
}


void LCD_send_command(uint8_t const command)
{
    LCD_wait_if_busy();
    DATA_LINES |= command;
    CONTROL_LINES &= ~((1 << RW) | (1 << RS));  /* write mode, command mode */
    LCD_execute_instruction();
    DATA_LINES &= ~0xFF;                        /* clear data lines */
}


void LCD_move_cursor(uint8_t const x, uint8_t const y)
{
    /* lookup table for ease of use, add more rows if needed */
    static uint8_t const row_pos[ROWS] = {
        FIRST_ROW_POSITION,
        SECOND_ROW_POSITION
    };

    LCD_send_command(FIRST_LINE_ADDRESS + x + row_pos[y]);
    _delay_us(LCD_DELAY_US_LONG);
}


void LCD_write_character(char const character)
{
    LCD_wait_if_busy();
    DATA_LINES |= character;        /* character sent through data lines */
    CONTROL_LINES &= ~(1 << RW);    /* write mode */
    CONTROL_LINES |= (1 << RS);     /* text mode */
    LCD_execute_instruction();
    DATA_LINES &= ~0xFF;            /* clear data lines */
    _delay_us(LCD_DELAY_US_SHORT);
}


void LCD_write_integer(uint8_t const x, uint8_t const y, int const number)
{
    char string_to_display[COLUMNS];

    itoa(number, string_to_display, DECIMAL_SYSTEM);
    LCD_write_string(x, y, string_to_display);
}


void LCD_write_string(uint8_t const x, uint8_t const y,
                      char const *string)
{
    LCD_move_cursor(x, y);
    while(*string != '\0') {
        LCD_write_character(*string++);
    }
}


void LCD_init(void)
{
    /* set control lines  */
    CONTROL_DIRECTION |= (1 << EN) | (1 << RW) | (1 << RS);
    _delay_us(LCD_DELAY_US_AFTER_VDD);

    LCD_send_command(SET_DATA_LINE_MODE);
    _delay_us(LCD_DELAY_US_SHORT);

    LCD_send_command(DISPLAY_ON_CURSOR_OFF);
    _delay_us(LCD_DELAY_US_SHORT);

    LCD_send_command(CLEAR_DISPLAY);
    _delay_us(LCD_DELAY_US_LONG);
}
