#include <avr/io.h>
#include "oled.h"


int main(void)
{
    init_oled();
    
    int k = 0;
    for(k=0;k<6;k++)
        draw_char(8,16,30+(k*10),2,font_money[k]);
    
    write_num_to_oled(230000);
    draw_data(128,8,0,0,0x0f);
    draw_data(4, 64,0,0,0xff);
    draw_data(4, 64,124,0,0xff);
    draw_data(120,8,4,8,0xf0);
    
    while(1)
    {
    
    }
    return 0;
}