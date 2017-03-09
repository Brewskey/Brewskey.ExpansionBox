#ifndef MOSFET_H
#define MOSFET_H

/* Masking bits for Mosfet switch on via network */
#ifdef EXPANSION_BOARD_4_PORTS
uint8_t MOSFET_NETWORK_MASK[PORT_COUNT];
#endif

/* TImeout for the Mosfet on with no flow */
#define MOSFET_NO_FLOW_TIMEOUT	5

typedef enum
{
	OFF = 0,
	ON
} STATE_t;

void Mosfet_Init(void);
uint8_t getStateMosfets(void);
void Incr_Mosfet_Timers(void);
uint8_t* getMosfetTimers(void);
void Mosfet_On_Off(uint8_t mosfetIndex, STATE_t state);

#endif