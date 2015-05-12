#include<avr/io.h>
#define F_CPU 20000000
#include<util/delay.h>

extern unsigned char font_money[6][16];
extern unsigned char font_num[11][16];


#define SPI_DDR      DDRB
#define SPI_PORT     PORTB
#define SS           PB4
#define SCK          PB7
#define MOSI         PB5
#define MISO         PB6
#define DC           PB3
#define RST          PB2
#define CHANGE_COLUM 0x21
#define CHANGE_PAGE  0xB0


void init_SPI_master(void);
void write_oled(unsigned char data);
void write_oled_data(unsigned char data);
void clear_oled(void);
void draw_char(int w, int h, int x, int y, char *data);
void draw_data(int w, int h, int x, int y, char data);
void write_num_to_oled(unsigned long current_money);
void init_oled(void);

