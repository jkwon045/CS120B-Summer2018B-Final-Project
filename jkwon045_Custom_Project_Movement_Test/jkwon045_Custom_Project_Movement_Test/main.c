/*
 * jkwon045_Custom_Project_Movement_Test.c
 *
 * Created: 8/23/2018 1:54:32 PM
 * Author : jckwo
 */ 

#include <avr/io.h>
#include "analog_stick.h"

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned char LED = 0;
	
	unsigned char yval;
	unsigned char xval;
	ADC_init();
    /* Replace with your application code */
    while (1) 
    {
		LED = 0;
		xval = readXAxis();
		yval = readYAxis();
		
		if ( yval == UP ){
			LED |= 0x01;
		} 
		
		if ( yval == DOWN ){
			LED |= 0x02;
		}
		
		if ( xval == RIGHT ){
			LED |= 0x04;
		}
		
		if ( xval == LEFT ){
			LED |= 0x08;
		}
		
		PORTB = LED;
    }
}

