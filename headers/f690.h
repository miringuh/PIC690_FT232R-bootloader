#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#if !defined(__f690)
#define __f690
// __interrupt(low_priority) void getData(void) {

// }
void led(uint8_t val){
    TRISC = 0;
    PORTC = val;
}

#endif // __f690