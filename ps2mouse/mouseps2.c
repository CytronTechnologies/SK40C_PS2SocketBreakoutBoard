  /*******************************************************************************

*******************************************************************************/
// PTK40A with PIC16F877A 
// This template C file should be compiled with:
// MPLAB IDE v8.63
// HI-TECH C for the PIC16 MCU Family
// HI-TECH operate in LITE Mode with v9.80
// Last Updated date: 27 June 2012
// Please email support@cytron.com.my if you have any problem

#include <htc.h>		//include hitech C header file
#include "system.h"		//include system header file
#include "lcd.h"		//include lcd header file


/*******************************************************************************
* DEVICE CONFIGURATION WORDS                                                   *
*******************************************************************************/
// This configuration bits are for PIC16F877A using 20MHz Crystal or HS for OSC
#if defined (HITECH_V9_80)	//if Hi-Tech V9.80 compiler is being used
__CONFIG(HS &			// High Speed Crystal.
		 WDTDIS &		// Disable Watchdog Timer.
		 PWRTEN &		// Enable Power Up Timer.
		 BORDIS &		// Disable Brown Out Reset.
		 LVPDIS);		// Disable Low Voltage Programming.

#elif defined (HITECH_V9_82)		//else if Hi-Tech V9.82 compiler is being used
// If Hi-TECH C Compiler v9.82 is use, this configuration bit should be used
__CONFIG(FOSC_HS &			// High Speed Crystal.
		 WDTE_OFF &		// Disable Watchdog Timer.
		 PWRTE_ON &		// Enable Power Up Timer.
		 BOREN_OFF &		// Disable Brown Out Reset.
		 LVP_OFF);		// Disable Low Voltage Programming.
#endif	//end of pre-processor if condition


/*******************************************************************************
* Defination of label		                                                  *
*******************************************************************************/
#define CLOCK RC3
#define DATA RC4

/*******************************************************************************
* PRIVATE FUNCTION PROTOTYPES                                                  *
*******************************************************************************/
void delay_ms(unsigned int ui_value);
void beep(unsigned char uc_count);
unsigned char conv(unsigned char value);
void chk(unsigned char value,unsigned int ver);
void AtoZ(unsigned char value);
void send(void);
void send1(void);
void send2(void);
/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/
unsigned char shift =0;

/*******************************************************************************
* MAIN FUNCTION                                                                *
*******************************************************************************/
int main(void)
{
	//unsigned int data[22] = {0};
	unsigned int data_in = 0;
	unsigned char data_out = 0;
	unsigned char data_chk = 0;
	unsigned int data_ver = 0;
	unsigned char i = 0;
	unsigned char value = 0;	// declare a variable to store 
	// ensure all the hardware port in zero initially
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;

	// Initialize the I/O port direction, this must be configured according to circuit
	// please refer to PTK40A schematic for details
	// TRISX control pin direction, output pin must be configure as '0'
	// while input must be configure as '1'
	TRISA = 0b00110001;
	TRISB = 0b00001111;
	TRISC = 0b10011011;
	TRISD = 0;
	TRISE = 0;

//
//	//initialize UART for using it
//	uart_initialize();
//	//Initialize SPI to be use
//	spi_initialize();		

	//Initialize LCD to be use
	lcd_initialize();
			

	lcd_goto(0x82);		
	lcd_putstr("Cytron Tech");
	lcd_goto(0xc4);
	lcd_putstr("PS/2");
	delay_ms(400);
	lcd_clear();
	while(1) 	// create an infinite loop
	{
		//this is the part where is mouse need to be initialize so that it start transmit data
		while(CLOCK==1);
		for(i=0;i<=10;i++)
		{
			while(CLOCK==1);
			while(CLOCK==0);	
		}	
		send();
		delay_ms(800);
		send1();
		delay_ms(10);
		send2();
		
	//	this is the part where we start to detect and movement of the mouse
			while(1)
			{
					while(CLOCK==1);
					
						for(i=0;i<=10;i++)
						{
							while(CLOCK==1);
							while(CLOCK==0);
							data_in = (data_in | DATA) << 1; //collect all the data to data_in in 1- bits 
						}
						data_in >>=1;		//shift back the data once to become 8 bits
						data_out=data_in>>2 ;  //shift out the parity bit and put to char to cancel up the first bits
						data_chk=conv(data_out);  //turn the data to become normal data
						lcd_goto(0x00);				
						lcd_bcd(5,data_chk);  //convert and display the original data value
										
					AtoZ(data_chk);   //to display the original key value
				}		
	}	
		while(1) continue;	// infinite loop to prevent PIC from reset if there is no more program	
}

/*******************************************************************************
* PRIVATE FUNCTION: delay_ms
*
* PARAMETERS:
* ~ ui_value	- The period for the delay in miliseconds.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Delay in miliseconds.
*
*******************************************************************************/
void delay_ms(unsigned int ui_value)
{
	while (ui_value-- > 0) {
		__delay_ms(1);		// macro from HI-TECH compiler which will generate 1ms delay base on value of _XTAL_FREQ in system.h
	}	
}

/*******************************************************************************
* PRIVATE FUNCTION: beep
*
* PARAMETERS:
* ~ uc_count	- How many times we want to beep.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Beep for the specified number of times.
*
*******************************************************************************/
void beep(unsigned char uc_count)
{
	while (uc_count-- > 0) {
		BUZZER = 1;	//buzzer is share at RC2 pin
		delay_ms(80);
		BUZZER = 0;
		delay_ms(100);
	}
}

unsigned char conv(unsigned char value)
{
	
	value = (((value & 0xaa) >> 1) | ((value & 0x55) << 1));
    value = (((value & 0xcc) >> 2) | ((value & 0x33) << 2));
    value = (((value & 0xf0) >> 4) | ((value & 0x0f) << 4));
	
	return value;
}	


void chk(unsigned char value,unsigned int ver)
{
	unsigned char adding,adding1,i;	
	adding=value & 0x01;
		for(i=0;i<=7;i++)
		{
			value=value>>1;	
			adding1=value & 0x01;
			adding = adding + adding1;
			adding=adding & 0x01;
		}	
		if(adding!=ver)
		{
			lcd_goto(0x40);
			lcd_putstr("pass");	
		}
		else
		{
			lcd_goto(0x40);
			lcd_putstr("fail");
		}
	
}	



void send(void)
{	DATA=1;
	TRISC = 0b10000011;
	DATA=1;
	CLOCK=0;
	__delay_us(100);
	DATA=0;
	TRISC = 0b10001011;
	__delay_us(40);
	while(CLOCK==1);

	DATA=1;					//bit0
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);
	DATA=1;					//bit1
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit2
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit3
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit4
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit5
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit6
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit7
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bitparity
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);

	TRISC = 0b10011011;
	
}	

void send1(void)
{	DATA=1;
	TRISC = 0b10000011;
	DATA=1;
	CLOCK=0;
	__delay_us(100);
	DATA=0;
	TRISC = 0b10001011;
	__delay_us(40);
	while(CLOCK==1);

	DATA=0;					//bit0
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);
	DATA=1;					//bit1
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit2
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=0;					//bit3
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit4
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit5
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit6
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit7
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bitparity
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);

	TRISC = 0b10011011;
	
}	

void send2(void)
{	DATA=1;
	TRISC = 0b10000011;
	DATA=1;
	CLOCK=0;
	__delay_us(100);
	DATA=0;
	TRISC = 0b10001011;
	__delay_us(40);
	while(CLOCK==1);

	DATA=0;					//bit0
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);
	DATA=0;					//bit1
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit2
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=0;					//bit3
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit4
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit5
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit6
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=1;					//bit7
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);	
	DATA=0;					//bitparity
	while(CLOCK==1);
	while(CLOCK==0);
	__delay_us(10);

	TRISC = 0b10011011;
	
}	

void AtoZ(unsigned char value)
{
		lcd_goto(0x40);
		if(value == 9)
		{
		lcd_putstr("L_CLK");
		}
		else if(value ==10)
		{
		lcd_putstr("R_CLK");
		}
		else if(value ==12)
		{
		lcd_putstr("M_CLK");
		}
		else if(value ==40)
		{
		lcd_putstr("Yaxis");
		}
		else if(value == 24)
		{
		lcd_putstr("Xaxis");
		}
		else lcd_clear();
}	