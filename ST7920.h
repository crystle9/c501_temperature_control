#ifndef ST7920_H
#define ST7920_H

void init_ST7920();
void put_line(unsigned char * str);
void set_cursor(unsigned char y, unsigned char x);

unsigned char draw_spline(char value);
void _geti_GDRAM(unsigned char c0, unsigned char r0);
void _set_xy(unsigned char c0, unsigned char r0);

void check_busy();
void send_command(unsigned char c);
void send_data(unsigned char c);
void _putc_ST7920(unsigned char c);
unsigned char receive_data();
unsigned char _getc_ST7920();

#endif // ST7920_H
