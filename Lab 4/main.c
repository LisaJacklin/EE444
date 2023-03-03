/*
Lisa Jacklin
EE 444 Lab 4
3/2/2023

Objective: 
- write a program that:
  - recieves characters from a terminal program
  - echoes the characters back to the terminal but all in lower case
*/

#include <msp430.h>
extern int IncrementVcore(void);

//int x;

void main(void) {
//note that in order to reach the correct frequency, the core voltage must be pulled up.
  IncrementVcore();
  IncrementVcore();
  IncrementVcore();

  //the first thing to set up is the SMCLK to be 17 MHz in frequency
   UCSCTL1 = DCORSEL_6;                           //THIS SETS THE FREQUENCY RANGE
   UCSCTL2 = 518;                                 //THIS SETS THE DIVISOR FOR FREQUENCY
   P7SEL |= (BIT0 + BIT1);                        //MAKES SURE THAT THE EXTERNAL XTI IS IN USE
   UCSCTL3 = 0;                     //USING XTI exterinal crystal AS THE REFERENCED CLOCK

   UCSCTL4 = SELS_3 + SELM_3;                              //SELM 0 SETS THE XTI SOURCE  
 
    do {
      UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);                   //clear the XT1 and DCO flags
      SFRIFG1 &= ~OFIFG;                                  //clear the OSC fault flag
      } while ((SFRIFG1 & OFIFG));

   UCSCTL6 &= ~XT1OFF;                            //MAKES SURE THAT THIS EXTERNAL CLOCK IS ON
 
   P11DIR |= BIT1 + BIT2;                                 //SET TO BE ABLE TO CHECK THE FREQUENCY
   P11SEL |= BIT1 + BIT2;    

  //FIRST THING THAT NEEDS TO OCCUR IS SETTING THE SMCLK OF 17MHZ TO SOURCE THE USCI
  UCA1CTL1 |= UCSWRST;                            //start the reset state here, reset when finished

  P5SEL = (BIT6 + BIT7);                          //THESE ARE THE PINS FOR TX AND RX
  P5DIR |= BIT6; //TRANSMIT TX TO OUTPUT
  P5DIR &=~BIT7; //RX TO BE AN INPUT

  //NOW TO SET UP THE 7-BIT DATA, EVEN PARITY, ONE STOP BIT PROTOCOL
 // UCA1CTL0 |= (UCPAR + UCPEN + UC7BIT );         //THIS SETS UP THE 7 BITS, EVEN PARITY AND PARITY ENABLED
  //UCA1CTL0 &=~ (UCSPB + UCSYNC);                              //ONE STOP BIT MUST BE ZERO
  UCA1CTL0 = UCPAR + UCPEN + UC7BIT;
  UCA1CTL1 |= UCSSEL__SMCLK ;                     //THIS SETS THE REFERENCE CLOCK TO BE THE SMCLK PREVIOUSLY SETUP

  //USING THIS, THE BAUD RATE OF 38400 BPS MUST BE DETERMINED 
  UCA1BR0 = 27;                                  //THIS IS THE MAJOR TERM RESULT 
  UCA1BR1 = 0;
  UCA1MCTL = UCBRF_10 + UCBRS_5 + UCOS16;                 //TIS SHOUD SET THE VALUES FOR THE MODUATE STAGE...
  //UCA1MCTL |= UCOS16;                            //IF SET TO 1, OVERSAMPLING MODE IS ENABLED

  UCA1CTL1 &= ~UCSWRST;                           //THIS WILL ENABLE THE USCI LOGIC HELD TO RESET STATE..

  UCA1IE |= UCRXIE;                   //THIS WILL ENABLE RX AND TX INTERRUPTS              
  //AS ALWAYS, INTERRUPTS ENABLED AND LOW POWER MODE
  _EINT();
  LPM0;
}

//INTERRUPT SERVICE VECTOR WHERE SENDING AND RECIEVING CHARACTERS SHOULD BE DONE!
void USC1_ISR(void) __interrupt[USCI_A1_VECTOR] {
//note the large amount of breaks throughout the code; this is to escape to a lower power mode.
//NOW TO SET UP THE RX AND TX
  switch (UCA1IV) {                              //this is what tells us the interrupt vector is for vector 1
  //now we have to give cases for what we should do 
  case 0:break;
  case 2: //this should be fo the usci vector a1
    while (!(UCTXIFG & UCRXIFG) ) {              //checking to see if these two flags are true
      //this line should echo back to the terminal
      UCA1TXBUF = UCA1RXBUF;

      //NOW, TO MAKE SURE IT ONLY TAKES LOWER CASE
      //if ( 'a' <= UCA1TXBUF <= 'z') {
      //     UCA1TXBUF = UCA1RXBUF;                   //takes transmit data and sends it to recieve data buffer.
      //};

      break;                                     //must break the loop to continue checking or continue on.
    }
    //UCTXIFG flag for when tx buffer is empty
    //UCRXIFG flag for when rx buffer has recieved a complete character

  default: break;                                //if nothing happens then you can get out of the ISR
  }

}
