#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#if !defined(__EUSART)
#define __EUSART

/*
rcsta<7> set 1
trisb<6> set 1
trisb<7> set 1
TXSTA, RCSTA, BAUDCTL, (SPBRGH_SPBRG)
    TXSTA
  default 0000 0010 (0x02) r/w **** **R*
CSRC     -(sync)- clk src 1-master 0 slave
TX9      -9 bit enable 1=9bit 0=8bit
TXEN     -tx enable 1=enable 0=disable **SREN/CREN overrides TXEN IN SYNC mode
SYNC     -1=sync 0=asynch
SENDB    -send brk char bit (asy 1=send 0-sync brk complete)(sync *null*)
BRGH     -high baud rate select (Asy 1-high 0-low speed) (sync *null*)
TRMT*(1) -tx reg status flag  1-empty 0-full
TX9D     -9th bit of TX-bit **** can be -address/data/parity

    RCSTA
default 0000 000x r/w **** *RRR
SPEN  -serial port enable 1=en 0-disable/(holds on reset)
RX9   -9th bit resv enable 1=en 0 8 bit resv
SREN  -single resv enable bit (async ***)
                              (sync _master mode 1-en 0=disable)
                              (sync slave mode ****)
CREN  -continous resv enable bit (asyn 1=en resv 0=disable resv)
            (sync 1-en resv until CREN==cleared 0=disable)
ADDEN -address detect bit(
        asyn @ RX9=1
        1=en *enable interrupt and load buffer if RSR<8> is set )
        0=disable address detection
        asyn @ RX9=0 **NULL**
FERR*  -frame error bit 1= updated by reading RCREG & resv next byte
                        0=no error
OERR*  -1=Overrun bit (can be cleared by clearing CREN)
        0=No error
RX9D   -9th resvd bit

    BAUDCTL
default 01-0 0-00  R/W =sckp brg16 wue abden R=abdovf rcidl
ABDOVF* -(async) auto baud detect overflow bit (1=overflow 0=no overflow)
          (sync **null**)
RCIDL*  -(asyn)resv idle flag bit
          1-idle
          0-start bit resvd & buffer reserving
          (sync **null**)
SCKP    -synchronous clk polarity select bit
          (asyn 1=tx inverted data 0=no inversion)
          (sync 1=data on rising edge of clk 0=data on falling edge of clk)
BRG16   -16 bit baudrate range select bit 1=16 bit 0=8 bit
WUE     -wakeup Enable bit
          1=next falling rx/dt edge will set RCIF and wake device(clrd by next raising/falling edge)
          0=rx/tx do not generate interrupt
ABDEN   -auto Baud detect bit
          (async 1=enable *cleared when auto baud is complete 0= disabled)
           (sync **null**)

NB: Asyc BRGH & BRG16 controls Baud rate /(sync BRGH is ignored)

baud=fosc/64( [spbrgh:spbrg] )+1
spbrgh:spbrg=fosc/(desired baudrate/64)-1
or
spbrgh:spbrg=((fosc/baud)-65/64)***
error= (calc baudrate-desired baudrate)/desired baudrate

PIR1 <TX/RX INT FLAGS> <TXIF RCIF> cleared by wr/rd txreg respectively
PIE1 <TX/RX INT EN bits> <TXEN RCIE>
TXSTA <TRMT>show status of TSR
PIR1  <TXIF>show status of TXREG
INTCON <7:6> gie peie
TXREG tx-buffer
RCREG rx-buffer
*/
#define _XTAL_FREQ 12000000
/*
FOSC/(64[SPBRGH/SPBRG]+1) asyn brgh=0 brg16=0
FOSC/(16[SPBRGH/SPBRG]+1) asyn brgh=1 brg16=0
FOSC/(16[SPBRGH/SPBRG]+1) asyn brgh=0 brg16=1
FOSC/(4[SPBRGH/SPBRG]+1) asyn brgh=1 brg16=1
**spbrg=(fosc/baud)/4)-1 *********
    brgh=1 and brg16=1
9600=416
19200=208
38400=104 ** 4
   brg=1 or brg16=1
9600=104
19200=52
38400=26  ** 16
   brg=0 and brg16=0
9600=26
19200=13
38400=6    **64
 */
// brgh=0 brg16=0    mode 0
#define l9600 26
#define l19200 13
#define l38400 6
// brgh=1 or brg16=1
// brgh=1            mode 1
#define m9600 103
#define m19200 51
#define m38400 25
// brg16=1            mode 2
#define m16_9600 103
#define m16_19200 51
#define m16_38400 25
// brgh=1 and brg16=1 mode 3
#define h9600 415
#define h19200 207
#define h38400 103
uint8_t baud_modes = 0;
uint8_t eu_dummy = 0;
uint8_t eu_data = 0;
// __interrupt() void get_twi(void)
// {

// }
void Baud(uint16_t bbaud, uint8_t mode)
{
    SPBRGH = (uint8_t)((bbaud & 0xFF00) >> 8);
    SPBRG = (uint8_t)(bbaud & 0x00FF);
    if (mode == 0)
    {
        TXSTAbits.BRGH = 0;
        BAUDCTLbits.BRG16 = 0;
    }
    if (mode == 1)
    {
        TXSTAbits.BRGH = 1;
        BAUDCTLbits.BRG16 = 0;
    }
    if (mode == 2)
    {
        TXSTAbits.BRGH = 0;
        BAUDCTLbits.BRG16 = 1;
    }
    if (mode == 3)
    {
        TXSTAbits.BRGH = 1;
        BAUDCTLbits.BRG16 = 1;
    }
}
// WRITE
void eusart_init(uint16_t bbaud, uint8_t mode)
{
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0XC0;
    Baud(bbaud, mode);
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    // PIE1bits.TXIE = 1;
    TXSTAbits.TXEN = 1; //
    // INTCON = 0XC0;
}
void eusart_wr(uint8_t val)
{
    while (PIR1bits.TXIF == 0)
        ;

    TXREG = val;
    
}
// READ///
void eusart_rx_init(uint16_t bbaud, uint8_t mode)
{
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0XC0;
    Baud(bbaud, mode);
    TXSTAbits.SYNC = 0;
    BAUDCTLbits.WUE = 0;
    RCSTAbits.SREN = 0;
    RCSTAbits.CREN = 1;
    PIE1bits.RCIE = 1;
    RCSTAbits.SPEN = 1;
    INTCON = 0XC0;
    // _nop();
}

uint8_t eusart_rd()
{
    eu_dummy = RCREG;
    while (PIR1bits.RCIF == 1)
        ;
    if (RCSTAbits.OERR == 1)
    {
        RCSTAbits.CREN = 0;
    }
    RCSTAbits.CREN = 1;
    return RCREG;
}
/// AUTO BAUD READ ////

void autobaud_rdInit(uint16_t bbaud, uint8_t mode)
{
    ANSEL = 0;
    ANSELH = 0;
    TRISC = 0XC0;
    Baud(bbaud, mode);
    BAUDCTLbits.WUE = 0;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SREN = 0;
    RCSTAbits.ADDEN = 0;
    RCSTAbits.CREN = 1;
    PIE1bits.RCIE = 1;
    RCSTAbits.SPEN = 1;
    INTCON = 0XC0;
    BAUDCTLbits.ABDEN = 1;
    // while (BAUDCTLbits.RCIDL == 1)
    //     ;
}
uint8_t autoBaud_rd()
{
    eu_dummy = RCREG;
    while (PIR1bits.RCIF == 1)
        ;
    if (RCSTAbits.OERR == 1)
    {
        RCSTAbits.CREN = 0;
    }
    RCSTAbits.CREN = 1;
    return RCREG;
}
//
void eusart_stop()
{
    TXSTA = 0; //*
    PIE1 = 0;
    PIR1 = 0;
    BAUDCTL = 0;
    TRISB = 0X00;
    INTCON = 0X00;
    PORTB = 0;
}
//
// uint8_t char2hex(uint8_t val)
// {
//     uint8_t u = 0;
//     if ((isdigit(val)))
//     {
//         u = (uint8_t)val - 0x1E; // 30
//         return u;
//     }
//     if ((isxdigit(val)))
//     {
//         u = (uint8_t) val - 0x37; // 55
//         return u;
//     }
//     return u;
// }
// ///

///
///
#endif // __EUSART
