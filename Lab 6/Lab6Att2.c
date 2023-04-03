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
#include <in430.h>

volatile int ModeCounter = 0;
volatile int Blink = 0;

void main(void)
{

  // Setting up MCLK to 1 MHz
  P7SEL |= BIT0 + BIT1;

  while (UCSCTL7 & XT1LFOFFG) { //stabalizing the crystal before continuing
    UCSCTL7 &= ~XT1LFOFFG;
  }

  //Now to specify the registers range, clocks, frequencies, etc.
  UCSCTL1 = DCORSEL_1;
  UCSCTL2 = 30;
  UCSCTL3 = SELREF__XT1CLK;
  UCSCTL4 = SELA__XT1CLK | SELS__DCOCLK | SELM__DCOCLK;

  //now that the clock should be set, we need to output them and check the result
  P11DIR = BIT1;
  P11SEL |= BIT1;

  // Setting up button
  P2DIR &= ~BIT7;
  P2SEL &= ~BIT7;
  P2IE |= BIT7;
  P2IES |= BIT7;
  P2REN = BIT7;
  P2OUT = BIT7;

  // Setting up LED
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;

  _EINT(); // enable interrupts

  // Blink LED several times at the beginning of the program
  for (int i = 0; i < 5; i++) {
    P1OUT ^= BIT0;
    __delay_cycles(100000);
  }
  P1OUT &= ~BIT0;

  while (1) {
    if (ModeCounter == 1) {
      LPM0;
    }
    if (ModeCounter == 2) {
      LPM1;
    }
    if (ModeCounter == 3) {
      LPM2;
    }
    if (ModeCounter == 4) {
      LPM3;
    }
    if (ModeCounter == 5) {
      LPM4;
    }
  }
}

// Button ISR for going through LPMs
void buttonLPM_ISR(void) __interrupt [PORT2_VECTOR]{
  if (P2IV == P2IV_P2IFG7) {
    Blink++;
    if (Blink == 1) {
      P1OUT |= BIT0;
      ModeCounter = 1;
    }
    if (Blink == 2) {
      P1OUT |= BIT0;
      ModeCounter = 2;
    }
    if (Blink == 3) {
      P1OUT |= BIT0;
      ModeCounter = 3;
    }
    if (Blink == 4) {
      P1OUT |= BIT0;
      ModeCounter = 4;
    }
    if (Blink == 5) {
      P1OUT |= BIT0;
      ModeCounter = 5;
    }
    if (Blink > 5) {
      Blink = 0;
      ModeCounter = 0;
      P1OUT &= ~BIT0;
    }
  }
}