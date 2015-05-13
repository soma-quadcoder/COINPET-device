#include <avr/io.h>
#include <avr/interrupt.h>
#include "oled.h"
#include "coinCheck.h"
#include "flowControl.h"


int main(void)
{
    init_oled();
    
    int k = 0;
    for(k=0;k<6;k++)
        draw_char(8,16,30+(k*10),2,font_money[k]);
    
    write_num_to_oled(0);
    draw_data(128,8,0,0,0x0f);
    draw_data(4, 64,0,0,0xff);
    draw_data(4, 64,124,0,0xff);
    draw_data(120,8,4,8,0xf0);
    
    sei();
    DDRB = 0xff;
    DDRC&=0x00;
    init_uart();
    init_adc();
    PORTB &=0xf7;

    
    
    while(1)
    {
        if((s_flag&INPUT_COIN)==INPUT_COIN)
        {
            proccess_coin();
            adc_max=1024;
        }
        
        else if(!(PINC & 0x02))
        {
            _delay_ms(25);
            while(1)
            {
                _delay_us(200);
                if(ADC < adc_max)
                    adc_max = ADC;

                else
                    PORTB = 0xff;
                
                if(PINC & 0x02)
                {
                    change_bit_val(INPUT_COIN,1);
                    break;
                }
                else
                    PORTB = 0xff;
            }
        }

    }
    return 0;
}