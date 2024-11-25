#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#if !defined(__PARSER2)
#define __PARSER2
uint16_t mem_addr = 0;
uint8_t conf_addr = 0;
uint8_t state = 0;
uint16_t cnt_addr = 0;
uint8_t char2hex(char val)
{
    switch (val)
    {
    case '0':
        return 0X00;
        break;
    case '1':
        return 0X01;
        break;
    case '2':
        return 0X02;
        break;
    case '3':
        return 0X03;
        break;
    case '4':
        return 0X04;
        break;
    case '5':
        return 0X05;
        break;
    case '6':
        return 0X06;
        break;
    case '7':
        return 0X07;
        break;
    case '8':
        return 0X08;
        break;
    case '9':
        return 0X09;
        break;
    case 'A':
        return 0X0A;
        break;
    case 'B':
        return 0X0B;
        break;
    case 'C':
        return 0X0C;
        break;
    case 'D':
        return 0X0D;
        break;
    case 'E':
        return 0X0E;
        break;
    case 'F':
        return 0X0F;
        break;
    default:
        exit(0);
        break;
    }
}

void sendWord(char a, char b, char c, char d)
{
    if (mem_addr < 0x2000)
    {
        printf("L%c%c%c%cGRI", a, b, c, d);
        cnt_addr++;
    }
    if (mem_addr >= 0x2000 && mem_addr <= 0x2006)
    {
        printf("l%c%c%c%cGRI", a, b, c, d);
    }
    if (mem_addr >= 0x2007)
    {
        conf_addr = 7;
        printf("l%c%c%c%cIIIIIIIGRI", a, b, c, d);
    }
    conf_addr++;
}
void address(char a, char b, char c, char d)
{
    uint16_t addr = (char2hex(a) << 12) + (char2hex(b) << 8) + (char2hex(c) << 4) + char2hex(d);
    mem_addr = (addr / 2);
    // printf("//addr %x\n\n", mem_addr);
}
void word(char a, char b, char c, char d)
{ // cdab
    sendWord(b, a, d, c);
}

void readFile2()
{
    FILE *fd;
    char buff[16000];
    uint16_t cnt = 0;
    uint16_t lncnt = 0;
    printf("SZzlFFF3ZS");
    uint8_t byteCnt = 0;
    fd = fopen("headers/f690.hex", "r");
    fread(buff, sizeof(buff), 1, fd);
    for (uint16_t i = 0; i < strlen(buff); i++)
    {
        if (buff[i] == ':')
        {
            byteCnt = (char2hex(buff[i + 1]) << 4) + char2hex(buff[i + 2]);
            if (byteCnt == 0x02)
            {
                address(buff[i + 3], buff[i + 4], buff[i + 5], buff[i + 6]);
                word(buff[i + 9], buff[i + 10], buff[i + 11], buff[i + 12]);
            }
            if (byteCnt == 0x04)
            {
                address(buff[i + 3], buff[i + 4], buff[i + 5], buff[i + 6]);
                word(buff[i + 9], buff[i + 10], buff[i + 11], buff[i + 12]);
                word(buff[i + 13], buff[i + 14], buff[i + 15], buff[i + 16]);
            }
            if (byteCnt == 0x06)
            {
                address(buff[i + 3], buff[i + 4], buff[i + 5], buff[i + 6]);
                word(buff[i + 9], buff[i + 10], buff[i + 11], buff[i + 12]);
                word(buff[i + 13], buff[i + 14], buff[i + 15], buff[i + 16]);
                word(buff[i + 17], buff[i + 18], buff[i + 19], buff[i + 20]);
            }
            if (byteCnt == 0x08)
            {
                address(buff[i + 3], buff[i + 4], buff[i + 5], buff[i + 6]);
                word(buff[i + 9], buff[i + 10], buff[i + 11], buff[i + 12]);
                word(buff[i + 13], buff[i + 14], buff[i + 15], buff[i + 16]);
                word(buff[i + 17], buff[i + 18], buff[i + 19], buff[i + 20]);
                word(buff[i + 21], buff[i + 22], buff[i + 23], buff[i + 24]);
            }
            if (byteCnt == 0x10)
            {
                address(buff[i + 3], buff[i + 4], buff[i + 5], buff[i + 6]);
                word(buff[i + 9], buff[i + 10], buff[i + 11], buff[i + 12]);
                word(buff[i + 13], buff[i + 14], buff[i + 15], buff[i + 16]);
                word(buff[i + 17], buff[i + 18], buff[i + 19], buff[i + 20]);
                word(buff[i + 21], buff[i + 22], buff[i + 23], buff[i + 24]);
                word(buff[i + 25], buff[i + 26], buff[i + 27], buff[i + 28]);
                word(buff[i + 29], buff[i + 30], buff[i + 31], buff[i + 32]);
                word(buff[i + 33], buff[i + 34], buff[i + 35], buff[i + 36]);
                word(buff[i + 37], buff[i + 38], buff[i + 39], buff[i + 40]);
            }
            if (byteCnt == 0x0C)
            {
                address(buff[i + 3], buff[i + 4], buff[i + 5], buff[i + 6]);
                word(buff[i + 9], buff[i + 10], buff[i + 11], buff[i + 12]);
                word(buff[i + 13], buff[i + 14], buff[i + 15], buff[i + 16]);
                word(buff[i + 17], buff[i + 18], buff[i + 19], buff[i + 20]);
                word(buff[i + 21], buff[i + 22], buff[i + 23], buff[i + 24]);
                word(buff[i + 25], buff[i + 26], buff[i + 27], buff[i + 28]);
                word(buff[i + 29], buff[i + 30], buff[i + 31], buff[i + 32]);
            }
            if (byteCnt == 0x00)
            {
                if (state == 1)
                {
                }
            }
        }
    }

    printf("T");
    fclose(fd);
}

#endif // __PARSER2
