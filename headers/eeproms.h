#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#if !defined(__EP_ROM)
#define __EP_ROM
/*
EEDAT EEDATH- data register 14bits  xx00 0000 0000 0000
EEADR EEADRH- address registers 12bits   0000 0000 0000
EECON1 controls EE-memory
    7 eepgd 1-prog 0 ee_mem
    6:4 000
    3 wrerr 0-complete 1-error
    2 wren 1-allow wr 0- inhibit wr
    1 wr 1-(eepgd=0) init write 0-complete (eepgd=1) ignored
        cleared by hardware set by software
    0 1-rd cleared by hardware set by software 0-no read
**eecon2 0x55 eecon2 0xaa

EECON2 controls prog-memory

sequence
***ee_wr ****
set addr
set data

clr eecon1 eepgd
set eecon1 wren
clear intrr
set eecon2 0x55
set eecon2 0xaa
set eecon1 wr
en intrr (gie)
sleep
...
clear wren (eecon1)

PIE2 (EEIE)
PIR2 (EEIF)
INTCON (GIE PEIE)
*/
volatile uint8_t prog_buff[2];
volatile uint8_t rd_val = 0;

void eprom_init()
{
    EECON1 = 0;
    PIE1 = 0;
    PIR2 = 0;
    EEADR = 0;
    EEADRH = 0;
    EECON1bits.WREN = 1;
    INTCON = 0;
    EECON2 = 0X55;
    EECON2 = 0XAA;
    EECON1bits.WR = 1;
    // PIE2bits.EEIE = 1;
    // INTCON = 0XC0;
    _nop();
    _nop();
}
uint8_t eprom_wr(uint16_t addr, uint8_t val)
{
    EEADRH = ((addr & 0xFF00) >> 8);
    EEADR = addr;
    EEDATH = 0;
    EEDAT = val;
    if (EECON1bits.WRERR == 1)
    {
        EECON1bits.WRERR = 0;
    }
    EECON1bits.WREN = 1;
    EECON1bits.WR = 1;
    _nop();
    while (EECON1bits.WR == 1)
    {
    }
    EECON1bits.WREN = 0;
    return EEDAT;
}
uint8_t eprom_rd(uint16_t addr)
{
    EECON1 = 0;
    EEADRH = (uint8_t)((addr & 0xFF00) >> 8);
    EEADR = (uint8_t)(addr & 0X00FF);
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    _nop();
    while (EECON1bits.RD == 1)
    {
    }
    rd_val = EEDAT;
    return rd_val;
}

uint8_t *read_prog(uint16_t addr)
{
    EEADRH = (uint8_t)((addr & 0xFF00) >> 8);
    EEADR = (uint8_t)(addr & 0X00FF);
    EECON1bits.EEPGD = 1;
    EECON1bits.RD = 1;
    _nop();
    _nop();
    prog_buff[0] = EEDATH;
    prog_buff[1] = EEDAT;
    return prog_buff;
}
#endif // __EP_ROM
