#include "delay.h"

void delay6us(unsigned char t)
{
  unsigned char i;
  for(i = t; i != 0; i--);
}
