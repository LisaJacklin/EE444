/*
Main.c
Lisa Jacklin
EE 444 Lab1
2/09/2023

Objectives:
1. Set the MCLK to run as close to 25MHz as possible
2. set the core voltage to be set to the max level.

Main Objective:
3.write a program that measures the time between consecutive button presses using
interrupts and ONE of the timers to implement this functionality.

note: it is acceptable to set the breakpoint in order to check the time measurement results   
*/

#include <msp430.h>
extern int IncrementVcore(void); //this is the required extern to up the voltage
extern int DecrementVcore(void); //required extern to down the voltage

int buttonPress = 0;

void main(void) {
	IncrementVcore();			//calling the function raises the core voltage
	IncrementVcore();
	IncrementVcore();			//rasing the voltage to the max, 3 times incremented

	UCSCTL1 = DCORSEL_6;		//range of frequencies for 25 MHz
	UCSCTL2 = 762;				// adjusts to 762 to divider to allow 25MHz frequency
	UCSCTL3 = 0x0000;			// setting n=1
	UCSCTL4 = SELM_3;			//source from XT1CLK crystal

	P11DIR |= BIT1;				// setting output
	P11SEL |= BIT1;				// sellecting 11.1 as the output to check the base clock frequency

	//note the changed format...
	P7SEL |= 0x03;				//sets the XT1 pins

	//check to make sure that the crystal is operating and working currectly
	UCSCTL6 &= ~(XT1OFF); //making sure XT1 is not off
	UCSCTL6 |= XCAP_3; //internal load capacitor 

	//loop to clear the flags
	do {
		UCSCTL7 &= ~(XT1LFOFFG + DCOFFG); //clear the XT1 and DCO flags
		SFRIFG1 &= ~OFIFG; //clear the OSC fault flag
	} while ((SFRIFG1 & OFIFG));

	//setup pin 11.0 to check to ACLK which will be used to operate the interrupt
	P11DIR |= BIT0;
	P11SEL |= BIT0; 

	//lets timer A be set to continuous mode and ACLK as the clock source
	TA0CTL = TASSEL__ACLK + MC__CONTINUOUS; 

	//pin 5.0 is the output pin to observe the button interrupt operating
	P5DIR |= BIT0;
	P5OUT &= ~BIT0;

	P2DIR &= ~(BIT6);			// sets button to input by default
	P2REN = BIT6;				//pullup resistor required to not burn out the LED
	P2OUT = BIT6;				//output set to the LED 2.6
	P2IE |= BIT6;				//enable interrupts for 2.6

	_EINT();					//allows interrupts
	LPM0;						//low power mode zero
}

void Button_Interrupt(void) __interrupt[PORT2_VECTOR]{
	//this will make sure that the flag will be clear or will set the button to zero
	if (P2IV == P2IV_P2IFG6) {
		buttonPress ^= BIT0; //each time button is pressed, toggle readvalue
	}

	//note that these two are swapped from the original from 0 to 1 statements
	//first press will set to one
	if (buttonPress == 1) {
	TA0CTL |= TACLR; //clears when initial pressing
	P5OUT ^= BIT0;
	}

	//second press will set to zero 
	if (buttonPress == 0) { //if the button is 0, toggle
	P5OUT ^= BIT0;
	buttonPress = 0;
	}
	//consider adding in debounce if issues still percist
}

