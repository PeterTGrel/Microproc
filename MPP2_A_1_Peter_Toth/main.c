/* Gruppe B, Vorname, Nachname => Péter, Tóth
 * main.c, 10.05.2021
 *
 * 4 bit counter (0-15) with 1 second between each step.
 *
 * Aufgabe 1.1/a
 *  F,N (PN1, PN0, PF4, PF0)
 *  GPIO_PORTN_DEN_R = 0x03;                // Enable of Port N (Pin 0, Pin 1)
 *  GPIO_PORTF_AHB_DEN_R = 0x11;            // Enable of Port F (Pin 0, Pin 4)
 *
 * Aufgabe 1.1/b
 *  interval load register ILR: TIMER0_TAILR_R // ILR = ceil(16M/16MHz*1) = 1s
 *
 * Aufgabe 1.1/c
 *  delta_t = 1/16Mhz = 62.5ns
 *  without Match register: T = (16Mio–1+1)*delta_t = 1 sec
 *
 * Aufgabe 1.1/d
 *  Prescaler = (T * f / Taktschritte) - 1 = (1s * 16MHz / 2^16) - 1 = 245 - 1
 *  Load value = (1s * 16Mhz / 245) - 1 = 68478 - 1  in code: ILV: TIMER0_TAILR_R = 16000000-1; // ILR = ceil(16M / 1 * 1) - 1
 *  Match value = (0.5 * 16MHz / 245) - 1 = 34239 - 1
 */

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>

// function prototype for GPIOPorts initialization
void GPIOPort_init();
// function prototype for TIMER initialization
void timer_init();

int main(void)
{
    int i, cnt = 0;
    GPIOPort_init();                // function call GPIO initialization
    timer_init();                   // function call TIMER initialization

    // LED control at start, turn off all LEDs
    GPIO_PORTF_AHB_DATA_R = 0x00;
    GPIO_PORTN_DATA_R = 0x00;
    for(i=0;i<5000;i++);

    // begin of endless loop
    while(1){

       // turn on LEDs (sel. pins high) according to bit pattern of counter
      //  GPIO_PORTN_DATA_R = (cnt & 0x01)<<1 | (cnt & 0x02)>>1;
       switch(cnt){
            case 1:
                GPIO_PORTF_AHB_DATA_R = 0x01; break;
            case 2:
                GPIO_PORTF_AHB_DATA_R = 0x10; break;
            case 3:
                GPIO_PORTF_AHB_DATA_R = 0x11; break;
            case 4:
                GPIO_PORTN_DATA_R = 0x01;
                GPIO_PORTF_AHB_DATA_R = 0x00; break;
            case 5:
                GPIO_PORTN_DATA_R = 0x01;
                GPIO_PORTF_AHB_DATA_R = 0x01; break;
            case 6:
                GPIO_PORTN_DATA_R = 0x01;
                GPIO_PORTF_AHB_DATA_R = 0x10; break;
            case 7:
                GPIO_PORTN_DATA_R = 0x01;
                GPIO_PORTF_AHB_DATA_R = 0x11; break;
            case 8:
                GPIO_PORTF_AHB_DATA_R = 0x00;
                GPIO_PORTN_DATA_R = 0x02; break;
            case 9:
                GPIO_PORTN_DATA_R = 0x02;
                GPIO_PORTF_AHB_DATA_R = 0x01; break;
            case 10:
                GPIO_PORTN_DATA_R = 0x02;
                GPIO_PORTF_AHB_DATA_R = 0x10; break;
            case 11:
                GPIO_PORTN_DATA_R = 0x02;
                GPIO_PORTF_AHB_DATA_R = 0x11; break;
            case 12:
                GPIO_PORTN_DATA_R = 0x03;
                GPIO_PORTF_AHB_DATA_R = 0x00; break;
            case 13:
                GPIO_PORTN_DATA_R = 0x03;
                GPIO_PORTF_AHB_DATA_R = 0x01; break;
            case 14:
                GPIO_PORTN_DATA_R = 0x03;
                GPIO_PORTF_AHB_DATA_R = 0x10; break;
            case 15:
                GPIO_PORTN_DATA_R = 0x03;
                GPIO_PORTF_AHB_DATA_R = 0x11; break;
            default: break;
        }
        // wait for the timeout flag, (masking and polling)
        while((TIMER0_RIS_R & (1<<4))==0);
        TIMER0_ICR_R |= (1<<4);

        // increment counter, handle the maximum countervalue
        cnt++;
        if(cnt == 16){                                     // reaching 16
            cnt = 0;
            GPIO_PORTN_DATA_R = 0x00;                      // turning off LEDs
            GPIO_PORTF_AHB_DATA_R = 0x00;
            for(i=0;i<10000;i++);                          // wait a bit before starting again
            // handle the timer flag
            while((TIMER0_RIS_R & (1<<0))==0);
            TIMER0_ICR_R |= 0x00000001;                    // clear flag TATORIS timer 0A
            TIMER0_CTL_R |= 0x00000001;                    // restart timer 0
        }

    }
}

void GPIOPort_init()
{
    SYSCTL_RCGCGPIO_R = 0x00001020 ;        // Clk for F,N,D
    while(!(SYSCTL_PRGPIO_R & 0x00001020)); // wait for ports activation

    GPIO_PORTN_DEN_R = 0x03;                // Enable of Port N (Pin 0, Pin 1)
    GPIO_PORTN_DIR_R = 0x03;                // Config Pins as output (write)

    GPIO_PORTF_AHB_DEN_R = 0x11;            // Enable of Port F (Pin 1, Pin 4)
    GPIO_PORTF_AHB_DIR_R = 0x11;            // Config Pins as output (write)
}
void timer_init()
{
    SYSCTL_RCGCTIMER_R |= (1<<0);
    while(!(SYSCTL_PRTIMER_R & 0x01));      // wait for Timer 0 activation
    TIMER0_CTL_R &= ~0x01;                  // disable Timer 0
    TIMER0_CFG_R = 0x0;                     // 32-bit mode
    TIMER0_TAMR_R |= (1<<5);                // match enable
   // TIMER0_TAPR_R = 245-1;
    TIMER0_TAILR_R = 16000000-1;            // ILR = ceil(16M/16MHz*1)-1
    TIMER0_TAMATCHR_R = 16000000-1;         // MV = ceil(16M/16MHz*1)-1
    TIMER0_CTL_R |= 0x01;                   // enable Timer
}
