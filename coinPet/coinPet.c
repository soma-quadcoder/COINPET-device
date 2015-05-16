#include <avr/io.h>
#include <avr/interrupt.h>
#include "oled.h"
#include "coinCheck.h"
#include "flowControl.h"
#include "interaction.h"

int main(void)
{
    init_oled();
    init_uart();
    init_adc();
    init_interaction();
    get_eeprom_data();  // EEPROM으로 부터 데이터 획득
    // s_flag => 인증여부, 쌓여있는 데이터 유무
    
    int k = 0;
    for(k=0;k<6;k++)
        draw_char(8,16,30+(k*10),2,font_money[k]);
    
    draw_data(128,8,0,0,0x0f);
    draw_data(4, 64,0,0,0xff);
    draw_data(4, 64,124,0,0xff);
    draw_data(120,8,4,8,0xf0);
    
    write_num_to_oled(current_money);
    
    sei();
    DDRB = 0xff;
    DDRC&=0x00;
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