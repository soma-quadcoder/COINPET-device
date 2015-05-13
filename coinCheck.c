#include "coinCheck.h"
#include "blueTooth.h"
#include "flowControl.h"


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
    char length;
    
    unsigned char   data[3];
    unsigned long   tmp_money;
    unsigned long   tmp_current;
    unsigned char   coin_flag;
    
    tmp_money = current_money;
    
    _delay_ms(1);
    // 금액 처리
    if( adc_max>800 && adc_max<1000 )
    {
        coin_flag = 0x00;
        current_money += SHIPWON;
    }
    else if( adc_max>650 && adc_max<800)
    {
        current_money += OHSHIPWON;
        coin_flag = 0x01;
    }
    else if( adc_max>450 && adc_max<600 )
    {
        current_money += BACKWON;
        coin_flag = 0x02;
    }
    else if (adc_max>200 && adc_max<400 )
    {
        current_money += OHBACKWON;
        coin_flag = 0x03;
    }
    else
        UDR0 = 0xed;
    
    length = 3;
    
    tmp_money = current_money-tmp_money;
    tmp_current = current_money;
    for( i=0;i<length;i++ )
    {
        // 8비트씩 char 배열에 저장
        data[2-i] = tmp_money & 0xff;
        tmp_money = tmp_money>>8;
    }
    
    // 저금한 금액 전송
    make_packet(SEND_MONEY,length,data);
    
    
    // s_flag 0으로 클리어
    change_bit_val(INPUT_COIN,0);
    _delay_ms(1);
    write_num_to_oled(current_money);
}