#include <avr/io.h>

#include "hardware.h"
#include "flow.h"
#include "mosfet.h"

/* Holds the pulses of the flow sensor */
FLOW_PULSES_t flowSensors[PORT_COUNT];

void Flow_Init(void)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		flowSensors[ii].flow = 0;
		flowSensors[ii].timer = 0;
	}
}

/***********************************************
Brief: Reset all flow sensors.
Return: None.

Description: Reset the pulses/timer of all flow meters
************************************************/
void Reset_All_Flow(void)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		Reset_Flow(ii);
	}
}

/***********************************************
Brief: Reset flow.
Param: Flow meter number.
Return: None.

Description: Reset the pulses/timer of the respective flow meter
************************************************/
void Reset_Flow(uint8_t flowMeter)
{
	if (flowMeter >= PORT_COUNT) {
		return;
	}

	flowSensors[flowMeter].flow = 0;
	flowSensors[flowMeter].timer = 0;
}

/***********************************************
Brief: Reset flow timer.
Param: Flow meter number.
Return: None.

Description: Reset the timer of the respective flow meter
************************************************/
void Reset_Flow_Timer(uint8_t flowMeter)
{
	if (flowMeter >= PORT_COUNT) {
		return;
	}

	flowSensors[flowMeter].timer = 0;
}

/***********************************************
Brief: Returns pulses.
Param: Flow meter number.
Return: Pulses of the respective flow meter.

Description: Returns pulses
************************************************/
uint32_t Get_Flow(uint8_t flowMeter)
{
	if (flowMeter >= PORT_COUNT) {
		return 0;
	}

	return flowSensors[flowMeter].flow;
}

/***********************************************
Brief: Increments Time out.
Param: None.
Return: None.

Description: Increments Time out.
************************************************/
void Incr_Flow_Timers(void)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		if (flowSensors[ii].flow != 0) {
			flowSensors[ii].timer++;
		}
	}
}

/***********************************************
Brief: Reads Flow.
Param: None.
Return: None.

Description: Reads Flow.
************************************************/
void Read_Flow(void)
{
	/* Check for any active pulses and record */
	/* With every active pulse, reset the flow timer - for time out detection */
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		flowSensors[ii].hall <<= 1;
		flowSensors[ii].hall |= FLOW_STATUS(ii) ? 1 : 0;
		
		if ((flowSensors[ii].hall & 0xFF) == 0xF0)
		{
			flowSensors[ii].flow++;
			flowSensors[ii].timer = 0;
		}
	}
}

/***********************************************
Brief: Time out Check.
Param: None.
Return: None.

Description: Checks for any time out of any active Mosfet.
       If a time out is detected, Mosfet is switched off.
************************************************/
void Check_Flow_Stop(void)
{
	/*
		For the time-out to trigger and switch off the Mosfet, the Mosfet need to be ON
		and need to have some flow.

		This happens when the client swipes the card but takes long to open the valve, hence
		not to trigger time out prematurely, it will need to wait for some flow before
		time out can happen
	*/
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		if (
			flowSensors[ii].timer >= FLOW_TIMEOUT
		) {
			Mosfet_On_Off(ii, OFF);
			Reset_Flow(ii);
		}
	}
}
