/*
Lisa Jacklin
EE 444 Challenge 1
3/7/2023

Objective: 
- write a program that:
  - sends a nicely formatted message to the PC terminal each time a user pushes a button
  -note that a counter should be embeded into the sentence
*/

#include <msp430.h>
extern int IncrementVcore(void);
//tens                       10         20        30 
//ones             012345678901234567890123456789012345678901234    the rest of this is to go to the next line 
char message[] = {"You pushed the button: 0000 times. 0x0a 0x0d "};
//above is a message that will be sent through the msp430 when the irs is used
int messageInc;                                      //this will increment the char through the msg above
int counter;
int ButtonPressed;

void main(void) {
//note that in order to reach the correct frequency, the core voltage must be pulled up.
  IncrementVcore();
  IncrementVcore();
  IncrementVcore();

   //the first thing to set up is the SMCLK to be 17 MHz in frequency
   UCSCTL1 = DCORSEL_6;                           //THIS SETS THE FREQUENCY RANGE
   UCSCTL2 = 518;                                 //THIS SETS THE DIVISOR FOR FREQUENCY
   P7SEL |= (BIT0 + BIT1);                        //MAKES SURE THAT THE EXTERNAL XTI IS IN USE
   UCSCTL3 = 0;                                   //USING XTI exterinal crystal AS THE REFERENCED CLOCK

   UCSCTL4 = SELS_3 + SELM_3;                     //SELM 0 SETS THE XTI SOURCE  
 
    do {
      UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);           //clear the XT1 and DCO flags
      SFRIFG1 &= ~OFIFG;                          //clear the OSC fault flag
      } while ((SFRIFG1 & OFIFG));

   UCSCTL6 &= ~XT1OFF;                            //MAKES SURE THAT THIS EXTERNAL CLOCK IS ON
 
   P11DIR |= BIT1 + BIT2;                         //SET TO BE ABLE TO CHECK THE FREQUENCY
   P11SEL |= BIT1 + BIT2;    

  //FIRST THING THAT NEEDS TO OCCUR IS SETTING THE SMCLK OF 17MHZ TO SOURCE THE USCI
  UCA1CTL1 |= UCSWRST;                            //start the reset state here, reset when finished
  P5SEL = (BIT6 + BIT7);                          //THESE ARE THE PINS FOR TX AND RX
  P5DIR |= BIT6;                                  //TRANSMIT TX TO OUTPUT
  P5DIR &=~BIT7;                                  //RX TO BE AN INPUT
  UCA1CTL0 = UCPAR + UCPEN + UC7BIT;
  UCA1CTL1 |= UCSSEL__SMCLK ;                      //THIS SETS THE REFERENCE CLOCK TO BE THE SMCLK PREVIOUSLY SETUP

  //USING THIS, THE BAUD RATE OF 38400 BPS MUST BE DETERMINED 
  UCA1BR0 = 27;                                   //THIS IS THE MAJOR TERM RESULT 
  UCA1BR1 = 0;
  UCA1MCTL = UCBRF_10 + UCBRS_5 + UCOS16;         //TIS SHOUD SET THE VALUES FOR THE MODUATE STAGE...
  UCA1CTL1 &= ~UCSWRST;                           //THIS WILL ENABLE THE USCI LOGIC HELD TO RESET STATE..
  UCA1IE |= UCTXIE;                               //THIS WILL ENABLE RX INTERRUPTS              
     
  //PUSH BUTTON 
    //push button is P2.7 so BIT7 is what we are messing with
    P2DIR &= ~BIT7;                              //sets to input direction
    P2IE |= BIT7;                                // interrupts are allowed for this button                  
    P2REN = BIT7;                                //input with pull up resistor 
    P2OUT = BIT7;                                //also required to enable pull up resistor.

/*   
 //example code for adjusting the msg count values
  message[24] = counter/100 + '0';                     //this is the 100s place
  message[25] = counter/10 + '0';                      //this will be for the 10s place
  message[26]= counter%10;                             //this is 
*/
                                                  //AS ALWAYS, INTERRUPTS ENABLED AND LOW POWER MODE                                             
  _EINT();
  LPM0;
}

void USC1_ISR(void) __interrupt[USCI_A1_VECTOR] {
                                                 //note the large amount of breaks throughout the code; this is to escape to a lower power mode.
                                                 //NOW TO SET UP THE RX AND TX
  switch (UCA1IV) {                              //this is what tells us the interrupt vector is for vector 1
                                                 //now we have to give cases for what we should do 
  case 0:break;
  case 2:                                         //this should be fo the usci vector a1
   
    while (!(UCTXIFG & P2IV_P2IFG7) ) {                          //checking to see if these two flags are true
    // UCA1TXBUF = UCA1RXBUF;                     //this line goes through and echos back the value but in this while loop, i want to print a message
     counter++;
    break;                                     //must break the loop to continue checking or continue on.
    }
                                                 //UCTXIFG flag for when tx buffer is empty
  default: break;                                //if nothing happens then you can get out of the ISR
  }
}

//I can confirm that the IRS for the button can be accessed
void InterruptService_BUTTON(void) __interrupt [PORT2_VECTOR] {
    //when there is an interrupt, the button constant must be changed
    //button constant is x.
  counter = 0;                                       //this will be used to increment the amount of times the button has been pressed
  messageInc = 0;

    if (P2IV == P2IV_P2IFG7) {
      if (ButtonPressed == 0) {

         if (messageInc < 45) {
          UCA1TXBUF = message[messageInc];
          messageInc++;
          ButtonPressed = 0;
          } 

        ButtonPressed = 1;                          //verify and set the loop again.
      } else {    
        counter++;                                    //when the button is done being pressed, it will increment the counter
        ButtonPressed = 0;           
      }
  }
}


