/** Includes **/
#include <stdint.h>


/** Defines **/
#define BIT0  0X01
#define BIT1  0X02
#define BIT2  0X04
#define BIT3  0X08
#define BIT4  0X10
#define BIT5  0X20
#define BIT6  0X40
#define BIT7  0X80

int main()
{
  /**
   *  Specifications:
   *  1. Configure SPI with:
   *      clock polarity = 0 (SCK idles low)
   *      clock phase = 0 (data sampled on leading edge of SCK, 
   *       (updated on trailing edge)
   *       send MSB first
   *       SCK prescalar = 128 *16MHz / 128) = 125kHz for sck
   *  2. Ssend 2 8-bit unsigned intergers to slave over MOSI over
   *  two 8-bit transfers. The slave will compute the sum of the two numbers
   *  and send back the result when the master initiates a third 8-bit transfer
   *  
   *  
   */

  // SSet THID DEVICE's CS (aka SS) pin (Pin 10, PB2) as an output high
  //Warning the datasheet specifies that if this chip's CS pin gets becomes low for ANY reason
}
