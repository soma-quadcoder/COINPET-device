#include <avr/io.h>


//--------s_flag를 위한 상수선언----------------
#define         BREAKTIME               0x80
#define         CONFIRM                 0x80
#define         CONNECT                 0x40
#define         ISDATA                  0x20
#define         ISGAME                  0x10
#define         GET_INSTRUCTION         0x02
#define         INPUT_COIN              0x01
//------------------------------------------

//-- EEPROM 데이터를 위한 주소 상수 선언 ----------
#define         DEFAULT_DATA            0xFF
#define         INIT_DATA               0x00
#define         S_FLAG_ADDRESS          0x00
#define         CURRENT_MONEY           0x06
#define         UNCONNECT_DATA_ADDRESS  0x07
//------------------------------------------

//---- 프로그램 흐름을위한 전역변수 선언 -----------
unsigned char   s_flag;
//------------------------------------------

void change_bit_val(unsigned char position, unsigned char value);