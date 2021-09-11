/** Includes **/
#include <stdint.h>
#include <Arduino.h>
#include <stdio.h>


/** memory mapped registers also our global variables **/
volatile uint8_t* ioADMUX;  //adc multiplexer selection register
volatile uint8_t* ioADCSRA; // control and status registers
volatile uint8_t* ioADCL;   // adc low conversion bits
volatile uint8_t* ioADCH;   //adc high conversion bits

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

      /* ADC conversion is complete */
      //read ADCL value first
      uint8_t adc_low_value = *ioADCL;
      
      //Now, read the ADCH register
      uint8_t adc_high_value = *ioADCH;
     // combine into a 16 bit integer
      uint16_t adcResult = adc_low_value & 0x00FF;
      //convert high value to 16 bits and left shift 8 bits
      adcResult = (adcResult) | ((uint16_t)adc_high_value << 8);


     //print out the ADC output code
      char message[80];
      sprintf(message, "ADC output code: %u \n", adcResult);
      Serial.write(message);

       float AdcVoltage;
       
      //convert the ADC code to a estimated voltage
      AdcVoltage = (5.0 / 1024.0) * adcResult;
      Serial.write("ADC voltage is: ");
      Serial.print(AdcVoltage);
      Serial.write("v\n");
      
    }
  }
  
