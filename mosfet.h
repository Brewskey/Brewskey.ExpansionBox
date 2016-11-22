#ifndef MOSFET_H
#define MOSFET_H

#define MOSFET1_ACTIVE	0x01
#define MOSFET2_ACTIVE	0x02
#define MOSFET3_ACTIVE	0x04
#define MOSFET4_ACTIVE	0x08

/* Masking bits for Mosfet switch on via network */
#define COMMS_MOSFET1_ON 0x03
#define COMMS_MOSFET2_ON 0x0C
#define COMMS_MOSFET3_ON 0x30
#define COMMS_MOSFET4_ON 0xC0

/* Masking bits for Mosfet override OFF via network */
/* Same bits as Mosfet ON, but did not use the 'ON' to eliminate confusion */
#define COMMS_MOSFET1_OFF 0x03
#define COMMS_MOSFET2_OFF 0x0C
#define COMMS_MOSFET3_OFF 0x30
#define COMMS_MOSFET4_OFF 0xC0

/* TImeout for the Mosfet on with no flow */
#define MOSFET_NO_FLOW_TIMEOUT	5

typedef struct
{
	uint8_t mosfet1;
	uint8_t mosfet2;
	uint8_t mosfet3;
	uint8_t mosfet4;
}MOSFET_TIMER_t;

typedef enum
{
	MOSFET_1,
	MOSFET_2,
	MOSFET_3,
	MOSFET_4	
}MOSFET_t;

typedef enum
{
	OFF = 0,
	ON
}STATE_t;



void Mosfet_Init(void);
uint8_t getStateMosfets(void);
void Incr_Mosfet_Timers(void);
MOSFET_TIMER_t getMosfetTimers(void);
void Mosfet_On_Off(MOSFET_t mosfet, STATE_t state);

#endif