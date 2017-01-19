#define SYS_CLK 16000000
#define BAUD_RATE 9600    /* Max usuable with Launchpad */
#define DIV_INT_UART             (SYS_CLK/BAUD_RATE)
#define DBG_TX BIT1     /* defaults to PORT1 */


void config_dbg(void)
{
     P1DIR |= DBG_TX;
     P1OUT |= DBG_TX;  //default logic 1

}

void lowTX(){
      P1OUT &=~ DBG_TX;
      __delay_cycles(DIV_INT_UART-15);
}
void highTX(){
      P1OUT |= DBG_TX;
      __delay_cycles(DIV_INT_UART-15);
}

void transferTX(char byte){
      //start transfer
      int bitSelect=0x01;
      lowTX();
      
      //send byte LSB, 8bit, no parity, bit 1 to stop
      while(bitSelect){
            if(byte&bitSelect)
                  highTX();
            else
                  lowTX();
            bitSelect=bitSelect<<1;
      }
      
      //sent 2 stop bit
      highTX();
      highTX();
}


void dbgString(const char *str)
{
     if(*str != 0) transferTX(*str++);
     while(*str != 0) transferTX(*str++);
}

void dbgInt(unsigned int n)
{
     unsigned char buffer[16];
     unsigned char i,j;

     if(n == 0) {
    	 transferTX('0');
          return;
     }

     for (i = 15; i > 0 && n > 0; i--) {
          buffer[i] = (n%10)+'0';
          n /= 10;
     }

     for(j = i+1; j <= 15; j++) {
    	 transferTX(buffer[j]);
     }
}

void dbgFloat(float x, unsigned char coma)
{
	unsigned long temp;
	if(coma>4)coma=4;
	// de trong 1 ki tu o dau cho hien thi dau
	if(x<0)
	{
		transferTX('-');			//In so am
		x*=-1;
	}
	temp = (unsigned long)x;									// Chuyan thanh so nguyen.

	dbgInt(temp);

	x=((float)x-temp);//*mysqr(10,coma);
	if(coma!=0) 
              transferTX('.');	// In ra dau "."
	while(coma>0)
	{
		x*=10;
		coma--;
	}
	temp=(unsigned long)(x+0.5);	//Lam tron
	dbgInt(temp);
}
