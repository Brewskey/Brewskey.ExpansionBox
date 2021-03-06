#ifndef UART_H
#define UART_H

#include "constants.h"

#define USART_BUFFER_SIZE 64 						// Must be power of 2, so that buffer rounding can be done just by AND operation
#define USART_BUFFER_MASK (USART_BUFFER_SIZE - 1)	// If buffer is 64, mask with 63, which comes 0b111111, and when doing '&' it behaves like '%64'
#define PACKET_BUFFER_SIZE 21						// We copy the RxBuffer into this in order to work with the data.

#define STATUS_ON		1
#define STATUS_OFF		0

#define RS232_TIMEOUT  40

#define BOOTLOADER_JUMP_U	20

// After configuration we store the address of this device (where it is on the bus)
#define DEVICE_ADDRESS_EEPROM_LOCATION 0x00

/* V1 Send Packet
*	00		- Destination (mainboard)
*	01		- Source
*	02		- Packet Type
*	03		- Extension Board UART message version
*	04		- Mosfet State
*	05-21	- Every four bytes is the pulses for a tap
*
*/

typedef volatile struct
{
	unsigned char TxBuffer[USART_BUFFER_SIZE];		// Buffer to write in to be sent to UART
	unsigned char RxBuffer[USART_BUFFER_SIZE];		// Buffer to read from the data written from UART 
	uint8_t RxHead;									// The index where the UART writes in the RxBuffer
	uint8_t RxTail;									// The index where the software reads from the RxBuffer
	uint8_t TxHead;									// The index where the software writes in the TxBuffer
	uint8_t TxTail;									// The index where the UART reads from the TxBuffer
	uint8_t transmit_active;
	uint8_t rs232_packet[PACKET_BUFFER_SIZE];
} USARTBuffer;

extern USARTBuffer usart0_buffer;

void uart_init();
void hardware_rs232_send_config(void);								// Build packet for config and start sending procedure
void hardware_rs232_send_pour(void);								// Build packet for pour and start sending procedure
void hardware_rs232_comms(void);									// Main function in main to check for communication
char hardware_USART0_receive(void);									// Read byte from RX buffer */
uint8_t hardware_rs232_receive(void);								// Decodes the packet and sets the hardware
uint8_t hardware_USART0_Rx_Check(void);								// Checking if the buffer is empty
uint8_t hardware_rs232_receive_packet(void);						// Checks for any new packets received and decode the if found
void hardware_USART0_send_esc(uint8_t* pstr,int length);		// Writing data in the TX buffer waiting to be sent through UART 


#endif