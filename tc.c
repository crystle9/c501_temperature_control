/****************************************************************
 * P0 <--------> DBi_ST7920
 * P2 <--------> CON_ST7920
 * P1[0-3] ----> step_motor[A-D]
 * P1.4 <------> DS18B20
 * P1.5 -------> PWD out
 * P1.6 -------> CLK_HD7279
 * P1.7 <------> DAT_HD7279
 * P3.2 <------- KEY_HD7279

 * GROUND: CS__HD7279
 ****************************************************************/
#include <reg51.h>
#include <stdio.h>
#include "DS18B20.h"
#include "ST7920.h"
#include "HD7279.h"
#include "stepper_motor.h"

#define LBUF 17
#define HIGH 95
#define LOW 15

#define SMILE 0x05

unsigned char line_buf[LBUF];
sbit PWM = P1^5;
unsigned char u, u2;
unsigned char t0,t3;
unsigned int t1;
int tp0,tp1,tp,old_tp,old_tp0,old_tp1;
bit timer1_flag = 1;

void init();
void init_t0();
void init_ie0();
void calc_u();
void calc_u2();
void render_static_obj();
void render_tp();
void render_tp0();
void render_tp1();
void render_spline();
void render_status(unsigned char status);
void clear_buf(unsigned char * buf, unsigned char size, unsigned char b0);

void init()
{
  init_ST7920();
  init_HD7279();
  init_t0();
  init_ie0();
  u = 0;
  u2 = 0xFF;
  old_tp0 = tp0 = 40;
  old_tp1 = tp1 = 20;
  EA = 1;
}


void main()
{
  init();
  render_static_obj();
  render_tp0();
  render_tp1();
  while(1)
    if (timer1_flag)
      {
	timer1_flag = 0;
	old_tp = tp;
	EA = 0;
	tp = get_temperature();
	EA = 1;
	if (tp != old_tp)
	  {
	    calc_u();
	    calc_u2();
	    render_tp();	
	  }
	if (tp0 != old_tp0)
	  render_tp0();
	if (tp1 != old_tp1)
	  render_tp1();
	render_spline();
      }
}

void render_spline()
{
  char value;
  unsigned char status;

  value = tp / 2;
  status = draw_spline(value); // 8:55
  render_status(status);
}

void render_status(unsigned char status)
{
  clear_buf(line_buf,LBUF,0);
  line_buf[0] = status == 0 ? SMILE : status;
  set_cursor(1,7);
  put_line(line_buf);
}

void render_static_obj()
{
  set_cursor(0,0);
  put_line("Temperature:");
}

void render_tp()
{
  unsigned char size;
  
  size = sprintf(line_buf,"%d  ",tp);
  clear_buf(line_buf,LBUF,size);
  set_cursor(0,6);
  put_line(line_buf);
}

void render_tp0()
{
  unsigned char size;
  
  size = sprintf(line_buf,"%d  ",tp0);
  clear_buf(line_buf,LBUF,size);
  set_cursor(1,0);
  put_line(line_buf);
}

void render_tp1()
{
  unsigned char size;
  
  size = sprintf(line_buf,"%d  ",tp1);
  clear_buf(line_buf,LBUF,size);
  set_cursor(1,3);
  put_line(line_buf);
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
  p = 10 * (tp0 - tp);
  if(p < 0) p = 0;
  if(p > 0xFF) p = 0xFF;
  u = p;
}

void calc_u2()
{
  int p;
  p = 10 * (tp - tp1);
  if(p < 0) p = 0;
  if(p > 0xFF) p = 0xFF;
  u2 = 0xFF -p;
}

void response_key() interrupt 0
{
  unsigned char keycode;

  keycode = get_key();
  old_tp0 = tp0;
  old_tp1 = tp1;
  if (keycode == 0x00)
    tp0--;
  else if (keycode == 0x01)
    tp0++;
  else if (keycode == 0x02)
    tp1--;
  else if (keycode == 0x03)
    tp1++;
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
  if (t3++ > u2)
    {
      step_mode1();
      t3 = 0;
    }
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
  t3 = 0;
}

void init_ie0()
{
  EX0 = 1;
  PX0 = 1;
  IT0 = 1;
}

