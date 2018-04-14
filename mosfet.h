#ifndef MOSFET_H
#define MOSFET_H

/* Masking bits for Mosfet switch on via network */
#ifdef EXPANSION_BOARD_4_PORTS
uint8_t MOSFET_NETWORK_MASK[PORT_COUNT];
#endif

typedef enum
{
	OFF = 0,
	ON
} STATE_t;

void Mosfet_Init(void);
uint8_t getStateMosfets(void);
void Mosfet_ALL_On_Off(STATE_t state);
void Mosfet_On_Off(uint8_t mosfetIndex, STATE_t state);

#endif