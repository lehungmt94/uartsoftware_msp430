//******************************************************************************
//                   
//                           DELAY FUNCTION FOR MSP430G2553 
//  Description: 
//  Project Name :        DELAY
//  Note : 
//  Created day : 25/07/2016                               By : Lê Hùng
//  Last fixed :  25/07/2016                               By : Lê Hùng
//*******************************************************************************

#define _1MHZ      1
#define _8MHZ      8
#define _12MHZ     12
#define _16MHZ     16

unsigned char SYSTEM_CLOCK_DELAY=1;           //Default 1MHz if not define delay_init();

void delay_init(unsigned char clock);    //config for delay with system clock
void delay(int ms);                     //function delay ms


void delay_init(unsigned char clock){
  SYSTEM_CLOCK_DELAY = clock;
}


void delay(int ms){
  switch(SYSTEM_CLOCK_DELAY){
    case _1MHZ:
     for(int i=0;i<ms;i++)
            __delay_cycles(1000);
     break;
    case _8MHZ:
      for(int i=0;i<ms;i++)
            __delay_cycles(8000);
      break;
    case _12MHZ:
      for(int i=0;i<ms;i++)
            __delay_cycles(12000);
      break;
    case _16MHZ:
      for(int i=0;i<ms;i++)
            __delay_cycles(16000);
      break;
  }
}

