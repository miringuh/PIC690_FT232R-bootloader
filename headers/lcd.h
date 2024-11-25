#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "twi.h"
#if !defined(__LCD)
#define __LCD
// commands

#define LCD_CLEARDISPLAY 0x01 // 1
#define LCD_RETURNHOME 0x02//+1
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08//+1
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20 
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80//+1

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02//4
#define LCD_ENTRYSHIFTINCREMENT 0x01//+4
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04 // 3
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02// +3
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00//+3

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00
/*
RS  0-instruction (wr) busy flag or (rd) addr counter
    1-data register
R/W 0-write  1-read
E   Start data rd/wr
D4--D7
    RS  RW
    0   0   command (display,clear etc) //
    0   1   read busy flag and addr cnt (Db0..Db7)
    1   0   DR Write to mem //
    1   1   DR read from mem
*/

void lcd_init(uint8_t rs, uint8_t rw, uint8_t val)
{
    __delay_ms(15000);
    twi_write(0x03); // 00 0011
    __delay_ms(5);
    twi_write(0x03); // 00 0011
    __delay_us(150);    
    twi_write(0x03); // 00 0011

    twi_write(0x02); // 
    twi_write(0x08); // set 4-bit mode 2 ln
    __delay_us(150);

    twi_write(0x00);
    twi_write(0x08);
    __delay_us(150);//disp-off

    twi_write(0x00);
    twi_write(0x01);
    __delay_us(150);//clr disp

    twi_write(0x00);
    twi_write(0x07);
    __delay_us(150);//entry mode

    twi_write(0x00);
    twi_write(0x0F);
    __delay_us(150);//disp on
}


void lcd_command(uint8_t val)
{
}
#endif // __LCD
