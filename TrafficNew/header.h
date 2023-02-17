/*
 * header.h
 *
 *  Author: Péter
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

#define LA_P	PA0
#define LA_S	PA1
#define LA_Z	PA2

#define LB_P	PA3
#define LB_S	PA4
#define LB_Z	PA5
#define LB_Z2	PA6

#define LD_P	PA7
#define LD_S	PC0
#define LD_Z	PC1

#define LC_P	PC2
#define LC_S	PC3
#define LC_Z	PC4

#define LE_P	PC5
#define LE_S	PC6
#define LE_Z	PC7
#define LE_Z2	PF0

#define LV_P1	PF1
#define LV_P2	PF2
#define LV_F	PF3

#define LG1_P	PE2
#define LG1_Z	PE3

#define LG2_P	PE6
#define LG2_Z	PE7

#define BTNV1	PE4
#define BTNV2	PE5
#define BTNV1_PRESSED()		(!(PINE & (1<<BTNV1)))
#define BTNV2_PRESSED()		(!(PINE & (1<<BTNV2)))

#define PORTA_MASK	0xFF
#define PORTC_MASK	0xFF
#define PORTF_MASK	0x0F
#define PORTE_MASK	0xCC

void timer_init();
void port_init();
void my_delay(uint8_t);
void train_button();
void led_out(uint8_t);
void led_pulsate();
void day_night_switch();