#include<avr/io.h>
#include<avr/interrupt.h>


//------ 수신 OPCODE를 위한 상수 선언(블루) --------
#define         REQUEST_PN              0x01
#define         ACK                     0x03
#define         GET_ACK                 0x03
#define         GET_UTC_TIME            0x04
#define         LOCK_UNLOCK             0x07
#define         GET_GOAL                0x05
#define         REQUEST_SAVED_COIN      0x09
#define         REQUEST_GAMECONNECT     0x10
//-------------------------------------------

//------ 송신 OPCODE를 위한 상수 선언(게임보드) --------
#define         START_GAME              0x11
#define         SEND_GAMEDATA           'G'
#define         SUMMIT_DATA             0x12
//-------------------------------------------


//------ 송신 데이터를 위한 상수 선언 --------------
#define         RESPONSE_PN             0x02
#define         SEND_MONEY              0x08
#define         SYNC_DATA               0x06
#define         SEND_ISGAME             0x11
#define         DISCONNECT              0x13

#define         SUCCESS_PN              "s"
#define         FAIL_PN                 "f"
#define         SUCCESS_ACK             "s"
#define         FAIL_ACK                "f"
#define         CONNECTTING             "y"
#define         NON_CONNECTTING         "n"

//-------------------------------------------

//----- 송/수신 OPCODE를 위한 상수 선언------------
#define         START_BYTE              'S'
#define         END_BYTE                'E'
#define         MAX_INDEX               20
//-------------------------------------------


//-- 프로그램 통신에 사용될 전역변수 선언  -----------
unsigned char   data_buffer[10];
unsigned char   idxArr;
extern unsigned char temp_pn[17];
unsigned long	goal_money;

char            isStart;
//-------------------------------------------


void init_uart();
void make_packet(unsigned char opcode, char length, unsigned char data[]);
char compare_pn(unsigned char idx);
void proccess_instruction(void);
void get_goal(unsigned char idx);


