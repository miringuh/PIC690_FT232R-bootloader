#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "f690.h"

#if !defined(__TWI_)
#define __TWI_
#define _XTAL_FREQ 16000000

/*
RB6 SCK/SCL
RB4 SDI/SDA
enabled SSPCON<SSPEN>
    REGISTERS
(7/10 bit addr and 7bit with Start/stop intrr) **master
(10bit addr start/stop intrr ) **master mode

SSPCON
      7 WCOL
      6 SSPOV 1 data resvd in sspbuf while holding data 0-no ovflow
      5 SSPEN
      4 CKP 1-enable clk
            0 -hold clk-low(enables setup)/clk streching
            (check the bus b4 wr data to sspbuf) (checks ACK)
      3:0 SSPM 0110 slave 7bit addr
      3:0 SSPM 0111 slave 10bit addr
      3:0 SSPM 1001  load SSPMSK data to SSPADD (any rd/wr on SSPADD >> accesses SSPMSK reg.)
      3:0 SSPM 1011 master mode**
      3:0 SSPM 1110 slave 7bit start/stop bit intrr enabled
      3:0 SSPM 1111 slave 10bit start/stop bit intrr enabled

SSPSTAT *
   r/0 5 D/A 1=byte txed/rxed data 0=byte txed/rxed  addr
   r/0 4 P stop (clears SSPEN)
             1-stop detected last
             0-no stop detected
   r/0 3 S start 1-detected 0-not detected
   r/0 2 R/W if 1-read 0-write
   r/0 1 UA (10bit addr only) 1-update addr 0-dont update
   r/0 0 BF txing 1-tx in progress 0-tx complete sspbuff full
            rxing 1-rx complete 0-rx not complete sspbuff empty
 */
/*
SSPBUFF <>SSPSR
SSPADD
SSPMSK
        slave mode
  scl/sck must be inputs (if slave tx it overrides pins)

initialisation
addressing
SSPADD is compared by SSPSR
if are same( BF & SSPOV) are cleared
sspsr value loads  to sspbuff (BF is set)
ACK pulse is generated
SSPIF is set

        7bit-mode "master"
pir1-sspif
pie1-sspie
intcon
*/
uint8_t twi_dummy = 0;
uint8_t twi_data = 0;
uint8_t twi_addr = 0;
// __interrupt() void get_twi(void)
// {
//     PIR1bits.SSPIF = 0;
//     INTCON = 0X00;
// }
// void twi_init(uint8_t addrs)
// {
//     TRISC = 0X00;
//     TRISB = 0X10; // x0x1 xxxx
//     // SSPBUF = 0;
//     SSPCON = 0;
//     PIR1 = 0;
//     PIE1 = 0;
//     INTCON = 0;
//     SSPSTATbits.SMP = 0;
//     SSPSTATbits.CKE = 0;
//     SSPCONbits.SSPM3 = 1;
//     SSPCONbits.SSPM2 = 0;
//     SSPCONbits.SSPM1 = 1;
//     SSPCONbits.SSPM0 = 1;
//     PIE1bits.SSPIE = 1;
//     INTCON = 0XC0;
//     __nop();
//     SSPBUF = addrs;
//     SSPCONbits.SSPEN = 1;
//     SSPCONbits.CKP = 1;
//     // led(SSPCON);
//     // led(SSPSTAT);
//     while (PIR1bits.SSPIF == 1)
//     {
//     }
//     SSPCONbits.CKP = 0;
//     twi_data = SSPBUF;
//     led(twi_data);
// }
// uint8_t twi_write(uint8_t val)
// {
//     SSPBUF = val;
//     SSPCONbits.SSPEN = 1;
//     SSPCONbits.CKP = 1;
//     // led(SSPCON);
//     // led(SSPSTAT);
//     while (PIR1bits.SSPIF == 1)
//     {
//     }
//     SSPCONbits.CKP = 0;
//     twi_data = SSPBUF;
//     led(twi_data);
//     return twi_data;
// }
// void twi_stop()
// {
//     SSPCONbits.SSPEN = 0;
//     SSPCONbits.CKP = 0;
// }

//
uint8_t mxbuff[8];
uint8_t mbuff[8];
uint8_t bytex = 0;
uint8_t ack = 0;
uint8_t cyc = 100;
#define data 0x10
#define clk 0x40
#define tbuff cyc     // min free buff
#define tsu_sta cyc   // start set-up
#define thd_sta 4     // start hold
#define tclk_low cyc  // clk-low
#define tclk_high cyc // clk-high
#define tr cyc        // clk/data rise
#define tf cyc        // clk/data fall
#define tsu_dat 250   // data-setup
#define thld_dat 0    // data hopd
#define tvd_dat -     // scl low to data valid
#define tsu_st 4      // stop set-up
uint8_t buff2byte(uint8_t buf[]) // msb
{
    uint8_t cnt = 128;
    bytex = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (buf[i] == 1)
        {
            bytex += cnt;
        }
        cnt = cnt / 2;
    }
    return bytex;
}
void msb_byte(uint8_t val)
{
    uint8_t cnt = 1;
    for (uint8_t i = 0; i < 8; i++)
    {
        mxbuff[i] = ((val & cnt) >> i);
        cnt = (cnt << 1);
    }
    mbuff[0] = mxbuff[7];
    mbuff[1] = mxbuff[6];
    mbuff[2] = mxbuff[5];
    mbuff[3] = mxbuff[4];
    mbuff[4] = mxbuff[3];
    mbuff[5] = mxbuff[2];
    mbuff[6] = mxbuff[1];
    mbuff[7] = mxbuff[0];
}
void twi_init()
{
    TRISB = 0;
    PORTB = clk | data;
    __delay_us(tsu_sta);
    PORTB = clk; ///////// start
    __delay_us(thd_sta);
    PORTB = 0;
    __delay_us(tclk_low);
}
uint8_t twi_address(uint8_t addrs)
{
    uint8_t cnt = 1;
    msb_byte(addrs);
    for (uint8_t i = 0; i < 8; i++)
    {
       
        if (mbuff[i] == 1)
        {
            PORTB = clk;
            __delay_us(5);
            PORTB = data | clk;
            __delay_us(5);
            PORTB = data;
            __delay_us(5);
        }
        if (mbuff[i] == 0)
        {
            PORTB = clk;
            __delay_us(5);
            PORTB = 0;
            __delay_us(5);
        }
    }
    TRISBbits.TRISB4 = 1;
    PORTB = clk;
    __delay_us(5);
    ack = ((PORTB & 0x10) >> 4);
    __delay_us(5);
    TRISB = 0;
    while (ack == 0)
    {
        // led(0X02);
    }
    // led(addrs);
    return ack;
}
uint8_t twi_write(uint8_t val)
{
    twi_address(val);
    // twi_address(val);
}
void twi_stop()
{
    PORTB = clk | data;
    __delay_us(tsu_sta);
}

#endif // __TWI_
