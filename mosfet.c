#include <avr/io.h>

#include "hardware.h"
#include "mosfet.h"
#include "flow.h"

/* Holds the states of the Mosfets */
uint8_t stateMosfets = 0;

/* Holds the ON time of the Mosfets */
MOSFET_TIMER_t mosfetTimers;

void Mosfet_Init(void)
{
	/* Switch off Mosfets */
	Mosfet_On_Off(MOSFET_1, OFF);
	Mosfet_On_Off(MOSFET_2, OFF);
	Mosfet_On_Off(MOSFET_3, OFF);
	Mosfet_On_Off(MOSFET_4, OFF);
	
	/* Resets Mosfet timers */
	mosfetTimers.mosfet1 = 0;
	mosfetTimers.mosfet2 = 0;
	mosfetTimers.mosfet3 = 0;
	mosfetTimers.mosfet4 = 0;
	
}

/***********************************************
Brief: Switches On/Off the Mosfets.
Param: Mosfet, State.
Retur: None.

Descr: Switches On/Off the Mosfets. When a Mosfet 
       is swithed on, it's repective flow counter
	   is reset.
************************************************/
void Mosfet_On_Off(MOSFET_t mosfet, STATE_t state)
{
	switch(mosfet)
	{
		case MOSFET_1:
				
				if(state)
				{
					if(!(stateMosfets & MOSFET1_ACTIVE))
					{
						/* Reset flow count only if Mosfet is off */
						/* else it will reset every time it receive on from network */
						Reset_Flow(FLOW1);
						
						/* Reset Mosfet Timer */
						mosfetTimers.mosfet1 = 0;
					}
					
					stateMosfets |= MOSFET1_ACTIVE; 	
					MOSFET1_ON;
				}
				else
				{
					stateMosfets &= ~MOSFET1_ACTIVE;
					MOSFET1_OFF;
				}
				
				break;
				
		case MOSFET_2:
		
				if(state)
				{
					if(!(stateMosfets & MOSFET2_ACTIVE))
					{
						/*reset flow count only if mosfet is off*/
						Reset_Flow(FLOW2);
						
						mosfetTimers.mosfet2 = 0;						
					}
					
					stateMosfets |= MOSFET2_ACTIVE; 
					MOSFET2_ON;
				}
				else
				{
					stateMosfets &= ~MOSFET2_ACTIVE;
					MOSFET2_OFF;
				}
				
				break;				

		case MOSFET_3:
		
				if(state)
				{
					if(!(stateMosfets & MOSFET3_ACTIVE))
					{
						/*reset flow count only if mosfet is off*/
						Reset_Flow(FLOW3);
						
						mosfetTimers.mosfet3 = 0;						
					}

					stateMosfets |= MOSFET3_ACTIVE; 
					MOSFET3_ON;
				}
				else
				{
					stateMosfets &= ~MOSFET3_ACTIVE;
					MOSFET3_OFF;
				}
				
				break;

		case MOSFET_4:
		
				if(state)
				{
					if(!(stateMosfets & MOSFET4_ACTIVE))
					{
						/*reset flow count only if mosfet is off*/
						Reset_Flow(FLOW4);
						
						mosfetTimers.mosfet4 = 0;						
					}

					stateMosfets |= MOSFET4_ACTIVE; 
					MOSFET4_ON;
				}
				else
				{
					stateMosfets &= ~MOSFET4_ACTIVE;
					MOSFET4_OFF;
				}
				
				break;				
	}
}

/***********************************************
Brief: Increments Mosfet Timer.
Param: None.
Retur: None.

Descr: Increments Mosfet Timer.
************************************************/
void Incr_Mosfet_Timers(void)
{
	if( mosfetTimers.mosfet1 < 250 )
		mosfetTimers.mosfet1++;
	
	if( mosfetTimers.mosfet2 < 250 )
		mosfetTimers.mosfet2++;

	if( mosfetTimers.mosfet3 < 250 )
		mosfetTimers.mosfet3++;

	if( mosfetTimers.mosfet4 < 250 )
		mosfetTimers.mosfet4++;
	
}

uint8_t getStateMosfets(void)
{
	return stateMosfets;
}

MOSFET_TIMER_t getMosfetTimers(void)
{
	return mosfetTimers;
}
