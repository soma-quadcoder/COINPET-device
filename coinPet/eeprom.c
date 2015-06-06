#include "eeprom.h"
#include "flowControl.h"
#include "coinCheck.h"

void get_eeprom_data()
{
    unsigned long tmp;
    unsigned char i;
    
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
    //eeprom_write(S_FLAG_ADDRESS,0x00);
    
    // 블루투스 미연결때 저장되어있던 동전갯수를 eeprom으로 부터 얻는다
    saved_coin_cnt  = eeprom_read(UNCONNECT_COIN_CNT_ADD);
    last_coin_add   = UNCONNECT_COIN_DATA_ADD + (saved_coin_cnt * 6);
    
    // eeprom에 저장되어있는 목표금액을 가져온다
    for( i=0;i<3;i++ )
    {
        tmp = eeprom_read(GOALMONEY_ADDRESS-i)&0xff;
        goal_money = goal_money|tmp;
        _delay_ms(2);
        if( i < 2 )
            goal_money = goal_money<<8;
    }

    //eeprom에서 획득한 s_flag의 데이터가 0xff라는 의미는
    //default 값이라는 의미이기 때문에 초기화 해준다.
    //if(s_flag == DEFAULT_DATA)
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
    if((s_flag&CONFIRM)==CONFIRM)
    {
        char i=0;
        unsigned tmp;
        
        for( ; ; )
        {
            tmp = eeprom_read(CURRENT_MONEY-i);
            _delay_ms(1);

            current_money = current_money | tmp;
            if( i==2 )
                break;
            
            i++;
            current_money = current_money<<8;
        }
    }
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
