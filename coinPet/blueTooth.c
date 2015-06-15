#include "blueTooth.h"
#include "coinCheck.h"
#include "flowControl.h"
#include "eeprom.h"
#include "oled.h"
#include "motor.h"
#include "Interaction.h"


unsigned char temp_pn[17]="1234123412341234";
unsigned long game_money = 0;
unsigned isConnect;

ISR(USART0_RX_vect)
{
    unsigned char data = UDR0;
    

    if(data==START_BYTE && idxArr==0)
        isStart = 1;
    
    else if(data==END_BYTE && idxArr!=0)
    {
        //s_flag Get_INSTRUCTION bit 1 set
        change_bit_val(GET_INSTRUCTION,1);
        idxArr  = INIT_DATA;
        isStart = INIT_DATA;

    }
    
    if(isStart && data!=START_BYTE)
    {
        data_buffer[idxArr] = data;
        
        // 받은 명령어가 ack 신호 일경우
        if(data==GET_ACK && idxArr==0)
            change_bit_val(CONNECT,1);

        idxArr++;
    }

}

ISR(USART1_RX_vect)
{
    unsigned char data = UDR1;
    
    if(data==START_BYTE && idxArr==0)
        isStart = 1;
    
    else if(data==END_BYTE && idxArr!=0)
    {
        //s_flag Get_INSTRUCTION bit 1 set
        change_bit_val(GET_INSTRUCTION,1);
        idxArr  = INIT_DATA;
        isStart = INIT_DATA;
    }
    
    if(isStart && data!=START_BYTE)
    {
        data_buffer[idxArr] = data;
        idxArr++;
    }
    
    // 게임보드와 연결상태를 알기위한 변수
    isConnect = 1;
}

unsigned char check_gameboard_connect()
{
    unsigned temp = isConnect;
    // 현재 게임보드 연결상태를 위한 변수
    
    isConnect = INIT_DATA;
    
    return temp;
}

void init_uart(void)
{
    /*
     uart0 사용
     속도 : 57600
     */
    UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    UBRR0L  = 21; // 20Mhz에서 57600은 21 16Mhz에서 57600은 16
                   // 9600은 129
    
    /*
     uart1 사용
     속도 : 57600
     */
    UCSR1B |= (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);
    UCSR1C |= (1<<UCSZ11)|(1<<UCSZ10);
    UBRR1L  = 129; // 20Mhz에서 57600은 21 16Mhz에서 57600은 16
                  // 9600은 129
}

void make_packet(unsigned char opcode, char length, unsigned char data[])
{
    //송신 패킷데이터를 만들어주는 함수
    unsigned char packet[MAX_INDEX];
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
    
    //start send packet
    for( i=0;i<length+4;i++ )
    {
        while(!(UCSR0A&(1<<UDRE0)));
        UDR0 = packet[i];
    }
    while(!(UCSR0A&(1<<UDRE0)));
}

char compare_pn(unsigned char idx)
{
    unsigned char length;
    char i;
    char isMatch;
    
    length = data_buffer[idx++];
    
    for( i=0;i<length;i++ )
    {
        if(temp_pn[i]==data_buffer[idx++])
            isMatch = 1;
        else
        {
            isMatch = 0;
            break;
        }
    }
    return isMatch;
}

void get_goal( unsigned char idx )
{
    unsigned long tmp;
    unsigned char len = data_buffer[idx++];
    unsigned char i;
    unsigned char percent;
    
    goal_money = INIT_DATA;
    
    for( i=0;i<len;i++ )
    {
        tmp = data_buffer[idx++]&0xff;
        _delay_ms(2);
        
        goal_money = goal_money|tmp;
        eeprom_write(GOALMONEY_ADDRESS-i,(unsigned char)tmp);
        if( i < len-1 )
            goal_money = goal_money<<8;
    }
    
    // 목표금액 등록문구 출력
    for(i=0;i<6;i++)
        draw_char(8,16,30+(i*10),2,font_goal[i]);
    
    write_num_to_oled(goal_money);
    _delay_ms(3000);
    
    // 현재금액에 따라 저축금액 퍼센테이지 출력
    percent = (current_money*112)/goal_money;
    
    draw_edge(1);
    draw_percentage(percent);
    
    for(i=0;i<6;i++)
        draw_char(8,16,30+(i*10),2,font_money[i]);
    
    write_num_to_oled(current_money);
    change_bit_val(ISGOAL,1);	// s_flag ISGOAL bit 1로 set
}

void proccess_instruction(void)
{
    unsigned char idx = 0;
    unsigned char d[3];
    unsigned char data[6];
    unsigned char tmp;
    unsigned char add = UNCONNECT_COIN_DATA_ADD;
    
    int num ;
    int j = 1;
    
    int m=0;
    int len ;
    int k;
    
    _delay_ms(1);
    switch(data_buffer[idx++])
    {
        case REQUEST_PN:
            if(!(s_flag&CONFIRM))
            {
                // PN 등록요청이오면 PN번호를 비교한다
                _delay_ms(10);
                if(compare_pn(idx++))
                {
                    change_bit_val(CONFIRM,s_flag-64);	// s_flag CONFIRM bit 1로 set
                    eeprom_write(S_FLAG_ADDRESS, 0x80);	// eeprom에 인증여부 저장
                    
                    //성공 문구 출력
                    for(k=0;k<2;k++)
                        draw_char(8,16,45+(k*10),5,font_success[k]);
                    
                    _delay_ms(5000);
                    
                    //앞에 그렸던 문구 클리어
                    draw_data(30, 16, 45,5,0x00);
                    make_packet(RESPONSE_PN,1,SUCCESS_PN);
                }
                
                else
                {
                    //실패 문구 출력
                    for(k=0;k<2;k++)
                        draw_char(8,16,45+(k*10),5,font_fail[k]);
                    
                    _delay_ms(5000);
                    
                    //앞에 그렸던 문구 클리어
                    draw_data(30, 16, 45,5,0x00);
                    make_packet(RESPONSE_PN,1,FAIL_PN);
                }
            }
            break;
            
        case START_GAME:
            //이전화면 모두 클리어
            clear_oled();
            draw_edge(0);
            
            for(k=0;k<6;k++)
                draw_char(8,16,30+(k*10),2,font_game[k]);
            change_bit_val(ISGAME,1);	// s_flag ISGAME bit 1로 set
            break;
        
        case SEND_GAMEDATA:
            game_money = 0;
            len = data_buffer[idx++];
            game_money += data_buffer[idx++]*500;
            game_money += data_buffer[idx++]*100;
            game_money += data_buffer[idx++]*50;
            game_money += data_buffer[idx++]*10;
            write_num_to_oled(game_money);
            break;
            
        case REQUEST_SAVED_COIN:
            saved_coin_cnt = eeprom_read(UNCONNECT_COIN_CNT_ADD);
            for(m=0;m<saved_coin_cnt;m++)
            {
                for(k=0;k<6;k++)
                    data[k] = eeprom_read(add++);
                make_packet(SYNC_DATA,6,data);
            }
            eeprom_write(UNCONNECT_COIN_CNT_ADD,INIT_DATA);
            
            // 블루투스 미연결때 저장되어있던 동전갯수를 eeprom으로 부터 얻는다
            saved_coin_cnt  = eeprom_read(UNCONNECT_COIN_CNT_ADD);
            last_coin_add   = UNCONNECT_COIN_DATA_ADD + (saved_coin_cnt * 6);
            break;
            
        case SUMMIT_DATA:
            for(m=0;m<3;m++ )
            {
                // 8비트씩 char 배열에 저장
                d[2-m] = game_money & 0xff;
                game_money = game_money>>8;
            }
            
            // 게임보드에서 전송 버튼을 클릭했을경우 전송
            make_packet(SUMMIT_DATA,3,d);
            led_interaction(BLUE_LED,350,1000,10);
            break;
            
            // 잠금or 해제명령어일경우
        case LOCK_UNLOCK:
            turnon_motor_voltage();
            lock_or_unlock(data_buffer[++idx]);

            //성공적으로 데이터 수신했음을 앱에게 알림
            led_interaction(GREEN_LED,350,1000,10);
            make_packet(ACK,1,SUCCESS_ACK);
            break;
            
        case GET_UTC_TIME:
            m   = 0;
            tmp = 0;
            
            len = data_buffer[idx++];
            for(m=0;m<len;m++)
            {
                data[m] = 0;
                tmp = data_buffer[idx++];
                data[m] |= (tmp/10)<<4;
                data[m] |= (tmp%10);
            }
            
            set_rtc(data[4],data[3],data[2],data[1],data[0]);
            // 사용자로부터 년월일시분값을 받아서 RTC에 셋한다.
            
            //성공적으로 데이터 수신했음을 앱에게 알림
            make_packet(ACK,1,SUCCESS_ACK);

            break;
            
            // 목표데이터를 전송받을경우
        case GET_GOAL:
            get_goal(idx++);
            turnon_motor_voltage();
            lock_or_unlock(data_buffer[++idx]);
            
            //성공적으로 데이터 수신했음을 앱에게 알림
            led_interaction(GREEN_LED,350,1000,10);
            make_packet(ACK,1,SUCCESS_ACK);
            break;
            
        case REQUEST_GAMECONNECT:
            if(s_flag&ISGAME)
                make_packet(SEND_ISGAME,1,CONNECTTING);
            else
                make_packet(SEND_ISGAME,1,NON_CONNECTTING);

            break;
            
        default:
            break;
    }
    change_bit_val(GET_INSTRUCTION,0); // 명령어 처리후 GET_INSTRUCTION 0 클리어
}
