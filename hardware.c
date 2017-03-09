#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>


#include "constants.h"
#include "hardware.h"
#include "timer.h"
#include "uart.h"

uint8_t MOSFET_PINS[PORT_COUNT] = {PD5, PD6, PD7, PB0};
uint8_t FLOW_SENSOR_PINS[PORT_COUNT] = {PC1, PC2, PC3, PC4};

void hardware_init(void)
{
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		MOSFET_DIR(ii);
		FLOW_DIR(ii);
	}
	
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

