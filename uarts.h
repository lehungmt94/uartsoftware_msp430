//******************************************************************************
//                   
//                           UART Software
//  Description: original Author: Nicholas J. Conn edit by Le Hung
//      http://mculearning.wordpress.com
//      http://www.msp430launchpad.com/2010/08/half-duplex-software-uart-on-launchpad.html
//  Project Name :        Giam sat luong mua
//  Note : TXD on P1.5,  RXD on P1.6
//  Created day : 23/06/2016                               By : Lê Hùng
//  Last fixed :  23/06/2016                               By : Lê Hùng
//  Email: lehungmt94@gmail.com
//*******************************************************************************

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize soft UART
 */
void uart_init(void);

/**
 * Read one character from UART non-blocking.
 *
 * @param[out]	*c	character received (if one was available)
 * @return			true if character received, false otherwise
 */
bool uart_getc(uint8_t *c);

/**
 * Write one chracter to the UART blocking.
 *
 * @param[in]	*c	the character to write
 */
void uart_putc(uint8_t c);

/**
 * Write string to the UART blocking.
 *
 * @param[in]	*str	the 0 terminated string to write
 */
void uart_puts(const char *str);


/**
 * TXD on P1.5
 */
#define TXD BIT5

/**
 * RXD on P1.6
 */
#define RXD BIT6

/**
 * CPU freq.
 */
#define FCPU 			1000000

/**
 * Baudrate
 */
#define BAUDRATE 		9600

/**
 * Bit time
 */
#define BIT_TIME        (FCPU / BAUDRATE)

/**
 * Half bit time
 */
#define HALF_BIT_TIME   (BIT_TIME / 1)

/**
 * Bit count, used when transmitting byte
 */
volatile uint8_t bitCount;

/**
 * Value sent over UART when uart_putc() is called
 */
unsigned int TXByte;

/**
 * Value recieved once hasRecieved is set
 */
unsigned int RXByte;

/**
 * Status for when the device is receiving
 */
static volatile bool isReceiving = false;

/**
 * Lets the program know when a byte is received
 */
volatile bool hasReceived = false;

void uart_init(void)
{
     P1SEL |= TXD;
     P1DIR |= TXD;

     P1IES |= RXD; 		// RXD Hi/lo edge interrupt
     P1IFG &= ~RXD; 		// Clear RXD (flag) before enabling interrupt
     P1IE  |= RXD; 		// Enable RXD interrupt
}

bool uart_getc(uint8_t *c)
{
     while (!hasReceived) {
          //return false;
     }

     *c = RXByte;
     hasReceived = false;

     return true;
}

void uart_putc(uint8_t c)
{
     TXByte = c;

     while(isReceiving); 					// Wait for RX completion

     CCTL0 = OUT; 							// TXD Idle as Mark
     TACTL = TASSEL_2 + MC_2; 				// SMCLK, continuous mode

     bitCount = 0xA; 						// Load Bit counter, 8 bits + ST/SP
     CCR0 = TAR; 							// Initialize compare register

     CCR0 += BIT_TIME; 						// Set time till first bit
     TXByte |= 0x100; 						// Add stop bit to TXByte (which is logical 1)
     TXByte = TXByte << 1; 					// Add start bit (which is logical 0)

     CCTL0 = CCIS_0 + OUTMOD_0 + CCIE + OUT; // Set signal, intial value, enable interrupts

     while ( CCTL0 & CCIE ); 				// Wait for previous TX completion
}

void uart_puts(const char *str)
{
     if(*str != 0) uart_putc(*str++);
     while(*str != 0) uart_putc(*str++);
}

/**
 * ISR for RXD
 */
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
     isReceiving = true;

     P1IE &= ~RXD; 					// Disable RXD interrupt
     P1IFG &= ~RXD; 					// Clear RXD IFG (interrupt flag)

     TACTL = TASSEL_2 + MC_2; 		// SMCLK, continuous mode
     CCR0 = TAR; 					// Initialize compare register
     CCR0 += HALF_BIT_TIME; 			// Set time till first bit
     CCTL0 = OUTMOD_1 + CCIE; 		// Disable TX and enable interrupts

     RXByte = 0; 					// Initialize RXByte
     bitCount = 9; 					// Load Bit counter, 8 bits + start bit
}

/**
 * ISR for TXD and RXD
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERA1_ISR(void)
{
     if(!isReceiving) {
          CCR0 += BIT_TIME; 						// Add Offset to CCR0
          if ( bitCount == 0) { 					// If all bits TXed
               TACTL = TASSEL_2; 					// SMCLK, timer off (for power consumption)
               CCTL0 &= ~ CCIE ; 					// Disable interrupt
          } else {
               if (TXByte & 0x01) {
                    CCTL0 = ((CCTL0 & ~OUTMOD_7 ) | OUTMOD_1);  //OUTMOD_7 defines the 'window' of the field.
               } else {
                    CCTL0 = ((CCTL0 & ~OUTMOD_7 ) | OUTMOD_5);  //OUTMOD_7 defines the 'window' of the field.
               }

               TXByte = TXByte >> 1;
               bitCount --;
          }
     } else {
          CCR0 += BIT_TIME; 						// Add Offset to CCR0

          if ( bitCount == 0) {

               TACTL = TASSEL_2; 					// SMCLK, timer off (for power consumption)
               CCTL0 &= ~ CCIE ; 					// Disable interrupt

               isReceiving = false;

               P1IFG &= ~RXD; 						// clear RXD IFG (interrupt flag)
               P1IE |= RXD; 						// enabled RXD interrupt

               if ( (RXByte & 0x201) == 0x200) { 	// Validate the start and stop bits are correct
                    RXByte = RXByte >> 1; 			// Remove start bit
                    RXByte &= 0xFF; 				// Remove stop bit
                    hasReceived = true;
               }
          } else {
               if ( (P1IN & RXD) == RXD) { 		// If bit is set?
                    RXByte |= 0x400; 				// Set the value in the RXByte
               }
               RXByte = RXByte >> 1; 				// Shift the bits down
               bitCount --;
          }
     }
}
