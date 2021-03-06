#include <avr/io.h>

#include "hardware.h"
#include "mosfet.h"
#include "flow.h"

uint8_t MOSFET_NETWORK_MASK[PORT_COUNT] = {0x03, 0x0C, 0x30, 0xC0};
	
/* Holds the states of the Mosfets */
uint8_t stateMosfets = 0;

void Mosfet_Init(void)
{
	Mosfet_ALL_On_Off(OFF);
}

/***********************************************
Brief: Switches all mosfets On/Off.
Param: State.
Return: None.

Description: Switches On/Off for all the Mosfets.
************************************************/
void Mosfet_ALL_On_Off(STATE_t state)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		Mosfet_On_Off(ii, state);
	}
}

/***********************************************
Brief: Switches On/Off the Mosfets.
Param: Mosfet, State.
Return: None.

Description: Switches On/Off the Mosfets.
************************************************/
void Mosfet_On_Off(uint8_t mosfetIndex, STATE_t state)
{
	if (mosfetIndex >= PORT_COUNT) {
		return;
	}
	
	uint8_t mosfetFlag = MOSFET_NETWORK_MASK[mosfetIndex];
	if(state)
	{
		stateMosfets |= mosfetFlag;
		mosfet_on(mosfetIndex);
	}
	else
	{
		stateMosfets &= ~mosfetFlag;
		mosfet_off(mosfetIndex);
	}
}

uint8_t getStateMosfets(void)
{
	return stateMosfets;
}