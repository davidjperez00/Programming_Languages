
#include <stdint.h>

volatile uint8_t* pDDRB;
volatile uint8_t* pPORTB;

void myHardDelay(uint32_t delayInMsec);

int main()
{

  //load pointers with addresses
  pDDRB = 0x24;
  pPORTB = 0x25;


  //make PB1 an output
  *pDDRB = 0x02; // 0x01 = 0b00000010

  
while(1)
{
 
  //set PB1 high then delay for 1 milliseconds
  *pPORTB = 0x02;

  //delay 1 millisecond
  myHardDelay(1);
  
  //set PB1 low then delay for 1 milliseconds
  *pPORTB = 0x00;
  myHardDelay(1);
  }
}
  void myHardDelay(uint32_t delayInMsec)
  {
    volatile uint32_t delayCount;
    delayCount = delayInMsec * 373;
    for(volatile uint32_t i=0;i <delayCount; i++){
    }
  }
