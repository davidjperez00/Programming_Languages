/* Includes */
#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

/** GLOBAL Variables */
volatile uint8_t* pPINB;
volatile uint8_t* pDDRB;
volatile uint8_t* pPORTB;

/* Function Declarations */
int8_t debouncePin(volatile uint8_t* mmrPINx, uint8_t bitToRead);
void myHardDelay(uint32_t delayInMsec);

// Main Function
int main(void)
{

  init();

  Serial.begin(9600);
    /* Define our pointers to the GPIO registers */
  pPINB = 0x23;
  pDDRB = 0x24;
  pPORTB = 0x25;

  
    //Initialize Variables
  uint8_t pinState;


    // set PB0 as an input
  *pDDRB = (*pDDRB) & (~0x01);  //0b00000000

  // Set PB0 as having a internal pullup resistor enabled
  *pPORTB = (*pPORTB) | 0x01; //0b00000001

  //Read PB0 first
   pinState = *pPINB & 0x01; // 0b00000001

   while(1)
   {
    
    int8_t state = debouncePin(0x23, 0x01);
   }



}

int8_t debouncePin(volatile uint8_t* mmrPINx, uint8_t bitToRead)
{
        //read PB0
        uint8_t firstDebouceSample = *mmrPINx & bitToRead;
        
        //Delay for 67 milliseconds
        //find a better delay value for the debounce function
        myHardDelay(20);

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


void myHardDelay (uint32_t delayInMsec)
  {
    volatile uint32_t i; 
    uint32_t delayCount = delayInMsec * 373;
    for(i=0;i <delayCount; i++){
      //do nothing
    }
  }
