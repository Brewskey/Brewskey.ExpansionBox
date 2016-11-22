#ifndef HARDWARE_H
#define HARDWARE_H

#define F_CPU 16000000UL

#define USART_BAUD 19200
#define USART_UBRR F_CPU/16/USART_BAUD-1

/* ================== MOSFETS ================== */
#define MOSFET1_ON		PORTD |= _BV(PD5)
#define MOSFET1_OFF		PORTD &= ~_BV(PD5)
#define MOSFET1_DIR		DDRD  |= _BV(PD5)

#define MOSFET2_ON		PORTD |= _BV(PD6)
#define MOSFET2_OFF		PORTD &= ~_BV(PD6)
#define MOSFET2_DIR		DDRD  |= _BV(PD6)

#define MOSFET3_ON		PORTD |= _BV(PD7)
#define MOSFET3_OFF		PORTD &= ~_BV(PD7)
#define MOSFET3_DIR		DDRD  |= _BV(PD7)

#define MOSFET4_ON		PORTB |= _BV(PB0)
#define MOSFET4_OFF		PORTB &= ~_BV(PB0)
#define MOSFET4_DIR		DDRB  |= _BV(PB0)

/* ================== FLOW SENSORS ================== */
#define FLOW1_DIR		DDRC &= ~_BV(PC1)
#define FLOW2_DIR		DDRC &= ~_BV(PC2)
#define FLOW3_DIR		DDRC &= ~_BV(PC3)
#define FLOW4_DIR		DDRC &= ~_BV(PC4)

#define FLOW1_STATUS	PINC & _BV(PC1)
#define FLOW2_STATUS	PINC & _BV(PC2)
#define FLOW3_STATUS	PINC & _BV(PC3)
#define FLOW4_STATUS	PINC & _BV(PC4)


/* ================== RS485 ================== */
#define RS485_DIR		DDRD  |= _BV(PD2)
#define RS485_TX		PORTD |= _BV(PD2)
#define RS485_RX		PORTD &= ~_BV(PD2)

/* ================== ADC ================== */
#define DISABLE_ADC		ADCSRA &= ~_BV(ADEN)

void hardware_init(void);

#endif