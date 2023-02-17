/*
 * functions.c
 *
 *  Author: Péter
 */ 

#include "header.h"

uint8_t train_flag, night_flag, dir, tic, halfsec;

void port_init(){	
	DDRA = 0xFF;	
	DDRC = 0xFF;
	DDRF = 0x0F;
	DDRE |= 0xCC;	// PE5 PE4 button
	DDRG = 0x00;
	// button
	PORTE = 0x30;
}

void timer_init(){
	TCCR0 = ((0<<WGM00) | (0<<WGM01) | (1<<COM01) | (0<<COM00) | (0<<CS02) | (1<<CS01) | (0<<CS00) );
	TCCR1B |= (0<<WGM13) | (1<<WGM12) | (0<<WGM11) | (0<<WGM10) | (1<<CS12) | (0<<CS11) | (1<<CS10);
	TCCR1A = TCCR1C = TCNT1 = 0;
	TIMSK |= (1<<OCIE1A) | (1<<TOIE0);
	OCR1A =  7812;	// ~0.5 sec 7812 15625
	sei();
}

void train_button(){
	if (BTNV1_PRESSED()){
		if(train_flag == 0){
			train_flag = 1;
		}
		else if ((train_flag == 1)) {
			train_flag = 0;
		}
	}
	if (BTNV2_PRESSED()){
		if(train_flag == 0){
			train_flag = 2;
		}
		else if ((train_flag == 2)) {
			train_flag = 0;
		}
	}
}

void led_out(uint8_t leds){
	PORTD = leds & 0xF0;
	PORTB = leds << 4 & 0xF0;
}


void day_night_switch()
{
	if (PING == 1){
		PORTA &= ~(PORTA_MASK);
		PORTC &= ~(PORTC_MASK);
		PORTE &= ~(PORTE_MASK);
		night_flag = 1;
		} else if (PING == 2) {
		night_flag = 0;
		tic = 0;
		halfsec = 0;
	}
	else if(PING == 4){
		train_flag = 0;
	}
}