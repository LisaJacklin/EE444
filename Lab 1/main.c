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

#include <MSP430.h>
extern int IncrementVcore(void);
extern int DecrementVcore(void);

int x;

void
main(void)
{
  P7DIR = BIT0;                       //THIS SHOULD SET THE PIN TO CRYSTAL MODE
  P7SEL |= BIT0 + BIT1;               //THIS SHOULD TELL THAT HEY! THE SPECCIL CRYSTAL IS HERE PLS USE ME
  
  //THIS WHILE LOOP WILL CHECK TO MAKE SURE THE CRYSTAL IS ONLINE
  while (UCSCTL7 & XT1LFOFFG) {
    UCSCTL7 &=~XT1LFOFFG;
    }

  //these should drop the voltage to the lowest voltage of the core before changing the clock
    DecrementVcore();
    DecrementVcore();
    DecrementVcore();

//Objective 1: setting up MCLK to read close to 25MHz
   UCSCTL1 = DCORSEL_6;            //THIS SETS THE FREQUENCY RANGE
   UCSCTL2 = 762;                  //THIS SETS THE DIVISOR FOR FREQUENCY

   UCSCTL3 |= SELREF_2;            //USING REFOCLK AS THE REFERENCED CLOCK
   UCSCTL4 |= SELM_2 ;             //SELM 2 SETS THE ACLK SOURCE TO REFOCLK 

   P11SEL |= BIT0;                 // this should set the functionality to periperial
   P11OUT |= BIT1 ;                // this should give an output to 11.1

//Objective 2: setting the core voltage to be the max level

  //ADD A BREAKPOINT BEFORE THESE INCREMENTS IN ORDER TO STEP THROUGH AND TEST THE VOLTAGE

   IncrementVcore();                 //this should be used as many times as needed to make sure the voltage 
   IncrementVcore();                 //is in the correct top range of 2.4 to 3.6 V

//Objective 3(Main): measure time between button presses

  //TIMER SETUP FOR MAIN OBJECTIVE
    TA0CTL = TASSEL_2;                //SETS THE BASE CLOCK TO BE SMCLK
    //CCIE ^= BIT4;                      //ALLOWS INTERRUPTS
    //CCIS   SHOULD THIS BE USED AS WELL?

  //PUSH BUTTON TO TIME REACTION SETUP
    //push button is P2.7 so BIT7 is what we are messing with
    P2DIR &= ~BIT7;                     //sets to output direction
    P2SEL &= ~BIT7;                     //this sets to I/O mode

    P2IE |= BIT7;                      // interrupts are allowed for this button                  
    P2IES |= BIT7;                      //this means the value should be 1 to enable
    P2REN |= BIT7;                      //input with pull up resistor 
    P2OUT |= BIT7;                      //also required to enable pull up resistor.


  //THIS IS TO MAKE SURE NO FAULT FLAGS HAVE OCCURRED.
    while (SFRIFG1&OFIFG)                 //TEST OSCILLATOR FAULT FLAG
      {
      UCSCTL7 &=~(XT2OFFG + XT1LFOFFG + XT1HFOFFG+DCOFFG);
      //CLEAR XT2, XT1, DCO FAULT FLAGS
      SFRIFG1 &=~OFIFG;                   //CLEAR FAULT FLAGS
      }

//THIS MUST STAY HERE AT THE END OF THE PROGRAM
 _EINT();
 LPM0;
}

void InterruptService_BUTTON(void) __interrupt [PORT2_VECTOR] {
    //push button is P2.7 
    //SET UP A WAY TO DO THE FIRST OR SECOND PUSH OF THE BUTTON...
    x = 0;

    if (P2IV == P2IV_P2IFG7){
      x++;
     P1OUT ^= BIT0;                   //this should toggle the LED in use

     TA0CCR1 ^= BIT1;                     //THIS SHOULD CAPTURE...

     } else {
     P1OUT = BIT0;                    //THIS SHOULD TURN THE LED IN USE, OFF
     TA0CCR2 ^= BIT1;                 //THIS SHOULD CAPTURE...
     }
    
   // X = TA0CCR2- TA0CCR1; //WOULD THIS BE A POSSIBLE WAY TO GET THE CAPTURE DIFFERENCE?

}  
