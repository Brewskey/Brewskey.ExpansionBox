#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>


#include "hardware.h"
#include "timer.h"
#include "uart.h"


void hardware_init(void)
{
	MOSFET1_DIR;
	MOSFET2_DIR;
	MOSFET3_DIR;
	MOSFET4_DIR;
	
	FLOW1_DIR;
	FLOW2_DIR;
	FLOW3_DIR;
	FLOW4_DIR;
	
	RS485_DIR;	
	
	DISABLE_ADC;
	
	/* Timer to act as system timer */	
	TCCR0 |= _BV(CS00) | _BV(CS02);
	TIMSK |= _BV(TOIE0);
	
	/*UART Initialisation */
	UCSRB |= _BV(RXEN) | _BV(TXEN) | _BV(RXCIE) | _BV(TXCIE);
	UBRRH = (uint8_t) (USART_UBRR >> 8);
	UBRRL = (uint8_t) (USART_UBRR);		
	
	RS485_RX;
	
	sei();
}

