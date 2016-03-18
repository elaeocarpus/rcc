/* uart.c
 *
 * This is coded without the STM32 HAL.
 */



#include "uart.h"
#include "led.h"


#define RX_BUFF_SIZE	1024
uint8_t usart2_rx_buff[RX_BUFF_SIZE];
uint8_t *usart2_rx_in;
uint8_t *usart2_rx_out;

#define TX_BUFF_SIZE	1024
uint8_t usart2_tx_buff[TX_BUFF_SIZE];
uint8_t *usart2_tx_in;
uint8_t *usart2_tx_out;



#define usart2_rxne_interrupt_enable()	(USART2->CR1 |= USART_CR1_RXNEIE)
#define usart2_rxne_interrupt_disable()	(USART2->CR1 &= ~USART_CR1_RXNEIE)




uint16_t rx_data_rdy;				// Flag indicating receive data in buffer.



/* uart2_init
 *
 * Initialise UART2.
 * Port PA2 has to be configured for alternate output push-pull.
 * Port PA3 has to be configured for alternate input.
 *
 * PA2/USART2_TX
 * PA3/USART2_RX
 *
 * fclk (PCLK1):	32MHz
 * BRR = 32MHz / (16 x 9600)
 *     = 203.333			-> 203 5/16 = 203.3125
 *
 * Initial baud rate 9600
 */
void uart2_init(void)
{
	// Initialize buffer pointers
	usart2_rx_in = usart2_rx_buff;
	usart2_rx_out = usart2_rx_buff;
	usart2_tx_in = usart2_tx_buff;
	usart2_tx_out = usart2_tx_buff;


	USART2->SR = 0;						// Clear Status Register.

	// UART Control Register 1
	USART2->CR1 |= USART_CR1_UE;		// Enable USART2.
	USART2->CR1 |= USART_CR1_RXNEIE;
//	USART2->CR1 |= USART_CR1_TXEIE;
		// Receive register not empty (RXNE) interrupt enabled
		// Transmit register empty (TXE) interrupt enabled


	USART2->CR2 = 0x0000;				//
		/// 1 stop bit

	USART2->CR3 = 0x0000;
		// No DMA

	// Baud rate
	USART2->BRR = (203 << 4) + 5;		// 203.3125

	USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
		// Transmit enabled.
		// Receive enabled.

}


/*
 * Polled mode serial byte transmit
 */
/*
void uart2_send_byte(uint8_t tx_data)
{
	// Wait until TX data register is empty
	while((USART2->SR & USART_SR_TXE) == 0);		// while transmit data register not empty...

	// Transmit data
	USART2->DR = (uint8_t)tx_data;					// Write byte directly into UART transmit data register.
}
*/


/*
 * Interrupt driven serial byte transmit.
 *
 *
 * UART Transmit interrupt operation
 * ---------------------------------
 * The UART transmit interrupt is triggered when TDR is empty.
 * The transmit interrupt is level triggered.  That is the interrupt remains
 * active as long as TXE=1 (ie: TDR is empty).
 *
 * Initially the TXE interrupt is disabled and the tx queue is empty. The input
 * and output buffer pointers point to the same location.
 * When the very first byte is sent, it is placed in the transmit queue and the input
 * pointer is incremented. The TXE interrupt is then enabled. Because the TDR register is empty,
 * hardware sets TXE=1 and the UART interrupt is triggered immediately. The UART interrupt will
 * run before send_byte() returns. The ISR sees the buffer pointers are not equal so it takes the byte
 * from the transmit queue and writes it to TDR. It then increments the out buffer pointer,
 * so now the transmit queue is empty, and the ISR exits with the TXE interrupt still enabled.
 * Because the transmit shift register is empty, the UART hardware immediately moves the
 * byte out of TDR and into the tx shift register. TDR is now empty and TXE is set. So when
 * the ISR returns, the TXE interrupt causes the ISR to immediately run again.  This occurs
 * before send_byte() has a chance to put the second byte in the transmit queue.
 * The second time the ISR runs it sees the buffer pointers are equal.  This means the transmit
 * queue is empty, so the ISR just disables the TXE interrupt and returns.
 *
 * Then send_byte() puts another byte in the queue, and again enables the TXE interrupt.  Once again
 * the ISR reads the byte from the transmit queue and puts it into TDR.  It moves the output buffer
 * pointer so the transmit queue is empty again.  The ISR exits with the TXE interrupt enabled.
 * However, the first byte is still in the tx shift register being clocked out.  So the second byte (now in
 * in TDR) isn't transferred to the shift register.  When the ISR exits, the TDR register is not empty so
 * the TXE interrupt is not active.
 * The next TXE interrupt won't occur until the shift register has finished shifting it's byte out and
 * the byte in TDR is transferred to the shift register.  This takes approx 1ms at 9600 baud.  In the
 * meantime send_byte() continues to loop putting bytes into the transmit data queue.  It also keeps
 * setting TXEIE which is already set.
 *
 * Eventually send_byte() stops putting bytes in the transmit data queue.  The TXE ISR runs each time a byte
 * is copied out from TDR into the shift register.  The TXE ISR in the background "catches up" with the
 * bytes in the transmit queue. Eventually TXE ISR empties the transmit queue.  The TXE ISR always runs
 * one more time after the last byte is removed from the transmit queue and written to TDR.  The last time
 * the ISR runs it sees the transmit queue is empty and so disables it's on interrupt so it doesn't
 * keep re-triggering.
 *
 * Note:
 * Interrupts are globally disabled is send_byte() because setting the TXEIE flag is not atomic and the
 * UART ISR may also clear this flag. Also, the ISR compares the value of the two buffer pointers, so
 * interrupts are disabled while the buffer pointer is being adjusted in this function.
 *
 */
void uart2_send_byte(uint8_t tx_data)
{

	__disable_irq();

	// Put byte in transmit queue.
	*usart2_tx_in++ = tx_data;
	if(usart2_tx_in >= (usart2_tx_buff + TX_BUFF_SIZE))
		usart2_tx_in = usart2_tx_buff;

	USART2->CR1 |= USART_CR1_TXEIE;					// Enable transmit interrupt

	__enable_irq();
}



/*
 * Polled mode serial byte receive.
 * Receives bytes directly from USART Rx data register.
 */
void usart2_rcv_byte(void)
{
	uint8_t rx_data;		//


	if(USART2->SR & USART_SR_RXNE)			// If Receive Data Register not empty...
	{
		rx_data = USART2->DR;				// Read receive data register.
		uart2_send_byte(rx_data);			// Echo
	}

}


/*
 * Reads a byte from USART2 receive data register.
 */
uint8_t usart2_read_byte(void)
{
	uint8_t rx_data;


	rx_data = USART2->DR;				// Read receive data register.

	return rx_data;
}



/* Buffered serial receive.
 * Check if serial receive buffer contains data.
 */
int usart2_rxdata_rdy(void)
{
	if(rx_data_rdy)
		return 1;

	return 0;
}





/*
 * Reads a byte from USART2 receive buffer.
 * Data is put into the buffer by the ISR.
 * Only one char is removed.
 *
 */
uint8_t usart2_read(void)
{
	uint8_t rx_data;


	usart2_rxne_interrupt_disable();

	rx_data = (uint8_t)*usart2_rx_out++;					// Read byte from receive buffer.
	if(usart2_rx_out >= (usart2_rx_buff + RX_BUFF_SIZE))
		usart2_rx_out = usart2_rx_buff;

	if(usart2_rx_out == usart2_rx_in)						// If buffer is empty
		rx_data_rdy = 0;									// Clear the flag.

	usart2_rxne_interrupt_enable();

	return rx_data;
}


/* USART2 Interrupt Handler
 *
 * There is only one interrupt for USART2.
 * The interrupt handler must determine the source of the interrupt.
 *
 * Reading the receive data register also clears the RXNE flag and the
 * associated interrupt. So there is no need to clear any flags.
 *
 * There is no check for receive buffer overrun.  The input pointer
 * can go past the output pointer.
 */
void __attribute__((interrupt("IRQ")))USART2_IRQHandler(void)
{
	uint8_t rx_data;
	uint8_t tx_data;


	// Receive Data Register not empty
	if(USART2->SR & USART_SR_RXNE)
	{
		rx_data = USART2->DR;				// Read receive data register.

		// Insert byte into receive buffer.
		*usart2_rx_in++ = rx_data;
		if(usart2_rx_in >= (usart2_rx_buff + RX_BUFF_SIZE))
			usart2_rx_in = usart2_rx_buff;

		// Signal that receive buffer has data.
		rx_data_rdy = 1;

	}

	// Transmit Data Register empty
	if(USART2->SR & USART_SR_TXE)
	{
		if(usart2_tx_out == usart2_tx_in)				// Transmit queue is empty
		{
			// There is no more data to send
			USART2->CR1 &= ~USART_CR1_TXEIE;			// Disable the transmit data register empty interrupt
		}
		else											// Transmit data queue has data
		{
			// Read byte from receive buffer.
			tx_data = *usart2_tx_out++;
			if(usart2_tx_out >= (usart2_tx_buff + TX_BUFF_SIZE))
				usart2_tx_out = usart2_tx_buff;

			USART2->DR = (uint8_t)tx_data;				// Write byte to transmit data register
		}

	}

}

