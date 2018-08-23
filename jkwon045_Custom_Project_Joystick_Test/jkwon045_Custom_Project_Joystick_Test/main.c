/*
 * jkwon045_fli034_lab08_part1.c
 *
 * Created: 8/15/2018 12:07:59 PM
 * Author : jckwo
 */ 

#include <avr/io.h>
#define REF_AVCC (1 <<REFS0) // Reference = AVCC = 5 V
#define REF_INT (1 << REFS0) | (1 << REFS1) //Internal reference 2.56 V
#define UPAxisInit 543
#define LRAxisInit 551
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

void wait(long);

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; //Setting A to be input
	DDRB = 0xFF; PORTB = 0x00; //Setting B to be output

	unsigned short val1;
	unsigned short val2;
	unsigned char bVal;
	
	ADC_init();
	
    while (1) 
    {
		bVal = 0x00;
		ADMUX = REF_AVCC | 0x00;
		wait(300);
		val1 = ADC;
		ADMUX = REF_AVCC | 0x01;
		wait(300);
		val2 = ADC;
		
		if (val1 > UPAxisInit+10){ 
			bVal |= 0x01;
		}
		else if (val1 < UPAxisInit-10) {
			 bVal |= 0x02;
		}
		
		if(val2 > LRAxisInit+10) bVal |= 0x04;
		else if (val2 < LRAxisInit-10) bVal |= 0x08;
		
		PORTB = bVal;
		
    }
}

void wait(long numOP){
	for( long i = 0; i < numOP; i++){
		asm("nop");
	}
}