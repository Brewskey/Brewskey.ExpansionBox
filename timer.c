#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "timer.h"
#include "hardware.h"

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 0x63;
	
	if(++(sysTimer._10ms) >= 10)
	{
		sysTimer._10ms = 0;
		
		if(++(sysTimer._100ms) >= 10)
		{
			sysTimer._100ms = 0;
			
			if(++(sysTimer._1s) >= 60)
			{
				sysTimer._1s = 0;
				
				if(++(sysTimer._1min) >= 60)
				{
					sysTimer._1min = 0;
					
					if(++(sysTimer._1hr) >= 24)
					{
						sysTimer._1hr = 0;
						
						if(sysTimer._1day != 0xFFFF)
						{
							sysTimer._1day++;
						}
					}
				}
			}
		}
	}
}

void timer_init(void)
{
	/*system timer*/
	sysTimer._10ms 	= 0;
	sysTimer._100ms = 0;
	sysTimer._1s 	= 0;
	sysTimer._1min 	= 0;
	sysTimer._1hr 	= 0;
	sysTimer._1day 	= 0;
	
	sysTimer_old = sysTimer;
	
	timer.rs232_timeout = 0;	
}
