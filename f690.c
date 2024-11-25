#include <xc.h>
#include "headers/f690.h"
#include "headers/eusart.h"
// #include "headers/timer0.h"
// #include "headers/timer1.h"
// #include "headers/eeproms.h"
#include "headers/config.h"
#include "headers/twi.h"
#include "headers/lcd.h"
#define _XTAL_FREQ 16000000
#define read (((0x27) << 1) + 1)
#define write ((0x27) << 1)
int main(int argc, char const *argv[])
{
    ANSEL = 0;
    ANSELH = 0;
    CM2CON0 = 0X04;
    PORTB = 0;
    // __delay_ms(400);
    // 0101 0100 (1-rd) (0-wr)
    twi_init(); // 0100 --- R
    twi_address(0X40);
    // twi_write(0X33);    

    lcd_init(0,0,0);

    twi_stop();
    while (1)
    {
    }

    return 0;
}
