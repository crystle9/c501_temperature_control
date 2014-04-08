#include <reg51.h>
#include "KS0108.h"
#include "intrins.h"
#include "courier.h"

#define ENABLE 1
#define DISABLE 0
#define READ 1
#define WRITE 0
#define DATA 1
#define COMMAND 0
#define DISPLAY_ON 0x3F
#define DISPLAY_OFF 0x3E

sbit  DI = P2^0;
sbit  RW = P2^1;
sbit  EN = P2^2;
sbit  CS1= P2^3;
sbit  CS2= P2^4;
sbit  RST_= P2^5;
sbit  BF = P0^7;
unsigned char current_x, current_n;

void init_LCD()
{
  RST_ = 1;
  putc_command(DISPLAY_ON);
  set_start_line(0);
  set_xy(0,0);
}

unsigned char draw_spline(unsigned char y, unsigned char value)
{
  unsigned char i;
  unsigned char x0,p0;

  if (y > 63) return 'Y';
  clear_y(y,2);
  
  if (value > 55) return 'H';
  if (value < 8) return 'L';

  x0 = 9 - value / 8;
  p0 = value % 8;
  if(p0 > 0)
    {
      set_xy(x0-1,y);
      p0 = 8 - p0;
      putc_data(0xFF<<p0);
    }
  for(i = x0; i < 8; i++)
    {
      set_xy(i,y);
      putc_data(0xFF);
    }
  return 0;
}
/****************************************************************
 * at current row
 * given a string (0 < length < 16)
 *
 * print a stream of CHARACTERs on a single line, auto RETURN
 ****************************************************************/
void put_line(unsigned char * str, unsigned char indent)
{
  unsigned char * p = str;

  set_x(current_x);
  if (indent < 8)
    {
      _put_line_LCD(p,1,indent);      
      set_x(current_x-2);
      p = str + 8 - indent;
      _put_line_LCD(p,2,0);
    }
  else
    _put_line_LCD(p,2,indent-8);
}

/****************************************************************
 * at current row
 *
 * FILL the given chip with CHARACTERs (occupies 2 rows)
 * and set X_ADDRESS to NEWLINE
 *
 * the given str should ONLY contain asicc CHARACTERS
 * and end with 0x00s
 * y0 specify this operation's start pixel 
 ****************************************************************/
void _put_line_LCD(unsigned char * str, unsigned char cs, unsigned char indent)
{
  unsigned char c, i, j;

  cs == 1 ? select1() : select2();

  set_y(indent * 8);
  for (i = 0; i < 8 - indent; i++)
    {
      c = str[i];
      if (c == 0x00) break;
      for (j = 0; j < 8; j++)
	putc_data(courier[j][c]);	
    }

  set_x(current_x+1);
  set_y(indent * 8);
  for (i = 0; i < 8 - indent; i++)
    {
      c = str[i];
      if (c == 0x00) break;
      for (j = 8; j < 16; j++)
	putc_data(courier[j][c]);	
    }
  set_x(current_x+1);
}

void putc_data(unsigned char c)
{
  DI=DATA;
  _putc_LCD(c);
}

void putc_command(unsigned char c)
{
  DI=COMMAND;
  _putc_LCD(c);
}

/****************************************************************
 * at CURRENT page, X_ADDRESS and Y_ADDRESS and DATA/INSTRUCTION
 *
 * emitt 8-bit to LCD
 ****************************************************************/
void _putc_LCD(unsigned char c)
{
  check_busy();
  RW=WRITE;
  P0=c;
  EN=ENABLE;EN=DISABLE;
  RW=READ;
}

void check_busy(void)
{
  _nop_();_nop_();_nop_();
  /*DI=COMMAND;
  RW=READ;
  P0=0xFF;
  EN=ENABLE;
  while(BF)
    {
      DI=COMMAND;
      RW=READ;
      P0=0xFF;
      EN=ENABLE; 
    }
  EN=DISABLE;
  RW=WRITE;
  DI=DATA;*/
}

void clear_screen()
{
  unsigned char i;

  select1();
  for(i = 0; i < 8; i++)
    clear_x(i,0);
  select2();
  for(i = 0; i < 8; i++)
    clear_x(i,0);
  select1();
}

/****************************************************************
 * at the CURRENT chip:
 *
 * set Y_ADDRESS and clear data at EACH X after x0
 ****************************************************************/
void clear_y(unsigned char y, unsigned char x0)
{
  unsigned char i;
  for(i = x0; i < 8; i++)
    {
      set_xy(i,y);
      putc_data(0x00);
    }
}

/****************************************************************
 * at the CURRENT chip:
 *
 * set X_ADDRESS and clear data at EACH Y after y0
 ****************************************************************/
void clear_x(unsigned char x, unsigned char y0)
{
  unsigned char  j;
  set_xy(x, y0);
  for(j = y0; j < 64; j++)
    putc_data(0x00);
}

/****************************************************************
 * use current_x:
 *
 * select chip 1 and set X_ADDRESS to current_x
 ****************************************************************/
void select1()
{
  CS1 = 0; CS2 = 1;
  set_x(current_x);
}

void select2()
{
  CS1 = 1; CS2 = 0;
  set_x(current_x);
}

void set_start_line(unsigned char n)
{
  if(n < 64)
    {
      putc_command(0xC0 + n);
      current_n = n;
    }
  else set_start_line(n % 64);
}

void set_xy(unsigned char x, unsigned char y)
{
  set_x(x);
  set_y(y);
}

/****************************************************************
 *
 *
 * set X_ADDRESS and UPDATE current_x
 ****************************************************************/
void set_x(unsigned char x)
{
  if(x < 8)
    {
      putc_command(0xB8 + x);
      current_x = x;
    }
  else
      set_x(x - 8);
}

void set_y(unsigned char y)
{
  if(y < 64)
    putc_command(0x40 + y);
  else
    set_x(y - 64);
}
