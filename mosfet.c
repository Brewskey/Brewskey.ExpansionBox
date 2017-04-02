#include <avr/io.h>

#include "hardware.h"
#include "mosfet.h"
#include "flow.h"

uint8_t MOSFET_NETWORK_MASK[PORT_COUNT] = {0x03, 0x0C, 0x30, 0xC0};
	
/* Holds the states of the Mosfets */
uint8_t stateMosfets = 0;

/* Holds the ON time of the Mosfets */
uint8_t mosfetTimers[PORT_COUNT];

void Mosfet_Init(void)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		// Switch off Mosfets
		Mosfet_On_Off(ii, OFF);
		// Resets Mosfet timers
		mosfetTimers[ii] = 0;
	}
	
}

/***********************************************
Brief: Switches On/Off the Mosfets.
Param: Mosfet, State.
Return: None.

Description: Switches On/Off the Mosfets. When a Mosfet 
       is switched on, it's respective flow counter
	   is reset.
************************************************/
void Mosfet_On_Off(uint8_t mosfetIndex, STATE_t state)
{
	if (mosfetIndex >= PORT_COUNT) {
		return;
	}
	
	uint8_t mosfetFlag = (uint8_t)pow(2, mosfetIndex);
	if(state)
	{
		bool isMosfetActive = stateMosfets & mosfetFlag;
		if(!isMosfetActive)
		{
			/* Reset flow count only if Mosfet is off */
			/* else it will reset every time it receive on from network */
			Reset_Flow(mosfetIndex);
			
			/* Reset Mosfet Timer */
			mosfetTimers[mosfetIndex] = 0;
		}
		
		stateMosfets |= mosfetFlag;
		mosfet_on(mosfetIndex);
	}
	else
	{
		stateMosfets &= ~mosfetFlag;
		mosfet_off(mosfetIndex);
	}
}

/***********************************************
Brief: Increments Mosfet Timer.
Param: None.
Return: None.

Description: Increments Mosfet Timer.
************************************************/
void Incr_Mosfet_Timers(void)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		if( mosfetTimers[ii] < 250 ) {
			mosfetTimers[ii]++;
		}
	}
	
}

uint8_t getStateMosfets(void)
{
	return stateMosfets;
}

uint8_t* getMosfetTimers(void)
{
	return mosfetTimers;
}
