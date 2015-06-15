#include "motor.h"

ISR(TIMER0_COMPA_vect)
{
    PORTD |= 0x10;
}

ISR(TIMER0_OVF_vect)
{
    static char cnt;
    
    cnt++;
    PORTD &= ~0x10;
    TCNT0  = 0x00;
    
    // 최소 모터 동작 주기후 모터 전원해제
    if(cnt==70)
    {
        shutdown_motor_voltage();
        cnt = 0;
    }
}

void init_motor()
{
    TCCR0A = (1<<WGM01)|(1<<WGM00);
    TCCR0B = (1<<CS02)|(1<<CS00);
    TIMSK0 = (1<<OCIE0A)|(1<<TOIE0);
    
    // 잠금 해제
    OCR0A  = 226;
}

void lock_or_unlock( unsigned char lock )
{
    // 잠금 명령일경우
    if(lock == 'l')
        OCR0A = 244;
    // 잠금 해제 명령일경우
    else
        OCR0A = 226;
}

void shutdown_motor_voltage()
{
    // pwm 발생 인터럽트 해제
    TIMSK0 = (0<<OCIE0A)|(0<<TOIE0);
    PORTD &=~0x10;
}

void turnon_motor_voltage()
{
    // pwm 발생 인터럽트 설정
    TIMSK0 |= (1<<OCIE0A)|(1<<TOIE0);
    PORTD  |= 0x10;
}