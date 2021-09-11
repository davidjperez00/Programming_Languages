/** Includes **/
#include <stdint.h>



/** Memory mapped registers **/
volatile uint8_t* ioDDRB;

volatile uint8_t* ioTCCR1A;
volatile uint8_t* ioTCCR1B;
volatile uint8_t* ioOCR1AH;
volatile uint8_t* ioOCR1AL;



/** Main function **/
int main()
{
	// Populate the memory mapped register pointers with their addresses
	ioDDRB		= (volatile uint8_t*) 0x24;

	ioTCCR1A	= (volatile uint8_t*) 0x80;
	ioTCCR1B	= (volatile uint8_t*) 0x81;
	ioOCR1AH	= (volatile uint8_t*) 0x89;
	ioOCR1AL	= (volatile uint8_t*) 0x88;
	
	/*
		Specifications:
		1. Output the square wave on pin OC1A, which is PB1
		2. Make a 50% duty cycle square wave (use CTC "clear timer on compare match" waveform generation mode
		3. Set period to 0.5 s (2 Hz)
	*/
	
	/*
		Determine bit fields
		WGM1[3:0] = 0b0100 for CTC mode (clear timer on compare match). The timer will reset back to 0 when its value equals OCR1A (output compare register 1 A)
		COM1A[1:0] = 0b01 for Toggle OC1A on a compare match
		CS1[2:0] = 0b011 for divide f_MCU by 64 (to get 250 kHz for the timer clock, the timer increments every 4 us)
	*/
	
	
	// Configure registers
	
	// Need to make PB1 an output with the DDRB register
	*ioDDRB |= 0x02;
	
	// for TCCR1A, we have COM1A[1:0] cat 0b0000 cat WGM1[1:0] == 0b01 cat 0b0000 cat 0b00 == 0b01000000 == 0x40
	*ioTCCR1A = 0x40;
	
	// for TCCR1B, we have 0b000 cat WGM1[3:2] cat CS1[2:0] == 0b000 cat 0b01 cat 0b011 == 0b00001011 == 0x0B
	*ioTCCR1B = 0x0B;
	
	// Determine the period of our square wave using OCR1A registers
	// OCR1A = T * f_MCU / (2 * prescalar)
	// OCR1A = 0.5 s * 16 MHz / (2 * 64) == 62,500 == 0xF424
	//*ioOCR1AH = 0xF4;
	//*ioOCR1AL = 0x24;
	uint16_t half_period = 62500;
	*ioOCR1AH = half_period >> 8;
	*ioOCR1AL = half_period & 0x00FF;
	
	
	// Infinite loop that does nothing, let the Timer peripheral control the pin
	while (1)
	{
		
	}
}
