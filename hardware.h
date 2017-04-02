#ifndef HARDWARE_H
#define HARDWARE_H

#include "constants.h"

#define F_CPU 16000000UL

#define USART_BAUD 19200
#define USART_UBRR F_CPU/16/USART_BAUD-1

#ifdef EXPANSION_BOARD_4_PORTS
uint8_t MOSFET_PINS[PORT_COUNT];
uint8_t FLOW_SENSOR_PINS[PORT_COUNT];

/* ================== FLOW SENSORS ================== */
#define FLOW_DIR(pinIndex)     DDRC &= ~_BV(FLOW_SENSOR_PINS[pinIndex])
#define FLOW_STATUS(pinIndex)  PINC & _BV(FLOW_SENSOR_PINS[pinIndex])

/* ================== RS485 ================== */
#define RS485_DIR		      DDRD  |= _BV(PD2)
#define RS485_TX		      PORTD |= _BV(PD2)
#define RS485_RX		      PORTD &= ~_BV(PD2)

/* ================== ADC ================== */
#define DISABLE_ADC		    ADCSRA &= ~_BV(ADEN)

#endif

void hardware_init(void);

void mosfet_on(uint8_t pinIndex);
void mosfet_off(uint8_t pinIndex);
void mosfet_dir(uint8_t pinIndex);

#endif
