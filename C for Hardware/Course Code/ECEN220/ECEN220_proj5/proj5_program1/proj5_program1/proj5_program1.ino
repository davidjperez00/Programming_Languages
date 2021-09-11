/** Includes **/
#include <stdint.h>
#include <avr/interrupt.h>

/** Memory mapped register defines **/
#define REG_DDRD  (*((volatile uint8_t*)0x2A))
#define REG_PORTD (*((volatile uint8_t*)0x2B))

#define REG_UBRR0H  (*((volatile uint8_t*)0xC5))
#define REG_UBRR0L  (*((volatile uint8_t*)0xC4))
#define REG_UDR0  (*((volatile uint8_t*)0xC6))
#define REG_UCSR0A  (*((volatile uint8_t*)0xC0))
#define REG_UCSR0B  (*((volatile uint8_t*)0xC1))
#define REG_UCSR0C  (*((volatile uint8_t*)0xC2))

/** Global interrupts register **/
#define REG_SREG   (*((volatile uint8_t*) 0x5F))

/** Defines **/
#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10
#define BIT5  0x20
#define BIT6  0x40
#define BIT7  0x80
#define RX_BUFFER_MAX_LENGTH  (100)

/** Global Variables **/
volatile uint8_t g_RXComplete;
volatile uint8_t g_head = 0;
volatile uint8_t g_tail = 0;
volatile char g_buf[20];



int main ()
{
  //initialize the uart peripheral with a 9600 baud rate
  uart_init_8N1(9600);

  uart_puts("David Perez \n");
  uart_puts("Project 5, Program 1 \n");

  while(1)
  {
    char str[RX_BUFFER_MAX_LENGTH];
    uart_gets_until(str, RX_BUFFER_MAX_LENGTH, '\n');
    uart_puts(str);
  }
}

//Inializes the UART with 8 data bits, no parity bits, and 1 stop bit(8N1)
//argumnet allows for a customizable baud rate (in bits per second)
void uart_init_8N1(uint32_t baud)
{
  //Enable global interrupts in the SREG register
  REG_SREG |= BIT7;
  
  uint32_t mcuClock = 16000000;
  uint32_t ubrr0 = round((mcuClock / (16 * baud)) - 1);
  //apply our values into baud rate registers
  REG_UBRR0H = ubrr0 >> 8;
  REG_UBRR0L = ubrr0 & 0x00FF;

  //UART control and status register A
  //don't want to use any of these bitfields so set all to 0
  REG_UCSR0A = 0x00;

  // Set up UCSR0C
  //  bits 7 and 6: UMSEL[1:0] = 0b00 for Asynchronous UART mode
  //  bits 5 and 4: UPM0[1:0] = 0b00 for disable parity bit
  //  bit 3: USBS0 = 0b0 for 1 stop bit
  // UCSZ0[2:0] = 0b011 for 8-bit data frames
  //  bits 2 and 1: UCSZ0[... 1:0] = 0b11
  // Concatenation: 0b00000110 == 0x06
  REG_UCSR0C = 0x06;

  // Set up UCSR0B (this register also enables the interrupt)
  //  bit 7: RXCIE0 = 0b1 to enable RX complete ISR
  //  bit 6: TXCIE0 = 0b0 to disable TX complete ISR
  //  bit 5: UDRIE0 = 0b0 to disable data register empty ISR
  //  bit 4: RXEN0 = 0b1 to enable UART receiver
  //  bit 3: TXEN0 = 0b1 to enable UART transmitter
  //  bit 2: UCSZ0[2 ...] = 0b0 (other two bits in UCSR0C)
  // Concatenation: 0b10011000 == 0x98
  REG_UCSR0B = 0x98;

  //initalize flag indicating RX complete
  g_RXComplete = 0;
  
  // Set pin TX (PD1) as an output
  REG_DDRD |= BIT1;
  
  // Set pin RX (PD0) as an input
  REG_DDRD &= ~BIT0;
}

//waits for a UART data register to be empty then
//transmits a single char to the cmoputer
void uart_putc(char c)
{
   while((REG_UCSR0A & BIT5) == 0)
   {
    // Wait for data register to be empty
   }

   // Re-transmit that same byte over the TX line of the UART (back to the computer)
   REG_UDR0 = c;
   g_RXComplete = 0;
}

void uart_puts(char* str)
{
  char c; // variable to hold each character from our string
  uint8_t i = 0;  //incrementing variable
  c = str[i]; //initializing our character to make sure it's not null
  
  while(c != '\0')
  {
    c = str[i]; // put each value from the inputed string into a char
    uart_putc(c); // transmit each character
    i++;
  } 
}

uint8_t uart_rx_available(void)
{
  //Return 1 if there in an unread char in the RX ring buffer
  // and returns 0 if not
  if(g_head == g_tail || (g_head + g_tail) == 0)
  {
    return 0;
  }else
  {
    return 1;
  }
}

char uart_getc(void)
{
  
  volatile uint8_t charAvailable = 0;
  while(charAvailable == 0)
  {
    //wait until there is a new char available in the RX ring buffer
    charAvailable = uart_rx_available();
  }
  char c = g_buf[g_tail]; //get the character from the ring buffer
  if(g_tail == 20)
  {
    g_tail=0;
  }else
  {
    g_tail++;
  }
  return c;
}

void uart_gets_until(char* buf, uint8_t buf_size, char end_char)
{
    uint8_t bufLen = 20;
    //this coditional checks if there are any bytes available to read by the buffer
   if((g_head >= g_tail && ((g_head - g_tail) != bufLen) || (g_head < g_tail && bufLen != bufLen - (g_tail-g_head))))
{
  uint8_t j = 0;
    char c = uart_getc();
    g_tail--; //tail for our buffer gets iterated after well call getc for the initial check
      while(c != end_char && (j < (buf_size - 1)))
    {
      c = uart_getc();
      buf[j] = c;
      j++;
    }
    //add a null terminator to the end of the string and reset our increment variable
    buf[j] = '\0';
    j = 0;
  }
}


/** Interrupt **/
ISR(USART_RX_vect, ISR_BLOCK)
{
  //check if the head flag has reached the end of the ring buffer
  if(g_head == 20)
  {
    g_head = 0;
    g_buf[g_head] = REG_UDR0;
    g_head++;
  }else
  {
    g_buf[g_head] = REG_UDR0;
    g_head++;
  }
  
}
