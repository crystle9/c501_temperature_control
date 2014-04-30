#include <reg51.h>
#include "ST7920.h"
#include "intrins.h"
#include "delay.h"

#define ENABLE 1
#define DISABLE 0
#define READ 1
#define WRITE 0
#define DATA 1
#define COMMAND 0

#define EXTEND_BEGIN 0x34
#define GRAPH_MODE_OFF 0x34
#define GRAPH_MODE_ON 0x36
#define EXTEND_END 0x30

sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;
sbit PSB = P2^3;

unsigned char unit_buf[2];
unsigned int * punit_buf = unit_buf;

void init_ST7920()
{
  PSB = 1;
  send_command(0x30); // basic cmd set, 8 bit data
  send_command(0x0c); // display on, cursor off, blink off
  send_command(0x01); // clear display
  delay6us(200);
}

unsigned char draw_spline(char value)
{
  static unsigned int sel1 = 0x8000;
  static unsigned char c0 = 0x00;

  unsigned char r0,i;

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
  
  send_command(EXTEND_BEGIN);
  send_command(GRAPH_MODE_OFF);
  r0 = 0x3F - value;
  for (i = 0x10; i < 0x40; i++)
    {
      _geti_GDRAM(c0,i);
      //*punit_buf |= _iror_(sel1,1);
      if (i <= r0)
	*punit_buf &= ~sel1;
      else
	*punit_buf |= sel1; 
      _set_xy(c0,i);
      send_data(unit_buf[0]);
      send_data(unit_buf[1]);
    }
  if (sel1 == 0x0001)
    if (++c0 > 0x07)
      c0 = 0;
  sel1 = _iror_(sel1,1);
  send_command(GRAPH_MODE_ON);
  send_command(EXTEND_END);
  return 0;
}

void put_line(unsigned char * str)
{
  unsigned char  * p = str;
  while(*p)
      send_data(*p++);
}

void set_cursor(unsigned char y, unsigned char x)
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
  check_busy();
  RS=DATA;
  _putc_ST7920(c);
}

void send_command(unsigned char c)
{
  check_busy();
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
  RW=WRITE;
  P0=c;
  EN=ENABLE;
  EN=DISABLE;
  RW=READ;
}

unsigned char receive_data()
{
  check_busy();
  RS=DATA;
  return _getc_ST7920();
}

unsigned char _getc_ST7920()
{
  unsigned char dat;
  
  RW=READ;
  P0=0xFF;
  EN=DISABLE;  
  EN=ENABLE;
  _nop_();_nop_();_nop_();
  dat=P0;
  EN=DISABLE;
  return dat;
}

void _geti_GDRAM(unsigned char c0, unsigned char r0)
{
  _set_xy(c0,r0);

  /* dummy read */
  receive_data();
  
  unit_buf[0] = receive_data();
  unit_buf[1] = receive_data();
}

void check_busy()
{
  RS=COMMAND;  
  while(_getc_ST7920()&0x80);
}

void _set_xy(unsigned char c0, unsigned char r0)
{
  unsigned char col, row, x, y;

  col = c0;
  row = r0;
  while(col < 0) col+=0x08;
  while(row < 0) row+=0x2F;
  while(col > 0x08) col-=0x08;
  while(row > 0x3F) row-=0x40;

  x = col;
  y = row;
  if (row > 0x1F)
    {
      x+=0x08;
      y-=0x20;
    }
  send_command(0x80+y);
  send_command(0x80+x);
}
