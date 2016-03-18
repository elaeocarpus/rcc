/*
 * spi.c
 *
 */

#include "spi.h"


void spi_set_baud(uint8_t baud);
void spi2_set_baud(uint8_t baud);


/*
 * SPI Initialise
 */
void spi_init(void)
{
	SPI1->CR1 = (uint16_t)0x0000;			// Disable SPI1.
		// CPOL	0 	Data is clocked on rising edge of SCK
		// CPHA	0	Clock is 0 when idle

	spi_set_baud(0x07);						// Set SPI baud rate divider (f/256).


	SPI1->CR1 |= SPI_CR1_BIDIOE;
	SPI1->CR1 |= SPI_CR1_MSTR;				// Set SPI Master mode.



	SPI1->CR2 = 0;
	// SPI interrupts disabled.


	SPI1->CR1 |= SPI_CR1_SPE;				// Enable SPI

}


/*
 * Set SPI Baud rate divider.
 */
void spi_set_baud(uint8_t baud)
{
	SPI1->CR1 &= ~SPI_CR1_BR;						// Clear BR[2:0]
	SPI1->CR1 |= (uint16_t)(baud & 0x07) << 3;		// Set baud divider bits.
}


/* spi_out
 *
 * Write 8-bit data to SPI data register.
 * SPI transmits data out MOSI and simultaneously reads data on MISO.
 *
 */
uint8_t spi_out(uint8_t d)
{
	uint8_t data;

	while(SPI1->SR & SPI_SR_BSY)			// Poll the SPI Busy flag until it is clear.
	{}

	SPI1->DR = (uint16_t)(d);				// Write 8-bit data to SPI transmit register.

	while(!(SPI1->SR & SPI_SR_RXNE))		// Wait for receive data.
	{}

	data = SPI1->DR;						// Read 8-bit data from SPI receive register.

	return data;
}


/* spi_wr
 *
 * Write one SPI byte
 * Polled mode SPI write.
 * Transmits 7-bit address and 8-bit data.
 *
 * Chip select must be asserted external to this function.
 *
 */
void spi_wr(uint8_t addr, uint8_t data)
{
	spi_out(addr | 0x80);		// Transmit address. Set bit7 for write access.
	spi_out(data);
}


/* spi_wr_array
 *
 * Transmits the SPI address followed by n-bytes of data.
 * Chip select must be asserted external to this function.
 *
 */
void spi_wr_array(uint8_t addr, uint8_t *data, uint8_t n)
{
	spi_out(addr | 0x80);		// Transmit address. Set bit7 for write access.

	while(n > 0)
	{
		spi_out(*data++);
		n--;
	}
}


/* spi_rd
 *
 * Read one SPI byte.
 *
 */
uint8_t spi_rd(uint8_t addr)
{
	uint8_t spi_data;

	spi_out(addr &= 0x7F);		// Transmit address. Clear bit7 for read access.
	spi_out(0x00);				// Send zeroes to generate SCK for slave to send data.

	spi_data = SPI1->DR;		//

	return spi_data;

}


/* spi_rd_array
 *
 *
 *
 */
void spi_rd_array(uint8_t addr, uint8_t *data, uint8_t n)
{
	spi_out(addr &= 0x7F);		// Transmit address. Clear bit7 for read access.

}


/*---------------------------------------------------------------
 *                            SPI2
 *---------------------------------------------------------------
 */


/* spi2_init
 *
 * SPI Initialise
 */
void spi2_init(void)
{
	SPI2->CR1 = (uint16_t)0x0000;			// Disable SPI2.
		// CPOL	0 	Data is clocked on rising edge of SCK
		// CPHA	0	Clock is 0 when idle

	spi2_set_baud(0x07);						// Set SPI baud rate divider (f/256).

	SPI2->CR1 |= SPI_CR1_BIDIOE;
	SPI2->CR1 |= SPI_CR1_MSTR;				// Set SPI Master mode.

	SPI2->CR2 = 0;
	// SPI interrupts disabled.


	SPI2->CR1 |= SPI_CR1_SPE;				// Enable SPI

}

/* spi2_set_baud
 *
 * Set SPI Baud rate divider.
 */
void spi2_set_baud(uint8_t baud)
{
	SPI2->CR1 &= ~SPI_CR1_BR;						// Clear BR[2:0]
	SPI2->CR1 |= (uint16_t)(baud & 0x07) << 3;		// Set baud divider bits.
}


/* spi2_out
 *
 * Parameters
 * d			Transmit data
 *
 * Returns
 *
 *
 * Write 8-bit data to SPI data register.
 * SPI transmits data out MOSI and simultaneously reads data on MISO.
 * Returns the the data received on MISO.
 * Chip select is driven outside this function.
 */
uint8_t spi2_out(uint8_t d)
{
	uint8_t spi_data;


	while(SPI2->SR & SPI_SR_BSY);			// Poll the SPI Busy flag until it is clear.

	SPI2->DR = (uint16_t)(d);				// Write 8-bit data to SPI transmit register.

	while(SPI2->SR & SPI_SR_BSY);			// Poll the SPI Busy flag until it is clear.

	spi_data = SPI2->DR;					// Read 8-bit data from SPI receive register


	return spi_data;
}

