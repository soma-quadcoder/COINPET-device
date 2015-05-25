#include <avr/io.h>
#include <avr/interrupt.h>

void init_motor();
void lock_or_unlock( unsigned char lock );
void shutdown_motor_voltage();
void turnon_motor_voltage();