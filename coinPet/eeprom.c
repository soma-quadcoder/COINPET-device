#include "eeprom.h"
#include "flowControl.h"
#include "coinCheck.h"

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
    
    // s_flag 데이터를 eeprom으로o i터 가져온다
    // 인증여부 & 쌓여있는 데이터유무를 알기위해서
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
    
    // 나중에 변경해야함 -> CONFIRM에서 ISDATA로
    //if((s_flag&CONFIRM)==CONFIRM)
    {
        char i=0;
        unsigned tmp;
        
        for( ; ; )
        {
            tmp = eeprom_read(CURRENT_MONEY-i);
            _delay_ms(1);
            UDR0 = tmp;
            current_money = current_money | tmp;
            if( i==2 )
                break;
            
            i++;
            current_money = current_money<<8;
        }
        
        // eeprom에 데이터가 저장되어있으면 데이터 호출후 버퍼에 저장
        if((s_flag&ISDATA)==ISDATA)
        {
            for( i=0;i<4;i++ )
            {
                data_unconnect[i] = eeprom_read(UNCONNECT_DATA_ADDRESS+i);
                _delay_ms(1);
            }
        }
    }
    _delay_ms(1);
}

void eeprom_write(int ad,unsigned char val)
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
