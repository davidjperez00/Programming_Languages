/** Includes **/
#include <stdint.h>



/** Memory mapped register defines **/
#define REG_DDRD	(*((volatile uint8_t*)0x2A))
#define REG_PORTD	(*((volatile uint8_t*)0x2B))

#define REG_UBRR0H	(*((volatile uint8_t*)0xC5))
#define REG_UBRR0L	(*((volatile uint8_t*)0xC4))
#define REG_UDR0	(*((volatile uint8_t*)0xC6))
#define REG_UCSR0A	(*((volatile uint8_t*)0xC0))
#define REG_UCSR0B	(*((volatile uint8_t*)0xC1))
#define REG_UCSR0C	(*((volatile uint8_t*)0xC2))



/** Defines **/
#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80



/** Main function **/
int main()
{
	/**
		Specifications:
		1. UART receives a character, then transmits it back immediately (loopback)
		2. No ISRs, just poll the UART bits to see when it is done TXing or RXing
		3. Use 4800 baud, 8N1 (8 data bits, no parity bits, 1 stop bit)
	**/
	
	// Calculate UBRR0[11:0]
	// Equation: UBRR0 = round((f_MCU / (16 * desired baud)) - 1)
	//   == round((16 MHz / (16 * 4800 bps)) - 1) == round(207.33) == 207
	uint32_t ubrr0 = 207;
	REG_UBRR0H = ubrr0 >> 8;
	REG_UBRR0L = ubrr0 & 0x00FF;
	
	// Set up UCSR0A
	//  bit 1: U2X0 = 0b0 to not double the UART transmission speed
	REG_UCSR0A = 0x00;
	
	// Set up UCSR0C
	//  bits 7 and 6: UMSEL[1:0] = 0b00 for Asynchronous UART mode
	//  bits 5 and 4: UPM0[1:0] = 0b00 for disable parity bit
	//  bit 3: USBS0 = 0b0 for 1 stop bit
	// UCSZ0[2:0] = 0b011 for 8-bit data frames
	//  bits 2 and 1: UCSZ0[... 1:0] = 0b11
	// Concatenation: 0b00000110 == 0x06
	REG_UCSR0C = 0x06;
	
	// Set up UCSR0B
	//  bit 7: RXCIE0 = 0b0 to disable RX complete ISR
	//  bit 6: TXCIE0 = 0b0 to disable TX complete ISR
	//  bit 5: UDRIE0 = 0b0 to disable data register empty ISR
	//  bit 4: RXEN0 = 0b1 to enable UART receiver
	//  bit 3: TXEN0 = 0b1 to enable UART transmitter
	//  bit 2: UCSZ0[2 ...] = 0b0
	// Concatenation: 0b00011000 == 0x18
	REG_UCSR0B = 0x18;
	
	// Set pin TX (PD1) as an output
	REG_DDRD |= BIT1;
	
	// Set pin RX (PD0) as an input
	REG_DDRD &= ~BIT0;
	
	// Begin loopback (repeater) procedure
	while (1)
	{
		// Wait until a new char is received from the RX line of the UART (from the computer).
		// The bit RXC0 (bit 7) of register UCSR0A becomes a 1 when a new byte is received
		while ((REG_UCSR0A & BIT7) == 0)
		{
			// Wait for a new byte to be received
		}
		
		// A new byte has been received. Read it in the UART data register
		char c = REG_UDR0;
		
		// Wait for the UART data register to be empty
		// If we did not do this, then we might overwrite the UDR0 register before the
		//"up next" byte gets transferred to the TX shift register! 
		//So, we always must wait for the data register to be empty
		// The bit UDRE0 (bit 5) of register UCSR0A becomes a 1 when the data register is empty
		while ((REG_UCSR0A & BIT5) == 0)
		{
			// Wait for the data register to be empty
		}
		
		// Re-transmit that same byte over the TX line of the UART (back to the computer)
		REG_UDR0 = c;
	}
}
