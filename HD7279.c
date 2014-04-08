#include <reg51.h>
#include "delay.h"

#define READ_KEY 0x15
#define LONG_DELAY delay6us(9)
#define SHORT_DELAY delay6us(1)

// CS_ === 0, GROUND
sbit DAT=P1^7;
sbit CLK=P1^6;
sbit KEY=P3^2;

void init_7279()
{
  return;
}

unsigned char get_key()
{
  putc_7279(READ_KEY);
  delay6us(4);
  return(getc_7279());
}

void putc_7279(unsigned char dat)
{
  unsigned char i;
  unsigned char select_bit = 0x80;

  LONG_DELAY; // around 50us
  for(i=0;i<8;i++)
  {
    DAT = data&select_bit ? 1 : 0;
    CLK=1;
    SHORT_DELAY;
    CLK=0;
    SHORT_DELAY;
    select_bit <<= 1;
  }
  DAT=0;
}

unsigned char getc_7279(void)
{
  unsigned char i;
  unsigned char rst=0x00;
  CS=0;
  LONG_DELAY;
  DAT=1;
  for(i=0;i<8;i++)
  {
    CLK=1;
    SHORT_DELAY;
    rst <<= rst;
    if(DAT)
      rst++;
    CLK=0;
    SHORT_DELAY;
   }
   DAT=0;
   return rst;
} 


