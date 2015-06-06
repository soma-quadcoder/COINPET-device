#include<avr/io.h>
#include<avr/interrupt.h>

#define DS1307_ADD  0x68
#define MINUTE      0x01
#define HOUR        0x02
#define DATE        0x04
#define MONTH       0x05
#define YEAR        0x06


void init_i2c();
void i2c_write(unsigned char dev_add, unsigned char reg_add,unsigned char data);
unsigned char i2c_read(unsigned char dev_add, unsigned char reg_add);
unsigned char get_time_vale(unsigned char data);

void set_rtc(unsigned char min,unsigned char hour,unsigned char date,
             unsigned char month,unsigned char year);


