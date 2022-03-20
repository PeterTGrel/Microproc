/* Gruppe B , Author => Péter, Tóth
 *
 * main.c
 *
 * The code changes the duty cycle of the signal (PWM - Port A with Timer2B) from 50 to 0 or 100% in 10% steps. The value is given to a LED on PortF.
 *
 * 10.05.2021, 0.1
 *
 * Aufgabe 2.1/a
 *  TIMER2_TBMATCHR_R = 4000-1, without prescaler
 *  delta_t = 1/16Mhz = 62.5ns
 *  T = (4000-1+1)*delta_t = 0.25ms
 * Aufgabe 2.1/b
 *  PA5 and PM1 are available for Timer2B. PA5 is the chosen one.
 *
 * Aufgabe 2.1/c
 *  GPIO_PORTA_AHB_AFSEL_R |= (1<<5); // PA(5) alternate function
 *
 * Aufgabe 2.1/d
 *  GPIO_PORTA_AHB_PCTL_R = 0x00300000 because PA(5). The PCTL_R config values must be set to 3 for the corresponding pin.
 *
 */
#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>
// function prototype for GPIOPorts initialization
void GPIOPort_init();
// function prototype for TIMER initialization
void timer_init();

int main(void)
{
    // Variables initialization
    int k=0;                                        // delay loop variable
    int dutyP = 50;                                 // duty cycle in %
    int cnt = 0;                                    // clock cycles
    // function call for GPIOPorts initialization
    GPIOPort_init();
    timer_init();
    // begin of endless loop
    while(1){
        cnt++;
        GPIO_PORTF_AHB_DATA_R = GPIO_PORTA_AHB_DATA_R;
        if (cnt % 50 == 0 && cnt != 0) {
            cnt = 0;
            if(GPIO_PORTJ_AHB_DATA_R == 0x02){                                  // button 1 pressed
                if(dutyP==100){                                                 // percentage is always between 0 and 100
                    dutyP = 100;
                }
                else {
                    dutyP += 10;
                    for(k=0;k<500000;k++);                                      // delay loop to make button usage slower
                }
                TIMER2_TBMATCHR_R = (TIMER2_TBMATCHR_R*(dutyP/100))-1;          // changing match value
                printf("percentage:%d, LED_Data: %4x Timer_Data: %4x\n", dutyP,GPIO_PORTF_AHB_DATA_R,TIMER2_TBMATCHR_R);
            }
            else if(GPIO_PORTJ_AHB_DATA_R == 0x01){                          // button 2 pressed
                if(dutyP == 0){                                              // cnt is always between 0 and 100
                    dutyP = 0;
                }
                else{
                    dutyP -= 10;
                    for(k=0;k<500000;k++);                                   // delay loop to make button usage slower
                }
                TIMER2_TBMATCHR_R = (TIMER2_TBMATCHR_R*(i/100))-1;           // changing match value
                printf("percentage:%d, LED_Data: %4x Timer_Data: %4x\n", dutyP,GPIO_PORTF_AHB_DATA_R, TIMER2_TBMATCHR_R);
            }
        }
    }
}
// function definition for GPIOPorts (LED and CCP initialization)
void GPIOPort_init() {
    SYSCTL_RCGCGPIO_R = 0x00000121;         // CLK for Ports A,F
    while(!(SYSCTL_PRGPIO_R & 0x00000121)); // wait for ports activation
    // CCP
    GPIO_PORTA_AHB_DEN_R |= (1<<5);         // PA(5) enable
    GPIO_PORTA_AHB_DATA_R &= ~(1<<5);       // clear PA(5)
    GPIO_PORTA_AHB_AFSEL_R |= (1<<5);       // PA(5) alternate function
    GPIO_PORTA_AHB_PCTL_R = 0x00300000;     // PA(5) connected to Timer2B
    // F for LEDs
    GPIO_PORTF_AHB_DEN_R = 0x11;            // Enable of Port F (Pin 1, Pin 4)
    GPIO_PORTF_AHB_DIR_R = 0x11;            // Config as output
    // J for buttons
    GPIO_PORTJ_AHB_DEN_R = 0x03;            // Enable of Port J (Pin 3)
    GPIO_PORTJ_AHB_PUR_R = 0x03;            // WPU (Weak Pull Up) for PIN PJ0 and PJ1
    GPIO_PORTJ_AHB_DIR_R = 0x00;            // Config as input

}
// function definition for Timer initialization
void timer_init()
{
    SYSCTL_RCGCTIMER_R |= (1<<2);
    while(!(SYSCTL_PRTIMER_R & (1<<2)));    // wait for timer 2 activation
    TIMER2_CTL_R &= ~0x04;                  // disable Timer 2
    TIMER2_CFG_R = 0x04;                    // 16-bit mode

    TIMER2_TBMR_R |= (1<<2) | 0x03;         // TAAMS: PWM-mode, TAMR: periodic
    TIMER2_CTL_R |= (1<<14);                // TBPWML=1 (inverting)
    //  TIMER2_TAPR_R = 269-1;              // prescaler PR
    TIMER2_TBILR_R = 16000;                 // ILR = 16000
    TIMER2_TBMATCHR_R = 4000;               // MV = 4000
    GPIO_PORTA_AHB_DATA_R |= (1<<5);        // starting PWM
    TIMER2_CTL_R |= 0x00000100;             // enable Timer2B


}
