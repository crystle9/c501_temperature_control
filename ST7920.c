#include <reg51.h>
#include "ST7920.h"
#include "intrins.h"

#define ENABLE 1
#define DISABLE 0
#define READ 1
#define WRITE 0
#define DATA 1
#define COMMAND 0

#define DISPLAY_ON 0x3F
#define DISPLAY_OFF 0x3E
#define GRAPH_MODE_ON 0x36
#define GRAPH_MODE_OFF 0x30

sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;
sbit PSB = P2^3;
sbit BF = P0^7;

void init_ST7920()
{
  PSB = 1;
  send_command(DISPLAY_ON);
}

unsigned char draw_spline(unsigned char value)
{
  static unsigned char sel1 = 0x80;
  static unsigned char c0 = 0x00;

  unsigned char unit_buf,r0,i;

  if (value < 0x00)
    {
      draw_spline(0x00);
      return 'L';
    }
  if (value > 0x2F)
    {
      draw_spline(0x2F);
      return 'H';
    }
  
  send_command(GRAPH_MODE_ON);
  r0 = 0x3F - value;
  for (i = 0x10; i < 0x3F; i++)
    {
      unit_buf = getc_GDRAM(c0,i);
      unit_buf|= _cror_(sel1,1);
      if (i < r0)
	unit_buf&= ~sel1;
      set_position(c0,i);
      send_data(unit_buf);
    }
  sel1 = _cror_(sel1,1);
  if (sel1 == 0x80)
      if (++c0 > 0x07)
	c0 = 0;
  send_command(GRAPH_MODE_OFF);
  return 0;
}
/****************************************************************
 * at current row
 * given a string (0 < length < 16)
 *
 * print a stream of CHARACTERs on a single line
 ****************************************************************/
void put_line(unsigned char * str, unsigned char length)
{
  unsigned char i;

  for (i = 0; i < length; i++)
    {
      send_data(*str++);
      if (*str == 0x00)
	break;
    }
}

void set_cursor(unsigned char x, unsigned char y)
{
  while(x < 0) x+=8;
  while(y < 0) y+=4;
  while(x > 7) x-=8;
  while(y > 3) y-=4;

  if (y == 0)
    send_command(0x80+x);
  else if (y == 1)
    send_command(0x90+x);
  else if (y == 2)
    send_command(0x88+x);
  else
    send_command(0x98+x);
}

void send_data(unsigned char c)
{
  RS=DATA;
  _putc_ST7920(c);
}

void send_command(unsigned char c)
{
  RS=COMMAND;
  _putc_ST7920(c);
}

/****************************************************************
 * at CURRENT page, X_ADDRESS and Y_ADDRESS and DATA/INSTRUCTION
 *
 * emitt 8-bit to LCD
 ****************************************************************/
void _putc_ST7920(unsigned char c)
{
  check_busy();
  RW=WRITE;
  P0=c;
  EN=ENABLE;EN=DISABLE;
  RW=READ;
}

unsigned char _getc_ST7920()
{
  unsigned char dat;
  
  check_busy();
  RW=READ;
  P0=0xFF;
  EN=ENABLE;
  dat = P0;
  EN=DISABLE;
}

unsigned char getc_GDRAM(unsigned char c0, unsigned char r0)
{
  set_position(c0,r0);
  RS = DATA;
  return _getc_ST7920();
}

void check_busy(void)
{
  RS=COMMAND;
  RW=READ;
  P0=0xFF;
  EN=ENABLE;
  while(BF)
    {
      EN=DISABLE;
      RS=COMMAND;
      RW=READ;
      P0=0xFF;
      EN=ENABLE;
    }
  EN=DISABLE;
}

void clear_screen()
{
  unsigned char i,j;

  for (i = 0; i < 4; i++)
    {
      set_cursor(i,0);
      for (j = 0; j < 16; j++)
	send_data(' ');
    }
}

/****************************************************************
 * pixel_clear
 ****************************************************************/
void clear_row(unsigned char row, unsigned char c0)
{
  unsigned char j;

  send_command(GRAPH_MODE_ON);
  for(j = c0; j < 0x08; j++)
    {
      set_position(j,row);
      send_data(0x00);
    }
  send_command(GRAPH_MODE_OFF);  
}

/****************************************************************
 * page_clear
 ****************************************************************/
void clear_col(unsigned char col, unsigned char r0)
{
  unsigned char i;

  send_command(GRAPH_MODE_ON);
  for(i = r0; i < 0x2F; i++)
    {
      set_position(col,i);
      send_data(0x00);
    }
  send_command(GRAPH_MODE_OFF);
}

void _set_xy(unsigned char x, unsigned char y)
{
  send_command(GRAPH_MODE_ON);
  send_command(y);
  send_command(x);
  send_command(GRAPH_MODE_OFF);
}

void set_position(unsigned char c0, unsigned char r0)
{
  unsigned char col, row, x, y;

  col = c0;
  row = r0;
  while(col < 0) col+=0x08;
  while(row < 0) row+=0x2F;
  while(col > 0x08) col-=0x08;
  while(row > 0x2F) row-=0x2F;

  x = col;
  y = row;
  if (row > 0x1F)
    {
      x+=0x08;
      y-=0x1F;
    }
  _set_xy(x,y);
}
