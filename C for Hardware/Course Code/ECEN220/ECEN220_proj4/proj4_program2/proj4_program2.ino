/** Includes **/
#include <stdint.h>
#include <avr/interrupt.h>

/** Memory mapped register defines **/
//Registers for Timer/Counter peripheral
#define REG_TCCR1A (*((volatile uint8_t*) 0x80))
#define REG_TCCR1B (*((volatile uint8_t*) 0x81))
#define REG_OCR1AH (*((volatile uint8_t*) 0x89))
#define REG_OCR1AL (*((volatile uint8_t*) 0x88))
#define REG_TCNT1H (*((volatile uint8_t*) 0x85))
#define REG_TCNT1L (*((volatile uint8_t*) 0x84))

//Global variables for ADC registers
#define REG_ADMUX  (*((volatile uint8_t*) 0x7C))
#define REG_ADCSRA (*((volatile uint8_t*) 0x7A))
#define REG_ADCL   (*((volatile uint8_t*) 0x78))
#define REG_ADCH   (*((volatile uint8_t*) 0x79))

//Interupt registers
#define REG_TIMSK1 (*((volatile uint8_t*) 0x6F))
#define REG_SREG   (*((volatile uint8_t*) 0x5F))

/** Global Variables for delay function **/
volatile uint32_t g_mSecsRemaining;
volatile uint8_t g_myDelayDone;
volatile uint32_t g_adcDataReady;


/** Defines **/
#define BIT0  0X01
#define BIT1  0X02
#define BIT2  0X04
#define BIT3  0X08
#define BIT4  0X10
#define BIT5  0X20
#define BIT6  0X40
#define BIT7  0X80

int main(void)
{
      /* Specifications:
   *  1.Use timer 1 in CTC mode to generate a "Timer 1 compare match" interrupt every 1 millisecond
   *  2.On a compare match call the Timer 1 compare match A ISR to decrement g_mSecsRemaining variable
   *    to create a delay for myDelay() function
   *  3. After the delay is done togle pin PB4
   */

    /*
    * Determine bit fields and relevant registers
    * Presclar >= f_MCU / (f_desired * TOP_max) == 16 MHz / (1000) * 65536) == .24414
    * 
    * WGM1[3:0] = 0b0100 for "Clear timer on Compare Match" (CTC)
    * mode, where the timer resets back to 0 when its value matches the output compare A registers OCR1A
    * COM1A[1:0] = 0b00, timer peripheral should not have control over pin OC1A/PB1
    * CS1[2:0] = 0b001 for a prescalar of 1
    * 
    * T = TOP * prescalar / f_MCU
    * OCR1A = TOP = T * f_MCU / prescalar == .001 * 16MHz / 1 == 16,000
    * 
    */

    //Initialize global variables
    g_mSecsRemaining = 0;
    g_myDelayDone = 0;
    g_adcDataReady = 0;

    //initialize our flag indicating 10 adc conversions and our variable for holding adc samples
    uint8_t tenSamplesFlag = 0;
    uint32_t adcSamples = 0;

    /* Set up Registers for timer peripheral*/
    
    //Set the period using OCR1AH and OCR1AL
    const uint16_t TOP = 16000;
    REG_OCR1AH = TOP >> 8;
    REG_OCR1AL = TOP & 0x00FF;

     //TCCR1A contains COM1A[1:0] and WGM1[1:0]: COM1A[1:0] cat 0bxxxx cat WGM1[1:0] == 0b00 cat 0b0000 cat 0b00
    REG_TCCR1A = 0x00;

    //TCCR1B contains WGM1[3:2] and CS1[2:0] == 0b000 0b01 0b001
    // when the CS1 is no longer 0 the timer begins counting
    REG_TCCR1B = 0x09;

    //Enable timer 1 compare match interrup in TIMSKI register
    REG_TIMSK1 |= BIT1;
    
    //Enable global interrupts in the SREG register
    REG_SREG |= BIT7;

    /** Set up ADC peripheral registers **/
    //REFS[1:0] = 0b11 for internal 1.1v reference
    //ADLAR(bit 5) = 0b0 to be "right adjusted"
    // bit 4 of mux == 0b0 (un-used bit)
    //MUX[3:0] = 0b1000 (to enable temperature sensor)
    //concate bits, ADMUX == 0b 1100 1000
    REG_ADMUX = 0xC8;
    
    /* Next, configure the ADCSRA register */
    //ADEN = 0b1 to enable ADC
    //ADSC = 0b0 so that that we don't start conversion early
    //bits 5 4 we ignore and set to 0's
    //bit 3 = 0b1 to enable adc interrupt
    // ADPS[2:0] = 0b111 to select the prescalar division ratio as 128
    // so that (16Mhz / 128 = 125Khz, which is in between 50 and 200khz as per spec
    //concat and place in ioADCSRA == 0b 1000 1111 == 0x
    REG_ADCSRA = 0x8F;

    while(1)
    {
      /* Start the ADC conversion */
      //To do this, we need to set ADSC (this is bit 6 of the
      //ADCSRA register) to a 0b1
      //Read-write-modify
      REG_ADCSRA |= 0x40;  // same as line above
      
      //Delay for 100ms
      myDelay(100);

      if(g_adcDataReady == 1)
      {
        //BEGIN CRITICAL SECTION OF CODE
        REG_SREG &= ~BIT7; // disable bit 7 (set to 0)
      
        //The ADC conversion has completed!
        //Now read the ADC value
        //Read ADCL register first
        uint8_t adc_low_value = REG_ADCL;
  
        //Now, read the ADCH register
        uint8_t adc_high_value = REG_ADCH;
  
        //combine the high and low value into a single 16 usigned integer
        uint16_t adcResult = adc_low_value & 0x00FF;
        adcResult = (adcResult) | ((uint16_t)adc_high_value << 8);

        //store our 16-bit adc value in a 32-bit to hold all our adc values
        uint32_t adcSamples = adcSamples + adcResult; 

        REG_SREG |= BIT7; //re enable global interrupts
        //END CRITICAL SECTION OF CODE 

        //reseting our flag that indiactees a completed adc conversion
        g_adcDataReady = 0;

        //increment our flag indicating 10 samples
        tenSamplesFlag++;

        // If we have 10 adc samples we're going to print the average of them
        if(tenSamplesFlag == 10)
        {
          
           //BEGIN CRITICAL SECTION OF CODE
           REG_SREG &= ~BIT7; // disable bit 7 (set to 0)

           //find the average of our adc samples
           uint32_t adcAverage = adcSamples / 10;
           
           //begin serial transmission and print our adc average
           Serial.begin(9600);

//           char message[80];
//           sprintf(message,"ADC value %u \n",adcAverage);
//           Serial.write(message);
           Serial.println(adcAverage);



           adcAverage = 0;
           adcSamples = 0;
           
           REG_SREG |= BIT7; //re enable global interrupts
           //END CRITICAL SECTION OF CODE 

          //reset our flag back to 0
          tenSamplesFlag = 0;
        }
      }
    }
}      


void myDelay(uint32_t delayInMsec)
{
    // Reset the Timer to 0
    REG_TCNT1H = 0x00;
    REG_TCNT1L = 0x00;

   //BEGINE CRITICAL SECTION OF CODE
   REG_SREG &= ~BIT7; // disable bit 7 (set to 0)

   //set our interupt milliseconds remaining value to the input to our delay function
   g_mSecsRemaining = delayInMsec; 
   //Also reset our delayDone flag to 0 in case it changed
   g_myDelayDone = 0;
  
   REG_SREG |= BIT7; //re enable global interrupts
   //END CRITICAL SECTION OF CODE

  while(g_myDelayDone != 1)
  {
    //Wait
  } 
}

/** Interrupt Service Routines **/

//ISR for Timer used by delay function
ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
  //Decrement the number of miliseconds remaining
  g_mSecsRemaining--;

  //check if there are any milliseconds remaining
  if(g_mSecsRemaining == 0)
  {
    //this would mean there isn't any milliseconds remaining
    //so we set out indicatior to 1
    g_myDelayDone = 1;
  }
}

//ISR fires when an adc conversion completes
ISR(ADC_vect, ISR_BLOCK)
{
       //Set data flag to 1 indicating ADC conversion is complete and new data available
       g_adcDataReady = 1;
}
