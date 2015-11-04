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
/*******************************************************************************
* PRIVATE FUNCTION PROTOTYPES                                                  *
*******************************************************************************/
void delay_ms(unsigned int ui_value);
void beep(unsigned char uc_count);
unsigned char conv(unsigned char value);
void chk(unsigned char value,unsigned int ver);
void AtoZ(unsigned char value);
/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/

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

	//Initialize LCD to be use
	lcd_initialize();
	
	lcd_goto(0x82);		
	lcd_putstr("Cytron Tech");
	lcd_goto(0xc4);
	lcd_putstr("PS/2");
	delay_ms(1000);
	lcd_clear();

	while(1) 	// create an infinite loop
	{
		while(RC3==1);
		{	
			for(i=0;i<=10;i++)
			{
				while(RC3==1);
				while(RC3==0);
				data_in = (data_in | RC4) << 1;
			}
			data_in >>=1;  				//shift back the data once to become 8 bits
			data_out=data_in>>2 ;		//shift out the parity bit and put to char to cancel up the first bits
			data_chk=conv(data_out);	//turn the data to become normal data
			lcd_goto(0x00);
			lcd_bcd(5,data_chk);  //convert and display the original data value
			data_ver=(data_in & 0x0002)>>1;
			chk(data_chk,data_ver);
			
			
			
		}
		AtoZ(data_chk);
		delay_ms(500);
		
		
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

unsigned char conv(unsigned char value)		//convertion to turn th LSB to MSB
{
	
	value = (((value & 0xaa) >> 1) | ((value & 0x55) << 1));
    value = (((value & 0xcc) >> 2) | ((value & 0x33) << 2));
    value = (((value & 0xf0) >> 4) | ((value & 0x0f) << 4));
	
	return value;
}	


void chk(unsigned char value,unsigned int ver)		//check the partiy bits weather it is correct or wrong
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


void AtoZ(unsigned char value)   //this is the part where to check the desire word to display
{
	lcd_goto(0x08);
		if(value==0x1C)lcd_putchar('A');
		else if(value==0x32)lcd_putchar('B');
		else if(value==0x21)lcd_putchar('C');
		else if(value==0x23)lcd_putchar('D');
		else if(value==0x24)lcd_putchar('E');
		else if(value==0x2B)lcd_putchar('F');
		else if(value==0x34)lcd_putchar('G');
		else if(value==0x33)lcd_putchar('H');
		else if(value==0x43)lcd_putchar('I');
		else if(value==0x3B)lcd_putchar('J');
		else if(value==0x42)lcd_putchar('K');
		else if(value==0x4B)lcd_putchar('L');
		else if(value==0x3A)lcd_putchar('M');
		else if(value==0x31)lcd_putchar('N');
		else if(value==0x44)lcd_putchar('O');
		else if(value==0x4D)lcd_putchar('P');
		else if(value==0x15)lcd_putchar('Q');
		else if(value==0x2D)lcd_putchar('R');
		else if(value==0x1B)lcd_putchar('S');
		else if(value==0x2C)lcd_putchar('T');
		else if(value==0x3C)lcd_putchar('U');
		else if(value==0x2A)lcd_putchar('V');
		else if(value==0x1D)lcd_putchar('W');
		else if(value==0x22)lcd_putchar('X');
		else if(value==0x35)lcd_putchar('Y');
		else if(value==0x1A)lcd_putchar('Z');
		else lcd_putchar(' ');
		
}	