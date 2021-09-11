/** includes **/
#include <stdint.h>


/** Global Variables **/
// means that these varibales can change at anytime
volatile uint8_t* pDDRB;
volatile uint8_t* pPORTB;
volatile uint8_t* pPORTB;
/** Main Function **/

int main(void){
//Define out pointers to the GPIO memory mapped registers
  pPINB = 0x23;
  pDDRB = 0x24;
  pPORTB = 0x25;

  //Set PB0 as an output with the DDRB register, and make PB1 an input 
 // (with the rest of the pint on portB)
  
  *pDDRB = 0x01; // 0x01 == 0b00000001

  //enable the pullup resistor for PB1
  *pPORTB = 0x02; // 0b 00000010

  // infinite loop
  volatile uint32_t i;
  while(1){

      // & refers to bitwise AND
      // && refers to logical AND
      if(*pPINB == (*pPINB & ob11111101))
      {
         //PB1 is being pressed, set the LED high
         *pPORTB = 0x03;  //0b0000011

      } else
      {
          //PB1 is not being pressed, set the LED low
          *pPORTB = 0x02; // 0b00000010
      }

      

      /*
      // set PB0 high
      *pPORTB = 0x01;

      // Wait for some time/delay
      for (i = 0; i < 1000; i++){
        // do nothing this is a delay
      }

      //clear PB0 low
      *pPORTB = 0x00;

            // Wait for some time/delay
      for (i = 0; i < 1000; i++){
        // do nothing this is a delay
      }
      */
      
  }
  return -1;
}
