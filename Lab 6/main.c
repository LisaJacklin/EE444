/*
Lisa Jacklin
EE 444 Lab 6

Lab 6 Objectives:
x1. MCLK as close to 1MHz as possible
2. button press to enter each LPM
3. Blink an LED several times at the beginning of the program
4. From here, do all you can to lower power consumption
------------------------------------------------------------------

Tests:
-first stop the project and follow the rest CAREFULLY
1. remove the JP1 umper and connect a multimeter to measure current draw of the MSP430
2. project items -> properties ->target -> release JTAG ->set to 'yes'
3. clock target -> reset MSP-FET430...
4. comment on results in your report
5. PUT THE JP1 JUMPER BACK

*/

#include <msp430.h>
#include <in430.h> //required for lpmx_exit to work!

//////////Setting up LPM exits //////////////////////


int ModeCounter;                             //initialized the LPM counter at zero
int Blink = 0;                                   //blink the LED based on the LPMs

void main(void)
{
  ///////Setting up MCLK to 1Mhz //////////////////////

  //specifying the external crystal
  P7SEL |= BIT0 + BIT1;
  while (UCSCTL7 & XT1LFOFFG) {                  //CHECKING TO SEE IF THE CRYSTAL IS STABALIZED BEFORE CONTINUING
    UCSCTL7 &=~XT1LFOFFG;
  }

  //Now to specify the registers range, clocks, frequencies, etc.
  UCSCTL1 = DCORSEL_1;                           //1Mhz is the center of range
  UCSCTL2 = 30;                                  //DIVIDER FOR 1MHZ FREQUENCY
  UCSCTL3 = 0X0000;                              //XT1CLK TO BE USED AS REFERENCE
  UCSCTL4 = SELM__DCOCLK;                        //SETS THE MCLK TO BE BASED ON THE DCO

  //now that the clock should be set, we need to output them and check the result
  P11DIR = BIT1;                                 //OUTPUT AT MCLK
  P11SEL |= BIT1;                                //VERIFY I/O TO THIS PIN

  ///////Setting up the Button/////////////////////////
    //push button is P2.7 so BIT7 is what we are messing with
    P2DIR &= ~BIT7;                              //sets to input direction
    P2SEL &= ~BIT7;                              //confirmation of I/O mode selection
    P2IE |= BIT7;                                // interrupts are allowed for this button                  
    P2IES |= BIT7;                               //falling edge trigger
    P2REN = BIT7;                                //input with pull up resistor 
    P2OUT = BIT7;                                //also required to enable pull up resistor.

  //////Setting up the LED //////////////////////////
   P1DIR |= BIT0;
   P1OUT &= ~BIT0;


   //PADIR = 0xFFFF; //turn off this to minimize power?
 _EINT();                                       //ENABLE INTERRUPTS

 //////Low Power Modes which will be skipped through using button///////////
  //this while loop is supposed to hold us in a LPM until the ISR is reached to bring us into the active region
  while (ModeCounter) { 
     LPM0;
     LPM1;
     LPM2;
     LPM3;
     LPM4;
   }
}


////////BUTTON ISR FOR GOING THROUGH LPMs ////////////////
void buttonLPM_ISR(void) __interrupt [PORT2_VECTOR]{

  //TO CHECK THAT THE PROGRAM IS STILL ALIVE, I WILL FLASH AN LED WHEN CHANGING LPMS IN THE BUTTON INTERRUPT
  if (P2IV == P2IV_P2IFG7) {
        Blink++;
    /////////Go through the different LPMs/////////////////////
      //during this, we want to bring us entirely out of the LPMs and into the active region,

    if (Blink = 1) {
      P1OUT = BIT0; //flash the LED on 
      LPM4_EXIT;
      __delay_cycles(10000); //delay cycles so the LED will stay on for long enough to see
      P1OUT ^= BIT0;  //LED off
      return;
    }

    if (Blink = 2) {
      P1OUT = BIT0; //flash the LED on 
      LPM4_EXIT;
      __delay_cycles(10000);
      P1OUT ^= BIT0;  //LED off
      return;
    }

    if (Blink = 3) {
      P1OUT = BIT0; //flash the LED on 
      LPM4_EXIT;
      __delay_cycles(10000);
      P1OUT ^= BIT0;  //LED off
      return;
    }

    if (Blink = 4) {
      P1OUT = BIT0; //flash the LED on 
      LPM4_EXIT;
      __delay_cycles(10000);
      P1OUT ^= BIT0;  //LED off
      return;
    }

    if (Blink = 5) {
      P1OUT = BIT0; //flash the LED on 
      LPM4_EXIT;
      __delay_cycles(10000);
      P1OUT ^= BIT0;  //LED off
      return;
    }

  }
}

