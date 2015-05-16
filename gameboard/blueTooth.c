#include "blueTooth.h"


ISR(USART_RX_vect)
{
    UDR = UDR;
}


void init_uart()
{
    /*
     uart 사용
     속도 : 57600
     */
    UCSRB |= (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
    UCSRC |= (1<<UCSZ1)|(1<<UCSZ0);
    UBRRL  = 80; // 12Mhz에서 9600은 80
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
        UDR = packet[i];
        _delay_ms(1);
    }
}