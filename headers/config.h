#include <xc.h>
#if !defined(CONFS)
#define CONFS
// #pragma config FOSC = INTRCIO
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config BOREN = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config IESO = OFF 
#pragma config FCMEN = OFF

// #pragma config IDLOC0 = 0x20
// #pragma config IDLOC1 = 0x03
// #pragma config IDLOC2 = 0x03
// #pragma config IDLOC3 = 0x24

#endif // CONFS
