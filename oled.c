#include <avr/io.h>
#include "oled.h"

unsigned char font_money[6][16] =
{
    {0xFF,0xFF,0xFF,0x07,0x07,0x07,0x07,0x07,0xFF,0xFF,0xFF,0xE0,0xE0,0xE0,0xE0,0xE0}, /*"["*/
    {0x03,0xff,0xff,0xff,0x03,0xC0,0xff,0xff,0x30,0x3F,0x0F,0x3F,0x30,0x01,0x7F,0x7F}, /*"저"*/
    {0x66,0x66,0x7E,0x3f,0x3f,0x7E,0x66,0x66,0x1B,0x1B,0x1B,0x1f,0x1f,0x1B,0xFB,0xFB}, /*"축"*/
    {0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xDF,0xDF,0xFC,0xFC,0xCC,0xCC,0xCC,0xCC,0xFC,0xFC}, /*"금"*/
    {0x7E,0xE7,0xE7,0x7E,0x00,0xFF,0x18,0xFF,0x06,0x06,0x06,0x06,0x06,0x06,0xFE,0xFE}, /*"액"*/
    {0x07,0x07,0x07,0x07,0x07,0xFF,0xFF,0xFF,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0xFF,0xFF} /*"]"*/
};

unsigned char font_num[11][16] =
{
    {0xFE,0xFF,0xFF,0x1F,0x1F,0xFF,0xFF,0xFE,0x7F,0xFF,0xFF,0xF8,0xF8,0xFF,0xFF,0x7F}, /*숫자0*/
    {0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00}, /*숫자1*/
    {0xCF,0xCF,0xCF,0xCF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF3,0xF3,0xF3,0xF3}, /*숫자2*/
    {0xCF,0xCF,0xCF,0xCF,0xFF,0xFF,0xFF,0xFF,0xF3,0xF3,0xF3,0xF3,0xFF,0xFF,0xFF,0xFF}, /*숫자3*/
    {0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x07,0x07,0x07,0x07,0xFF,0xFF,0xFF,0x07}, /*숫자4*/
    {0xFF,0xFF,0xFF,0xFF,0xCF,0xCF,0xCF,0xCF,0xF3,0xF3,0xF3,0xF3,0xFF,0xFF,0xFF,0xFF}, /*숫자5*/
    {0xFF,0xFF,0xFF,0xFF,0xC0,0xC0,0xC0,0xC0,0xFF,0xFF,0xFF,0xE1,0xE1,0xFF,0xFF,0xFF}, /*숫자6*/
    {0x0F,0x0F,0x0F,0x0F,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF}, /*숫자7*/
    {0xFE,0xFF,0xFF,0xC7,0xC7,0xFF,0xFF,0xFE,0x7F,0xFF,0xFF,0xE3,0xE3,0xFF,0xFF,0x7F}, /*숫자8*/
    {0xFF,0xFF,0xFF,0xC7,0xC7,0xFF,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF,0xFF}, /*숫자9*/
    {0x7E,0xE7,0xE7,0x7E,0x00,0x30,0xFF,0xFF,0xE6,0xEE,0xCE,0xCE,0xC6,0xC0,0xCF,0xCF}  /*"원"*/
};

void init_SPI_master(void)
{
    SPI_DDR |= ((1 << SS) | (1 << SCK) | (1 << MOSI) | (1<<DC)|(1<<RST));
    SPI_PORT|= (1 << SS)|(1<<RST);
    SPCR = ((1 << SPE)|(1 << MSTR)|(1 << SPR0)|(0<<SPR1)); // 16M/16=1Mhz

}

void write_oled(unsigned char data)
{
    SPI_PORT &=~(1<<SS);
    SPI_PORT &=~(1<<DC);
    SPDR = data;
    
    while(!(SPSR&(1<<SPIF)));
    SPI_PORT|=(1<<SS);
}

void write_oled_data(unsigned char data)
{
    SPI_PORT &=~(1<<SS);
    SPI_PORT |= (1<<DC);
    SPDR = data;
    
    while(!(SPSR&(1<<SPIF)));
    SPI_PORT|=(1<<SS);
    SPI_PORT &=~(1<<DC);
}

void clear_oled(void)
{
    unsigned char i, j;
    
    for (i = 0; i < 8; i ++)
    {
        write_oled(CHANGE_PAGE+i);
        for (j = 0; j < 128; j ++)
        {
            write_oled_data(0x00);
            _delay_us(100);
        }
    }   
}

void draw_char(int w, int h, int x, int y, char *data)
{
	/*
	 * w : 그려질 글자의 width
	 * h : 그려질 글자의 height
	 * x : 그려질 글자의 시작 x 좌표 (0~128)
	 * y : 그려질 글자의 시작 y 좌표 (0~7)
	 * data : 그려질 데이터 배열
	 */
	
	unsigned char i,j;
	unsigned char idx	= 0 ;
	unsigned height		= h/8; 	    //OLED는 한 페이지당 8픽셀을 차지
	unsigned startY		= CHANGE_PAGE + y; //0xB0은 OLED의 어떤 페이지를 선택하는지에대한 명령어이다
	unsigned startX         = x+5;	    //+1을하는 이유는 가독성을 높이기 위해서이다(이전글자로부터1픽셀 띄어서 )
	unsigned endX		= startX + w;
	for( i=0;i<height;i++ )
	{
		write_oled(startY+i);
		write_oled(CHANGE_COLUM);
		write_oled(startX);
		write_oled(endX);

		for( j=0;j<w;j++ )
		{
			write_oled_data(data[idx++]);
			_delay_us(100);
		}
	}
}

void draw_data(int w, int h, int x, int y, char data)
{
	/*
	 * w : 그려질 글자의 width
	 * h : 그려질 글자의 height
	 * x : 그려질 글자의 시작 x 좌표 (0~128)
	 * y : 그려질 글자의 시작 y 좌표 (0~7)
	 * data : 그려질 데이터
	 */
	
	unsigned char i,j;
	unsigned height		= h/8; 	    //OLED는 한 페이지당 8픽셀을 차지
	unsigned startY		= CHANGE_PAGE + y; //0xB0은 OLED의 어떤 페이지를 선택하는지에대한 명령어이다
	unsigned startX         = x;	    //+1을하는 이유는 가독성을 높이기 위해서이다(이전글자로부터1픽셀 띄어서 )
	unsigned endX		= startX + w;
	for( i=0;i<height;i++ )
	{
		write_oled(startY+i);
		write_oled(CHANGE_COLUM);
		write_oled(startX);
		write_oled(endX-1);
        
		for(j=0;j<w;j++)
		{	
			write_oled_data(data);
			_delay_us(100);
		}
	}
}

void write_num_to_oled(unsigned long current_money)
{
	unsigned char curr_coin[10];
	unsigned char position 	= 0;
	unsigned long cu_co	= 0;
		
	unsigned char i = 0;
	unsigned startX = 0;

	cu_co = current_money;
	while(1)
	{
		curr_coin[position++] = cu_co%10;
		cu_co = cu_co/10;

		if(cu_co == 0)
			break;
	}

	startX = (100 - (position * 8))/2 ; // 출력할 숫자를 가운데 정렬하기위한 

	for( position = position-1 ; position > 0 ; position-- )
   		draw_char(8,16,startX+(i++*10),5,font_num[curr_coin[position]]);
	
   	draw_char(8,16,startX+(i++*10),5,font_num[curr_coin[0]]);
   	draw_char(8,16,startX+(i++*10),5,font_num[10]); //글자 "원"출력
}

void init_oled(void)
{
    init_SPI_master(); //init SPI
    
    write_oled(0xAE); //display off
    write_oled(0x20); //Set Memory Addressing Mode
    write_oled(0x02); //00);Horizontal Addressing Mode;01);Vertical Addressing Mode;10);Page Addressing Mode (RESET);11);Invalid
    write_oled(CHANGE_PAGE); //Set Page Start Address for Page Addressing Mode);0-7
    write_oled(0xc8); //Set COM Output Scan Direction
    write_oled(0x00); //---set low column address
    write_oled(0x10); //---set high column address
    write_oled(0x40); //--set start line address
    write_oled(0x81); //--set contrast control register
    write_oled(0x7f);
    write_oled(0xa1); //--set segment re-map 0 to 127
    write_oled(0xa8); //--set multiplex ratio(1 to 64)
    write_oled(0x3F);
    write_oled(0xa4); //0xa4);Output follows RAM content;0xa5);Output ignores RAM content
    write_oled(0xd3); //-set display offset
    write_oled(0x00); //-not offset
    write_oled(0xd5); //--set display clock divide ratio/oscillator frequency
    write_oled(0xf0); //--set divide ratio
    write_oled(0xd9); //--set pre-charge period
    write_oled(0x22); //
    write_oled(0xda); //--set com pins hardware configuration
    write_oled(0x12);
    write_oled(0xdb); //--set vcomh
    write_oled(0x20); //0x20);0.77xVcc
    write_oled(0x8d); //--set DC-DC enable
    write_oled(0x14); //
    write_oled(0xa6); //--set normal display  // a7 - inverse
    write_oled(0xaf); //--turn on oled panel
    
    clear_oled(); // 화면 클리어
}
