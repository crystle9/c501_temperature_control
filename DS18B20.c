#include "DS18B20.h"
#include <intrins.h>
#include <reg51.h>
#include "delay.h"

sbit DQ = P1^0;

int get_temperature()
{
  int t = 0;
  unsigned char * p = &t;
  if(init_DS18B20())
    {
      putc_DS18B20(0xCC); // skip ROM
      putc_DS18B20(0x44); // start transforming
      delay6us(100); // transformation time
      if(init_DS18B20())
	{
	  putc_DS18B20(0xCC);
	  putc_DS18B20(0xBE); // read RAM
	  p[1] = getc_DS18B20(); // L
	  p[0] = getc_DS18B20(); // M
	  t = t * 5 / 80;
	}
      else t = -1;
    }
  else t = -2;
  return t;
}

unsigned char init_DS18B20()
{
  bit flag = 0;
  
  DQ = 1;
  _nop_();_nop_();_nop_();
  DQ = 0;
  delay6us(80); // 480us minimum
  DQ = 1;
  delay6us(10);
  flag = (DQ == 0) ? 1 : 0; // DQ should be 0
  delay6us(70);  
  return flag;
}

unsigned char getc_DS18B20()
{
  unsigned char i = 0;
  unsigned char dat = 0;
  for (i = 8; i != 0; i--)
    {
      // recovery time
      DQ = 1;
      _nop_();_nop_();_nop_();
      // pulling low
      DQ = 0;
      _nop_();_nop_();_nop_();
      _nop_();_nop_();_nop_();
      dat >>= 1;
      // read bit
      DQ = 1;
      _nop_();_nop_();
      _nop_();_nop_();
      if(DQ)
	dat |= 0x80;
      delay6us(15);
    }
  DQ = 1;
  return dat;
}

void putc_DS18B20(unsigned char dat)
{
  unsigned char i = 0;
  for(i = 8; i != 0; i--)
    {
      // recovery time
      DQ = 1;
      _nop_();_nop_();_nop_();
      _nop_();_nop_();_nop_();
      // pulling low
      DQ = 0;
      _nop_();_nop_();_nop_();
      _nop_();_nop_();_nop_();
      _nop_();_nop_();_nop_();
      // write bit
      DQ = dat&0x01;
      delay6us(10);
      dat >>= 1; // LSB
    }
  DQ = 1;
  delay6us(2);
}
