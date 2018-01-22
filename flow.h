#ifndef FLOW_H
#define FLOW_H

/* Set to 1 second since increment function executed every 100ms */
#define FLOW_TIMEOUT	5 // 5 seconds
#define MIN_FLOW		1

typedef struct
{
	uint8_t hall;
	uint8_t timer;
	uint32_t flow;
} FLOW_PULSES_t;

void Flow_Init(void);
void Read_Flow(void);
void Check_Flow_Stop(void);
void Incr_Flow_Timers(void);
uint32_t Get_Flow(uint8_t flowMeter);
void Reset_Flow(uint8_t flowMeter);

#endif
