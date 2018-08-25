/*
*	Jasmine Kwong
*	Header file for reading analog stick input
*	analogStick.h
*/

#ifndef ANALOGSTICK_H
#define ANALOGSTICK_H

#include <avr/io.h>

//Defined for if there is no input
#define NONE 0 
// Defining cardinal directions as ints
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define YAXISINIT 543
#define XAXISINIT 551

#define REF_AVCC ( 1 << REFS0 )
#define REF_INIT ( 1 << REFS0 ) | ( 1 << REFS1 ) 

#define YSENSOR 0x01 //Y-axis sensor is on port ADC1
#define XSENSOR 0x00 //X-axis sensor is on port ADC0

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

void wait( unsigned long numOPs ) {
	for (unsigned long i = 0; i < numOPs; i++) {
		asm("nop");
	}
}

unsigned char readYAxis(void) {
	unsigned short adcVal;
	
	ADMUX = REF_AVCC | YSENSOR; 
	wait(300);

	adcVal = ADC;

	if (adcVal > (YAXISINIT + 20)) {
		return UP;
	}
	else if (adcVal < (YAXISINIT - 20)) {
		return DOWN;
	}

	return NONE;
}

unsigned char readXAxis(void) {
	unsigned short adcVal;

	ADMUX = REF_AVCC | XSENSOR;
	wait(300);

	adcVal = ADC;

	if (adcVal > (XAXISINIT + 20)) {
		return RIGHT;
	}
	else if (adcVal < (XAXISINIT - 20)) {
		return LEFT;
	}

	return NONE;
}
#endif
