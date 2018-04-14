#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "uart.h"
#include "hardware.h"
#include "timer.h"
#include "mosfet.h"
#include "flow.h"

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

#define PULSES_FOR_ADDRESS 20

// UART states	
#define NORMAL_STATE 0x01
#define CONFIGURATION_STATE 0x02
#define WAITING_FOR_ADDRESS_STATE 0x03
#define ADDRESS_SET_STATE 0x04
	
/* Holds the number of 'U's to jump in bootloader */
uint8_t bootloaderCnt = 0;
uint8_t device_address = 0xFF;
uint8_t device_state = NORMAL_STATE;


void uart_init()
{
	device_address = eeprom_read_byte((uint8_t*)DEVICE_ADDRESS_EEPROM_LOCATION);
}

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

void hardware_USART0_send_esc(uint8_t* pstr, int length)
{
	uint8_t * str = pstr;  //.. str = pstr
	uint8_t temp_head;
	int i = 0;
	
	temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;			/* Increment temp head */

	if( temp_head != usart0_buffer.TxTail)								/* Check if temp head is equal to tail */
	{
		usart0_buffer.TxHead = temp_head;								/* If not equal, set temp head to head, basic increment head */
		usart0_buffer.TxBuffer[usart0_buffer.TxHead] = '+';				/* Write first char '+' in head position */
	}
	
	while((i < length)) 												/* Loop around the data buffer */
	{
			
	    temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;		/* Increment temp head */

	    if( (*str == '*' || *str == '+' || *str == '-' || *str == '#') && temp_head != usart0_buffer.TxTail)
	    {
			usart0_buffer.TxHead = temp_head;						/* If any symbol found, inc head */
			usart0_buffer.TxBuffer[usart0_buffer.TxHead] = '#';		/* Place '#' in current location */
	    }

		temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;	/* Increment temp head (same condition as before if no symbol found) */

	    if( temp_head != usart0_buffer.TxTail)						/* If tail not reached */
	    {
			usart0_buffer.TxHead = temp_head;						/* Increment head */
			usart0_buffer.TxBuffer[usart0_buffer.TxHead] = *str;	/* Place char in buffer */
	    }
		
		str++;														/* Point to next char */
		i++;														/* Increment length index */
	}
	
	temp_head = (usart0_buffer.TxHead + 1) & USART_BUFFER_MASK;		/* Increment temp head */

	if( temp_head != usart0_buffer.TxTail)							/* If tail not reached */
	{
		usart0_buffer.TxHead = temp_head;							/* Increment head */
		usart0_buffer.TxBuffer[usart0_buffer.TxHead] = '-';			/* Place '-' */
	}
	
	if(usart0_buffer.transmit_active == 0)							
	{
		usart0_buffer.TxTail = (usart0_buffer.TxTail + 1) & USART_BUFFER_MASK;	/* Inc tail, which now will point to the first written byte */
		UDR = usart0_buffer.TxBuffer[usart0_buffer.TxTail];						/* Send read byte to UART, routine then continues in ISR */
		usart0_buffer.transmit_active = 1;										/* Set flag */
	}
	
}

/* Get byte from buffer */
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
	return usart0_buffer.RxTail != usart0_buffer.RxHead
		? 1		// Buffer not empty
		: 0;	// Empty buffer
}

uint32_t read_uint32_from_buffer(uint8_t* data_array)
{
	return 
		(data_array[0] << 24) |
		(data_array[1] << 16) |
		(data_array[2] << 8) |
		(data_array[3]);
}

void write_uint32_to_buffer(uint8_t* data_array, uint32_t data)
{
	data_array[0] =  (uint8_t) (data >> 24);
	data_array[1] =  (uint8_t) (data >> 16);
	data_array[2] =  (uint8_t) (data >> 8);
	data_array[3] =  (uint8_t) (data);	
}

uint32_t get_max_flow()
{
	uint32_t tempFlow = 0;
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++) {
		tempFlow = max(tempFlow, Get_Flow(ii));
	}
	return tempFlow;
}

void hardware_rs232_send_config(void)
{
	const uint8_t PACKET_SIZE = 8;
	uint8_t data_packet[PACKET_SIZE];
	uint8_t checksum = 0;
	uint8_t ii;
	
	RS485_TX;
	
	data_packet[0] =  (uint8_t) 0x00;
	data_packet[1] =  (uint8_t) device_address;
	data_packet[2] =  (uint8_t) 0x12; // Config response
	
	write_uint32_to_buffer(&data_packet[3], get_max_flow());
		
	// Calculate checksum
	for(ii = 0; ii < PACKET_SIZE - 1; ii++)
	{
		checksum ^= data_packet[ii];
	}
	
	data_packet[PACKET_SIZE - 1] = (255 - checksum);
	
	// Start the transmission routines
	//
	hardware_USART0_send_esc(data_packet, PACKET_SIZE);
}

void hardware_rs232_send_pour(void)
{
	const uint8_t PACKET_SIZE = 21;
	uint8_t data_packet[PACKET_SIZE];
	uint8_t checksum = 0;
	uint8_t ii;
	
	uint32_t tempFlow;	
		
	RS485_TX;	
		
	data_packet[0] =  (uint8_t) 0x00;
	data_packet[1] =  (uint8_t) device_address;
	data_packet[2] =  (uint8_t) 0x33;     
	data_packet[3] =  (uint8_t) getStateMosfets();
	
	for (ii = 0; ii < PORT_COUNT; ii++) {
		tempFlow = Get_Flow(ii);
		uint8_t offset = ii * 4;
		write_uint32_to_buffer(&data_packet[4 + offset], tempFlow);
	}
	
	// Calculate checksum
	for(ii = 0; ii < PACKET_SIZE - 1; ii++)									
	{
		checksum ^= data_packet[ii];
	}
	
	data_packet[PACKET_SIZE - 1] = (255 - checksum);
	
	
	// Start the transmission routines
	// 
	hardware_USART0_send_esc(data_packet, PACKET_SIZE);			
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
					for(ii = 0; ii < PACKET_BUFFER_SIZE; ii++)	
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
			
		
			if(count < PACKET_BUFFER_SIZE)						/* If count still less than packet buffer size */
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

void receive_config_packet(void)
{
	uint8_t destination = usart0_buffer.rs232_packet[0];
	
	// Destination is zero. This is the initial packet sent to tell the devices to
	// enter configuration mode.
	if (destination == 0) {
		Reset_All_Flow();
		Mosfet_ALL_On_Off(ON);
		device_address = 0xFF;
		eeprom_write_byte((uint8_t*)DEVICE_ADDRESS_EEPROM_LOCATION, device_address);
		device_state = CONFIGURATION_STATE;
		return;
	}
	
	uint32_t deviceMaxFlow = get_max_flow();
	uint32_t packetMaxFlow = read_uint32_from_buffer(&usart0_buffer.rs232_packet[3]);
		
	// The master sends back a packet containing the pulses. We use this to determine
	// that this device is the sender and use the destination address from the packet
	if (
		deviceMaxFlow >= packetMaxFlow && 
		device_state == WAITING_FOR_ADDRESS_STATE
	) {
		Reset_All_Flow();
		Mosfet_ALL_On_Off(OFF);
		eeprom_write_byte((uint8_t*)DEVICE_ADDRESS_EEPROM_LOCATION, destination);
		device_address = destination;
		device_state = ADDRESS_SET_STATE;
		return;
	} 
}

void receive_command_packet(void)
{
	// We don't do anything with this right now but maybe later the packet will be shaped differently and
	// we'll need different logic.
	uint8_t mosfetActive = usart0_buffer.rs232_packet[3];
	uint8_t mosfetDeActive = usart0_buffer.rs232_packet[4];
	uint8_t resetFlow = usart0_buffer.rs232_packet[5];
	
	/* Each Mosfet is switched On via network */
	/* It can be also switched OFF, but switch on takes priority */
	
	for (uint8_t ii = 0; ii < PORT_COUNT; ii++)
	{
		if ((mosfetActive & MOSFET_NETWORK_MASK[ii]) == MOSFET_NETWORK_MASK[ii])
		{
			Mosfet_On_Off(ii, ON);
		}
		else if ((mosfetDeActive & MOSFET_NETWORK_MASK[ii]) == MOSFET_NETWORK_MASK[ii])
		{
			Mosfet_On_Off(ii, OFF);
		}

		if ((resetFlow & MOSFET_NETWORK_MASK[ii]) == MOSFET_NETWORK_MASK[ii])
		{
			Reset_Flow(ii);
		}
	}
}

uint8_t hardware_rs232_receive(void)
{
	if(!hardware_rs232_receive_packet())
	{
		return 0;
	}
	
	if (
		usart0_buffer.rs232_packet[1] == 0x00 &&
		usart0_buffer.rs232_packet[2] == 0x11 // Config packet
	) {
		receive_config_packet();
		return 0;
	}
	
	if( 
		usart0_buffer.rs232_packet[0] == device_address && 
		usart0_buffer.rs232_packet[1] == 0x00 && 
		usart0_buffer.rs232_packet[2] == 0x22 // Command packet
	) {
		receive_command_packet();
		return 1;
	}
	
	return 0;
}

void hardware_rs232_comms(void)
{
	// If reception/decoding is OK - send reply
	uint8_t rs232_receive_flag = hardware_rs232_receive();		
	
	// Standard packet flow where we are expected to respond
	// to the packet
	if( rs232_receive_flag != 0)
	{
		uint8_t packet_type = usart0_buffer.rs232_packet[2];
	
		// Send reply back		
		if (1 || packet_type == 0x22) {
			device_state = NORMAL_STATE;
			hardware_rs232_send_pour();
			return;
		}
	}
	
	if (device_state == CONFIGURATION_STATE)
	{
		uint32_t flow = get_max_flow();
		if (flow >= PULSES_FOR_ADDRESS)
		{
			hardware_rs232_send_config();
			device_state = WAITING_FOR_ADDRESS_STATE;
		}
	}
}