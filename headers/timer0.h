#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#if !defined(__TMR0)
#define __TMR0
// (FOSC/4) /(255-val) == SPEED

// __interrupt(low_priority) void getTimer0(void)
// {
//     INTCONbits.T0IF = 0;
//     INTCONbits.GIE = 0;
//     INTCONbits.PEIE = 0;
// }
//// TIMERS //////
void tmr0_init()
{
    ANSEL = 0X0F; // I -INPUT 0 OUTPUT
    ANSELH = 0x0E;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0; // prescaler timer0
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.T0IE = 1; // tmro int enable
}
void tmr0(uint8_t val)
{
    TMR0 = val;
    while ((INTCONbits.T0IF) == 1)
        ;
}
void mydel(uint16_t del, uint8_t reg)
{
    uint16_t cnt = 0;
    timer_0_init();
    while (cnt <= del)
    {
        timer0(reg);
        cnt++;
    }
}
/// COUNTER ///
void counter_0_init()
{
    ANSEL = 0X0F; // ra2 I -INPUT 0 OUTPUT
    ANSELH = 0x0E;
    TRISAbits.TRISA2 = 1;
    TMR0 = 0;
    OPTION_REGbits.T0CS = 1;
    OPTION_REGbits.PSA = 0;  // prescaler timer0
    OPTION_REGbits.T0SE = 0; // 0-LH 1-HL
    OPTION_REGbits.PS2 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1; // counter
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.T0IE = 1; // tmro int enable
}
void counter0(uint8_t val)
{
    // TMR0 = val;
    while ((INTCONbits.T0IF) == 1)
        ;
}
void mycnt(uint16_t val, uint8_t reg)
{
    uint16_t cnt = 0;
    counter_0_init();
    while (cnt <= reg)
    {
        counter0(reg);
        cnt++;
    }
}
#endif // __TMR0
