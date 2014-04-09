#include "stepper_motor.h"
#include <reg51.h>

sbit A_=P1^0;
sbit B_=P1^1;
sbit C_=P1^2;
sbit D_=P1^3;

const unsigned char code3[8]={1,3,4,6,4,12,8,9};
const unsigned char code2[4]={3,6,12,9};
const unsigned char code1[4]={1,2,4,8};

static unsigned char i = 0;

void step_mode3()
{
  if(++i>7)
    i = 0;
  P1 &= 0xF0;
  P1 |= code3[i];
}

void step_mode2()
{
  if(++i>3)
    i = 0;
  P1 &= 0xF0;
  P1 |= code2[i];
}

void step_mode1()
{
  if(++i>3)
    i = 0;
  P1 &= 0xF0;
  P1 |= code1[i];
}

