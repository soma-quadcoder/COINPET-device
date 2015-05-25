#include "blueTooth.h"
#include "coinCheck.h"
#include "flowControl.h"
#include "eeprom.h"
#include "oled.h"
#include "motor.h"


unsigned char temp_pn[17]="1234123412341234";

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
        UDR0 = data;
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
        UDR0 = data;
    }
}

void init_uart(void)
{
    /*
     uart0 사용
     속도 : 57600
     */
    UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    UBRR0L  = 129; // 20Mhz에서 57600은 21 16Mhz에서 57600은 16
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
        UDR0 = packet[i];
        _delay_ms(1);
    }
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
    
    for( i=0;i<len;i++ )
    {
        tmp = data_buffer[idx++]&0xff;
        _delay_ms(2);
        
        goal_money = goal_money|tmp;
        eeprom_write(GOALMONEY_ADDRESS-i,(unsigned char)tmp);
        if( i < len-1 )
            goal_money = goal_money<<8;
    }
}

void proccess_instruction(void)
{
    unsigned char idx = 0;
    unsigned char d[4];
    unsigned char data[7];
    unsigned char add = UNCONNECT_COIN_DATA_ADD;
    int num ;
    int j = 1;
    
    int m=0;
    int len ;
    int k;
    
    
    switch(data_buffer[idx++])
    {
        case REQUEST_PN:
           	// PN 등록요청이오면 PN번호를 비교한다
            if(compare_pn(idx++))
           	{
                make_packet(RESPONSE_PN,1,SUCCESS_PN);
                change_bit_val(CONFIRM,s_flag-64);	// s_flag CONFIRM bit 1로 set
                eeprom_write(S_FLAG_ADDRESS, 0x80);	// eeprom에 인증여부 저장
                
                //성공 문구 출력
                for(k=0;k<2;k++)
                    draw_char(8,16,45+(k*10),5,font_success[k]);
                
                _delay_ms(5000);
                
                //앞에 그렸던 문구 클리어
                draw_data(30, 16, 45,5,0x00);
            }
            
            else
            {
                make_packet(RESPONSE_PN,1,FAIL_PN);
                //실패 문구 출력
                for(k=0;k<2;k++)
                    draw_char(8,16,45+(k*10),5,font_fail[k]);
                
                _delay_ms(5000);
                
                //앞에 그렸던 문구 클리어
                draw_data(30, 16, 45,5,0x00);
            }
            break;
        case START_GAME:
            // 이전에 그려져있던 그림 클리어
            draw_data(119,44,5,2,0x00);
            
            for(k=0;k<6;k++)
                draw_char(8,16,30+(k*10),2,font_game[k]);
            change_bit_val(ISGAME,1);	// s_flag ISGAME bit 1로 set
            break;
        
        case SEND_GAMEDATA:
            len = data_buffer[idx++];
            for(m=0;m<len ; m++)
            {
                num = data_buffer[idx++];
                draw_char(8,16,(j),5,font_num[m+1]);
                j+=8;
                draw_char(8,16,(j),5,font_num[11]);
                j+=8;
                draw_char(8,16,(j),5,font_num[num]);
                j+=14;
            }
            break;
            
        case REQUEST_SAVED_COIN:
            saved_coin_cnt = eeprom_read(UNCONNECT_COIN_CNT_ADD);
            for(m=0;m<saved_coin_cnt;m++)
            {
                for(k=0;k<7;k++)
                {
                    data[k] = eeprom_read(add++);
                    UDR0 = data[k];
                }
                make_packet(SYNC_DATA,7,data);
            }
            eeprom_write(UNCONNECT_COIN_CNT_ADD,INIT_DATA);
            
            // 블루투스 미연결때 저장되어있던 동전갯수를 eeprom으로 부터 얻는다
            saved_coin_cnt  = eeprom_read(UNCONNECT_COIN_CNT_ADD);
            last_coin_add   = UNCONNECT_COIN_DATA_ADD + (saved_coin_cnt * 7);
            break;
            
        case SUMMIT_DATA:
            
            len = data_buffer[idx++];
            
            for(m=0;m<len;m++)
                d[m] = data_buffer[idx++];
            
            // 게임보드에서 전송 버튼을 클릭했을경우 전송
            make_packet(SUMMIT_DATA, len, d);
            break;
            
            // 잠금or 해제명령어일경우
        case LOCK_UNLOCK:
            turnon_motor_voltage();
            lock_or_unlock(data_buffer[++idx]);
            break;
            
            // 목표데이터를 전송받을경우
        case GET_GOAL:
            get_goal(idx++);
            break;
            
        default:
            break;
    }
    change_bit_val(GET_INSTRUCTION,0); // 명령어 처리후 GET_INSTRUCTION 0 클리어
}