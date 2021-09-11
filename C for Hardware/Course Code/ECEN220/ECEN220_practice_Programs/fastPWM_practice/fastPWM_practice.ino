/** Includes **/
#include <stdint.h>
//#include <Arduino.h>


/** Memory mapped register defines **/
#define REG_DDRB (*((volatile uint8_t*) 0x24))

#define REG_TCCR1A (*((volatile uint8_t*) 0x80))
#define REG_TCCR1B (*((volatile uint8_t*) 0x81))
#define REG_OCR1AH (*((volatile uint8_t*) 0x89))
#define REG_OCR1AL (*((volatile uint8_t*) 0x88))

/** Function declarations **/
void myHardDelay(uint32_t delayInMsec);

int main() 
{
  /*
    Specifications:
    1.Output a square wave on pin OC1A, which is PB1
    2. Make the duty cycle customizable, and make it proportional to OCR1A / TOP (aka non-inverting mode)
    3. Use the fast PWM mode that sets the TOP value as 0x03FF == 1023
    4. Use a Timer/counter 1 clock source of f_MCU / 1.
      Note: this will give a period of T = TOP * prescalar / f_MCU
   */

   /*
     Determine bit fields
     WGM1[3:0] = 0B0111 for fast PWM mode where TOP = 0x03FF = 1023
     COM1A[1:0] = 0b10 for non-inverting mode on pin OCR1A / PB1
     CS1[2:0] = 0b001 for a clock prescalar of / 1 (no prescalar)
        Now our timer will increment every 1/16MHz = 62.5ns
    */

    // Set the registers

    //Configure PB1 as an output
    REG_DDRB = 02;
    //TCCR1A contains COM1A[1:0] bit field and the WGM1[1:0] partial bit field.
    //    COM1A[1:0] cat 0b0000 cat WGM1[1:0] == 0b10 cat 0b0000 cat 0b11 == 0b10000011 == 0x83
    REG_TCCR1A = 0x83;

    // Set the initial duty cycle to 25%. Do this before the timer is turned on,
     //which happens when CS1 is no longer 0b000 (this is
     //done when TCCR1B is configured, so do it before then!)
    uint16_t high_time = 256;
    REG_OCR1AH = high_time >> 8;
    REG_OCR1AL = high_time & 0x00FF;
 
    //TCCR1B = 0b000 cat WGM1[3:2] cat CS1[2:0] == 0b000 cat 0b01 cat 0b001 == 0b00001001 == 0x09
    REG_TCCR1B = 0x09;

    //continuously cycle the brightness 
    while(1)
    {
        for(high_time = 0; high_time < 1024; high_time++)
        {
          REG_OCR1AH = high_time >> 8;
          REG_OCR1AL = high_time & 0x00FF;
         myHardDelay(1);
        }

          myHardDelay(100);
//        for(high_time = 1023; high_time > 0; high_time--)
//        {
//          REG_OCR1AH = high_time >> 8;
//          REG_OCR1AL = high_time & 0x00FF;
//          myHardDelay(1);
//        }
      
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
