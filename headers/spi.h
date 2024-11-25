#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#if !defined(__SPI)
#define __SPI
#define _XTAL_FREQ 12000000

/*
  SPI_MODE
  RC7 -sdo > 00xx xxxx
  RC6 -ss
  RB4 -sdi/sda
  RB6 -sck/scl > x0x1 xxxx
if slave mode ss is set to vcc (spi resets)
if slave mode CKE==1 SS must be enabled
    SSPSTAT
 7 smp (master)  1 sample @ end of data out
                 0 smpled @ middle data out
       (slave)   1(smp) must be cleared
 6      (master)
   cke clk-edge ckp==0 1 data txed clk-2-up
                       0 data txed on clk-2-down
                ckp==1 1 data txed on clk-2-down
                       1 data txed clk-2-up
 5 D/A (I2C)
 4 P (I2C)
 3 S (I2C)
 2 R/W (I2C)
 1 UA  (I2C)
 0 BF (spi-rxed) 1 resved on sspbuff (READ-ONLU)
                 0 tx complete sspbuff==empty

           SSPCON
    7 wcol 1 sspbuff has resvd while rxing 0-no collision
    6 sspov (slave) resv overflow bit 1-resvd while hold data
                                      0- no overflow
    5 sspen 1-enable port  0-disable port
    4 ckp (polarity) 1-idle-clk==high
                     0-idle-clk==low
    3:0 sspm
        0000 master fosc/4
        0001 master fosc/16
        0010 master fosc/64
        0011 master tmr2/2

pir1-sspif
pie1-sspie
intcon
trisb trisc
*/
#define fosc_4 0
#define fosc_16 1
#define fosc_64 2
#define fosc_trm2 3

volatile uint8_t spi_dummy = 0;
volatile uint8_t spi_data = 0;
// __interrupt() void get_twi(void)
// {
    
// }
void spi_fosc(uint8_t val)
{
    switch (val)
    {
    case 0:
        SSPCONbits.SSPM3 = 0;
        SSPCONbits.SSPM2 = 0;
        SSPCONbits.SSPM1 = 0;
        SSPCONbits.SSPM0 = 0;
        break;
    case 1:
        SSPCONbits.SSPM3 = 0;
        SSPCONbits.SSPM2 = 0;
        SSPCONbits.SSPM1 = 0;
        SSPCONbits.SSPM0 = 1;
        break;
    case 2:
        SSPCONbits.SSPM3 = 0;
        SSPCONbits.SSPM2 = 0;
        SSPCONbits.SSPM1 = 1;
        SSPCONbits.SSPM0 = 0;
        break;
    case 3:
        SSPCONbits.SSPM3 = 0;
        SSPCONbits.SSPM2 = 0;
        SSPCONbits.SSPM1 = 1;
        SSPCONbits.SSPM0 = 1;
        break;
    default:
        SSPCONbits.SSPM3 = 0;
        SSPCONbits.SSPM2 = 0;
        SSPCONbits.SSPM1 = 0;
        SSPCONbits.SSPM0 = 0;
        break;
    }
}

void spi_init(uint8_t speed)
{ // fosc_4/16 -very high speed
    TRISC = 0X00;
    TRISB = 0X10; // xx0x 1xxx
    SSPSTAT = 0;
    SSPBUF = 0;
    SSPCON = 0;
    PIR1 = 0;
    PIE1 = 0;
    SSPSTATbits.SMP = 0;
    SSPSTATbits.CKE = 0;
    SSPCONbits.CKP = 0;
    spi_fosc(speed);
    SSPCONbits.SSPEN = 1;
    _nop();
}
void latch()
{
    PORTCbits.RC6 = 1;
    __delay_us(1);
    PORTCbits.RC6 = 0;
    __delay_us(1);
}
void spi_wr(uint16_t val)
{
    SSPBUF = val;
    while (SSPCONbits.WCOL == 1)
    {
        spi_dummy = SSPBUF;
        SSPCONbits.WCOL = 0;
    }
    spi_data = SSPBUF;
    latch();
}
uint8_t spi_rd()
{
    SSPCON = 0x05; //  x0xx 0101
    if (SSPCONbits.SSPOV == 1)
    {
        spi_dummy = SSPBUF;
    }
    while (SSPSTATbits.BF == 0)
    {
        
    }
    spi_data = SSPBUF;
    return spi_data;
}
#endif // __SPI
