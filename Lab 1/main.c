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

volatile int ButtonPressed = 0;
volatile unsigned int time = 0;

void
main(void)
{
    //ADD A BREAKPOINT BEFORE THESE INCREMENTS IN ORDER TO STEP THROUGH AND TEST THE VOLTAGE
    IncrementVcore();                   //this should be used as many times as needed to make sure the voltage 
    IncrementVcore();                   //is in the correct top range of 2.4 to 3.6 V
    IncrementVcore();                   // this should raise the max core voltage three times


//Objective 1: setting up MCLK to read close to 25MHz
   UCSCTL1 = DCORSEL_6;                 //THIS SETS THE FREQUENCY RANGE
   UCSCTL2 = 762;                       //THIS SETS THE DIVISOR FOR FREQUENCY

   UCSCTL3 |= SELREF_3;                 //USING REFOCLK AS THE REFERENCED CLOCK
   UCSCTL4 = SELM_3 ;                  //SELM 2 SETS THE ACLK SOURCE TO REFOCLK 

   P11DIR |= BIT1;
   P11SEL |= BIT1;                      // this should set the functionality to periperial
   //P11OUT |= BIT1 ;                     // this should give an output to 11.1

//Objective 3(Main): measure time between button presses

  //TIMER SETUP FOR MAIN OBJECTIVE
   P7SEL |= 0X03;                       //XT1 CLOCK PINS
   UCSCTL6 &= ~(XT1OFF);                //CHECKING TO SEE IF THE CLOCK IS OFF
   UCSCTL6 |= XCAP_3;                   //LOAD CAPACITOR SPECIFYER

    do {
    UCSCTL7 &= ~(XT1LFOFFG + DCOFFG); //clear the XT1 and DCO flags
    SFRIFG1 &= ~OFIFG; //clear the OSC fault flag
    } while ((SFRIFG1 & OFIFG));

   //sets timer A to depend on the ACLK and to be continuous 
   TA0CTL = TASSEL__SMCLK + MC__CONTINUOUS;

   //output pin for the button operations
   P5DIR |= BIT0;
   P5OUT &= ~BIT0;

  //PUSH BUTTON TO TIME REACTION SETUP
    //push button is P2.7 so BIT7 is what we are messing with
    P2DIR &= ~BIT7;                    //sets to input direction
    P2IE |= BIT7;                      // interrupts are allowed for this button                  
    P2REN = BIT7;                      //input with pull up resistor 
    P2OUT = BIT7;                      //also required to enable pull up resistor.


//THIS MUST STAY HERE AT THE END OF THE PROGRAM
 _EINT();
 LPM0;
}

//FIX THE INTERRUPT SERVICE ROUTINE SO THE BUTTON WILL RESPOND TO THE BUTTON WITHOUT BOUNCING BACK IMMEDIATELY
void InterruptService_BUTTON(void) __interrupt [PORT2_VECTOR] {
    //when there is an interrupt, the button constant must be changed
    //button constant is x.
    if (P2IV == P2IV_P2IFG7) {
        //ButtonPressed ^= BIT0;                      //toggles x when the button is pressed
    
     
      //if the button const is toggled to zero
      if (ButtonPressed == 0) {
          TA0CTL |= TACLR;                             //resets the timer; clear the timer on initial press
          time = 0;
          P5OUT ^= BIT0;                              //displays an output when button at zero
          ButtonPressed = 1;                          //verify and set the loop again.
      }

      //when the button constant is toggled to one
      //if (ButtonPressed == 1) {
      else {
        P5OUT ^= BIT0;  
        time = TA0R;  
        ButtonPressed = 0;           
      }

  }

}


