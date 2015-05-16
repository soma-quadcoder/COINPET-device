#include<avr/io.h>
#include<avr/interrupt.h>
#define F_CPU 20000000
#include<util/delay.h>

//----- 송/수신 OPCODE를 위한 상수 선언------------
#define         START_BYTE              'S'
#define         END_BYTE                'E'
#define         MAX_INDEX               20
//-------------------------------------------


//-- 프로그램 통신에 사용될 전역변수 선언  -----------
unsigned char   data_buffer[10];
unsigned char   idxArr;

char            isStart;
//-------------------------------------------

void init_uart();
void make_packet(unsigned char opcode, char length, unsigned char data[]);


