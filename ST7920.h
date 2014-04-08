#ifndef LCD12864_H
#define LCD12864_H

void init_LCD();
void put_line(unsigned char * str, unsigned char indent);
void _put_line_LCD(unsigned char * str, unsigned char cs, unsigned char indent);
void clear_screen();
void clear_x(unsigned char x, unsigned char y0);
void clear_y(unsigned char y, unsigned char x0);
unsigned char draw_spline(unsigned char y, unsigned char value);
void putc_command(unsigned char c);
void putc_data(unsigned char c);
void _putc_LCD(unsigned char c);
void set_start_line(unsigned char n);
void set_xy(unsigned char x, unsigned char y);
void set_x(unsigned char x);
void set_y(unsigned char y);
void select1();
void select2();
void check_busy(void);


#endif // LCD12864_H
