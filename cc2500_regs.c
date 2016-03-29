/*
 * cc2500.c
 *
 *  Created on: Feb 28, 2016
 *      Author: pmatthews
 *
 * Low-level access to CC2500 registers.
 */


#include "cc2500_regs.h"
#include "gpio.h"
#include "spi.h"




// CC2500 Chip Select
#define CSn_LO() 	GPIO_BitReset(GPIOB, GPIO_PIN12);			// CSn low
#define CSn_HI() 	GPIO_BitSet(GPIOB, GPIO_PIN12);				// CSn low



// CC2500 status byte
uint8_t status;

// Status byte is updated on every SPI read or write.
// The CC2500 clocks out the status on MISO as the header byte is
// clocked out on MOSI.
//
// bit 7		CHIP_RDYn
// bit [6-4]	State
//				000		Idle
//				001		Rx
//				010		Tx
//				011		FSTXON
//				100		Calibrate
//				101		Settling
//				110		RX FIFO Overflow
//				111		TX FIFO Underflow
// bit [3-0]	FIFO bytes available




/******************************************************************************
 * CC2500 Register access and command/status interface.
 ******************************************************************************/


/* cc_write_cmd
 *
 * Write a command to the CC2500.
 *
 * Command in sent in the SPI header byte.
 * Commands are in the range 0x30-0x3F
 * The header byte is the only byte sent by SPI.
 * The CC2500 returns it's status on MISO as the command
 * is clocked out on MOSI.
 * This function waits until the status byte has been received.
 * CC2500 status is updated.
 *
 * A command is a single byte SPI transfer of header only
 * in the address range 0x30 - 0x3D.
 *
 * R/W = 0
 * B = 0
 *
 */
uint8_t cc_write_cmd(uint8_t cmd)
{
	uint8_t d;


	// Check range command
	if(!((cmd >= 0x30) && (cmd <= 0x3D)))
		return 0;

	d = cmd & 0x3F;						// R/W=0, B=0

	CSn_LO();
	status = spi_out(d);
	CSn_HI();

	return 1;
}


/* cc_read_status
 *
 * Reads a status register.
 * Status registers are 0x30-0x3D.
 *
 * Status registers addresses are the same as commands register addresses
 * Status register are read-only.
 *
 * State is updated
 *
 * R/W = 1
 * B = 1
 *
 */
uint8_t cc_read_status(uint8_t reg)
{
	uint8_t status_reg;
	uint8_t hdr;


	if(!((reg >= 0x30) && (reg <= 0x3D)))
		return 0;

	hdr = 0xC0 | reg;					// Set R/W bit, clear burst bit.
	CSn_LO();
	status = spi_out(hdr);				// Send register address, read status.
	status_reg = spi_out(0x00);			// Send zeroes to read in data.
	CSn_HI();

	return status_reg;
}


/* cc_read
 *
 * Parameters
 * reg			Address of register
 *
 * Returns
 * data			8-bit data in register
 *
 * Read one config register.
 * Status is updated.
 */
uint8_t cc_read(uint8_t reg)
{
	uint8_t data;
	uint8_t hdr;


	hdr = 0x80 | (reg & 0x3F);			// Set R/W bit, clear burst bit.

	CSn_LO();
	status = spi_out(hdr);				// Send register address, read status.
	data = spi_out(0x00);				// Send zeroes to read in data.
	CSn_HI();

	return data;
}


/* cc_read_b
 *
 * Read n-consecutive config registers starting at addr.
 * Reads registers using SPI burst read.
 * Returns the number of registers actually read.
 *
 * Status is updated.
 */
int cc_read_b(uint8_t addr, uint8_t *data, uint8_t n)
{
	uint8_t hdr;
	int i;


	if(addr > 0x3D)
		return 0;

	addr &= 0x3F;					// 6-bit register address
	hdr = 0xC0 | addr;				// Set R/W bit, set burst bit.

	CSn_LO();
	status = spi_out(hdr);				// Send register address, read status.
	for(i=0; i<n; i++)
	{
		if(addr > 0x3D)					//
			break;

		*data++ = spi_out(0x00);		// Send zeroes to read in data.

		addr++;
	}
	CSn_HI();

	return i;
}


/* cc_write
 *
 * Parameters
 * reg					Address of register to write to
 * data					Data to write to register
 *
 *
 * Status is updated.
 */
uint8_t cc_write(uint8_t reg, uint8_t data)
{
	uint8_t hdr;


	hdr = reg & 0x3F;					// R/W=0, burst=0.

	CSn_LO();
	status = spi_out(hdr);				// Send register address, read status.
	spi_out(data);						// Send data byte.
	CSn_HI();

	return 0;
}


/* cc_write_b
 *
 *  Write n-consecutive config registers starting at addr.
 *  Writes registers using SPI burst write.
 *  Returns the actual number of register written.
 */
int cc_write_b(uint8_t addr, uint8_t *data, uint8_t n)
{
	uint8_t hdr;
	int i;


	if(addr > 0x3D)
		return 0;

	addr &= 0x3F;					// 6-bit register address
	hdr = 0x40 | addr;				// R/W=0, burst=1.

	CSn_LO();
	status = spi_out(hdr);			// Send register address, read status.
	for(i=0; i<n; i++)
	{
		if(addr > 0x3D)
			break;

		 spi_out(*data++);			// Send data byte.

		addr++;
	}
	CSn_HI();


	return i;
}

/* cc_write_fifo
 *
 * Write to TX FIFO.
 * This function doesn't check if there is room in the FIFO buffer.
 */
int cc_write_fifo(uint8_t *data, uint8_t n)
{
	uint8_t hdr;
	int i;


	hdr = 0x40 | TX_FIFO;			// R/W=0, burst=1.

	CSn_LO();
	status = spi_out(hdr);			// Send register address, read status.
	for(i=0; i<n; i++)
	{
		 spi_out(*data++);			// Send data byte.
	}
	CSn_HI();


	return i;
}


/* cc_get_state
 *
 * Returns the STATE field of the CC2500 status.
 *
 * The global status variable is updated on every read and write
 * to the CC2500.  The CC2500 status byte is returned when the
 * SPI header byte is clocked out.
 *
 * The CC2500 chip state is status.bit[6-4]
 */
uint8_t cc_get_state(void)
{
	return ((status >> 4) & 0x07);
}







