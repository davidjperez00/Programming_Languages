

int main()
{
  /*
  Specifications:
    1. UART recieve a character, then transmits it back immediatley (looppack)
    2. no ISR's, just poll the UART bits to see when it is done transmitting and recievning
    3. Use 4800 baud, 8N1 ( 8 daata bits, not parity bits, 1 stop bit)
  
  */

  //Calculate the UBRR0[11:0]
  //Equation: UBRR- = round((f_MCU / (16 * desired)) - 1)
  //== round((16 MHz / (16 * 4800 bps)) - 1)+ == round (207.33) == 207
  uint32_t ubrr0 = 207;
  REG_UBBR0H = ubrr0 >> 8;
  REG_UBRR0L = ubrr0 & 0x00FF; // only selects least significant btyes of 32 bit variable

  //Set up UCSR0A 
  //bit 1: U2X0 = ob0 to no double the UART transmission speed
  REG_UCSR0A = 0x00;

  //Set up USCR0C
  //bits 7 and 6: UMSEL[1:0] = 0b00 for ansynchronous uart mode
  //bits 5 and 4: UMP0[1:0] 0b00 for disable parity bit
  //bit 3: USB0 = 0b0 for 1 stop bit
  //UCSZ0 [2:0] 0b 011 for 8-bit data frames (SPLIT BETWEEN TWO REGISTERS)
     //bits 2 and 1: UCSZ0[....1:0] = 0b11
  //Concatienations 0b0000 0110 = 0x06
  REG_UCSR0C = 0x06;

  //Set up UCSR0b
  //bit 7: RCVIE0 = 0b0 to disable RX complete ISR

  //
  REG_UCSR0B = 0X18;

  //Set pin TX (PD1) as an output
  REG_DDRD |= BIT1;

  //Set pin RX (PD0) as an inpput
  REG_DDRD &=  ~BIT0;

  while(1)
  {
    //Wait until a neew chararcter is recieved from the RX line oft eh UART (from the computer)
    //Bit RCX0 (bit 7) of register UCSR0A vecomes a 1 whena  new byte is recieved
    while ((REG_UCSR0A & BIT7) == 0)
    {
      //Wait for a new byte to be recieved
    }
    //A new byte has been recieved. Read it in the UART data register
    char c = REG_UDR0;

    //Wait for teh UART dat register to be empty
    //If we did not do this, then we might overrite the UBR-=9 register before the "up next"
    //byte gets transferred to the Tx shift register!.....
    // 
    while ((REG_UCSR0Z & BIT5) == 0)
    {
      //Wait for the data register to be empty
      
    }
    //Re-transmit that same byte over the TX line fo the UART (back to the computer)
    REG_UDR0 = c;
  }
  
  



  
}
