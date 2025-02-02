
#include "remote.h"
#include "ir_remote.h"
#include "utility.h"


unsigned char mCmdQ[128];	 		// mCmdQ[0] = index
unsigned char mCmdQFlag=0;	 		// Cmd flag
volatile char mpUART;				// Store previous Uart data

char mLinux_user[16]={'r', 'o', 'o', 't',0,0,0,0,0,0,0,0,0,0,0,0};
char mLinux_passwd[16]={'a', 'd', 'm', 'i','n',0,0,0,0,0,0,0,0,0,0,0};
//const char T_PRONTO_CODE[] = "~R0S ff 006d 015400aa 00150015 00150040 00160040 00150016 0015003f 001505e4 00000000 22 H,0010,0000,1101,1111,1000,0010,0111,1101,F~";
//const char T_PRONTO_CODE[] = "~RS0 ff 006d 015400aa 00150015 00150040 00160040 00150016 0015003f 001505e4 00000000 22 0112111112212223221111124122522126~";
//const char T_PRONTO_CODE[] = "~RS0 ff 006C 015500AB 00150015 00150040 00000000 00000000 00000000 00000000 00000000 22 0112111111212221211212111221212221~";
const char T_PRONTO_CODE[] = "~RS0 ff 0001 006C 03 22 015500AB 00150015 00150040 0112111111212221221121111122122221~";

volatile unsigned char 	mLinuxMode=0xff;
volatile unsigned int 	mCmdCnt=0;
volatile unsigned char 	mTimer0_Flag=0;
volatile unsigned char 	mTimer0_Cnt=0;

volatile unsigned char  mFunctionMode=0;	// 0 = Idle
											// 1 - Send Pulse Function
											// 2 - Capture Pulse Function	

// Define Interrupt

SIGNAL(SIG_UART_RECV) { // USART RX interrupt

	ISR_UART_RECV();

}

void ISR_UART_RECV(void)
{

	 //uart has received a character in UDR 	 
	 unsigned char data;
//	 static unsigned char pdata;
	 data = UDR;

	 PORTC ^= (1<<LED1);

	 // Set CmdReady Flag 
	 if ((data=='\r') || (data=='\n')) 
	 {
//	 	mCmdQ[0] |= 0x80;	
	 	mCmdQFlag = 1;	
		// Ignore empty message
	 	if (mCmdQ[0] == 0x00)
		{
			mCmdQFlag = 0x00;
		}
		else if ((mCmdQ[0] == 0x01) && (mCmdQ[1] == 0xff))	// Process message with ONE BYTE
		{													// 0xff = Linux Login Reset
			mCmdQ[0] = 0x00;		
			mCmdQFlag = 0x00;
			mLinuxMode = 0x00;
		}

		if (mLinuxMode <=1)									// Ignor message when mLinux Mode <=1
		{
	 		mCmdQ[0] = 0x00;	
			mCmdQFlag = 0x00;
		}
		else if ((mpUART != '~') | (mCmdQ[1] != '~'))		// Ignor message not started with and terminated by "~"
		{
	 		mCmdQ[0] = 0x00;	
			mCmdQFlag = 0x00;
		}
	 }
	 else if (mCmdQ[0] < ((sizeof(mCmdQ))-1))	 
	 {
		 if ((mLinuxMode<=0x01) && (mpUART == ':') && data==' ')
		 {
//			mCmdQ[0] |= 0x80;
			mCmdQFlag = 1;
		 }
	 	 mCmdQ[0]++;
		 mCmdQ[mCmdQ[0]]=data;
	 }	

// 	 pdata = data;
	 mpUART = data;


}

void POLL_UART_RECV(void)
{

 	 //uart has received a character in UDR 	 
	 unsigned char data;
//	 static unsigned char pdata;
	 data = UDR;

	 PORTC ^= (1<<LED1);

	 // Set CmdReady Flag 
	 if ((data=='\r') || (data=='\n')) 
	 {
//	 	mCmdQ[0] |= 0x80;	
	 	mCmdQFlag = 1;	
		// Ignore empty message
	 	if (mCmdQ[0] == 0x00)
		{
			mCmdQFlag = 0x00;
		}
		else if ((mCmdQ[0] == 0x01) && (mCmdQ[1] == 0xff))	// Process message with ONE BYTE
		{													// 0xff = Linux Login Reset
			mCmdQ[0] = 0x00;		
			mCmdQFlag = 0x00;
			mLinuxMode = 0x00;
		}

		if (mLinuxMode <=1)									// Ignor message when mLinux Mode <=1
		{
	 		mCmdQ[0] = 0x00;	
			mCmdQFlag = 0x00;
		}
		else if ((mpUART != '~') | (mCmdQ[1] != '~'))		// Ignor message not started with and terminated by "~"
		{
	 		mCmdQ[0] = 0x00;	
			mCmdQFlag = 0x00;
		}
	 }
	 else if (mCmdQ[0] < ((sizeof(mCmdQ))-1))	 
	 {
		 if ((mLinuxMode<=0x01) && (mpUART == ':') && data==' ')
		 {
//			mCmdQ[0] |= 0x80;
			mCmdQFlag = 1;
		 }
	 	 mCmdQ[0]++;
		 mCmdQ[mCmdQ[0]]=data;
	 }	

// 	 pdata = data;
	 mpUART = data;

}


// uses timer0 for base timer
// Interrupt for every 21.845333mS
SIGNAL (TIMER0_OVF_vect)	
{
	ISR_TIMER0_OVR();
}

void ISR_TIMER0_OVR (void)
{
	mTimer0_Cnt++;
	if (mTimer0_Cnt>=9)	// 21.84533ms * 9 = 196.6ms
	{
		mTimer0_Flag = 1;
		mTimer0_Cnt =0;
	}
}

void POLL_TIMER0_OVR(void)
{

	TIFR |= 0x01;			// Clear TOV0 Flag
	mTimer0_Cnt++;
	if (mTimer0_Cnt>=9)	// 21.84533ms * 9 = 196.6ms
	{
		mTimer0_Flag = 1;
		mTimer0_Cnt =0;
	}
}


SIGNAL (SIG_OUTPUT_COMPARE1A)
{

	mPulseCnt --;

	if (mPulseCnt==mPulse[1])
	{
		PORTB  &= ~(0x02);			// Set OC1A pin to "0"
		TCCR1A &= ~(0b11000000);	// Disconnect OC1A on compare
		PORTC  &= ~(1 << LED3);		// Clear LED3
	}

	if (mPulseCnt==0)
	{
		mTxFlag = 0;
		TCCR1B &= 0b11111000;		// Stop Clock
	}

}

SIGNAL (SIG_INPUT_CAPTURE1)
{
	unsigned char tmp;
	mCapturePulse[mCaptureIndex] = ICR1;	// Capture Timer1
	TCNT1 = 0x00;							// Set Timer1 = 0x00;

//	TCCR1B ^= 0x01000000;					// Toggle Trigger Edge
	tmp = TCCR1B;
	tmp ^= 0x40;
	TCCR1B = tmp;							// Toggle Trigger Edge

	mCaptureIndex++;
	if (mCaptureIndex > (sizeof(mCapturePulse)/2))
		mCaptureIndex = sizeof(mCapturePulse)/2;
}



void init_hardware(void)
{

//	init UART

	UCSRA = 0x02; // U2X = 1
	UCSRB = 0x00;	
	UCSRC = 0x86; // No Parity | 1 Stop Bit | 8 Data Bit
	UBRRH = 0x00; // 115200bps @ 12.00MHz 0x4D=9600 0x33 = 115200
	UBRRL = 0x0C; // 115200bps @ 12.00MHz 0x4D/9B=9600 ??/0x0C = 115200
	UCSRB = 0x98; // Enable Rx Interrupt, Rx/Tx PIN	

	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 125.000 kHz
	TCCR0=0x05;		// Clk/1024 = 12000000/1024 = 11718.75Hz
	TCNT0=0x00;
	TIMSK |= 0x01;	// Turn ON Timer0 Interrup


	//  prepare 36kHz for IR - Communication
	TCCR1A = 0b01001000;	// Toggle OC1A on compare
	TCCR1B = 0b00001000;	// CLK=STOP
	OCR1AH  = 0x00; 		// 12MHz / 40Kz / 2 = 150 = 0x0096
	OCR1AL  = 0x94; 		// 
	TCNT1   = 0x0000;
	TIMSK |= 0x10;	// Turn ON OC1A compare interrupt


//	init IO
	DDRC = 0x07; 
	DDRB = _BV(1);	// PB1 


}

void init_system(void)
{
	unsigned char i;
	u_puts(EXIT);
	for (i=0; i<sizeof(T_PRONTO_CODE); i++)
	{
		mCmdQ[i+1]=T_PRONTO_CODE[i];
	}	
	mCmdQ[0] = sizeof(T_PRONTO_CODE)-1;
	mCmdQFlag = 1;
	process_cmd();


}

void process_tmr(void)
{
	static unsigned int tCnt=0;


	if (mTimer0_Flag==1)	// @ very 196.6ms
	{
		mTimer0_Flag=0;
		tCnt ++	;
		if (mLinuxMode!=2)	
		{
			tCnt=0xff;
		}

		if (tCnt>=10)		
		{
			tCnt=0;
			PORTC ^= (1 << LED1);
		}
		switch (mFunctionMode)
		{
			case 0:
				break;
			case 1:
				ir_send_pulseEx();
				break;
			case 2:
				ir_capture_pulse();
				break;
			default:
				break;
		}
	}

}

void process_cmd(void)
{
	unsigned char index;

//	if (mCmdQ[0]>=0x80)	// Message available for process
	if (mCmdQFlag==1)	// Message available for process
	{
//		index = mCmdQ[0] & 0x7f;
		index = mCmdQ[0];
		PORTC ^=(1 << LED1);

		// Valid message neeed to have at least FOUR character
		if (index>=3) 
		{
			if (mLinuxMode<=0x01)
				process_cmd_linux(mLinuxMode);
			else if ((mCmdQ[1]=='~') && (mCmdQ[index]=='~'))
				process_cmd_hal(&mCmdQ[2],index-2);

		}
		mCmdQ[0]=0;		
		mCmdQFlag=0;
	}
}

void process_cmd_linux(unsigned char iMode)
{
	switch (iMode)
	{
		case 0x00:
			mLinuxMode = 0x01;
			u_puts(mLinux_user);
			u_puts("\r\n");
			break;

		case 0x01:
			mLinuxMode = 0x02;
			u_puts(mLinux_passwd);
			u_puts("\r\n");
			break;

		default:
			mLinuxMode=0xff;
	}

}

void process_cmd_hal(unsigned char *sptr, unsigned char len)
{

	unsigned char i,j;
	unsigned char tCmd;
	char tmp;

	tCmd=u_toupper(*sptr);
	mCmdCnt ++;
	switch (tCmd)
	{
		case '~':	// Echo message back
			for (i=1; i<len; i++)
			{
				tmp = *(sptr+i);
				u_putch(tmp);
			}
			u_puts("\r\n");
			break;

		case 'L':	// Login information
			for (i=1; i<len; i++)
			{
				tmp = *(sptr+i);
				if (tmp==' ')
					break;
			}

			if (i<(len-1))
			{
				for (j=0; j<sizeof(mLinux_user); j++)
				{
					mLinux_user[j]=0x00;
					mLinux_passwd[j]=0x00;
				}

				for (j=1; j<i; j++)
				{
					mLinux_user[j-1]=*(sptr+j);
				}
				for (j=(i+1); j<len; j++)
				{
					mLinux_passwd[j-1-i]=*(sptr+j);
				}

				u_puts("\r\n");
			}

  			break;

		case 'S':	// Status
			u_puts(LinuxCmd);
			u_puts("'S' mcu_status:");				// Status command
			u_puts(mLinux_user);
			u_puts("/");
			u_puts(mLinux_passwd);u_puts(":");
			u_putHexByte(mLinuxMode,':');	// mLinuxMode = 0x02 : Login Linux already
			u_putHexWord(mCmdCnt);u_puts(":");
			u_puts("\r\n");
			break;

		case 'V':	// Version
			u_puts(LinuxCmd);
			u_puts("'V' ");
			u_puts(VERSION);
			u_puts("'\r\n");
			break;

		case 'R': 	// Remote command
			process_remote_command((char*) (sptr));
			break;

		case 'T': 	// Test Command
			u_puts("Test :");
			for (i=1; i<len; i++)
			{
				tmp = *(sptr+i);
				u_putch(tmp);
			}
			u_puts("\r\n");
			break;

	}
}





