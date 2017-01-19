
#include "msp430g2553.h"
#include "clock.h"
#include "delay.h"
#include "uarts.h"
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  clock_init(_1MHZ);
  delay_init(_1MHZ);
  uart_init();
  __enable_interrupt();

     uart_puts("\n\r\n\r\n\r***************\n\r");
     uart_puts("MSP430 softuart\n\r");
     uart_puts("***************\n\r\n\r");
     uint8_t c;
     while(1) {

          if(uart_getc(&c)) {

               if(c == 'a') {
                 uart_puts("\n\rLe Hung - mculearning.wordpress.com\n\r");
               }   
          }
     }
}