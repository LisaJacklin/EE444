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
  P7DIR = BIT0;                         //THIS SHOULD SET THE PIN TO CRYSTAL MODE
  P7SEL |= BIT0 + BIT1;                 //THIS SHOULD TELL THAT HEY! THE SPECCIL CRYSTAL IS HERE PLS USE ME
  
  //THIS WHILE LOOP WILL CHECK TO MAKE SURE THE CRYSTAL IS ONLINE
  while (UCSCTL7 & XT1LFOFFG) {
    UCSCTL7 &=~XT1LFOFFG;
    }

  //these should drop the voltage to the lowest voltage of the core before changing the clock

  IncrementVcore();                     //this should be used as many times as needed to make sure the voltage 
  IncrementVcore();                     //is in the correct top range of 2.4 to 3.6 V
  IncrementVcore();                     // this should raise the max core voltage three times

//Objective 1: setting up MCLK to read close to 25MHz
   UCSCTL1 = DCORSEL_6;                 //THIS SETS THE FREQUENCY RANGE
   UCSCTL2 = 762;                       //THIS SETS THE DIVISOR FOR FREQUENCY

   UCSCTL3 |= SELREF_2;                 //USING REFOCLK AS THE REFERENCED CLOCK
   UCSCTL4 |= SELM_2 ;                  //SELM 2 SETS THE ACLK SOURCE TO REFOCLK 

   P11SEL |= BIT0;                      // this should set the functionality to periperial
   P11OUT |= BIT1 ;                     // this should give an output to 11.1

//Objective 2: setting the core voltage to be the max level

  //ADD A BREAKPOINT BEFORE THESE INCREMENTS IN ORDER TO STEP THROUGH AND TEST THE VOLTAGE

//Objective 3(Main): measure time between button presses

  //TIMER SETUP FOR MAIN OBJECTIVE
   P7SEL |= 0X03;                        //XT1 CLOCK PINS
   UCSCTL6 &= ~(XT1OFF);                 //CHECKING TO SEE IF THE CLOCK IS OFF
   UCSCTL6 |= XCAP_3;                    //LOAD CAPACITOR SPECIFYER


   //THIS IS TO MAKE SURE NO FAULT FLAGS HAVE OCCURRED.
   do {
       UCSCTL7 &= ~(XT1LOFFG + DCOFFG); //CLEARS CLOCK INTERRUPT FLAGS
       SFRIFG1 &= ~OFIFG;               //THIS SHOULD CLEAR THE FLAG 
   } while ((SFRIFG1 & OFIFG));         //only performs if this is occuring

   P11DIR |= BIT0;                      // sets the bit for the base clock frequency
   P11SEL |= BIT0;                      //selects the correct clock for the ACLK

   //sets timmer A to depend on the ACLK and to be continuous 
   TA0CTL = TASSEL_ACLK + MC_CONTINUOUS;

   P5DIR |= BIT0;
   P5OUT &= ~BIT0;

  //PUSH BUTTON TO TIME REACTION SETUP
    //push button is P2.7 so BIT7 is what we are messing with
    P2DIR &= ~BIT6;                    //sets to input direction
    P2IE |= BIT6;                      // interrupts are allowed for this button                  
    P2REN = BIT6;                      //input with pull up resistor 
    P2OUT = BIT6;                      //also required to enable pull up resistor.


//THIS MUST STAY HERE AT THE END OF THE PROGRAM
 _EINT();
 LPM0;
}

void InterruptService_BUTTON(void) __interrupt [PORT2_VECTOR] {
    //when there is an interrupt, the button constant must be changed
    //button constant is x.
    if (P21V == P2IV_P2IFG6) {
        x ^= BIT0;                     //toggles x when the button is pressed
    }
     
    //when the button constant is toggled to one
    if (x == 1) {
        TA0CTL |= TACLR;               //resets the timer; clear the timer on initial press
        P5OUT ^= BIT0;                 //displays an output when pressed
    }

    //if the button const is toggled to zero
    if (x == 0) {
        P5OUT ^= BIT0;                  //displays an output when button at zero
        x = 0;                          //verify and set the loop again.
    }

}
    

}  
