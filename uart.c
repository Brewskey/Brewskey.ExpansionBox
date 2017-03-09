#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "uart.h"
#include "hardware.h"
#include "timer.h"
#include "mosfet.h"
#include "flow.h"

/* Holds the number of 'U's to jump in bootloader */
uint8_t bootloaderCnt = 0;

ISR(USART_RXC_vect)
{
    unsigned char data;
    uint8_t temp_head;

    data = UDR;													
	
    temp_head = (usart0_buffer.RxHead + 1) & USART_BUFFER_MASK;

    if(temp_head != usart0_buffer.RxTail)
    {	
		usart0_buffer.RxHead = temp_head;
		usart0_buffer.RxBuffer[usart0_buffer.RxHead] = data;
    }
	

}

ISR(USART_TXC_vect)
{
	
    if(usart0_buffer.TxTail != usart0_buffer.TxHead)							/* Check if head had been reached */
    {
		usart0_buffer.TxTail = (usart0_buffer.TxTail + 1) & USART_BUFFER_MASK;	/* Incr tail if no */

		UDR = usart0_buffer.TxBuffer[usart0_buffer.TxTail];						/* Send current bye pointed by tail to uart */
    }
    else
    {
		usart0_buffer.transmit_active = 0;										/* Reset flag */
		RS485_RX;
	}

}


void hardware_USART0_send_str(char* str)
{
	while(*str) /*while not null character*/
	{		
		wdt_reset();
		
		while ( !( UCSRA & (1<<UDRE)) );
		
		UDR = *str++;
	}
	
}

void hardware_USART0_send_esc(char stx, char* pstr, int length)
{
	char * str = pstr;  //.. str = pstr
	uint8_t temp_head;
	int i = 0;
	
	temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;			/* Increment temp head */

	if( temp_head != usart0_buffer.TxTail)								/* Check if temp head is equal to tail */
	{
		usart0_buffer.TxHead = temp_head;								/* If not equal, set temp head to head, basic incr head */
		usart0_buffer.TxBuffer[usart0_buffer.TxHead] = stx;				/* Write first char '+' in head position */
	}
	
	while((i < length)) 												/* Loop around the data buffer */
	{
			
	    temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;		/* Inc temp head */

	    if( (*str == '*' || *str == '+' || *str == '-' || *str == '#') && temp_head != usart0_buffer.TxTail)
	    {
			usart0_buffer.TxHead = temp_head;						/* If any symbol found, inc head */
			usart0_buffer.TxBuffer[usart0_buffer.TxHead] = '#';		/* Place '#' in current location */
	    }

		temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;	/* Incr temp head (same condition as before if no symbol found) */

	    if( temp_head != usart0_buffer.TxTail)						/* If tail not reached */
	    {
			usart0_buffer.TxHead = temp_head;						/* Incr head */
			usart0_buffer.TxBuffer[usart0_buffer.TxHead] = *str;	/* Place char in buffer */
	    }
		
		str++;														/* Point to next char */
		i++;														/* Incr length index */
	}
	
	temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;		/* Incr temp head */

	if( temp_head != usart0_buffer.TxTail)							/* If tail not reached */
	{
		usart0_buffer.TxHead = temp_head;							/* Incr head */
		usart0_buffer.TxBuffer[usart0_buffer.TxHead] = '-';			/* Place '-' */
	}
	
	if(usart0_buffer.transmit_active == 0)							
	{
		usart0_buffer.TxTail = (usart0_buffer.TxTail + 1) & USART_BUFFER_MASK;	/* Inc tail, which now will point to the first written byte */
		UDR = usart0_buffer.TxBuffer[usart0_buffer.TxTail];						/* Send read byte to uart, routine then coninues in ISR */
		usart0_buffer.transmit_active = 1;										/* Set flag */
	}
	
}

/* Get byte fom buffer */
char hardware_USART0_receive(void)
{
      if(usart0_buffer.RxTail != usart0_buffer.RxHead)								/* If buffer no empty */	
      {
			usart0_buffer.RxTail = (usart0_buffer.RxTail + 1) & USART_BUFFER_MASK; 	/* Increment tail to read next byte */
			return usart0_buffer.RxBuffer[usart0_buffer.RxTail];					/* return the byte */
      }

      return 0;																		/* If buffer empty (head = tail) return 0 */
}

/* Checking if the buffer is empty */
uint8_t hardware_USART0_Rx_Check(void)
{
	if(usart0_buffer.RxTail != usart0_buffer.RxHead)
	  return 1;		/* Buffer not empty */
	else
	  return 0;		/* Empty buffer */
}


void hardware_rs232_send(void)
{
	uint8_t checksumIndex = (PORT_COUNT + 1) * 4;
	char data_packet[checksumIndex];
	uint8_t checksum = 0;
	uint8_t ii;
	
	uint32_t tempFlow;	
		
	RS485_TX;	
		
	data_packet[0] =  (uint8_t) 0x00;
	data_packet[1] =  (uint8_t) 0x01;
	data_packet[2] =  (uint8_t) 0x33;     
	data_packet[3] =  (uint8_t) getStateMosfets();
	
	for (ii = 0; ii < PORT_COUNT; ii++) {
		tempFlow = Get_Flow(ii);
		uint8_t offset = ii * 4;
		data_packet[4 + offset] =  (uint8_t) (tempFlow >> 24);
		data_packet[5 + offset] =  (uint8_t) (tempFlow >> 16);
		data_packet[6 + offset] =  (uint8_t) (tempFlow >> 8);
		data_packet[7 + offset] =  (uint8_t) (tempFlow);
	}
	
	// Calculate checksum
	for(ii = 0; ii < 20; ii++)									
	{
		checksum ^= data_packet[ii];
	}
	
	data_packet[checksumIndex] = (255 - checksum);
	
	
	// Start the transmission routines
	// 
	hardware_USART0_send_esc('+', data_packet, checksumIndex + 1);			
}


uint8_t hardware_rs232_receive_packet(void)		/* Call this function every so often for any received packets */
{
	uint8_t ii, data;
	static uint8_t count = 0;
	static uint8_t esc_flag = 0;
	uint8_t checksum = 0;


	if(hardware_USART0_Rx_Check())					/* Check if buffer is empty */
	{
		data = hardware_USART0_receive();			/* Get data if not empty */ 
	
		if(data == '#' && !esc_flag)				/* If finding first escape byte */ 
		{
			esc_flag = 1;							/* Set escape byte flag */
		}
		else
		{
			if(!esc_flag)							/* Escape byte not set */
			{
				if(data == '+')						/* Getting sync byte of packet, since no escape byte beore it */
				{
					count = 0;						/* Reset Counter - since start of packet */
					for(ii = 0; ii < PACKET_BUFFER; ii++)	
					{
						usart0_buffer.rs232_packet[ii] = 0;	/* Clearing packet buffer */ 
					}
					
					return 0;
				}
				
				if(data == '-')						/* End of packet */
				{
					checksum = 0;					/* Reset checksum */
					
					for(ii = 0; ii< count - 1; ii++)		/* Calculating checksum of packet */
					{
						checksum ^= usart0_buffer.rs232_packet[ii];
					}
						
					checksum = 255 - checksum;
					
					if(checksum == usart0_buffer.rs232_packet[count - 1])
					{
						return 1;					/* If checksum matches, return true */
					}
					else
					{
						return 0;					/* Else return negative */
					}
				}
				
			}
			else
			{
				esc_flag = 0;	
			}
			
		
			if(count < PACKET_BUFFER)						/* If count still less than packet buffer size */
			{
				usart0_buffer.rs232_packet[count] = data;	/* Store data in buffer */
				count++;									/* Increment counter */
			}
			
		}
		
		/* Boot loader jump */
		if(data == 0x55)
		{
			if(bootloaderCnt++ > BOOTLOADER_JUMP_U)
			{
				while(1);
			}
		}
		else
		{
			bootloaderCnt = 0;
		}
	
	
	}
	
	return 0;
}

uint8_t hardware_rs232_receive(void)
{
	uint8_t mosfetActive;
	uint8_t mosfetDeActive;
	
	if(!hardware_rs232_receive_packet())
	{
		return 0;
	}
	
	if( 
		usart0_buffer.rs232_packet[0] != 0x01 || 
		usart0_buffer.rs232_packet[1] != 0x00 || 
		usart0_buffer.rs232_packet[2] != 0x22 
	) {
		return 0;
	}
	
	mosfetActive = usart0_buffer.rs232_packet[3];
	mosfetDeActive = usart0_buffer.rs232_packet[4]; 
			
	/* Each Mosfet is switched On via network */
	/* It can be also switched OFF, but switch on takes priority */
			
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		if( (mosfetActive & MOSFET_NETWORK_MASK[ii]) == MOSFET_NETWORK_MASK[ii])
		{
			Mosfet_On_Off(ii, ON);
		}
		else if( (mosfetDeActive & MOSFET_NETWORK_MASK[ii]) == MOSFET_NETWORK_MASK[ii])
		{
			Mosfet_On_Off(ii, OFF);
		}
	}
	
	return 0;
}

void hardware_rs232_comms(void)
{
	// If reception/decoding is OK - send reply
	uint8_t rs232_receive_flag = hardware_rs232_receive();		
		
	if( rs232_receive_flag != 0)
	{
		if(rs232_receive_flag == 1)
		{
			usart0_buffer.rs232_status = STATUS_ON;
			timer.rs232_timeout = 0;
			
		}
		
		// Send reply back		
		hardware_rs232_send();									
	}

	if(timer.rs232_timeout > RS232_TIMEOUT && usart0_buffer.rs232_status == STATUS_ON)
	{
		usart0_buffer.rs232_status = STATUS_OFF;
	}
	
}