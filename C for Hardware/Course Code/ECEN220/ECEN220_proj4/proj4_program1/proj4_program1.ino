/** Includes **/
#include <stdint.h>
#include <avr/interrupt.h>

/** Memory mapped register defines **/
//Registers for GPIO pins
#define REG_DDRB (*((volatile uint8_t*) 0x24))
#define REG_PORTB (*((volatile uint8_t*) 0x25))

//Registers for Timer/Counter peripheral
#define REG_TCCR1A (*((volatile uint8_t*) 0x80))
#define REG_TCCR1B (*((volatile uint8_t*) 0x81))
#define REG_OCR1AH (*((volatile uint8_t*) 0x89))
#define REG_OCR1AL (*((volatile uint8_t*) 0x88))
#define REG_TCNT1H (*((volatile uint8_t*) 0x85))
#define REG_TCNT1L (*((volatile uint8_t*) 0x84))

//Interupt registers
#define REG_TIMSK1 (*((volatile uint8_t*) 0x6F))
#define REG_SREG   (*((volatile uint8_t*) 0x5F))

/** Global Variables **/
volatile uint32_t g_mSecsRemaining;
volatile uint8_t g_myDelayDone;

/** Defines **/
#define BIT0  0X01
#define BIT1  0X02
#define BIT2  0X04
#define BIT3  0X08
#define BIT4  0X10
#define BIT5  0X20
#define BIT6  0X40
#define BIT7  0X80

/* Functions */
void myDelay(uint32_t delayInMsec);

int main()
{
    /* Specifications:
   *  1.Use timer 1 in CTC mode to generate a "Timer 1 compare match" interrupt every 1 millisecond
   *  2.On a compare match call the Timer 1 compare match A ISR to decrement g_mSecsRemaining variable
   *    to create a delay for myDelay() function
   *  3. After the delay is done toggle pin PB4
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

    /* Set up Registers */
    //set PB4 as an output and initially low
    REG_DDRB |= BIT4;
    REG_PORTB &= ~BIT4; //clears only bit 4

    //Set the period using OCR1AH and OCR1AL
    const uint16_t TOP = 16000;
    REG_OCR1AH = TOP >> 8;
    REG_OCR1AL = TOP & 0x00FF;

     //TCCR1A contains COM1A[1:0] and WGM1[1:0]: COM1A[1:0] cat 0bxxxx cat WGM1[1:0] == 0b00 cat 0b0000 cat 0b00
    REG_TCCR1A = 0x00;

    //TCCR1B contains WGM1[3:2] and CS1[2:0] == 0b000 0b01 0b001
    REG_TCCR1B = 0x09;
    
     //Enable global interrupts in the SREG register
    REG_SREG |= BIT7;

    //Enable timer 1 compare match interrup in TIMSKI register
    REG_TIMSK1 |= BIT1;

    while(1)
    {
      //Turn PB4 low
      REG_PORTB &= ~BIT4;
      
      myDelay(3);
      
      //set PB4 High
      REG_PORTB |= BIT4;

      myDelay(1);
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
