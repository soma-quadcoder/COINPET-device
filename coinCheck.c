#include "coinCheck.h"
#include "blueTooth.h"
#include "flowControl.h"
#include "interaction.h"

void init_adc()
{
    ADMUX   |= (1<<REFS0) | (1<<REFS1) | (1<<MUX0);
    ADCSRA  |= (1<<ADEN) | (1<<ADPS2)|(2<<ADPS1)|(ADPS0)|
    (1<<ADSC) | (1<<ADATE) | (0<<ADIE);
    adc_max = 1024;
}

void proccess_coin()
{
    /* adc 값에 따른 동전 구별
     * 10~30        : 10원
     * 100~200      : 50원
     * 400~470      : 100원
     * 490~550      : 500원
     */
    
    char i,j;
    char length = 3; //지연이랑 저금금액 보낼때 데이터 패킷의 길이는 3으로 고정하기로함
    
    unsigned char   data[3];
    unsigned char   tmp;
    unsigned long   tmp_money;
    unsigned long   tmp_current;
    unsigned long   coin_flag;
    
    _delay_ms(1);
    // 금액 처리
    if( adc_max>800 && adc_max<1000 )
        coin_flag = SHIPWON;
    else if( adc_max>650 && adc_max<800)
        coin_flag = OHSHIPWON;
    else if( adc_max>450 && adc_max<600 )
        coin_flag = BACKWON;
    else if (adc_max>200 && adc_max<400 )
        coin_flag = OHBACKWON;
    else
        UDR0 = 0xed;
    
    current_money += coin_flag; // 현재 저축된 총금액 연산
    tmp_current = current_money;
    
    for( i=0;i<length;i++ )
    {
        // 8비트씩 char 배열에 저장
        data[2-i] = coin_flag & 0xff;
        coin_flag = coin_flag>>8;
        tmp = tmp_current & 0xff;
        
        //eeprom에 현재금액 저장
        eeprom_write((CURRENT_MONEY-2)+i,tmp);
        tmp_current = tmp_current>>8;
    }
    
    // 저금한 금액 전송
    make_packet(SEND_MONEY,length,data);
    
    // s_flag 0으로 클리어
    change_bit_val(INPUT_COIN,0);
    _delay_ms(1);
    write_num_to_oled(current_money);
    led_interaction(10000,1800,400);
}