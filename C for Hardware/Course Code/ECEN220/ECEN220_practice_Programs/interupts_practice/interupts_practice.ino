#include <stdint.h>
#include <avr/interrupt.h>

/** Memory mapped register defines **/
#define REG_DDRB (*((volatile uint8_t*) 0x24))
#define REG_PORTB (*((volatile uint8_t*) 0x25))

#define REG_TCCR1A (*((volatile uint8_t*) 0x80))
#define REG_TCCR1B (*((volatile uint8_t*) 0x81))
#define REG_OCR1AH (*((volatile uint8_t*) 0x89))
#define REG_OCR1AL (*((volatile uint8_t*) 0x88))
#define REG_TIMSK1 (*((volatile uint8_t*) 0x6F))

#define REG_SREG   (*((volatile uint8_t*) 0x5F))

/** Defines **/
#define BIT0  0X01
#define BIT1  0X02
#define BIT2  0X04
#define BIT3  0X08
#define BIT4  0X10
#define BIT5  0X20
#define BIT6  0X40
#define BIT7  0X80

/** Global variables **/
volatile uint32_t g_secondsElapsed;
volatile uint8_t g_newSecondsValue;


int main()
{
  /* Specifications:
   *  1.Use timeer 1 in CTC mode to generate a "Timer 1 compare match" interrupt every 1 second
   *  2. In the Timer 1 compare match ISR, increment a global variable that indicaTs how many 
   *  seconds have elasped since the start of the program. Also seet an 8-bit variable indicating the "seconds
   *  elapsed" variable has incremented
   *  3.
   *  
   */

   /*
    * Determine bit fields and relevant registers
    * Presclar >= f_MCU / (f_desired * TOP_max) == 16 MHz / (1/(1s) * 65536) == 244.14
    * 
    * WGM1[3:0] = 0b0100 for "Clear timer on Compare Match" (CTC)
    * mode, where the timer resets back to 0 when its value matches the output compare A registers OCR1A
    * COM1A[1:0] = 0b00, timer peripheral should not have control over pint OC1A/PB1
    * CS1[2:0] = 0b100 for a prescalar of 256
    * 
    * T = TOP * prescalar / f_MCU
    * OCR1A = TOP = T * f_MCU / prescalar == 1s * 16MHz / 256 == 62500
    */

    //Initialize global varibles
    g_newSecondsValue = 0;
    g_secondsElapsed = 0;

    //Set up the registers
    //set PB5 as an output, initially low
    REG_DDRB |= BIT5;
    REG_PORTB &= ~BIT5; // clearing only bit 5

    //Set the period using OCR1AH and OCR1AL
    //what value of OCR1A will get the TOP value in exactly 1s?
    // T = TOP * prescalar / f_mcu
    //OCR1A = TOP = 62500
    const uint16_t TOP = 62500;
    REG_OCR1AH = TOP >> 8;
    REG_OCR1AL = TOP & 0x00FF;

    //TCCR1A contains COM1A[1:0] and WGM1[1:0]: COM1A[1:0] cat 0bxxxx cat WGM1[1:0] == 0b00 cat 0b0000 cat 0b00
    REG_TCCR1A = 0x00;

    //TCCR1B contains WGM1[3:2] and CS1[2:0]
    // 
    REG_TCCR1B = 0x0C;

    //Enable Time 1 copare match interrupt int TIMSK1 register
    REG_TIMSKS1 = BIT1;

    //Enable global interrupts in the SREG register
    REG_SREG |= BIT7;
    
    //infinite loop
    while(1)
    {
      //wait for another second to elapse
      //waiting for g_newSecondsValue to get set to 1
      while (g_newSecondsValue != 1)
      {
        //Wait
      }

      // We know now that there is a new value in g_secondsElapsed
      //Read the value of g_secondsElapsed adn put it in a new local variable
      // BEGINE CRITCAL SECTION OF CODE
      REG_SREG &= ~BIT7; // disable bit 7 (set to 0)
      uint32_t newTime = g_secondsElapsed;
      g_newSecondsValue = 0;
      REG_SREG |= BIT7; //re enable global interrupts
      //END CTRITICAL SECTION OF CODE

      if(newTime % 2 == 0)
      {
        //newTime is even 
        //Turn off PB5
        REG_PORTB &= ~BIT5;
        
      }else
      {
        //newTime is odd
        //Turn on PB5
        REG_PORTB |= BIT5;
      }
      
    }



    
}


/** Interrupt service routines **/
ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
  //This ISR is executed every one second precisely
  
  //increment the seconds elapsed global variable
  g_secondsElapsed++;

  // Set a global variable indicating a new seconds elapsed valeu
  g_newSeconds = 1;
}
