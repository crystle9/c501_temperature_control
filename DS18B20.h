#ifndef DS18B20_H
#define DS18B20_H

int get_temperature();
unsigned char init_DS18B20();
unsigned char getc_DS18B20();
void putc_DS18B20(unsigned char dat);

#endif // DS18B20_H
