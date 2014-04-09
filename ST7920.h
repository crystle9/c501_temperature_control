#ifndef ST7920_H
#define ST7920_H

void init_ST7920();
void put_line(unsigned char * str, unsigned char indent);
void set_cursor(unsigned char x, unsigned char y);
void clear_screen();

unsigned char draw_spline(unsigned char value);
void set_position(unsigned char c0, unsigned char r0);
void clear_row(unsigned char row, unsigned char c0);
void clear_col(unsigned char col, unsigned char r0);
void _set_xy(unsigned char x, unsigned char y);

void check_busy(void);
void send_command(unsigned char c);
void send_data(unsigned char c);
unsigned char getc_GDRAM(unsigned char c0, unsigned char r0);
unsigned char _getc_ST7920();
void _putc_ST7920(unsigned char c);

#endif // ST7920_H
