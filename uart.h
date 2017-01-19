#define USCI_INPUT_CLK      (1000000)  // in Hz
#define USCI_BAUD_RATE      (9600)

#define USCI_DIV_INT              (USCI_INPUT_CLK/USCI_BAUD_RATE)
#define USCI_BR0_VAL              (USCI_DIV_INT & 0x00FF)
#define USCI_BR1_VAL              ((USCI_DIV_INT >> 8) & 0xFF)

#define USCI_DIV_FRAC_NUMERATOR   (USCI_INPUT_CLK - (USCI_DIV_INT*USCI_BAUD_RATE))
#define USCI_DIV_FRAC_NUM_X_8     (USCI_DIV_FRAC_NUMERATOR*8)
#define USCI_DIV_FRAC_X_8         (USCI_DIV_FRAC_NUM_X_8/USCI_BAUD_RATE)

#if (((USCI_DIV_FRAC_NUM_X_8-(USCI_DIV_FRAC_X_8*USCI_BAUD_RATE))*10)/USCI_BAUD_RATE < 5)
#define USCI_BRS_VAL              (USCI_DIV_FRAC_X_8<< 1)
#else
#define USCI_BRS_VAL              ((USCI_DIV_FRAC_X_8+1)<< 1)
#endif

void config_uart(void){
    // setup USCI UART registers
    UCA0CTL1 |= UCSSEL_2 + UCSWRST;
    UCA0BR0 = USCI_BR0_VAL;
    UCA0BR1 = USCI_BR1_VAL;
    UCA0MCTL = USCI_BRS_VAL;
    
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    
    UCA0CTL1 &= ~UCSWRST;
   
    IE2 |= UCA0RXIE;//Cho phep ngat tai Uart
}

//Gui 1 byte UART
void UartSendByte(unsigned char byte){
      while(!(IFG2 & UCA0TXIFG));
      UCA0TXBUF = byte;
}
//Gui 1 so kieu int
void UartSendInt(unsigned int n)
{
     unsigned char buffer[16];
     unsigned char i,j;

     if(n == 0) {
    	 UartSendByte('0');
          return;
     }

     for (i = 15; i > 0 && n > 0; i--) {
          buffer[i] = (n%10)+'0';
          n /= 10;
     }

     for(j = i+1; j <= 15; j++) {
    	 UartSendByte(buffer[j]);
     }
}
//Gui 1 String UART
void UartSendString(char* str){
      int i=0;
      while(str[i]){
      UartSendByte(str[i]);
      i++;
      }
        i=0;
}
//Gui so Uart
void UartSendFloat(float x, unsigned char coma)
{
	unsigned long temp;
	if(coma>4)coma=4;
	// de trong 1 ki tu o dau cho hien thi dau
	if(x<0)
	{
		UartSendByte('-');			//In so am
		x*=-1;
	}
	temp = (unsigned long)x;									// Chuyan thanh so nguyen.

	UartSendInt(temp);

	x=((float)x-temp);//*mysqr(10,coma);
	if(coma!=0) 
              UartSendByte('.');	// In ra dau "."
	while(coma>0)
	{
		x*=10;
		coma--;
	}
	temp=(unsigned long)(x+0.5);	//Lam tron
	UartSendInt(temp);
}
//nhan 1 byte UART
char UartRecieved(void){
      while(! (IFG2 & UCA0RXIFG)); // cho den khi bo dem receiver tran - UCA0RXIFG is set when BufferRX  has received a complete character
      return UCA0RXBUF ; 
}

//nhan 1 String UART
char *UartReadString(void){
      char *str;
      char temp[80];
      int i=0;
       while(1){	
              temp[i]=UartRecieved();
               if (temp[i] == 0x0A)		// C string moi chua ki tu NULL, con external string ko chua, chi dung ki tu dac biet de nhan biet end chuoi		
                   break;
                i++;
             }
      temp[i++]='\0'; 
      
      str=temp;			// Point to the next char space in the string
     
      return str;
}
