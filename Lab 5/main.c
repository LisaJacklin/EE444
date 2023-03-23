/*
Lisa Jacklin
EE 444 lab5
3/9/2023

Objective: 
- write a program that:
  - starts a slow timer that can measure time at 1sec resolution
  - calibrate the temperature sensor
  -setup the UART communication with the PC
  - sleeps unless the user presses a button
  -when pressed:
    - take 8 samples and average them
    - convert into degrees celsius
    - and setup output message
*/

#include <msp430.h>
extern int IncrementVcore(void);

//tens                               10         20        30      40         50       60
//ones                      0123456789012345678901234567890123456789012345678901234567890123    the rest of this is to go to the next line 
volatile char message[] = {"000. The temperature is 00 *C. Running time is 0:00  "};

//all variables are defined here before being used elseware.
volatile int messageInc = 0;                                      //this will increment the char through the msg above
int counter = 0;
int duration = 0;
int ButtonPressed;
unsigned int ref30;// = *((unsigned int *)0x01A1A);
unsigned int ref85;// = *((unsigned int *)0x01A1C);
float slope;// = (85 - 30) / (ref85 - ref30);
volatile int temperature;

void main(void) {

  ref30 = *((unsigned int *)0x01A1A);
  ref85 = *((unsigned int *)0x01A1C);
  slope = (85 - 30) / (ref85 - ref30);

//note that in order to reach the correct frequency, the core voltage must be pulled up.
  IncrementVcore();
  IncrementVcore();
  IncrementVcore();

  //these two are the new line and carriage that must be added to the message.
  message[51] = 10;
  message[52] = 13;

   //the first thing to set up is the SMCLK to be 17 MHz in frequency
   UCSCTL1 = DCORSEL_6;                           //THIS SETS THE FREQUENCY RANGE
   UCSCTL2 = 518;                                 //THIS SETS THE DIVISOR FOR FREQUENCY
   P7SEL |= (BIT0 + BIT1);                        //MAKES SURE THAT THE EXTERNAL XTI IS IN USE
   UCSCTL3 = 0;                                   //USING XTI exterinal crystal AS THE REFERENCED CLOCK

   UCSCTL4 = SELS_3 + SELM_3 +SELA__XT1CLK;                     //SELM 0 SETS THE XTI SOURCE  
 
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
  UCA1CTL1 &= ~UCSWRST;
  UCA1IFG = 0;
   
  UCA1IE |= UCTXIE;                               //THIS WILL ENABLE TX INTERRUPTS              
     
  //PUSH BUTTON 
    //push button is P2.7 so BIT7 is what we are messing with
    P2DIR &= ~BIT7;                              //sets to input direction
    P2SEL &= ~BIT7;                              //confirmation of I/O mode selection
    P2IE |= BIT7;                                // interrupts are allowed for this button                  
    P2IES |= BIT7;                               //falling edge trigger
    P2REN = BIT7;                                //input with pull up resistor 
    P2OUT = BIT7;                                //also required to enable pull up resistor.

//SETTING UP THE ADC12 NOW TO WORK WITH SMCLK
    REFCTL0 = REFGENACT + REFMSTR + REFVSEL_0 + REFON;//reference voltage setup 1.5V and output mode

    ADC12CTL0 = ADC12SHT0_15 + ADC12MSC + ADC12ON;          //TURNS ON THE ADC12, AND USES 1024 ADC12CLK CYCLES
    ADC12CTL1 = ADC12SHS_0 + ADC12SHP + ADC12SSEL_3 + ADC12CONSEQ_1; //THIS SHOULD SET SMCLK AS THE BASE, SINGLE/SINGLE, AND SAMPLING TIMER

    ADC12MCTL0 = ADC12INCH_10 + ADC12SREF_1;     //this will set the temerature diode AND REFERENCE VOLTAGE;
    ADC12MCTL1 = ADC12INCH_10 + ADC12SREF_1;
    ADC12MCTL2 = ADC12INCH_10 + ADC12SREF_1;
    ADC12MCTL3 = ADC12INCH_10 + ADC12SREF_1;
    ADC12MCTL4 = ADC12INCH_10 + ADC12SREF_1;
    ADC12MCTL5 = ADC12INCH_10 + ADC12SREF_1;
    ADC12MCTL6 = ADC12INCH_10 + ADC12SREF_1;
    ADC12MCTL7 = ADC12INCH_10 + ADC12SREF_1 + ADC12EOS;
    ADC12IE |= ADC12IE7;                        //THIS IS WHERE THE ISR WOULD COME IN
    ADC12CTL0 |= ADC12ENC;

//NOW SETTING UP THE TIMER TO SEE HOW LONG IT TAKES TO RUN
   TA1CTL = TASSEL__ACLK + MC__CONTINOUS;     //SET UP TO WORK WITH THE SMCLK AS REFERENCE
   TA1CCTL0 |= CCIE;                            //INTERRUPTS ARE ENABLED
   TA1CCR0 = 32768;                              //THIS IS THE TOP VALUE OF THE TIMER set to 1second sets

  _EINT();
  LPM0;
}

//I can confirm that the IRS for the button can be accessed
void InterruptService_BUTTON(void) __interrupt [PORT2_VECTOR] {
    counter++;                                    //loop itiration is now counting up          

    if (P2IV == P2IV_P2IFG7) {
      if (ButtonPressed == 0) { //this extra if statement is really not needed, but keeping for now
         // timer ^= BIT0;
          //displays counter values to user
          message[0] = (counter / 100) + '0';
          message[1] = (counter % 100)/10 + '0';
          message[2] = (counter % 10)+'0';

          //begin sending the message to the user
          UCA1TXBUF = message[messageInc];

       ButtonPressed = 1;                          //verify and set the loop again.
       ADC12CTL0 |= ADC12SC;                       //sampling and conversion begins

       //debug_printf("\nduration = %i", duration);

       
      } 
   }
//duration = TA0R;  //will display the total time in duration variable.
}

void USC1_ISR(void) __interrupt[USCI_A1_VECTOR] {                                          
  switch (UCA1IV) {                                 //this is what tells us the interrupt vector is for vector 1
                                                    //now we have to give cases for what we should do 
  case 0:break;
  case 2: break;                                    //this should be fo the usci vector a1 
  case 4://this is for transmitting interrupt

    if (messageInc >= 52) {
      messageInc = 0;
      ButtonPressed = 0;
      break;
      }
    else {
      messageInc++; //increments the character in message
      UCA1TXBUF = message[messageInc];              //print the next value of the message
    }  
    break;
  default: break;                                   //if nothing happens then you can get out of the ISR
  }
}

//TIMER A INTERRUPT SERVICE ROUTINE
void TimerA_ISR(void) __interrupt[TIMER1_A0_VECTOR]{
  duration++;                                        //increments counter by 1 second each type the ISR is reached
  }

//ACD12 INTERRUPT SERVICE ROUTINE
  void ACD12_ISR (void) __interrupt [ADC12_VECTOR] {

    unsigned int adcAverage;
    //give the result to the memory location....
  if (ADC12IV == ADC12IV_ADC12IFG7) { // CHECKS TO SEE IF THE FLAG HAS BEEN SET BEFORE TESTING AND AVERAGING
      adcAverage = (ADC12MEM0 / 8) + (ADC12MEM1 / 8) + (ADC12MEM2 / 8) + (ADC12MEM3 / 8) + (ADC12MEM4 / 8) + (ADC12MEM5 / 8) + (ADC12MEM6 / 8) + (ADC12MEM7 / 8);
      temperature = (adcAverage - ref30) * slope + 30;


      // temperature values 
      message[24] = temperature / 10 + '0'; //TENS PLACE
      message[25] = (temperature % 10) + '0'; //singles place

      // timer values
          message[47] = (duration / 60) + '0';
          message[49] = (duration % 60) / 10 + '0';
          message[50] = (duration % 10) + '0';

          //debug_printf("\nduration = %i", duration);
          //debug_printf(" message[47] = %i", message[47]);
          //debug_printf(" message[49] = %i", message[49]);
          //debug_printf(" message[50] = %i", message[50]);

      //lastly, the USCI_A1 interrupts should be allowed to take again/no longer blocked.
      UCA1IE |= UCTXIE;
    }

  }

  