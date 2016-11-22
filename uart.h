#ifndef UART_H
#define UART_H



#define USART_BUFFER_SIZE 64 						/* Must be power of 2, so that buffer rounding can be done just by AND operation */
#define USART_BUFFER_MASK (USART_BUFFER_SIZE - 1)	/* If buffer is 64, mask with 63, which comes 0b111111, and when doing '&' it behaves like '%64'*/
#define PACKET_BUFFER 20

#define STATUS_ON		1
#define STATUS_OFF		0

#define RS232_TIMEOUT  40

#define BOOTLOADER_JUMP_U	20

typedef volatile struct
{
	unsigned char TxBuffer[USART_BUFFER_SIZE];		/* Buffer to write in to be sent to uart*/
	unsigned char RxBuffer[USART_BUFFER_SIZE];		/* Buffer to read from the data written from uart */
	uint8_t RxHead;									/* The index where the UART writes in the RxBuffer*/
	uint8_t RxTail;									/* The index where the software reads from the RxBuffer*/
	uint8_t TxHead;									/* The index where the software writes in the TxBuffer*/
	uint8_t TxTail;									/* The index where the UART reads from the TxBuffer*/
	uint8_t transmit_active;
	uint8_t rs232_packet[PACKET_BUFFER];
	uint8_t rs232_status;
} USARTBuffer;

extern USARTBuffer usart0_buffer;


void hardware_rs232_send(void);										/* Build packet and start sending procedure */
void hardware_rs232_comms(void);									/* Main function in main to check for comms */
char hardware_USART0_receive(void);									/* Read byte from RX buffer */
uint8_t hardware_rs232_receive(void);								/* Decodes the packet and sets the hardware */
uint8_t hardware_USART0_Rx_Check(void);								/* Checking if the buffer is empty */
void hardware_USART0_send_str(char* str);							/* Sending a sting through UART - not normaly used */
uint8_t hardware_rs232_receive_packet(void);						/* Checks for any new packets received and decode the if found */
void hardware_USART0_send_esc(char stx,char* pstr,int length);		/* Writing data in the TX buffer waiting to be sent through UART */


#endif