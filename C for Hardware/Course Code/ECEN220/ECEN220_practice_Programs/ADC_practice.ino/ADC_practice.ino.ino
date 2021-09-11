/** Includes **/
#include <stdint.h>
#include <Arduino.h>
#include <stdio.h>


/** memory mapped registers also our global variables **/
volatile uint8_t* ioADMUX;  //adc multiplexer selection register
volatile uint8_t* ioADCSRA;
volatile uint8_t* ioADCL;
volatile uint8_t* ioADCH;



/** Main function **/
int main(void){
  
  //Initialize the Arduino Wiring Library
  init();

  //Set up our serial port at 9600 bps (baud rate)
  Serial.begin(9600);
  
  //Populate the MMR pointers with the adresses
  ioADMUX = (volatile uint8_t) 0x7C;
  ioADCSRA= (volatile uint8_t) 0x7A;
  ioADCL = (volatile uint8_t) 0x78;
  ioADCH = (volatile uint8_t) 0x79;

  // Set up the ADC

  //First, we will configure the ADMUX register
  /*REFS[1:0] = 0b01 for selecting AREF as the voltage 
    reference as the ADC input maximum */
  /*ADLAR = 0b0 to right-shift the output ADC code 
  in the ADCL and ADCH registers */
  /* MUX [3:0] = 0b0001 to select the ADC 
  innput as pin A1 (aka ADC0, or PC1)*/
  /* ADMUX registers is going to be: 0b00 cat 0b0 cat 0b0 cat 0b0000
    /* this corresponds to : REFS[1:0] cat ADLAR cat empty-bit cat MUX[3:0]
  //ADMUX is going to end up as 0b01000001 == 0x41 */
  *ioADMUX = 0x41;

  //Next, configure the ADCSRA register
  //ADEN = 0b1 to enable the ADC
  //ADSC = 0b0 so that we don't start a conversion prematurely
  //Bits 5,4,3 we will ignore and just set the 0's
  // ADPS[2:0] = 0b111 to selecte the prescalar division ratio as 128
  // so that (16Mhz / 128 = 125Khz, which is in between 50 and 200khz as per spec
  //ADCSRA is going to be: 0b1 cat 0b0 cat 0b000 cat 0b111
  // That comes out to be 0b10000111 = 0x87
  *ioADCSRA = 0x87;

  //Do an infinite loop that continously prints the ADC value after it finished converting
  while(1)
  {
      //Start the ADC conversion
      //To do this, we need to set ADSC (thich is the bit 6 of the
      //ADCSRA register) to a 0b1
      //Read-write-modify
    //  *ioADCSRA = (*ioADCSRA) | 0x40;   //0b01000000
      *ioADCSRA |= 0x40;  // same as line above

      /*Wait for the ADC to finish converting */
      
      while(((*ioADCSRA) & 0x40) != 0x00)
      {
          //loop until ADCSRA bit 6 (which is ADSC) it a 0b0,
            //indicating that the ADC conversion has completed
      }

      //The ADC conversion has completed!
      //Now read the ADC value
      //Read ADCL register first
      uint8_t adc_low_value = *ioADCL;

      //Now, read the ADCH register
      uint8_t adc_high_value = *ioADCH;

      //combine the high and low value into a single 16 usigned integer
      uint16_t adcResult = adc_low_value & 0x00FF;
      adcResult = (adcResult) | ((uint16_t)adc_high_value << 8);

      //Now adcResult contains our 10-bit ADC value in its 10 least sig bits

      //print out the ADC output code
      char message[80];
      sprintf(message, "ADC output code: %u \n", adcResult);
      Serial.write(message);


      float AdcVoltage;

      AdcVoltage = (5.0 / 1024.0) * adcResult;
//      sprintf(message, "ADC voltage is: %f\n", AdcVoltage);
      Serial.write("ADC voltage is: ");
      Serial.print(AdcVoltage);
      Serial.write("v\n");
      


      
  }
}
