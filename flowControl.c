#include "flowControl.h"

void change_bit_val(unsigned char position, unsigned char value)
{
    //값을 1로 set 하는 경우
    if(value)
        s_flag|=position;
    //값을 0으로 clear하는 경우
    else
        s_flag&=~(position);
}
