#include "blueTooth.h"
#include "coinCheck.h"
#include "flowControl.h"

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
        idxArr++;
        UDR0 = data;
    }
}

ISR(USART1_RX_vect)
{
    unsigned char data = UDR1;
    UDR0 = data;
}

void init_uart()
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
    UBRR1L  = 16; // 20Mhz에서 57600은 21 16Mhz에서 57600은 16
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
    
    _delay_ms(2);
    //start send packet
    for( i=0;i<length+4;i++ )
    {
        UDR0 = packet[i];
        _delay_ms(1);
    }
}