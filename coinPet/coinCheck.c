#include "coinCheck.h"
#include "blueTooth.h"
#include "flowControl.h"
#include "interaction.h"
#include "eeprom.h"
#include "i2c.h"

void init_adc()
{
    ADMUX   |= (1<<REFS0) | (1<<REFS1) | (1<<MUX0);
    ADCSRA  |= (1<<ADEN) | (1<<ADPS2)|(2<<ADPS1)|(ADPS0)|
    (1<<ADSC) | (1<<ADATE) | (0<<ADIE);
    
    DDRB &= ~(1<<COINDETEC_PIN);
    
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
    UDR0 = 0xee;
    char i,j;
    char length = 3; //지연이랑 저금금액 보낼때 데이터 패킷의 길이는 3으로 고정하기로함
    
    unsigned char   data[3];
    unsigned char   tmp;
    unsigned char   tpm;
    unsigned long   tmp_money;
    unsigned long   tmp_current;
    unsigned long   coin_flag;
    unsigned char percent;
    
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
        coin_flag = 0;
    
    tpm = coin_flag/10;
    
    
    // 제대로 인식되었을경우 led 인터렉션
    if(coin_flag!=0)
    {
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
        
        // 저금한 금액 전송 시도
        make_packet(SEND_MONEY,length,data);
        
        i=0;
        
        while(1)
        {
            if(s_flag&CONNECT)
                break;
            
            _delay_ms(50);  // 전송성공 ack 대기
            if(i>4)         // 전송성공 ack를 200ms 동안 대기
                break;
            i++;
        }
        
        // 블루투스 미연결 상태일경우 정보 eeprom에 저장
        if(!(s_flag&CONNECT))
        {
            saved_coin_cnt++;
            eeprom_write(UNCONNECT_COIN_CNT_ADD,saved_coin_cnt);
            // 블루 투스 미연결시 데이터 저장
            eeprom_write(last_coin_add++,i2c_read(DS1307_ADD,0x06)); // 년
            _delay_ms(1);
            eeprom_write(last_coin_add++,i2c_read(DS1307_ADD,0x05)); // 월
            _delay_ms(1);
            eeprom_write(last_coin_add++,i2c_read(DS1307_ADD,0x04)); // 일
            _delay_ms(1);
            eeprom_write(last_coin_add++,i2c_read(DS1307_ADD,0x02)); // 시
            _delay_ms(1);
            eeprom_write(last_coin_add++,i2c_read(DS1307_ADD,0x01)); // 분
            _delay_ms(1);
            eeprom_write(last_coin_add++,i2c_read(DS1307_ADD,0x00)); // 초
            _delay_ms(1);
            eeprom_write(last_coin_add++,tpm); // 돈
            
            change_bit_val( ISDATA, 1 );
            eeprom_write( S_FLAG_ADDRESS, s_flag);
            _delay_ms(1);
        }
        
        percent = (current_money*112)/goal_money;
        
        UDR0 = percent;
        draw_percentage(percent);
        write_num_to_oled(current_money);
        led_interaction(350,1200,10);
    }
    
    // s_flag 0으로 클리어
    change_bit_val(INPUT_COIN,0);
    change_bit_val(CONNECT,0);
    _delay_ms(1);
}