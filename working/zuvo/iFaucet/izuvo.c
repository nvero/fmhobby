
#include "izuvo.h"
#include "izuvo_mcu.h"
#include "utility.h"
#include <avr/eeprom.h>


unsigned char mCmdQ[64];	 		// mCmdQ[0] = index
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
volatile unsigned int 	mTimer0_Cnt=0;

volatile unsigned char  mFunctionMode=0;	// 0 = Idle
											// 1 - Send Pulse Function
											// 2 - Capture Pulse Function	


volatile unsigned int 	mPulseT[3]={0,0,0};
volatile unsigned char  mPulseIndex=0;

volatile unsigned int  mLED[4];
volatile unsigned char mLED_Brightness[4];
volatile unsigned char mLED_Cnt[4];
volatile unsigned char mLED_Fade[4];

extern volatile unsigned char  mZuvoAuto;
volatile unsigned char mTestMode=0;
volatile unsigned char mZuVoMode=0;


void process_cmd_izuvo(char *sptr, unsigned char len);
void process_cmd_ifaucet(char *sptr, unsigned char len);
void process_cmd_izuvo_led(char *sptr);
void process_cmd_izuvo_mode(unsigned char iMode);
void izuvo_test_mode(unsigned char iMode);

void led_task(void);
void led_driver(void);

#define LED_PWM 32

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

//	 PORTC ^= (1<<LED1);

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
/*		else if ((mpUART != '~') | (mCmdQ[1] != '~'))		// Ignor message not started with and terminated by "~"
		{
	 		mCmdQ[0] = 0x00;	
			mCmdQFlag = 0x00;
		}
*/
	 }
	 else if (mCmdQ[0] < ((sizeof(mCmdQ))-1))	 
	 {
//		 if ((mLinuxMode<=0x01) && (mpUART == ':') && data==' ')
//		 {
//			mCmdQ[0] |= 0x80;
//			mCmdQFlag = 1;
//		 }
	 	 mCmdQ[0]++;
		 mCmdQ[mCmdQ[0]]=data;
	 }	

// 	 pdata = data;
	 mpUART = data;


}
/*
void POLL_UART_RECV(void)
{

 	 //uart has received a character in UDR 	 
	 unsigned char data;
//	 static unsigned char pdata;
	 data = UDR;

//	 PORTC ^= (1<<LED1);

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
*/

// uses timer0 for base timer
// Interrupt for every 21.845333mS
SIGNAL (TIMER0_OVF_vect)	
{
	ISR_TIMER0_OVR();
}

void ISR_TIMER0_OVR (void)
{
//	TCNT0=0x4E;
//	TCNT0=0x58;
	TCNT0=0xAC;
	
	led_driver();
	mTimer0_Cnt++;
//	if (mTimer0_Cnt>=9)	// 21.84533ms * 9 = 196.6ms
//	if (mTimer0_Cnt>=200)	// 1ms * 200 = 196.6ms
	if (mTimer0_Cnt>=200)	// 1ms * 200 = 196.6ms
	{
		mTimer0_Flag = 1;
		mTimer0_Cnt =0;
	}
}
/*
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
*/

/*
SIGNAL (SIG_OUTPUT_COMPARE1A)
{

	mPulseCnt --;

	if (mPulseCnt==mPulse[1])
	{
		PORTB  &= ~(0x02);			// Set OC1A pin to "0"
		TCCR1A &= ~(0b11000000);	// Disconnect OC1A on compare
//		PORTC  &= ~(1 << LED3);		// Clear LED3
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
	unsigned int tCnt;

	tCnt = ICR1;

	mCapturePulse[mCaptureIndex] = tCnt;	// Capture Timer1
	mPulseT[2]=mPulseT[1];
	mPulseT[1]=mPulseT[0];
	mPulseT[0] = mCapturePulse[mCaptureIndex] >> 1;

	TCNT1 = 0x00;							// Set Timer1 = 0x00;

//	TCCR1B ^= 0x01000000;					// Toggle Trigger Edge
	tmp = TCCR1B;
	tmp ^= 0x40;
	TCCR1B = tmp;							// Toggle Trigger Edge

//	izuvo_rx_pulse(tCnt);

//  0135 26 13 13 13

	if ( mPulseT[2]>0x0100 && mPulseT[1]<0x0030 && mPulseT[1]>0x0020 && mPulseT[0]<0x0020 && mPulseIndex==0) 
	{
		mPulseIndex = mCaptureIndex-1;
	}


	mCaptureIndex++;
	if (mCaptureIndex > (sizeof(mCapturePulse)/2))
		mCaptureIndex = sizeof(mCapturePulse)/2;

}


*/
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
//	TCCR0=0x05;		// Clk/1024 = 12000000/1024 = 11718.75Hz
	TCCR0=0x03;		// Clk/1024 = 12000000/1024 = 11718.75Hz
	TCNT0=0x4E;
	TIMSK |= 0x01;	// Turn ON Timer0 Interrup



	//  prepare 36kHz for IR - Communication
	TCCR1A = 0b01001000;	// Toggle OC1A on compare
	TCCR1B = 0b00001000;	// CLK=STOP
	OCR1AH  = 0x00; 		// 12MHz / 40Kz / 2 = 150 = 0x0096
	OCR1AL  = 0x94; 		// 
	TCNT1   = 0x0000;
	TIMSK |= 0x10;	// Turn ON Timer0 Interrup


//	init IO
	DDRC = 0x07; 
//	DDRB = _BV(1);	// PB1 
	DDRB = 0x3B;
	DDRD = 0x02;
	PORTD = 0x00;

//	UCSRB = 0x98; // Enable Rx Interrupt, Rx/Tx PIN	
	UCSRB = 0x90; // Enable Rx Interrupt, Rx PIN	

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
//	process_cmd();
/*
	for (i=0; i<=100; i++)
	{
		mCapturePulse[i]=0x00;		
	}

//  0135 26 13 13 13 4E 13 26
	i=2;
	mCapturePulse[i]=0x0135; i++;
	mCapturePulse[i]=0x0026; i++;
	mCapturePulse[i]=0x0013; i++;
	mCapturePulse[i]=0x0013; i++;
	mCapturePulse[i]=0x0013; i++;
*/
//	mCaptureIndex=100;

//	i = izuvo_search_header();

//	mZuvoAuto = 1;
//	izuvo_init_capture_pulse();

	i=eeprom_read_byte(0x00);	
	mTestMode = (i+1) & 0x03;
	eeprom_write_byte(0x00,mTestMode);

	izuvo_test_mode	(mTestMode);

	u_puts(VERSION);
	u_putHexByte(mTestMode,' ');
	u_puts("\r\n");

}


void process_tmr(void)
{
//	static unsigned int tCnt=0;
	static unsigned char tSecCnt=0;	
//	static unsigned char tLED=0;

	if (mTimer0_Flag==1)	// @ very 196.6ms
	{
		mTimer0_Flag=0;
		tSecCnt++;
		if (tSecCnt>=10)
		{
			tSecCnt=0;

//			if (mLinuxMode!=2)
//				u_puts(EXIT);
		}

		led_task();

/*		tCnt ++	;


		tLED++;
		if (tLED>=(LED_PWM<<1))
		{
			tLED = 0x00;
		}

		if (tLED<LED_PWM)
		{
			mLED[2] = tLED;
		}
		else
		{
			mLED[2] = (tLED ^ 0xff) & 0x1f;
		}

		if (mLED[2]&0x0f >=LED_PWM)
		{
			mLED[2]&=0xf0;
		}
		else
		{
			mLED[2]++;
		}

		if ((tCnt)>=10)		
		{
			tCnt=0;
			if (mLED[3]==0x00)
			{
				mLED[3]=0xff;
			}
			else
			{
				mLED[3]=0x00;
			}

			mLED[0]=mLED[3];
			mLED[1]=mLED[0] ^ 0xff;

		}


		switch (mFunctionMode)
		{
			case 0:
				break;
			case 1:
//				ir_send_pulseEx();
				break;
			case 2:
				if (mZuvoAuto==0)
				{
//					ir_capture_pulse();
				}
				break;
			default:
				break;
		}

*/

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
//		PORTC ^=(1 << LED1);

		// Valid message neeed to have at least FOUR character
/*		if (index>=3) 
		{
			if (mLinuxMode<=0x01)
				process_cmd_linux(mLinuxMode);
			else if ((mCmdQ[1]=='~') && (mCmdQ[index]=='~'))
				process_cmd_hal(&mCmdQ[2],index-2);

		}
*/

		process_cmd_hal(&mCmdQ[2],index-2);
		mCmdQ[0]=0;		
		mCmdQFlag=0;
	}
}
/*
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
*/
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
/*
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
*/
		case 'V':	// Version
			u_puts(LinuxCmd);
			u_puts("'V' ");
			u_puts(VERSION);
			u_puts(" |");
			u_putHexByte(mTestMode,'|');
			u_putHexByte(mZuVoMode,'|');
			u_puts("'\r\n");
			break;

		case 'R': 	// Remote command
//			process_remote_command((char*) (sptr));
			break;

		case 'Z': 	// iFaucet
//			mZuvoAuto ^= 0x01;
//			izuvo_init_capture_pulse();
//			u_puts("ZuVo ");
//			u_putHexByte(mZuvoAuto,' ');
//			u_puts("\r\n");
			process_cmd_ifaucet((char*)sptr, len);
			break;

		case 'E': 	// iZuVo echo command
			process_cmd_izuvo((char*)sptr, len);
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

void izuvo_test_mode(unsigned char iMode)
{

	switch (iMode)
	{
		case 0:
			//1F301F301F301F30  : all fade
			mLED[0]=0x1F30;
			mLED[1]=0x1F30;
			mLED[2]=0x1F30;
			mLED[3]=0x1F30;
			break;
		case 1:
			//1F301F251F2A1F2F : Fade, Blink , Blink, Blink
			mLED[0]=0x1F30;
			mLED[1]=0x1F25;
			mLED[2]=0x1F2A;
			mLED[3]=0x1F2F;
			break;
		case 2:
			//1F101F151F1A1F1F : ON, ON , ON, ON
			mLED[0]=0x1F10;
			mLED[1]=0x1F10;
			mLED[2]=0x1F10;
			mLED[3]=0x1F10;
			break;
		default:
			//EF1F101F251F4A1F6F : ON, Blink-InVBlink, Blink
			mLED[0]=0x1F10;
			mLED[1]=0x1F25;
			mLED[2]=0x1F4A;
			mLED[3]=0x1F6F;
			break;	
	}





}

void process_cmd_izuvo_mode(unsigned char iMode)
{

	mZuVoMode = iMode;
/*
	u_puts("Mode = ");	
	u_putHexByte(tMode,' ');
	u_delay(10);
	u_puts("\r\n");	
*/
	u_putHexByte(mZuVoMode,' ');
	u_delay(10);
	u_puts("\r\n");	

	switch (mZuVoMode)
	{
		case State_Filter_Need_Flush_On:
			break;
		case State_Filter_Need_Flush_Off:
			break;
		case State_Filter_Normal_On:
			break;
		case State_Filter_Normal_Off:
			break;
		case State_Filter_Replace_Soon_On :
			break;
		case State_Filter_Replace_Soon_Off :
			break;
		case State_Filter_Replace_Now_On :
			break;
		case State_Filter_Replace_Now_Off :
			break;
		case State_FlowTimeToLong :
			break;
		default:
			break;
	}


}


//izuvo_update.sh ZFFFF000000000012001D0

void process_cmd_ifaucet(char *sptr, unsigned char len)
{
	unsigned char i;
	unsigned int tmp;


	if (len == 0x24)
	{
//		u_puts("Valid command length\r\n");	
		tmp = u_asc2uint(2,	sptr + 30);
		process_cmd_izuvo_mode(tmp & 0xff);
	}
	else
	{
		u_puts("iFaucet command 0x");	
		u_putHexByte(len, ' ');
		u_puts("/0x24 \r\n");

		u_puts("Invalid Length ");	
		for (i=1; i<len; i++)
		{
			tmp = *(sptr+i);
			u_putch(tmp);
		}
		u_puts("\r\n");
	}
}


//ELZ384E120005014D01001


void process_cmd_izuvo(char *sptr, unsigned char len)
{
	unsigned char i;
	char tmp;
	if (len <=2 )
	{
		u_puts("iZuVo echo command 0x");	
		u_putHexByte(len, ' ');
		u_puts("/0x16 \r\n");
	}
	else
	{

		tmp = *(sptr+1);

		if (tmp== 'L')
		{
			u_puts("iZuVo Local echo\r\n");	
		}

		if (tmp == 'S')
		{
			u_puts("iZuVo Server echo\r\n");	
		}


		if (tmp == 'F')
		{
			u_puts("iZuVo Faucet echo ");	
			u_putHexByte(len, ' ');
			if (len>=0x12)
			{
				process_cmd_izuvo_led(sptr+2);				
			}
			for (i=0;i<=3;i++)
			{
				u_putHexWord(mLED[i]);
				u_puts(" ");
			}
			u_puts("\r\n");
		}


/*		for (i=1; i<len; i++)
		{
			tmp = *(sptr+i);
			u_putch(tmp);
		}
		u_puts("\r\n");
*/
	}
}


void process_cmd_izuvo_led(char *sptr)
{

		mLED[0]=u_asc2uint(4,sptr);
		mLED[1]=u_asc2uint(4,sptr+4);
		mLED[2]=u_asc2uint(4,sptr+8);
		mLED[3]=u_asc2uint(4,sptr+12);


}

void led_task(void)
{

// @ very 200ms

	unsigned char i=0;
	unsigned char mode=0;
	unsigned char period=0;
	unsigned char brightness=0;

	for (i=0; i<=3; i++)
	{
		brightness = (mLED[i] >> 8) & 0xff;
		mode = (mLED[i] >> 4) & 0x000f;
		period = (mLED[i] & 0x000f)+1;

		mLED_Cnt[i]++;
		if (mLED_Cnt[i]>(period<<1))
		{
			mLED_Cnt[i] = 0x00;
		}
		
		switch (mode)
		{
			case 0:			// OFF
				mLED_Brightness[i]=0x00;
				break;
			case 1:			// ON
				mLED_Brightness[i]=brightness;
				break;
			case 2:			// Blink
				if (mLED_Cnt[i]<period)
				{	
					mLED_Brightness[i]=brightness;
				}
				else
				{
					mLED_Brightness[i]=0x00;
				}
				break;
			case 3:			// Fade

				if ((mLED_Cnt[i] == 0) || (mLED_Cnt[i] == period))
				{
					mLED_Fade[i] ++;
					if (mLED_Fade[i] >= (brightness<<1))
					{
						mLED_Fade[i]=0x00;
					}

					if (mLED_Fade[i]<brightness)
					{
						mLED_Brightness[i] = mLED_Fade[i];
					}
					else
					{
						mLED_Brightness[i] = (brightness<<1) - mLED_Fade[i];
					}
				}
				break;
			case 4:			// Inv
				if (i==1)	// Y Inv_B
				{
					if (mLED_Brightness[2]==0)
					{
						mLED_Brightness[1]=brightness;
					}
					else
					{
						mLED_Brightness[1]=0;
					}
				}
				if (i==2)	// B Inv_Y
				{
					if (mLED_Brightness[1]==0)
					{
						mLED_Brightness[2]=brightness;
					}
					else
					{
						mLED_Brightness[2]=0;
					}

				}
				if (i==3)	// R Inv_Y
				{
					if (mLED_Brightness[1]==0)
					{
						mLED_Brightness[3]=brightness;
					}
					else
					{
						mLED_Brightness[3]=0;
					}
				}
				break;
			case 5:			// Inv
				if (i==1)	// Y Inv_R
				{
					if (mLED_Brightness[3]==0)
					{
						mLED_Brightness[1]=brightness;
					}
					else
					{
						mLED_Brightness[1]=0;
					}
				}
				if (i==2)	// B Inv_R
				{
					if (mLED_Brightness[3]==0)
					{
						mLED_Brightness[2]=brightness;
					}
					else
					{
						mLED_Brightness[2]=0;
					}

				}
				if (i==3)	// R Inv_B
				{
					if (mLED_Brightness[2]==0)
					{
						mLED_Brightness[3]=brightness;
					}
					else
					{
						mLED_Brightness[3]=0;
					}
				}
				break;
			case 6:			// Sync
				if (i==1)	// Y_B
				{
					if (mLED_Brightness[2]==0)
					{
						mLED_Brightness[1]=0;
					}
					else
					{
						mLED_Brightness[1]=brightness;
					}
				}
				if (i==2)	// B_Y
				{
					if (mLED_Brightness[1]==0)
					{
						mLED_Brightness[2]=0;
					}
					else
					{
						mLED_Brightness[2]=brightness;
					}

				}
				if (i==3)	// R_Y
				{
					if (mLED_Brightness[1]==0)
					{
						mLED_Brightness[3]=0;
					}
					else
					{
						mLED_Brightness[3]=brightness;
					}
				}
				break;
			case 7:			// Sync
				if (i==1)	// Y_R
				{
					if (mLED_Brightness[3]==0)
					{
						mLED_Brightness[1]=brightness;
					}
					else
					{
						mLED_Brightness[1]=0;
					}
				}
				if (i==2)	// B_R
				{
					if (mLED_Brightness[3]==0)
					{
						mLED_Brightness[2]=0;
					}
					else
					{
						mLED_Brightness[2]=brightness;
					}

				}
				if (i==3)	// R_B
				{
					if (mLED_Brightness[2]==0)
					{
						mLED_Brightness[3]=0;
					}
					else
					{
						mLED_Brightness[3]=brightness;
					}
				}
				break;


		}	


	}


}

void led_driver(void)
{

	static unsigned char tCnt=0;
	unsigned char tLED=0;

	tCnt++;
	if (tCnt>LED_PWM)
	{
		tCnt=0;
	}

	tLED = mLED_Brightness[0];
	if (tLED==0)
	{
		LED_L_OFF();
	}
	else if (tLED>tCnt)
	{
		LED_L_ON();
	}
	else
	{
		LED_L_OFF();
	}

	tLED = mLED_Brightness[1];
	if (tLED==0)
	{
		LED_Y_OFF();
		LED_YY_OFF();
	}
	else if (tLED>tCnt)
	{
//		LED_Y_ON();
		LED_YY_ON();
	}
	else
	{
		LED_Y_OFF();
		LED_YY_OFF();
	}

	tLED = mLED_Brightness[2];
	if (tLED==0)
	{
		LED_B_OFF();
		LED_BB_OFF();
	}
	else if (tLED>tCnt)
	{
//		LED_B_ON();
		LED_BB_ON();
	}
	else
	{
		LED_B_OFF();
		LED_BB_OFF();
	}

	tLED = mLED_Brightness[3];
	if (tLED==0)
	{
		LED_R_OFF();
		LED_RR_OFF();
	}
	else if (tLED>tCnt)
	{
//		LED_R_ON();
		LED_RR_ON();
	}
	else
	{
		LED_R_OFF();
		LED_RR_OFF();
	}


}
