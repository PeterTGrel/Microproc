/*
 * main.c
 * Author : Péter
 *
 * controls the directions (LEDs) and functions (buttons) on the TBird-Traffic board
 */ 

#include <avr/io.h>
#include "header.h"		

// ABCDE - lanes, ryg - colors
uint8_t states_a[32] = 
{
//	ABDr	Byr		Bg		Bg		Bg		Bg		Ar-By	Ar-By	Ar-By	ABDr	ABDr	ABDr	ABDr	ABDr
	0x89,	0x99,	0x61,	0x61,	0x61,	0x61,	0x11,	0x11,	0x11,	0x89,	0x89,	0x89,	0x89,	0x89,
//															Ayr		Ag		Ag		Ag		Ag		Ay		Ay
	0x89,	0x89,	0x89,	0x89,	0x89,	0x89,	0x89,	0x8B,	0x8C,	0x8C,	0x8C,	0x8C,	0x8A,	0x8A,
//	Ay
	0x8A,	0x89,	0x89,	0x89
};

uint8_t states_c[32] =
{
//	DECr	Dyr-ECr	Dg-ECr	Dg-ECr	Dg-ECr	Dg-ECr	Dy-ECr	Dy-ECr	Dy-ECr	DECr	DECr	EyrCyr	ECg		ECg	
	0x24,	0x25,	0x26,	0x26,	0x26,	0x26,	0x25,	0x25,	0x25,	0x24,	0x24,	0x6C,	0x90,	0x90,
//	ECg		ECg		ECy		ECy		ECy		DECr
	0x90,	0x90,	0x48,	0x48,	0x48,	0x24,	0x24,	0x24,	0x24,	0x24,	0x24,	0x24,	0x24,	0x24,
	
	0x24,	0x24,	0x24,	0x24
};

uint8_t states_e[32] =
{
//	1r2r	1r2r	1g2r	1g2r	1g2r	1g2r	2r		1g2r	2r		1r2r	1r2r	1r2r	1r2g	1r2g
	0x44,	0x44,	0x48,	0x48,	0x48,	0x48,	0x40,	0x48,	0x40,	0x44,	0x44,	0x44,	0x84,	0x84,
//	1r2g	1r2g	1r		1r2g	1r		1r2r
	0x84,	0x84,	0x04,	0x84,	0x04,	0x44,	0x44,	0x44,	0x44,	0x44,	0x44,	0x44,	0x44,	0x44,
	
	0x44,	0x44,	0x44,	0x44
};

uint8_t states_f[32] =
{
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,

	0x01,	0x01,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	
	0x00,	0x00,	0x00,	0x00
};

// sync, flags, pwm
volatile uint8_t tic, train_flag = 0, night_flag = 0, pwm_cnt, pwmval, halfsec, tmp = 0, p1 = 1, p2, dir;

int main(void)
{
	port_init();
	timer_init();
    while (1) 
    {
	    day_night_switch();
		 PORTB = night_flag << 4;
		 train_button();
    }
}

ISR(TIMER0_OVF_vect)
{
	if (train_flag == 0) {
		PORTF = (PORTF & 0xF1) | (((pwm_cnt & 0x1F) < pwmval) << LV_F);
	}
	
	switch(tic) {
		case 7: case 8: case 9: 
			PORTE = (PORTE & 0xF7) | (((pwm_cnt & 0x1F) < pwmval) << LG1_Z);
		case 17: case 18: case 19:
			PORTE = (PORTE & 0x7F) | (((pwm_cnt & 0x1F) < pwmval) << LG2_Z);
		default: break;
	}
	
	// night service mode
	if(night_flag == 1){
		PORTA = (PORTA & 0xFD) | ( ((pwm_cnt & 0x1F) < pwmval) << LA_S);
		PORTA = (PORTA & 0xEF) | ( ((pwm_cnt & 0x1F) < pwmval) << LB_S);
		PORTC = (PORTC & 0xFE) | ( ((pwm_cnt & 0x1F) < pwmval) << LD_S);
		PORTC = (PORTC & 0xF7) | ( ((pwm_cnt & 0x1F) < pwmval) << LC_S);
		PORTC = (PORTC & 0xBF) | ( ((pwm_cnt & 0x1F) < pwmval) << LE_S);
	}
	
	if (!pwm_cnt) {
		if ((halfsec & 1) && (pwmval < 255)) pwmval++;
		else if (pwmval) pwmval--;
	}
	pwm_cnt++;
}

ISR(TIMER1_COMPA_vect)
{
	if (night_flag == 0) {
		if ( (halfsec & 1) && (halfsec != 32) ) {
			PORTA = states_a[tic];
			PORTC = states_c[tic];
			PORTF = states_e[tic];
			PORTE = (states_e[tic] & PORTE_MASK) | 0x30;
			PORTF = (states_f[tic] & PORTF_MASK);
			if (tic > 32) tic = 0;
			tic++;
		}
	} else {
		if (night_flag == 1) {
			PORTA ^= (1<<LA_S);
			PORTA ^= (1<<LB_S);
			PORTC ^= (1<<LD_S);
			PORTC ^= (1<<LC_S);
			PORTC ^= (1<<LE_S);
			PORTF &= ~(1<<LE_Z2);
		}
	}
	if (train_flag != 0) {
		PORTF &= 0xF7;
		PORTF = ( (p1<<LV_P1) | (p2<<LV_P2) );
		p1 ^= 0x01;
		p2 ^= 0x01;
	}
	halfsec++;
}
