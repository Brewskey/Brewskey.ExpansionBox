#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "timer.h"
#include "hardware.h"

ISR(TIMER0_OVF_vect) // Runs interrupt as a timer
{
	TCNT0 = 0x63; // Set this for 16MHZ
	
	if(++(sysTimer._10ms) < 10)
	{
		return;
	}
	
	sysTimer._10ms = 0;
		
	if(++(sysTimer._100ms) < 10)
	{
		return;
	}
	
	sysTimer._100ms = 0;
			
	if(++(sysTimer._1s) < 60)
	{
		return;
	}
	
	sysTimer._1s = 0;
}

void timer_init(void)
{
	/*system timer*/
	sysTimer._10ms 	= 0;
	sysTimer._100ms = 0;
	sysTimer._1s 	= 0;
	
	sysTimer_old = sysTimer;
}
