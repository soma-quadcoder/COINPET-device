#include<avr/io.h>
#define F_CPU 16000000
#include<util/delay.h>
#include<avr/interrupt.h>
#include<avr/eeprom.h>
#include<stdio.h>

//--------s_flag를 위한 상수선언------------
#define		BREAKTIME		0x80
#define		CONFIRM			0x80
#define		CONNECT			0x40
#define		ISDATA			0x20
#define		GET_INSTRUCTION		0x02
#define 	INPUTCOIN		0x01
//------------------------------------------

//--------i_flag를 위한 상수선언------------
#define         GET_GOAL                0x80
#define         LOCK                    0x40
#define         UNLOCK	                0x02
#define         REQ_PN	                0x01
////------------------------------------------

//-------- CLCD를 위한 상수선언 ---------
#define RS_1        PORTA|= 0x01
#define RS_0        PORTA&=~0x01
#define RW_1        PORTA|= 0x02
#define RW_0        PORTA&=~0x02
#define E_1         PORTA|= 0x04
#define E_0         PORTA&=~0x04
#define OPEN_APORT  DDRA |= 0xff

#define FUNCTION 	0x2C
#define ENTRY    	0x06
#define CLEAR    	0x01
#define DISPLAY  	0x0C
#define LEFT     	0x18
#define LINE2    	0xC0
#define RIGHT    	0x1E
//----------------------------------

//-- EEPROM 데이터를 위한 주소 상수 선언 --
#define		DEFAULT_DATA		0xFF
#define		INIT_DATA		0x00
#define		S_FLAG_ADDRESS      	0x00
//-----------------------------------------


//----- 외부 인터럽트를 위한 상수 선언 ----
#define         INPUT_BPORT	DDRB|=0x00
//-----------------------------------------


//------ 동전인식을 위한 상수 선언 -------
#define		SHIPWON             10
#define		OHSHIPWON           50
#define		BACKWON             100
#define		OHBACKWON           500
//----------------------------------

//---- 프로그램 흐름을위한 전역변수 선언 ----
unsigned char	s_flag;
unsigned char	i_flag;
//-------------------------------------------

//-- 프로그램 통신에 사용될 전역변수 선언  --
unsigned char	data_buffer[10];
unsigned char	idxArr;

char 		isStart;
//-------------------------------------------

void get_eeprom_data();
void init_clcd();
void clcd_write_data(unsigned char byte);
void clcd_write_string(char str[]);
void lcd_control(unsigned char byte);
void eeprom_write(int ad,char val);
void init_pcint();
void init_uart();
unsigned char eeprom_read(int ad);

void get_eeprom_data()
{
	/*프로그램 실행시 EEPROM에 저장되어있는 
	1. 인증여부데이터	2. 목표금액
	3. 저금금액 		4. 블루투스와 연결안되어있을때 저금한 정보를
	불러온다.
	5. P/N 넘버
	[eeprom 주소별 데이터]
	000 0007 ~ 000 0010 	 (동전별 쌓여있는 금액)데이터
	000 0004 ~ 000 0006	 (현재저금금액)데이터
	000 0001 ~ 000 0003	 (목표금액)데이터
	000 0000 ~ 000 0000	 (s_flag)데이터	 
	*/

    // s_flag 데이터를 eeprom으로 부터 가져온다
    // 인증여부 & 쌓여있는 데이터유무를 알기위해서
    s_flag = eeprom_read(S_FLAG_ADDRESS);
    
    //eeprom에서 획득한 s_flag의 데이터가 0xff라는 의미는
    //default 값이라는 의미이기 때문에 초기화 해준다.
    if(s_flag == DEFAULT_DATA)
    	s_flag = INIT_DATA;

    _delay_ms(1);
}

void clcd_write_data(unsigned char byte)
{
    _delay_ms(1);
    PORTA = (byte & 0xf0);
    
    RS_1;
    RW_0;
    
    _delay_us(1);
    E_1;
    
    _delay_us(1);
    E_0;
    
    PORTA = (( byte<<4 ) & 0xf0);
    RS_1;
    RW_0;
    
    _delay_us(1);
    E_1;
    
    _delay_us(1);
    E_0;
}

void init_clcd()
{
    OPEN_APORT;
    
    _delay_ms(34);
    
    lcd_control(0x20);
    lcd_control(FUNCTION);
    lcd_control(DISPLAY);
    _delay_us(40);
    
    lcd_control(CLEAR);
    _delay_ms(1.53);
    
    lcd_control(ENTRY);
    lcd_control(0x40);
    _delay_us(40);
    
    lcd_control(0x80);
    _delay_us(40);
}

void clcd_write_string(char str[])
{
    char *pStr=0;
    
    pStr = str;
    
    while(*pStr)
        clcd_write_data(*pStr++);
}

void lcd_control(unsigned char byte)
{
    _delay_ms(1);
    
    PORTA = (byte & 0xf0);
    RS_0;
    RW_0;
    _delay_us(1);
    
    E_1;
    _delay_us(1);
    
    E_0;
    PORTA = ((byte<<4)& 0xF0);
    RS_0;
    RW_0;
    _delay_us(1);
    
    E_1;
    _delay_us(1);
    
    E_0;
}

void eeprom_write(int ad,char val)
{
    //EEPROM 원하는 주소에 데이터 저장
    eeprom_write_byte(ad,val);
}

unsigned char eeprom_read(int ad)
{
    //EEPROM 원하는 주소에 데이터 호출
    unsigned char val;
    
    val = eeprom_read_byte(ad);
    return val;
}

void init_pcint()
{
	INPUT_BPORT;
	
	//pcint 0번 핀을 사용하기위한 레지스트 설정
	PCICR	= (1<<PCIE0);
	PCMSK0	= (1<<PCINT0);

	sei();
}

void init_uart()
{
    /*
     uart2 사용
     속도 : 57600
     */
    UCSR2B|=(1<<RXCIE2)|(1<<RXEN2)|(1<<RXEN2);
    UCSR2C|=(1<<UCSZ21)|(1<<UCSZ20);
    UBRR2H=0x00;
    UBRR2L=0x10;
}

//-------------End 함수구현-----------------
//------------------------------------------

ISR(PCINT0_vect)
{	
	//연결된상태였으면
	if((s_flag & CONNECT)==CONNECT)
		s_flag-=64; //CONNECT bit 0 clear

	else
		s_flag+=64; //CONNECT bit 1 set
}

ISR(USART2_RX_vect)
{
	char data = UDR2;

	if(data=='s' && idxArr==0)
	{
		isStart=1;
	}

	else if(data=='e')
	{	
		//s_flag Get_INSTRUCTION bit 1 set
		s_flag+=2;
		idxArr=0;
		isStart=0;
	}
	
	if(isStart && data!='s')
	{
		data_buffer[idxArr] = data;
		idxArr++;
	}
}

int main()
{
	char tmp_pn='p';

	init_clcd();	    // CLCD를 사용하기위한 초기화
	init_pcint();       // 블루투스 연결유무를 판별하기위한 pcint레지스트 초기화
    	init_uart();        // 통신 레지스트 초기화
    
	get_eeprom_data();  // EEPROM으로 부터 데이터 획득
                            // s_flag => 인증여부, 쌓여있는 데이터 유무
	DDRD=0xff;
	while(1)
	{
		//P/N 미인증 상태일경우
		if((s_flag&CONFIRM)!=CONFIRM)
		{
			while(1)
			{	
				//블루투스와 미연결 상태일경우
				if((s_flag&CONNECT)!=CONNECT)
				{
					clcd_write_string("CONNECT BLUETOOTH!!");
					
					//블루투스 연결까지 대기
					while(!((s_flag&CONNECT)==CONNECT)){PORTD=s_flag;}
				}
				
				//이전에 썼던 문자 클리어
				lcd_control(CLEAR);
				clcd_write_string("---ENROLL PN NUM!---");
				break;
			}

			//인증완료될때까지 대기
			while(!((s_flag&CONFIRM)==CONFIRM))
            {
                if((s_flag&GET_INSTRUCTION)==GET_INSTRUCTION)
                {
                    switch(data_buffer[idxArr])
                    {
                        case '1':
                            idxArr++;
                            if(data_buffer[idxArr]==tmp_pn)
			    {
                                s_flag+=128; // s_flag CONFIRM bit 1로 set
				s_flag-=2;   // s_flag GET_INSTRUCTION bit 0로 clear
			    }
                            break;
                            
                        default:
                            break;
                    }
                }
            }
			
			//이전에 썼던 문자 클리어
			lcd_control(CLEAR);
			clcd_write_string("---SUCCESS ENROLL!---");
			break;
        }

		//P/N 인증 상태일경우
		else	
			break;
	}
	while(1)
	{}
}
