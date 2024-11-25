#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#if !defined(__TMR1)
#define __TMR1

__interrupt(low_priority) void getTimer1(void)
{

    PIR1bits.TMR1IF = 0;
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
}
void trm1_init()
{
    ANSEL = 0X0F; // I -INPUT 0 OUTPUT
    ANSELH = 0x0E;
    // TMR1H = 0;
    // TMR1L = 0;
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.TMR1ON = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}
void tmr1(uint16_t val)
{
    TMR1H = ((val & 0x3F00) >> 8);
    TMR1L = (val & 0xFF);
    while ((PIR1bits.TMR1IF) == 1)
        ;
}
void tmr1_delay(uint16_t val,uint16_t reg)
{
    uint16_t cnt = 0;
    trm1_init();
    while (cnt <= val)
    {
        tmr1(reg);
        cnt++;
    }
}
#endif // __TMR1
