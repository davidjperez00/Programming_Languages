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

//Registers for debouncing function
volatile uint8_t* pPINB;
volatile uint8_t* pDDRB;
volatile uint8_t* pPORTB;

/** Function declarations **/
int8_t debouncePin(volatile uint8_t* mmrPINx, uint8_t bitToRead);
void myHardDelay(uint32_t delayInMsec);


int main()
{
  //assign register values to variables
  pPINB = 0x23;
  pDDRB = 0x24;
  pPORTB = 0x25;
  
    //Seting up registers for debouncing function
    // set PB0 as an input
  *pDDRB = 0x00;  //0b00000000

  // Set PB0 as having a internal pullup resistor enabled
  *pPORTB = (*pPORTB) | 0x01; //0b00000001
    /*
    Specifications:
    1.Output a square wave on pin OC1A, which is PB1
    2. Make the duty cycle customizable, and make it proportional to OCR1A / TOP (aka non-inverting mode)
    3. Use the fast PWM mode that sets the TOP value as ICR1A
    4. Use a Timer/counter 1 clock source of f_MCU / 1.
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

    //Set the registers for Fast PWM

    //Configure PB1 as an output
    REG_DDRB = 0x02;

    //TCCR1A contains COM1A[1:0] bit field and the WGM1[1:0] partial bit field.
    // COM1A[1:0] cat 0b0000 cat WGM1[1:0]= 0b10 == 0b10000010 == 0x82
    REG_TCCR1A = 0x82;

     //Configure the TOP value aka ICR1AH and ICR1AL
    uint16_t top_value = 40000;
    REG_ICR1H = top_value >> 8;
    REG_ICR1L = top_value & 0x00FF;

    //set a small high time to prevent the servo from moving once the TCCR1B register is initialized
    uint16_t high_time = 1;
    REG_OCR1AH = high_time >> 8;
    REG_OCR1AL = high_time & 0x00FF;

    //TCCR1B = 0b000 cat WGM1[3:2] cat CS1[2:0] == 0b000 cat 0b11 cat 0b010 == 00011010 == 0x1A
    REG_TCCR1B = 0x1A;

    //initialize flag for the servo direction where a 0 rotates clockwise and a 1 rotates counter clockwise
   int8_t servoDirection = 0;
  while(1)
  {
      //debounce the pushbutton
      int8_t state1 = debouncePin(0x23, 0x01);
       
      //check if the button is done bouncing 
      if(state1 == (-1) | state1 == 1)
      {
        int8_t state2 = *pPINB & 0x01;
        //check for a state change on PB0
        if(state1 != state2)
        {
          //check if the button is being pushed
          if(state1 == (-1))
          {
            //rotate the servo in a clockwise directoins
            if(servoDirection == 0)
            {
          
              for(volatile uint32_t z=0; z < 100000; z++)
              {
                high_time = 2250;
                REG_OCR1AH = high_time >> 8;
                REG_OCR1AL = high_time & 0x00FF;
              }
               high_time = 0;
               REG_OCR1AH = high_time >> 8;
               REG_OCR1AL = high_time & 0x00FF;
               //change the servo direction next time loop occurs
               servoDirection = 1;
            }
            
            //rotate the servo counterclockwise
            else if (servoDirection == 1)
            {
              for(volatile uint32_t j=0; j < 100000; j++)
              {
                high_time = 3800;
                REG_OCR1AH = high_time >> 8;
                REG_OCR1AL = high_time & 0x00FF;
              }
               high_time = 0;
               REG_OCR1AH = high_time >> 8;
               REG_OCR1AL = high_time & 0x00FF;
               
               ///change the servo direction next time loop occurs
               servoDirection = 0;
            }
          }
        }
      }
  }
}


int8_t debouncePin(volatile uint8_t* mmrPINx, uint8_t bitToRead)
{
        //read PB0
        uint8_t firstDebouceSample = *mmrPINx & bitToRead;
        
        //Delay for 30 milliseconds
        myHardDelay(30);

        uint8_t secondDebouceSample = *mmrPINx & bitToRead;

        if(firstDebouceSample == secondDebouceSample)
        {
          //The pin is successfully debounced
          if(firstDebouceSample == 0x00)
          {
            //The pin is being pushed (debounced and low)
            return -1;
          }
          else
          {
            // The pin is not being pushed (debounced and high)
            return 1;
          }
        }
        else{
          // The pin may be bouncing
          return 0;
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
