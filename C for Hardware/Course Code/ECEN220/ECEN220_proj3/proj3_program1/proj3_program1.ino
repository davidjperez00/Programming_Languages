/** Includes **/
#include <stdint.h>


/** Memory mapped register defines **/
#define REG_DDRB (*((volatile uint8_t*) 0x24))

#define REG_TCCR1A (*((volatile uint8_t*) 0x80))
#define REG_TCCR1B (*((volatile uint8_t*) 0x81))
#define REG_OCR1AH (*((volatile uint8_t*) 0x89))
#define REG_OCR1AL (*((volatile uint8_t*) 0x88))

#define REG_ICR1H (*((volatile uint8_t*) 0x87))
#define REG_ICR1L (*((volatile uint8_t*) 0x86))

/** Function declarations **/
void myHardDelay(uint32_t delayInMsec);


int main()
{
    /*
    Specifications:
    1.Output a square wave on pin OC1A, which is PB1
    2. Make the duty cycle customizable, and make it proportional to OCR1A / TOP (aka non-inverting mode)
    3. Use the fast PWM mode that sets the TOP value as ICR1A
    4. Use a Timer/counter 1/8 clock source of f_MCU / 8.
      Note: this will give a period of T = TOP * prescalar / f_MCU
   */


   /*
     Determine bit fields
     WGM1[3:0] = 0B1110 for fast PWM mode where TOP = ICR1A 
     COM1A[1:0] = 0b10 for non-inverting mode on pin OCR1A / PB1

     // determine WHAT PRESCALAR AND ICR1A VALUE TO USE
     CS1[2:0] = 0b010 for a clock prescalar of / 8 
        Now our timer will increment every 8/16MHz = 5x10^(-7) 50 micosecond??
    */

    //Set the registers

    //Configure PB1 as an output
    REG_DDRB = 02;

    //TCCR1A contains COM1A[1:0] bit field and the WGM1[1:0] partial bit field.
    // COM1A[1:0] cat 0b0000 cat WGM1[1:0]= 0b10 == 0b10000010 == 0x82
    REG_TCCR1A = 0x82;

        //Configure the TOP value aka ICR1AH and ICR1AL
    uint16_t top_value = 40000;
    REG_ICR1H = top_value >> 8;
    REG_ICR1L = top_value & 0x00FF;
    
    // Set the initial duty cycle to 25%. Do this before the timer is turned on,
     //which happens when CS1 is no longer 0b000 (this is
     //done when TCCR1B is configured, so do it before then!)
     
    uint16_t high_time = 10000; //to create a 25% duty cycle
   /* uint16_t high_time = 20000; //to create a 50% duty cycle
    uint16_t high_time = 30000; // to create a 75% duty cycle */
    
    REG_OCR1AH = high_time >> 8;
    REG_OCR1AL = high_time & 0x00FF;

    //TCCR1B = 0b000 cat WGM1[3:2] cat CS1[2:0] == 0b000 cat 0b11 cat 0b010 == 00011010 == 0x1A
    REG_TCCR1B = 0x1A;


      //continuously cycle the fast PWM 
    while(1)
    {
    high_time = 10000;
    REG_OCR1AH = high_time >> 8;
    REG_OCR1AL = high_time & 0x00FF;
    }
}

  void myHardDelay(uint32_t delayInMsec)
  {
    volatile uint16_t delayCount;
    volatile uint16_t i;
    delayCount = delayInMsec * 373;
    for(i=0;i <delayCount; i++){
    }
  }
