#include<avr/io.h>


//------ 수신 OPCODE를 위한 상수 선언 ------------
#define         REQUEST_PN              0x01
#define         LOCK_UNLOCK             0x07
#define         GET_GOAL                0x05
//-------------------------------------------


//------ 송신 데이터를 위한 상수 선언 --------------
#define         RESPONSE_PN             0x02
#define         SEND_MONEY              0x08
#define         SYNC_DATA               0x06

#define         SUCCESS_PN              "s"
#define         FAIL_PN                 "f"
#define         SUCCESS_ACK             "s"
#define         FAIL_ACK                "f"
//-------------------------------------------

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
void make_packet(char opcode, char length, unsigned char data[]);

