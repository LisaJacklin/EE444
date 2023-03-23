/*
Lisa Jacklin
EE 444 Lab 6

Lab 6 Objectives:
1. MCLK as close to 1MHz as possible
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
//do we need to increment the Vcore at this point? Don't think so since we are working with low power...

void main(void)
{
  ///////Setting up MCLK to 1Mhz //////////////////////

  //specifying the external crystal
  P7SEL |= BIT0 + BIT1;
  while (UCSCTL7 & XT1LFOFFG) { //CHECKING TO SEE IF THE CRYSTAL IS STABALIZED BEFORE CONTINUING
    UCSCTL7 &=~XT1LFOFFG;
  }

  //Now to specify the registers range, clocks, frequencies, etc.
  UCSCTL1 = DCORSEL_1; //1Mhz is the center of range
  UCSCTL2 = 30;//DIVIDER FOR 1MHZ FREQUENCY
  UCSCTL3 = 0X0000;//XT1CLK TO BE USED AS REFERENCE
  UCSCTL4 = SELM__DCOCLK;//SETS THE MCLK TO BE BASED ON THE DCO

  //now that the clock should be set, we need to output them and check the result
  P11DIR = BIT1;  //OUTPUT AT MCLK
  P11SEL |= BIT1; //VERIFY I/O TO THIS PIN

  ////////////////////////////////////////////////////


}

/*
////////BUTTON ISR FOR GOING THROUGH LPMs ////////////////
void buttonLPM_ISR(void) __interrupt [PORT2_VECTOR]{

}

*/