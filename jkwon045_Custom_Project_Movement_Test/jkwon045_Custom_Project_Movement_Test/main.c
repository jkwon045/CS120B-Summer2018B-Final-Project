/*
 * jkwon045_Custom_Project_Movement_Test.c
 *
 * Created: 8/23/2018 1:54:32 PM
 * Author : jckwo
 */ 

#include <avr/io.h>
#define REF_AVCC (1 <<REFS0) // Reference = AVCC = 5 V
#define REF_INT (1 << REFS0) | (1 << REFS1) //Internal reference 2.56 V
#define UPAxisInit 543
#define LRAxisInit 551

void ADC_init();
void wait(long);

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

void wait(long numOP){
	for( long i = 0; i < numOP; i++){
		asm("nop");
	}
}