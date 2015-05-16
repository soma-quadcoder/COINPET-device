#include <avr/io.h>
#include <avr/interrupt.h>
#include "bluetooth.h"

#define GAMEBOARD_PORT  0x00
#define SHIP_BTN        0x01
#define OHSHIP_BTN      0x02
#define BACK_BTN        0x04
#define OHBACK_BTN      0x08
#define CONFIRM_BTN     0x10
#define CANCEL_BTN      0x20

#define SHIPWON         0
#define OHSHIPWON       1
#define BACKWON         2
#define OHBACKWON       3

#define SEND_GAMEDATA   'G'

void init_gameboard(void);
void init_arr(unsigned char *data);


void init_gameboard(void)
{
    DDRB = GAMEBOARD_PORT;
    PINB = 0x00;
}

void init_arr(unsigned char *data)
{
    unsigned char i;
    
    //배열초기화
    for( i=0 ; i<4 ; i++ )
        data[i] = 0;
}


int main(void)
{
    unsigned char btn_num = 0;// 어떤 버튼이 클릭되었는지 확인하기위한변수
    unsigned char data[4];    // 버튼 클릭횟수를 저장할 변수

    init_gameboard();
    init_uart();
    init_arr(data);
    sei();
    
    while(1)
    {
        btn_num = PINB & 0x7f; // PB7핀은 사용하지않기때문에 혹시나 PB7핀으로 신호가 들어오면 신호를 무시하기위한 연산
        
        // 7번핀을 클릭할때 값이 튀는 현상이 발생하여 이부분을 처리하기위한 연산부분
        if(btn_num & 0x20)
            btn_num = PINB & 0x20;
    
        switch (btn_num)
        {
            case SHIP_BTN:
                while(1)
                {
                    _delay_ms(150); //채터링 방지용 딜레이
                    if(!(PINB & (1<<SHIP_BTN)))
                    {
                        data[SHIPWON]++;
                        break;
                        
                    }
                }
                break;
                
            case OHSHIP_BTN:
                while(1)
                {
                    _delay_ms(150); //채터링 방지용 딜레이
                    if(!(PINB & (1<<OHSHIP_BTN)))
                    {
                        data[OHSHIPWON]++;
                        break;
                        
                    }
                }
                break;
                
            case BACK_BTN:
                while(1)
                {
                    _delay_ms(150); //채터링 방지용 딜레이
                    if(!(PINB & (1<<BACK_BTN)))
                    {
                        data[BACKWON]++;
                        break;
                        
                    }
                }
                break;
                
            case OHBACK_BTN:
                while(1)
                {
                    _delay_ms(150); //채터링 방지용 딜레이
                    if(!(PINB & (1<<OHBACK_BTN)))
                    {
                        data[OHBACKWON]++;
                        break;
                        
                    }
                }
                break;
                
            case CANCEL_BTN:
                while(1)
                {
                    _delay_ms(150); //채터링 방지용 딜레이
                    if(!(PINB & (1<<CANCEL_BTN)))
                    {
                        init_arr(data);
                        break;
                        
                    }
                }
                break;
                
            case CONFIRM_BTN:
                while(1)
                {
                    _delay_ms(150); //채터링 방지용 딜레이
                    if(!(PINB & (1<<CONFIRM_BTN)))
                    {
                        make_packet(SEND_GAMEDATA,4,data);
                        init_arr(data);
                        break;
                        
                    }
                }
                break;
                
            default:
                break;
        }

    }
    return 0;
}