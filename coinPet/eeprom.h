#include<avr/io.h>
#define F_CPU 20000000
#include<util/delay.h>
#include<avr/eeprom.h>

//-- EEPROM 데이터를 위한 주소 상수 선언 ---------
#define		DEFAULT_DATA            0xFF
#define		INIT_DATA               0x00
#define		S_FLAG_ADDRESS      	0x00
#define		CURRENT_MONEY           0x06
#define		UNCONNECT_DATA_ADDRESS	0x07
//-----------------------------------------


void get_eeprom_data();
void eeprom_write(int ad,unsigned char val);
unsigned char eeprom_read(int ad);