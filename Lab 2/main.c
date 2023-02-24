/*
 Lisa Jacklin
 EE 444 Lab 2
 2/15/2023

 Objective: set MCLK and SMCLK to be 25MHz and setup Timer A to generate two PWM signals on two available pins
  on the board. These PwM signals are both set to have a period of 10 us and a duty cycle of 25% and 75% repectively.
*/

#include <msp430.h>
extern int IncrementVcore(void);
void main(void){


IncrementVcore();
IncrementVcore();
IncrementVcore();

//CHECKS THAT XT1 IS NOT TURNED OFF BY DEFAULT
 P7SEL |= (BIT0 + BIT1);

 while (SFRIFG1 & OFIFG) {
  UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
  SFRIFG1 &= ~OFIFG;
 }

//First, seting up the MCLK to be as close to 25 MHz like in lab 1
   UCSCTL1 = DCORSEL_6;               //this sets the range for the frequency
   UCSCTL2 = 762;                     //this is the N factoring for the main signal
   UCSCTL4 = SELM__DCOCLK;            // SETS THE DCOCLK AS THE CLOCK SOURCE FOR MCLK
                                      //THAT WAY THERE IS NO DIVISION ON THE CLOCK FREQUENCY 
   UCSCTL6 &= ~XT1OFF;
   //This is the output destination of the signal from the MCLK
   P11DIR |= BIT1;
   P11SEL |= BIT1;                    // this should set the functionality to periperial
   P11OUT |= BIT1 ;                   // this should give an output to 11.1

//Then setting SMCLK to be 25 MHz as well so it can be used as Timer A source
  UCSCTL4 = SELS__DCOCLK;             //just like how MCLK was set with DCOCLK as reference, same was done for SMCLK
  UCSCTL8 |= SMCLKREQEN;              //this should disable requests logic for the SMCLK
    //This is the output destination of the signal from the MCLK
   P11DIR |= BIT2;
   P11SEL |= BIT2;                    // this should set the functionality to periperial
  // P11OUT |= BIT2 ;                   // this should give an output to 11.1

 //TIMER SETUP FOR MAIN OBJECTIVE
   //sets timer A1 to depend on the SMCLK and to be continuous 
   TA1CTL = TASSEL__SMCLK + MC__UP;   //THE UP SO WE CAN SET THE TOP VALUE COUNT IN CCR0
   //ta1ctl should apply for all timer A's
   //TAIE |=
   TA1CCTL0 = OUTMOD2;                //THIS SHOULD SET TO TOGGLE/RESET MODE
   
   //SETTING UP THE 25% DUTY CYCLE
   TA1CCR0 = 249;                     //THIS IS THE TOP CYCLE VALUE SINCE IT WOULD RESULT IN 10US RESULT
   TA1CCR1 = 62;                      //VALUE THAT IS COUNTED TO THAT IS 25% OF THE MAX CYCLE VALUE

   P8SEL |= BIT5;                     //PIN 8.5 SELECTED AS TIMERA1 OUTPUT                   
   P8DIR |= BIT5;                     
   
  //SETUP TIMER A0 TO DEPEND ON THE SMCLK AND TO BE CONTINUOUS FOR 75% CYCLE
  //setting up Ta1.2 timer for the second one
  TA1CCTL2 = OUTMOD2;                 //THIS SHOULD SET TO TOGGLE/RESET MODE

  //this needs to be checked to verify that it is setup for the correct section of the timer...
  TA1CCR2 = 186; //VALUE THAT IS COUNTED TO THAT IS 75% OF THE MAX CYCLE VALUE

  //THIS SHOULD SETUP THE OUTPUT FOR THE SECOND CYCLE...
 
   P7SEL |= BIT3;                     //PIN 8.5 SELECTED AS TIMERA1 OUTPUT                   
   P7DIR |= BIT3;                     

_EINT();
LPM0;                                 //THIS IS WHAT WILL BE CHANGING DURING THIS LAB

}
