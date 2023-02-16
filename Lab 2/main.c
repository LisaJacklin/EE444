/*
 Lisa Jacklin
 EE 444 Lab 2
 2/15/2023

*/

#include <msp430.h>


void main(void){

//CHECKS THAT XT1 IS NOT TURNED OFF BY DEFAULT
P7SEL |= (BIT0 + BIT1);
while (UCSCTL7 & XT1LFOFFG) {
  UCSCTL7 &= ~XT1LFOFFG;
}

//First, seting up the MCLK to be as close to 25 MHz like in lab 1
   UCSCTL1 = DCORSEL_6;               //this sets the range for the frequency
   UCSCTL2 = 762;                     //this is the N factoring for the main signal
   UCSCTL3 = SELREF_2;                // this will set the reference clock to XT1CLK
   UCSCTL4 = SELM__DCOCLK;            // SETS THE DCOCLK AS THE CLOCK SOURCE FOR MCLK
                                      //DCOCLKDIV is required for MCLK

   //This is the output destination of the signal from the MCLK
   P11SEL |= BIT1;                    // this should set the functionality to periperial
   P11OUT |= BIT1 ;                   // this should give an output to 11.1

//Then setting SMCLK to be 25 MHz as well so it can be used as Timer A source

  UCSCTL4 = SELS__DCOCLK;             //just like how MCLK was set with DCOCLK as reference, same was done for SMCLK
  UCSCTL8 |= SMCLKREQEN;              //this should disable conditional requests for the SMCLK

 //TIMER SETUP FOR MAIN OBJECTIVE
    //note that since we want a 10us period, the frequency should be 6553 MHz...? based on 65535 max
    //ALSO NOTE THAT THERE SHOULD BE ONE SET FOR 25% DUTY N ANOTHER FOR 75% DUTY

   //sets timer A1 to depend on the SMCLK and to be continuous 
   TA1CTL = TASSEL__SMCLK + MC__CONTINUOUS;
   TA1CCTL0 = OUTMOD2;                //THIS SHOULD SET TO TOGGLE/RESET MODE
   
   //SETTING UP THE 25% DUTY CYCLE
   TA1CCR1 = 16353; //VALUE THAT IS COUNTED TO THAT IS 25% OF THE MAX CYCLE VALUE

   P8SEL |= BIT5;                     //PIN 8.5 SELECTED AS TIMERA1 OUTPUT                   
   P8DIR |= BIT5;                     
   P8OUT &= ~BIT5;

  //SETUP TIMER A0 TO DEPEND ON THE SMCLK AND TO BE CONTINUOUS FOR 75% CYCLE
  TA0CTL = TASSEL__SMCLK + MC__CONTINOUS;
  TA0CCTL0 = OUTMOD2;                 //SET TO TOGGLE/RESET MODE

  TA0CCR0 = 49151; //VALUE THAT IS COUNTED TO THAT IS 75% OF THE MAX CYCLE VALUE

  //THIS SHOULD SETUP THE OUTPUT FOR THE SECOND CYCLE...
  P8SEL |= BIT0;
  P8DIR |= BIT0;
  P8OUT &=~BIT0;



//CHECKS TO MAKE SURE THERE ARE NO FAULT FLAGS AND IF SO TO CLEAR THEM
while (SFRIFG1&OFIFG)
{
  UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
  SFRIFG1 &=~OFIFG;
}

_EINT();
LPM0;                                 //THIS IS WHAT WILL BE CHANGING DURING THIS LAB

}
