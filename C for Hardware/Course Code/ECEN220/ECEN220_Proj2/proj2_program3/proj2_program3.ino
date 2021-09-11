/** Includes **/
#include <stdint.h>
#include <stdio.h>

/** GLOBAL Variables */
volatile uint8_t* pDDRB;
volatile uint8_t* pPORTB;

//global variables for ADC
volatile uint8_t* ioADMUX;  //adc multiplexer selection register
volatile uint8_t* ioADCSRA; // control and status registers
volatile uint8_t* ioADCL;   // adc low conversion bits
volatile uint8_t* ioADCH;   //adc high conversion bits

/* Main Function */
int main(void)
{
  // Define our pointers to the GPIO registers 
  pDDRB = 0x24;
  pPORTB = 0x25;

  // Set PB0 as an output with the DDRB register
  *pDDRB = 0x01; // 0x01 = 0b00000001

  //Populate the MMR pointers with the adresses
  ioADMUX = (volatile uint8_t) 0x7C;
  ioADCSRA= (volatile uint8_t) 0x7A;
  ioADCL = (volatile uint8_t) 0x78;
  ioADCH = (volatile uint8_t) 0x79;

  /*First, configure the ADMUX register */
  //REFS[1:0] = 0b01 for AVCC (0b11 for 1.1v reference)
  // bit 4 of mux == 0b0
  //ADLAR(bit 5) = 0b0 to be "right adjusted"
  //MUX[3:0] = 0b0010 to select ADC2 which is pin A2 on arduino or PC2
  // concat and place in ioADMUX == 0b01000010
  *ioADMUX = 0x42;

  /* Next, configure the ADCSRA register */
  //ADEN = 0b1 to enable ADC
  //ADSC = 0b0 so that that we don't start conversion early
  //bits 5 4 3 we ignore and set to 0's
  // ADPS[2:0] = 0b111 to selecte the prescalar division ratio as 128
  // so that (16Mhz / 128 = 125Khz, which is in between 50 and 200khz as per spec
  //concat and place in ioADCSRA == 0b10000111 = 0x87
  *ioADCSRA = 0x87;



//Do an infinite loop that continously prints the ADC value after it finished converting
  while(1)
  {
      /* Set PB0 high */
      *pPORTB = 0x01;
    
      //Start the ADC conversion
      //To do this, we need to set ADSC (thich is the bit 6 of the
      //ADCSRA register) to a 0b1
      //Read-write-modify
      //  *ioADCSRA = (*ioADCSRA) | 0x40;   //0b01000000
      *ioADCSRA |= 0x40;  // same as line above

      /* wait for ADC to finish converting */

      while(((*ioADCSRA) & 0x40) != 0x00)
      {
        // loop untils bit 6 of ADCSRA is 0 
        // indicating a completed conversion
      }
      
      /* Clear PB1 low */
      *pPORTB = 0x00;

    /* DO WE NEED TO INCLUDE CONVERTING TO VOLTAGES 
      AND LEFT SHITING THE 8 TO 16 BIT */
      // HOW DO WE KNOW HOW LONG THE CLOCK CYCLE ON OUR MICROCONTROLLER IS
      //
  }
}
