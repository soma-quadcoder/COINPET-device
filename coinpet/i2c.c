#include "i2c.h"

void init_i2c()
{
    TWCR |= 1<<TWEN; // TWI 사용 ON
    TWSR  = 0x00;    // TWI 분주비 1로 set
    TWBR |= 0x12;    // 전송 속도 144
}

void i2c_write(unsigned char dev_add, unsigned char reg_add,unsigned char data)
{
    /*
     * dev_add : 슬레이브 디바이스 어드레스
     * reg_add : 디바이스 레지스트 어드레스
     * data    : 슬레이브 레지스트에 셋할 데이터
     */
    
    TWCR |= 1<<TWINT | 1<< TWSTA | 1<<TWEN;  // start 비트
    while (!(TWCR&0x80));           // START 처리가 완료됨을 대기
    
    TWDR = dev_add<<1;              // 슬레이브 디바이스 어드레스설정 : 최하단 비트 0 : write
    TWCR = 1<<TWINT| 1<<TWEN;
    while (!(TWCR&0x80));
    
    TWDR = reg_add;                 // 쓰고자할 레지스터 주소 설정
    TWCR = 1<<TWINT| 1<<TWEN;
    while (!(TWCR&0x80));
    
    TWDR = data;   // 전송 할  데이터
    TWCR = 1<<TWINT| 1<<TWEN;
    while (!(TWCR&0x80));
    
    TWCR = 1<<TWINT | 1<< TWSTO| 1<<TWEN;  //stop 비트
}
unsigned char i2c_read(unsigned char dev_add, unsigned char reg_add)
{
    /*
     * dev_add : 슬레이브 디바이스 어드레스
     * reg_add : 디바이스 레지스트 어드레스
    */
    
    char data=0;
    
    TWCR |= 1<<TWINT | 1<< TWSTA | 1<<TWEN;   // start 비트
    while(!(TWCR&0x80));            // TWINT 가 1값으로 클리어 될때까지 기다림
    
    TWDR = dev_add<<1;              // 슬레이브 디바이스 어드레스설정 : 최하단 비트 0 : write
    TWCR = 1<<TWINT| 1<<TWEN;
    while(!(TWCR&0x80));
    
    TWDR = reg_add;                 // 읽고자 할 레지스터 주소를 셋
    TWCR = 1<<TWINT| 1<<TWEN;
    while(!(TWCR&0x80));
    
    TWCR |= 1<<TWINT | 1<< TWSTA| 1<<TWEN;
    while(!(TWCR&0x80));
    
    TWDR = dev_add<<1 | 0x01;       // 데이터를 읽겠다는 주소 셋
    TWCR = 1<<TWINT| 1<<TWEN;
    while(!(TWCR&0x80));
    
    TWCR = 1<<TWINT| 1<<TWEN;
    while (!(TWCR&0x80));
    
    data=TWDR;
    TWCR = 1<<TWINT | 1<< TWSTO| 1<<TWEN;  //stop 비트
    
    return get_time_vale(data);
}

void set_rtc(unsigned char sec,unsigned char min,unsigned char hour,unsigned char date,
              unsigned char month,unsigned char year)
{
    i2c_write( DS1307_ADD ,SEC,     sec );  // 초
    i2c_write( DS1307_ADD ,MINUTE,  min );  // 분
    i2c_write( DS1307_ADD ,HOUR,    hour );  // 시간
    i2c_write( DS1307_ADD ,DATE,    date );  // 일
    i2c_write( DS1307_ADD ,MONTH,   month);   // 월
    i2c_write( DS1307_ADD ,YEAR,    year );  // 년
}


unsigned char get_time_vale(unsigned char data)
{
    /*
     * RTC에 맞는 규격 데이터를 실제 숫자 데이터로 변환하는 함수
     * data :  RTC에서 받아온 데이터 형식
     * 데이터 형식
     * 7 6 5 4       3 2 1 0
     * -10YEAR- -    YEAR -
       x x x M(10)   M O N TH
    */
    
    unsigned char real_data;
    char upper_num;
    char lower_num;
    
    lower_num = data & 0x0f;   // 하단부 데이터
    upper_num = (data>>4 &0x0f)*10; // 상단부 데이터
    real_data = upper_num + lower_num;
    
    return real_data;
}