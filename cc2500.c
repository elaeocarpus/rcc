/*
 * cc2500.c
 *
 *  Created on: Feb 28, 2016
 *      Author: pmatthews
 */


#include "cc2500.h"
#include "gpio.h"
#include "spi.h"




// CC2500 Chip Select
#define CSn_LO() 	GPIO_BitReset(GPIOB, GPIO_PIN12);			// CSn low
#define CSn_HI() 	GPIO_BitSet(GPIOB, GPIO_PIN12);				// CSn low




static uint8_t status;					// CC2500 status byte



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
 * Status registers addresses are the same as commands
 * Status register are read-only.
 *
 * Status is updated
 *
 * R/W = 1
 * B = 1
 *
 */
uint8_t cc_read_status(uint8_t reg)
{
	uint8_t status_reg;
	uint8_t hdr;


	// Check reg range
	if(!((reg >= 0x30) && (reg <= 0x3D)))
		return 0;

	hdr = 0xC0 | (reg & 0x3F);			// Set R/W bit, clear burst bit.
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


/* cc_read_regs
 *
 * Read n-consecutive config registers starting at addr.
 * Returns the number of registers actually read.
 *
 * Status is updated.
 */
int cc_read_regs(uint8_t addr, uint8_t *data, uint8_t n)
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


/* cc_write_regs
 *
 *  Write n-consecutive config registers starting at addr.
 *  Returns the actual number of register written
 */
int cc_write_regs(uint8_t addr, uint8_t *data, uint8_t n)
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


/******************************************************************************
 *
 ******************************************************************************/



/* cc_get_status
 *
 * Returns the STATUS field of the CC2500 status.
 *
 * The global status variable is updated on every read and write
 * to the CC2500.  The CC2500 status byte is returned when the
 * SPI header byte is clocked out.
 *
 * The Cc2500 status is status.bit[6-4]
 */
uint8_t cc_get_status(void)
{
	return ((status>>4) & 0x07);
}



/* cc_status_update
 *
 * Request CC2500 status.
 * Sends a NOP command to the CC2500 to return the status.
 *
 *
 */
uint8_t cc_status_update(void)
{
	cc_write_cmd(CC_SNOP);

	return status;
}


/* cc_reset
 *
 * Send reset command
 */
uint8_t cc_reset(void)
{
	return (cc_write_cmd(CC_SRES));
}



