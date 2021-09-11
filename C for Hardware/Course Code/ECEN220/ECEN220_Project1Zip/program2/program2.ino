/** Includes **/
// arduino wiring library (contains serial.begin() and serial.write()
#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>  // contains sprintf() and snprintf()

/** Global Variable **/
volatile uint8_t* ioPORTC;
volatile uint8_t* ioDDRC;
uint32_t loopCount;
char message[80];

int main() 
{
  /* initialize the arduino wiring library */
  init();

  /* inintialize the serial port with a band of 9600 bits per second */
  Serial.begin(9600);

  /* load pointers with adresses */
  ioPORTC = 0x28;
  ioDDRC = 0x27;

  /* make PC2 an output */
  *ioDDRC = 0x04;

  /* Infinite loop */
  loopCount = 0;
  while(1)
  {
    /* set PC2 high */
    *ioPORTC = 0x04;

    /* write loop number to serial port */
    loopCount = loopCount + 1;
    sprintf(message, "This is loop %lu \n", loopCount);
    Serial.write(message);

    /* Set PC2 low */
    *ioPORTC = 0x00;
  }
  return 0;
}
