#include <stdint.h>

/** GLOBAL Variables */
volatile uint8_t* pDDRB;
volatile uint8_t* pPORTB;


/* Main Function */

int main(void)
{
  /* Define our pointers to the GPIO registers */
  pDDRB = 0x24;
  pPORTB = 0x25;

  /* Set PB1 as an output with the DDRB register */
  *pDDRB = 0x02; // 0x01 = 0b00000010


  while(1)
  {
    /* Set PB1 high */
    *pPORTB = 0x02;

    /* Clear PB1 low */
    *pPORTB = 0x00;
  }
}
 
