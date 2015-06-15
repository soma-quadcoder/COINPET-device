#include <avr/io.h>
#include <avr/interrupt.h>
#include "oled.h"
#include "coinCheck.h"
#include "flowControl.h"
#include "interaction.h"
#include "motor.h"
#include "i2c.h"
#include "bluetooth.h"

#define GAMEBOARD_DETEC_PIN     PA0


int main(void)
{
    unsigned char percent;
    int k = 0;
    
    _delay_ms(500);
    
    init_oled();
    init_uart();
    init_adc();
    init_interaction();
    init_i2c();
    init_motor();
    get_eeprom_data();  // EEPROM으로 부터 데이터 획득
    
    
    sei();
    DDRA &= ~(1<<GAMEBOARD_DETEC_PIN);
    PORTB &=0xf7;
    PINB = 0x00;
    PINA = 0x00;
    
    draw_edge(0);
    

    // P/N 인증 과정
    while(1)
    {
        //P/N 미인증 상태일경우
        if((s_flag&CONFIRM)!=CONFIRM)
        {
            // 제품등록 문구 출력
            for(k=0;k<6;k++)
                draw_char(8,16,30+(k*10),2,font_register[k]);
            
            //인증완료될때까지 대기
            while(!(s_flag&CONFIRM))
            {
                _delay_ms(2);
                if(s_flag&GET_INSTRUCTION)
                    proccess_instruction();
            }
            break;
        }
        
        //P/N 인증 상태일경우
        else break;
    }
    
    for(k=0;k<6;k++)
        draw_char(8,16,30+(k*10),2,font_money[k]);
    
    write_num_to_oled(current_money);
    
    
    if(s_flag&ISGOAL)
    {
        draw_edge(1);
        percent = (current_money*112)/goal_money;
        draw_percentage(percent);
    }

    while(1)
    {
        if(s_flag&INPUT_COIN)
        {
            proccess_coin();
            adc_max=1024;
        }
        
        // 명령어가 전송되었을경우
        else if(s_flag&GET_INSTRUCTION)
            proccess_instruction();

        // 동전인식이 시작되었을경우
        else if(!(PINB & 0x01))
        {
            while(1)
            {
               
                _delay_us(10);
                if(ADC < adc_max)
                    adc_max = ADC;

                else
                    _delay_us(1);
                
                // 동전인식이 종료 & 처리
                if(PINB & 0x01)
                {
                    _delay_us(10);
                    if(adc_max<1000)
                    {
                        change_bit_val(INPUT_COIN,1);
                        break;
                    }
                    else
                        break;
                }
                else
                    _delay_us(1);
            }
        }
        
        // 게임보드와 연결되었던 상태일경우
        else if(s_flag&ISGAME)
        {
            unsigned isconnected;
            
            // 게임보드 연결확인 패킷 전송
            UDR1 = ISCONNECTED;
            while(!(UCSR1A&(1<<UDRE1)));
            
            _delay_ms(5);

            if(!check_gameboard_connect())
            {

                // 이전에 그려져있던 그림 클리어
                draw_data(119,44,5,2,0x00);
                if(s_flag&ISGOAL)
                {
                    draw_edge(1);
                    percent = (current_money*112)/goal_money;
                    draw_percentage(percent);
                }
                
                for(k=0;k<6;k++)
                    draw_char(8,16,30+(k*10),2,font_money[k]);
                
                write_num_to_oled(current_money);
                change_bit_val(ISGAME,0);
                make_packet(DISCONNECT,1,"d");
            }
        }
    }
    return 0;
}
