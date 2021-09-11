/** includes **/
#include <stdint.h>


/** Global Variables **/
unit8_t* pDDRB;
unit8_t* pPORTB;
/** Main Function **/

//Define out pointers to the GPIO memory mapped registers
  pDDRB = 0x24;
  pPORTB = 0x25;

  //Set PB0 as an output with the DDRB register
  *pDDRB = 0x01; // 0x01 == 0b00000001
