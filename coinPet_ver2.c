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
#define 	INPUT_COIN		0x01
//------------------------------------------

//--------i_flag를 위한 상수선언------------
#define         GET_GOAL                0x80
#define         LOCK                    0x40
#define         UNLOCK	                0x02
#define         REQ_PN	                0x01
//------------------------------------------

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
#define	LINE1		0x80
#define LINE2    	0xC0
#define	LINE3		0x94
#define RIGHT    	0x1E
//--------------------------------------

//-- EEPROM 데이터를 위한 주소 상수 선언 --
#define		DEFAULT_DATA		0xFF
#define		INIT_DATA		0x00
#define		S_FLAG_ADDRESS      	0x00
#define		CURRENT_MONEY		0x04
//-----------------------------------------


//----- 외부 인터럽트를 위한 상수 선언 ----
#define         INPUT_BPORT	DDRB|=0x00
//-----------------------------------------


//------ 동전인식을 위한 상수 선언 -------
#define		SHIPWON             10
#define		OHSHIPWON           50
#define		BACKWON             100
#define		OHBACKWON           500
//----------------------------------------

//------ 수신 OPCODE를 위한 상수 선언 -------
#define		REQUEST_PN		0x01
#define 	LOCK_UNLOCK		0x07
//-------------------------------------------

//------ 송신 데이터를 위한 상수 선언 -------
#define         RESPONSE_PN             0x02
#define		SEND_MONEY		0x08

#define		SUCCESS_PN		"s"
#define		FAIL_PN			"f"
#define		SUCCESS_ACK		"s"
#define		FAIL_ACK		"f"
//-------------------------------------------

//----- 송/수신 OPCODE를 위한 상수 선언------
#define		START_BYTE		'S'
#define		END_BYTE		'E'
#define         MAX_INDEX               20
//-------------------------------------------

//---- 프로그램 흐름을위한 전역변수 선언 ----
unsigned char	s_flag;
unsigned char	i_flag;
//-------------------------------------------

//---- 동전구별에 사용될 전역변수 선언 ------
unsigned int	adc_max;
unsigned int	current_money;
//-------------------------------------------

//-- 프로그램 통신에 사용될 전역변수 선언  --
unsigned char	data_buffer[10];
unsigned char	idxArr;

char 		isStart;
//-------------------------------------------

unsigned char temp_pn[17]="1234123412341234";

void get_eeprom_data();
void init_clcd();
void clcd_write_data(unsigned char byte);
void clcd_write_string(char str[]);
void lcd_control(unsigned char byte);
void eeprom_write(int ad,char val);
void init_pcint();
void init_int();
void init_uart();
void change_bit_val(char position, char value);
void make_packet(char opcode, char length, char data[]);
char compare_pn();
void proccess_instruction();
void init_adc();
void proccess_coin();
char get_money_len();
void check_bluetooth();
void write_num_lcd(unsigned int current_money);
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
    	//eeprom_write(0x00, 0xff);
    	s_flag = eeprom_read(S_FLAG_ADDRESS);
    	//eeprom에서 획득한 s_flag의 데이터가 0xff라는 의미는
    	//default 값이라는 의미이기 때문에 초기화 해준다.
    	if(s_flag == DEFAULT_DATA)
	{
		char i;
		for( i=0;i<10;i++ )
		{
			eeprom_write(i,INIT_DATA);
			_delay_ms(1);
		}
    		s_flag = INIT_DATA;
	}
/*	
	if((s_flag&CONFIRM)==CONFIRM)
	{
		char i=0;
		unsigned tmp;
		for( ; ; )
		{
			tmp = eeprom_read(CURRENT_MONEY+i);
			_delay_ms(1);
			current_money = current_money|(current_money<<8
			if(tmp==0)
				break;
			i++;
		{
	}
	*/
  	_delay_ms(1);
}

void check_bluetooth()
{
	// 연결이 안된상태일경우
	if(PINB & 0x01==0x01)
		change_bit_val(CONNECT,0);
	// 연결된 경우
	else
		change_bit_val(CONNECT,1);
	
	PORTC = s_flag;
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

void init_int()
{
	EIMSK	= (1<<INT1);
	EICRA	= (1<<ISC11)|(1<<ISC10);
}

void init_uart()
{
    /*
     uart2 사용
     속도 : 57600
     */
    UCSR2B|=(1<<RXCIE2)|(1<<RXEN2)|(1<<TXEN2);
    UCSR2C|=(1<<UCSZ21)|(1<<UCSZ20);
    UBRR2H=0x00;
    UBRR2L=0x10;
}


void change_bit_val(char position, char value)
{
	//값을 1로 set 하는 경우
	if(value)
		s_flag|=position;
	//값을 0으로 clear하는 경우
	else
		s_flag&=~(position);
}

void proccess_instruction()
{
    	switch(data_buffer[idxArr++])
    	{
        	unsigned char		data[MAX_INDEX];
       	
        	case REQUEST_PN:
           	// PN 등록요청이오면 PN번호를 비교한다
            	if(compare_pn())
           	{
                	make_packet(RESPONSE_PN,1,SUCCESS_PN);
			change_bit_val(CONFIRM,s_flag-64);	// s_flag CONFIRM bit 1로 set
                	eeprom_write(S_FLAG_ADDRESS, 0x80);	// eeprom에 인증여부 저장
            	}
            
            	else
                	make_packet(RESPONSE_PN,1,FAIL_PN);
            
            	change_bit_val(GET_INSTRUCTION,0);	// s_flag GET_INSTRUCTION bit 0로 clear
            	break;
            
        	case LOCK_UNLOCK:
            
            	break;
            
        	default:
            	break;
    	}
}


void make_packet(char opcode, char length, char data[])
{
	//송신 패킷데이터를 만들어주는 함수
	char packet[MAX_INDEX];
	char idx;
	char i;

	idx = 0;
	//start make packet
	packet[idx++] = START_BYTE;
	packet[idx++] = opcode;
	packet[idx++] = length;
	
	for( i=0 ;i<length;i++ )
		packet[idx++] = data[i];
	
	packet[idx] = END_BYTE;
	
	_delay_ms(2);
	//start send packet
	for( i=0;i<length+4;i++ )
	{
		UDR2 = packet[i];
		_delay_ms(1);
	}
}

char compare_pn()
{
	unsigned char length;
	char i;
	char isMatch;

	length = data_buffer[idxArr++];

	for( i=0;i<length;i++ )
	{
		if(temp_pn[i]==data_buffer[idxArr++])
			isMatch = 1;
		else
		{
			isMatch = 0;
			break;
		}
	}
	return isMatch;
}

void init_adc()
{
	ADMUX 	|= (1<<REFS0) | (1<<REFS1);
	ADCSRA	|= (0<<ADEN) | (1<<ADPS2)|(2<<ADPS1)|(ADPS0)|
		   (1<<ADSC) | (1<<ADATE) | (1<<ADIE);
}

char get_money_len()
{
	if( current_money<256 )
		return 1;
	else if( current_money<65536 && current_money>255)
		return 2;
	else
		return 3;
}

void proccess_coin()
{
	/* adc 값에 따른 동전 구별
	 * 10~30 	: 10원
	 * 100~200 	: 50원
	 * 400~470 	: 100원
	 * 490~550	: 500원
	 */
	char i;
	char length;
	
	unsigned char	data[3];
	unsigned int	tmp_money;

	_delay_ms(10);
	// 금액 처리
	if( adc_max>10 && adc_max<30 ){
		current_money += SHIPWON; UDR2 = 0x44;}
	else if( adc_max>100 && adc_max<200 ){
		current_money += OHSHIPWON;UDR2 = 0x55;}
	else if( adc_max>400 && adc_max<470 ){
		current_money += BACKWON;UDR2 = 0x66;}
	else if (adc_max>490 && adc_max<550 ){
	 	current_money += OHBACKWON;UDR2 = 0x77;}
	else
		UDR2 = 0xed;

	tmp_money = current_money;

	_delay_ms(10);
	// 현제 저금금액에따른 바이트 길이를 구한다
	length = get_money_len();
	for( i=0;i<length;i++ )
	{
		// 8비트씩 char 배열에 저장
		data[i] = tmp_money & 0xff;
		// eeprom에 현재 저금금액 저장
		eeprom_write(CURRENT_MONEY+i,data[i]);
		_delay_ms(1);

		tmp_money = tmp_money>>8;
	}

	// 저금한 금액 전송
	make_packet(SEND_MONEY,length,data);
	// 저금금액 lcd에 출력
	write_num_lcd(current_money);
	// s_flag 0으로 클리어
	change_bit_val(INPUT_COIN,0);
	_delay_ms(1);
}

void write_num_lcd(unsigned int current_money)
{
	char curr_coin[10];
	unsigned char position=0;
	unsigned int cu_co=0;

	cu_co = current_money;

	while(1)
	{
		curr_coin[position++]=cu_co%10+48;
		cu_co=cu_co/10;

		if(cu_co==0)
			break;
	}

	lcd_control(CLEAR);
	clcd_write_string(" money:");
	for(position=position-1;position>0;position--)
		clcd_write_data(curr_coin[position]);
	clcd_write_data(curr_coin[0]);

}

//-------------End 함수구현-----------------
//------------------------------------------

ISR(PCINT0_vect)
{	
	//연결된상태였으면
	if((s_flag & CONNECT)==CONNECT)
		change_bit_val(CONNECT,0); //CONNECT bit 0 clear

	else
		change_bit_val(CONNECT,1); //CONNECT bit 1 set
}

ISR(INT1_vect)
{

	//동전투입 시작
	if( adc_max ==0 )
	{
		ADCSRA = (1<< ADEN)|(1<<ADPS2)|(2<<ADPS1)|(ADPS0);
		ADCSRA = ADCSRA | (1<<ADSC) | (1<<ADATE) | (1<<ADIE);
		EICRA   = (1<<ISC11) | (1<<ISC10);
	}
	//동전투입 끝
	else
	{
	
		ADCSRA = (0<< ADEN)|(1<<ADPS2)|(2<<ADPS1)|(ADPS0);
		ADCSRA = ADCSRA | (1<<ADSC) | (1<<ADATE) | (1<<ADIE);
		EICRA   = (1<<ISC11) | (0<<ISC10);
		//동전인식 시작을위한 s_flag INPUT_COIN 1로 set
		change_bit_val(INPUT_COIN,1);
	}

	_delay_ms(2);
}

ISR(USART2_RX_vect)
{
	char data = UDR2;
	

	if(data==START_BYTE && idxArr==0)
		isStart = 1;

	else if(data==END_BYTE && idxArr!=0)
	{	
		//s_flag Get_INSTRUCTION bit 1 set
		change_bit_val(GET_INSTRUCTION,1);
		idxArr	= INIT_DATA;
		isStart	= INIT_DATA;
	}
	
	if(isStart && data!=START_BYTE)
	{
		data_buffer[idxArr] = data;
		idxArr++;
	}
}

ISR(ADC_vect)
{
	if( ADC>adc_max )
		adc_max = ADC;
}

int main()
{
	init_clcd();	    // CLCD를 사용하기위한 초기화
	init_pcint();       // 블루투스 연결유무를 판별하기위한 pcint레지스트 초기화
    	init_uart();        // 통신 레지스트 초기화
   	init_adc();	    // 동전인식에 사용될 adc 레지스트 설정
	init_int();

	get_eeprom_data();  // EEPROM으로 부터 데이터 획득
       			    // s_flag => 인증여부, 쌓여있는 데이터 유무
	check_bluetooth();  // 블루투스 연결 유무를 체크
	
	DDRD=0x00;
	DDRC=0xff;
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
					while(!((s_flag&CONNECT)==CONNECT)){PORTC=s_flag;}
				}
				
				//이전에 썼던 문자 클리어
				lcd_control(CLEAR);
				lcd_control(LINE1);
				clcd_write_string("---ENROLL PN NUM!---");
				lcd_control(LINE2);
				clcd_write_string("PN:");
				clcd_write_string(temp_pn);
				break;
			}

			//인증완료될때까지 대기
			while(!((s_flag&CONFIRM)==CONFIRM))
            		{
                		PORTC = s_flag; // 디버깅을 위한 ....
               			if((s_flag&GET_INSTRUCTION)==GET_INSTRUCTION)
                			proccess_instruction();
            		}
			PORTC=s_flag;
			
			//이전에 썼던 문자 클리어
			lcd_control(CLEAR);
			clcd_write_string("---SUCCESS ENROLL!---");
			break;
        	}

		//P/N 인증 상태일경우
		else	
			break;
	}
	int i = 0;
	lcd_control(LINE1);
	_delay_ms(2000);
	for(i=0;i<80;i++)
	{
		_delay_ms(100);
		clcd_write_data(0xff);
	}

	init_int();
	while(1)
	{
		if((s_flag&INPUT_COIN)==INPUT_COIN)
		{
			proccess_coin();	
			adc_max=0;
		}
		else if((s_flag&GET_INSTRUCTION)==GET_INSTRUCTION)
			proccess_instruction();

		else
			PORTC=s_flag;
		
		PORTC = s_flag;
	}
}
