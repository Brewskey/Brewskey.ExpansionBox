#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "hardware.h"
#include "timer.h"
#include "uart.h"
#include "mosfet.h"
#include "flow.h"

SysTimer_t sysTimer;
SysTimer_t sysTimer_old;

Timer_t timer;

USARTBuffer usart0_buffer;


void main(void)
{
	
	hardware_init();	
	timer_init();
	Flow_Init();
	Mosfet_Init();
	
	
	wdt_enable(WDTO_2S);
	
	while(1)
	{
		wdt_reset();
							
		hardware_rs232_comms();		
		
		if(sysTimer._100ms != sysTimer_old._100ms)
		{
			sysTimer_old._100ms = sysTimer._100ms;
			
			Incr_Flow_Timers();
			
			Check_Flow_Stop();			
		}
		
		if(sysTimer._1s != sysTimer_old._1s)
		{
			sysTimer_old._1s = sysTimer._1s;

			Incr_Mosfet_Timers();			
		}	

		/* Read the flow continuously */	
		Read_Flow();


		 
	}
}

