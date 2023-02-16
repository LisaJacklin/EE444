//Lisa Jacklin
//EE 444 LAb 0
//board 5
//2/2/2023

/*
Objective: set MCLK to 4MHz
MCLK is originally set to 

Objective 2: add an interupt service routine that will toggle an LED
  every time  apush button is pressed
*/

#include <MSP430.h>
void 
main(void) {
//Objective 1: Set MCLK to 4MHz
    
  //Setting up the clock
   UCSCTL1 = DCORSEL_6;               //this sets the range for the frequency
   UCSCTL2 = 121;                     //this is the N factoring for the main signal
   //these two are what give the correct range and figure of frequency

   UCSCTL3 = SELREF_2;                // this will set the reference clock to XT1CLK
   //note that the SELREF_0 was grabbed from the header file for the microprocessor
   UCSCTL4 = SELM_2;                  //this should allow for DCOCLKDIV to be used.
                                      //DCOCLKDIV is required for MCLK

   P11SEL |= BIT1;                    // this should set the functionality to periperial
   P11OUT |= BIT1 ;                   // this should give an output to 11.1

//Objective 2: Interrupt service routine...
  //set LED to display...
  P1DIR |= BIT0;                      //pin set to output 
  P1SEL &= ~BIT0;                      //set pin to I/O mode


  //push button is P2.7 so BIT7 is what we are messing with
  P2DIR &= ~BIT7;                     //sets to output direction
  P2SEL &= ~BIT7;                     //this sets to I/O mode

  //interrupts must be allowed and adjusted...
  P2IE |= BIT7;                      // interrupts are allowed for this button                  
  P2IES |= BIT7;                      //this means the value should be 1 to enable
  P2REN |= BIT7;                      //input with pull up resistor 
  P2OUT |= BIT7;                      //also required to enable pull up resistor.

  _EINT();                            // interrupts enabled

  LPM0;                               //low power mode which allows 1 to be active and 0 active for 
                                      //MCLK which is what we are using now.

}

//remember: interrupts require their own void.
void InterruptService_LED(void) __interrupt [PORT2_VECTOR] {
    //push button is P2.7
    if (P2IV == P2IV_P2IFG7){
     P1OUT ^= BIT0;                   //this should toggle the LED in use
     }   

}
