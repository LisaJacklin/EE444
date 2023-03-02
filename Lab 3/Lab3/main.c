/*
Lisa Jacklin
EE 444 Lab 3
2/23/2023

OBJECTIVES:
1. SET SMCLK AND MCLK TO BE 16 MHZ
2. SETUP BUTTON SWITCH S1
2. LET THE BUTTON SWITCH START AND CLEAR THE TIMER
3. TAKE 8 SAMPLES OF THE INTERNAL TEMPERATURE SENSOR USING SMCLK AS THE ADC12 CLK
4. IN THE ADC12 ISR CAPTURE CURRENT TIMER VLUE, CALCULATE AVERAGE TEMP IN C
5. CALIBRATE TEMPERATURE SENSOR USING DATA PROVIDED IN TLV STRUCTURE
6. STOP THE TIMER/CAPTURE THE CURRENT VALUE
7. SET BREAKPOINT TO CHECK CALCULATED TEMPERATURE VALUE

*/

#include <msp430.h>
extern int IncrementVcore(void);

int buttonpressed = 0;
unsigned int temperature;         //converted value of temperature

void main(void) {
  //in order to reach the required frequency level, the voltage must be bumped up.
  IncrementVcore();
  IncrementVcore();
  //IncrementVcore();

//1. SET SMCLK AND MCLK TO BE 16 MHZ
    //first setting up MCLK and SMCLK to 14 MHz
    UCSCTL1 = DCORSEL_6;                                    //DOUBLE CHECK THAT THIS IS THE CORRECT SETTING RANGE
    UCSCTL2 = 427;                                          //N = 487 SO  n+1 = 488. this is the frequency divider
    UCSCTL3 = SELREF_0;                                     //AUTOMATICALLY SETS TO ZERO HERE FOR THE USE OF THE XT1CLK
    //THIS SHOULD SET BOTH SMCLK AND MCLK TO BE DEPENDENT ON THE crystal clock 
    P7SEL |= (BIT0 + BIT1); 
    UCSCTL6 &=~BIT0;                                        //THIS SETS THAT THE EXTERNAL CLOCK CRYSTAL IS ON FOR USAGE.
    UCSCTL4 = SELS__XT1CLK + SELM__XT1CLK;  
                                //THIS MUST BE SET FOR XT1CLK EXTERNAL TO WORK PROPERLY
    //SETTING UP PIN TO ALLOW FOR THE FREQUENCY TO BE CHECKED.
    P11DIR |= BIT0;                                        // sets the bit for the base clock frequency
    P11SEL |= BIT0;                                        //selects the correct clock for the ACLK
   
// 2. SETUP BUTTON SWITCH S1
    //SETUP BUTTON SWITCH S2 -- BOARD 3 DOES NOT HAVE A S1 BUTTON 
    //note THAT THE BUTTON SWITCH S2 IS PIN 2.7
      P2DIR &= ~BIT7;                                      //sets to input direction
      P2IES &=~ BIT7;                                      //allows interrupts for this button
      P2IE |= BIT7;                                        // interrupts are allowed for this button                  
      P2REN = BIT7;                                        //input with pull up resistor 
      P2OUT = BIT7;                                        //also required to enable pull up resistor.

  //SET TIMER A FOR US TO BE ABLE TO SEE THE RESULTS TAKEN FOR ANY OF THE ADC12 TIMER....?
    //TIMER SETUP FOR MAIN OBJECTIVE
     P7SEL |= 0X03;                                        //XT1 CLOCK PINS
     UCSCTL6 &= ~(XT1OFF);                                 //CHECKING TO SEE IF THE CLOCK IS OFF
     UCSCTL6 |= XCAP_3;                                    //LOAD CAPACITOR SPECIFYER

      do {
      UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);                   //clear the XT1 and DCO flags
      SFRIFG1 &= ~OFIFG;                                  //clear the OSC fault flag
      } while ((SFRIFG1 & OFIFG));

     //sets timer A to depend on the SMCLK and to be continuous 
     TA0CTL = TASSEL__SMCLK + MC__CONTINUOUS;

//NOW, SETTING UP THE ADC12
    //noting to set the clk to 16 cycles in the sampling period.
    //ADJUST THE SAMPLING PERIOD, CLOCK IS ON, AND BASE IS SET TO SMCLK.
    ADC12CTL0 = (ADC12SHT02_H | ADC12ON_L | ADC12ENC_L | ADC12REFON_L | ADC12MSC ); 
    ADC12CTL1 = (ADC12SSEL_3 | ADC12SHS_1); 
    ADC12CTL2 = ADC12TCOFF | ADC12RES_2;                //THIS SHOULD SET THE TEMPERATURE SENSOR TO BE ONE
    ADC12IE = ADC12IE0;                                  //enables inTerrupts
    //note THAT 1 IS OFF FOR THE SENSOR
    ADC12MCTL0 = ADC12INCH_10 + ADC12SREF0;              //THIS IS THE TEMPERATURE DIODE

    P6SEL |= BIT0;                                      //THIS SHOULD BE THE OUTPUT FOR ADC
    P1DIR |= BIT0;
  //EVERYTHING ELSE HAPPENS WITHIN THE IRS FOR BUTTON/ADC12A

  _EINT();
  LPM0;
}

//TO USE THE TIMER, SETUP THE BUTTONS ISR
void InterruptService_BUTTON(void) __interrupt [PORT2_VECTOR] {
  //this must LET THE BUTTON SWITCH START AND CLEAR THE TIMER
  //this stars and clear the timer each time the button is pressed.

  if(P2IV==P2IV_P2IFG7) {
    buttonpressed ^= BIT0; //each time button is pressed, toggle BUTTONPRESS
  }
  //this will clear the timer before counting for 8 samples....?
  if (buttonpressed==1) { 
    TA0CTL|=TACLR; //this will clear the timer when the button is pressed
    //THIS WILL START THE CONVERSION AND ENABLES CONVERSION WHEN THE BUTTON IS PRESSED
    ADC12CTL0 |= ADC12ENC | ADC12SC;                     //ENABLES CONVERSION AND SAYS TO START CONVERSION
    P5OUT ^= BIT0;
    temperature = ADC12MEM0;    //raw converted value

  }

 }

//THEN SETUP THE ADC12 ISR
 void InterruptSeverice_ADC12A(void) __interrupt [ADC12_VECTOR] {
 //IN THE ADC12 ISR CAPTURE CURRENT TIMER VLUE, CALCULATE AVERAGE TEMP IN C


  ADC12CTL0 = ADC12ENC_L;       //this should disable the adc12 before turning off.
  ADC12CTL0 = ADC12ON_L;        //turns the ADC12 off
  ADC12CTL0 = REFON_L;          //turns off the reference voltage.

  if (ADC12IV != 0x0006) {
    for (;;){}                  //loop here incase an error pops
  }
  else{
    temperature = ADC12MEM0;    //raw converted value
    temperature *= 4200;        //scaling factor (Vref/temp coefficient)
    temperature += 2048;       //allow for rounding in division

    //temperature /= 4096;  this should be the range of ADC12 that must be divided.

    temperature -= 2780;      //subtract offset to give celsius.
    //need to now display the value for temperature...


  }

  
 }

