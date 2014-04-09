#include "stepper_motor.h"
#include <reg51.h>

sbit A_=P1^0;
sbit B_=P1^1;
sbit C_=P1^2;
sbit D_=P1^3;

const unsigned char code3[8]={1,3,4,6,4,12,8,9};

void step_mode3()
{
  static unsigned char i = 0;

  P1 &= 0xF0;
  P1 |= code3[i];
  if(++i>7)
    i = 0;
}
  
