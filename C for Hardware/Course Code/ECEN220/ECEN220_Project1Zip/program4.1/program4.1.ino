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
  // Initialize the Arduino Wiring Library
  init();
  //Initialize the serial port with a band of 9600 bits per second
  Serial.begin(9600);
  
  /* Define our pointers to the GPIO registers */
  pPINB = 0x23;
  pDDRB = 0x24;
  pPORTB = 0x25;

  //Initialize Variables
  uint8_t pinState;

  // set PB0 as an input
  *pDDRB = 0x00;  //0b00000001

  // Set PB0 as having a internal pullup resistor enabled
  *pPORTB = 0x01; //0b00000001

  //Read PB0 first
   pinState = *pPINB & 0x01; // 0b00000001

  while(1)
  {
    /* NOT SURE IF THIS IS SUPPOSED TO GO HERE
      //Read PB0 first
   pinState = *pPINB & 0x01; // 0b00000001
    */
   
      // Check the debounce state from my debounce function
     int8_t state = debouncePin(0x23, 0x01);
    
    if (state == 0)
    {
      // Pin is bouncing
      Serial.write("The pin is bouncing\n");
    }
    else if (state == 1)
    {
      // Pin is debounced high
      Serial.write("The pin is being pushed\n");
    }
    else if (state == -1)
    {
      // Pin is debounced low
      Serial.write("The pin is NOT being pushed\n");
    } 
  }
}

int8_t debouncePin(volatile uint8_t* mmrPINx, uint8_t bitToRead)
{
        //read PB0
        uint8_t firstDebouceSample = *mmrPINx & bitToRead;
        //Delay for 67 milliseconds
        myHardDelay(70);

        uint8_t secondDebouceSample = *mmrPINx & bitToRead;

        if(firstDebouceSample == secondDebouceSample)
        {
          //The pin is successfully debounced
          if(firstDebouceSample == 0x00)
          {
            //The pin is being pushed (debounced and high)
            return 1;
          }
          else
          {
            // The pin is not being pushed (debounced and low)
            return -1;
          }
        }
        else{
          // The pin is bouncing
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
  
