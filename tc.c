#include <reg51.h>
#include <stdio.h>
#include "DS18B20.h"
#include "ST7920.h"
#include "HD7279.h"

#define LBUF 17
#define HIGH 95
#define LOW 15

unsigned char line_buf[LBUF];
sbit PWM = P1^1;
unsigned char t0,u = 0;
unsigned int t1;
unsigned char t2 = 0;
int tp0 = 50;
int tp = -128;
int old_tp;
bit timer1_flag = 1;

void init();
void init_t0();
void init_ex0();
void calc_u();
void render_static_obj();
void render_tp();
void render_spline();
void clear_buf(unsigned char * buf, unsigned char size, unsigned char b0);

void init()
{
  EA = 1;
  init_LCD();
  clear_screen();
  init_t0();
  render_static_obj();
}

void main()
{
  init();
  while(1)
    if (timer1_flag)
      {
	timer1_flag = 0;
	PWM = 0;
	old_tp = tp;
	EA = 0;
	tp = get_temperature();
	EA = 1;
	if (tp != old_tp)
	  {
	    calc_u();
	    render_tp();
	  }
	render_spline();
      }
}

void render_spline()
{
  unsigned char value;

  value = tp > HIGH ? HIGH : tp;
  value /= 2;
  select1();
  draw_spline(t2++,value);
  if(t2 > 63) t2 = 0;
}

void render_static_obj()
{
  unsigned char size;
  size = sprintf(line_buf,"Temperature:");
  clear_buf(line_buf,LBUF,size);
  set_x(0);
  put_line(line_buf,0);
}

void render_tp()
{
  unsigned char size;
  size = sprintf(line_buf,"%d  ",tp);
  clear_buf(line_buf,LBUF,size);
  set_x(0);
  put_line(line_buf,13);
}

void clear_buf(unsigned char * buf, unsigned char size, unsigned char b0)
{
  unsigned char i;
  for (i = b0; i < size; i++)
    buf[i] = 0x00;
}

void calc_u()
{
  int p;
  p = tp0 - tp;
  if(p < 0) p = 0;
  if(p > 10) p = 10;
  u = p * 20;
}

void pwm() interrupt 1
{
  TR0 = 0;
  TH0 = 0xfc;
  TL0 = 0x66;
  if(t1++ > 250)
    {
      timer1_flag = 1;
      t1 = 0;
    }
  t0 ++;
  PWM = t0 < u ? 1 : 0;
  TR0 = 1;
}

void init_t0()
{
  TMOD &= 0xf0;
  TMOD |= 0x01;
  TH0 = 0xfc; // 1ms
  TL0 = 0x66;
  TR0 = 1;
  ET0 = 1;
  t0 = 0;
  t1 = 0;
}
