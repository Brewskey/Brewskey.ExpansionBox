#include <avr/io.h>

#include "hardware.h"
#include "flow.h"
#include "mosfet.h"

/* Holds the pulses of the flow sensor */
FLOW_PULSES_t flow1;
FLOW_PULSES_t flow2;
FLOW_PULSES_t flow3;
FLOW_PULSES_t flow4;

void Flow_Init(void)
{
	flow1.flow = 0;
	flow2.flow = 0;
	flow3.flow = 0;
	flow4.flow = 0;
	
	flow1.timer = 0;
	flow2.timer = 0;
	flow3.timer = 0;
	flow4.timer = 0;
}

/***********************************************
Brief: Reset flow.
Param: Flow meter number.
Retur: None.

Descr: Reset the pulses of the respective flow meter
************************************************/
void Reset_Flow(FLOW_METER_t flowMeter)
{
	switch(flowMeter)
	{
		case FLOW1:
			flow1.flow = 0;
			break;
			
		case FLOW2:
			flow2.flow = 0;
			break;			
			
		case FLOW3:
			flow3.flow = 0;
			break;

		case FLOW4:
			flow4.flow = 0;
			break;			
	}
}

/***********************************************
Brief: Returns pulses.
Param: Flow meter number.
Retur: Pulses of the respective flow meter.

Descr: Returns pulses
************************************************/
uint32_t Get_Flow(FLOW_METER_t flowMeter)
{
	uint32_t pulses;
	
	switch(flowMeter)
	{
		case FLOW1:
			pulses = flow1.flow;
			break;
			
		case FLOW2:
			pulses = flow2.flow;
			break;			
			
		case FLOW3:
			pulses = flow3.flow;
			break;

		case FLOW4:
			pulses = flow4.flow;
			break;			
	}
	
	return pulses;
}

/***********************************************
Brief: Increments Time out.
Param: None.
Retur: None.

Descr: Increments Time out.
************************************************/
void Incr_Flow_Timers(void)
{
	if(flow1.timer < 250)
		flow1.timer++;
	
	if(flow2.timer < 250)
		flow2.timer++;

	if(flow3.timer < 250)
		flow3.timer++;
	
	if(flow4.timer < 250)
		flow4.timer++;	
}

/***********************************************
Brief: Reads Flow.
Param: None.
Retur: None.

Descr: Reads Flow.
************************************************/
void Read_Flow(void)
{
	/* Check for any active pulses and record */
	/* With every active pulse, reset the flow timer - for time out detection */
	
	flow1.hall <<= 1;
	flow2.hall <<= 1;
	flow3.hall <<= 1;
	flow4.hall <<= 1;
	
	flow1.hall |= (FLOW1_STATUS) ? 1 : 0;
	flow2.hall |= (FLOW2_STATUS) ? 1 : 0;
	flow3.hall |= (FLOW3_STATUS) ? 1 : 0;
	flow4.hall |= (FLOW4_STATUS) ? 1 : 0;
	
	if( (flow1.hall & 0xFF) == 0xF0)
	{
		flow1.flow++;
		flow1.timer = 0;
	}
	
	if( (flow2.hall & 0xFF) == 0xF0)
	{
		flow2.flow++;
		flow2.timer = 0;
	}

	if( (flow3.hall & 0xFF) == 0xF0)
	{
		flow3.flow++;
		flow3.timer = 0;
	}

	if( (flow4.hall & 0xFF) == 0xF0)
	{
		flow4.flow++;
		flow4.timer	= 0;
	}
}

/***********************************************
Brief: Time out Check.
Param: None.
Retur: None.

Descr: Checks for any time out of any active Mosfet.
       If a time out is detected, Mosfet is switched off.	 
************************************************/
void Check_Flow_Stop(void)
{
	/* Get the state of the Mosfets to know which ones are active */
	uint8_t mosfetsActive = getStateMosfets();
	
	/* Get the Mosfet timers*/
	MOSFET_TIMER_t mosfetTimers = getMosfetTimers();
	
	/*
		For the time-out to trigger and switch off the Mosfet, the Mosfet need to be ON
		and need to have some flow. 
		
		This happens when the client swipes the card but takes long to open the valve, hence
		not to trigger time out pre-maturely, it will need to wait for some flow before 
		time out can happen
	*/
	
	if( 
		( (mosfetsActive & MOSFET1_ACTIVE) && (flow1.timer > FLOW_TIMEOUT) && (flow1.flow > MIN_FLOW) )	|
		( (mosfetsActive & MOSFET1_ACTIVE) && (mosfetTimers.mosfet1 > MOSFET_NO_FLOW_TIMEOUT) && (flow1.flow == 0) )		
	  )
	{
		/* If Mosfet Active, and a flow was present, wait for flow time out to switch off Mosfet */
		/* If Mosfet Active, and no flow was present, wait for Mosfet time out to switch off Mosfet */
		Mosfet_On_Off(MOSFET_1, OFF);
	}
	
	if( 
		( (mosfetsActive & MOSFET2_ACTIVE) && (flow2.timer > FLOW_TIMEOUT) && (flow2.flow > MIN_FLOW) ) |
		( (mosfetsActive & MOSFET2_ACTIVE) && (mosfetTimers.mosfet2 > MOSFET_NO_FLOW_TIMEOUT) && (flow2.flow == 0) )
	  )		
	{
		Mosfet_On_Off(MOSFET_2, OFF);
	}	
	
	if( 
		( (mosfetsActive & MOSFET3_ACTIVE) && (flow3.timer > FLOW_TIMEOUT) && (flow3.flow > MIN_FLOW))	|
		( (mosfetsActive & MOSFET3_ACTIVE) && (mosfetTimers.mosfet3 > MOSFET_NO_FLOW_TIMEOUT) && (flow3.flow == 0) )
	  )
	{
		Mosfet_On_Off(MOSFET_3, OFF);
	}

	if( 
		( (mosfetsActive & MOSFET4_ACTIVE) && (flow4.timer > FLOW_TIMEOUT) && (flow4.flow > MIN_FLOW))	|
		( (mosfetsActive & MOSFET4_ACTIVE) && (mosfetTimers.mosfet4 > MOSFET_NO_FLOW_TIMEOUT) && (flow4.flow == 0) )	
	  )		
	{
		Mosfet_On_Off(MOSFET_4, OFF);
	}	
}


