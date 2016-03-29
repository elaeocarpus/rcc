/*
 * cc_hal.c
 *
 *  Created on: Mar 4, 2016
 *      Author: pmatthews
 *
 * CC2500 Hardware Abstraction Layer.
 *
 * The application uses the HAL to access the radio.
 * This provides a simplified interface to the radio that is
 * abstracted according to how the radio is used.
 *
 * Generally the radio is configured.  Then the application reads
 * receive packets from the radio and send packets to the radio to
 * be transmitted.
 *
 *
 */

#include <string.h>

#include "cc2500_regs.h"
#include "cc_hal.h"


extern uint8_t status;



typedef struct cmd_tbl_entry_struct {
	char *txt;
	uint8_t value;
} CMD_TBL_ENTRY;

// Lookup table for command strings
const CMD_TBL_ENTRY cmd_table [] =
{
	{"sres",    SRES },
	{"sfstxon", SFSTXON },
	{"sxoff",   SXOFF },
	{"scal",    SCAL },
	{"srx",     SRX },
	{"stx",     STX },
	{"sidle",   SIDLE },
	{"swor",    SWOR },
	{"spwd",    SPWD },
	{"sfrx",    SFRX },
	{"sftx",    SFTX },
	{"sworrst", SWORRST },
	{"snop",    SNOP }
};


const char *radio_states [] =
{
	"IDLE",
	"RX",
	"TX",
	"FSTXON",
	"CALIBRATE",
	"SETTLING",
	"RXFIFO_UNDERFLOW",
	"TXFIFO OVERFLOW"
};


// Configuration registers
uint8_t config_regs[N_CONFIG_REGS];



/* cc_reset
 *
 * Send reset command.
 */
uint8_t cc_reset(void)
{
	return (cc_write_cmd(SRES));
}



/*
 *
 */
uint8_t cc_cmd_rx(void)
{
	return (cc_write_cmd(SRX));

}


/* cc_status_update
 *
 * Request CC2500 status.
 * Sends a NOP command to the CC2500 to return the status.
 *
 * Returns the global status byte.
 */
uint8_t cc_status_update(void)
{
	cc_write_cmd(SNOP);

	return status;
}


/*
 *
 */
int cc_set_base_frequency(uint32_t base_freq)
{



	return 0;
}

/*
 *
 */
int cc_read_rssi(void)
{

		return 0;
}


/* Write data to the Tx FIFO buffer
 *
 * *buf			Buffer holding data to write
 * n			No. of bytes to write
 *
 *
 */
int rf_write_tx_fifo(uint8_t *buf, int n)
{

	// Add the LEN and ADDR bytes to the packet


	cc_write_fifo(buf, n);

	return n;
}


/* Read data from the Rx FIFO buffer.
 *
 * *buf			Pointer to buffer to hold data read from Rx FIFO.
 * n 			Number of bytes to read
 *
 * Returns
 * No. of bytes read
 */
int rf_read_rx_fifo(uint8_t *buf, int n)
{

	return n;
}




/******************************************************************************
 * Public functions
 ******************************************************************************/


/* cc_radio_config
 *
 * Parameters
 * *config			Pointer to configuration data structure.
 *
 * Returns
 *
 *
 * Configure HAL.
 * Call this before calling start.
 * Fill in a configuration data structure.
 *
 * Reads the configuration registers from the radio.
 * Updates the registers according to values in the
 * sonfig data structure.
 * Writes the config registers back to the radio.
 *
 * This must be called when the radio is in idle state.
 */
int cc_radio_config(RF_CONFIG *config)
{

	// Read all the config registers
	cc_read_b(0, config_regs, N_CONFIG_REGS);


	// Write the config registers



	return 0;
}

/* cc_radio_start
 *
 * Radio starts in receive state
 */
int cc_radio_start(void)
{
	// Check radio is in idle state.


	return 0;
}


/* cc_radio_stop
 *
 * Radio stops receiving or transmitting.
 * Radio changes to idle state.
 * This is necessary to be able to update the config registers.
 * It can also be used to recover from error states such as RX FIFO
 * overflow.
 */
int cc_radio_stop(void)
{



	// Check the radio is in the idle state before returning

	return 0;
}

/* cc_receive_pkt
 *
 * Reads a receive packet from the radio.
 * This is called in response to the radio indicating it has received a packet.
 * Typically the radio sets one of the GDOx pins. The application either
 * polls the pin or an interrupt is triggered.
 * The radio remains in the receive state.
 *
 * Note: The HAL could be set up to automatically receive
 * packets and store them in a packet queue.
 */
int cc_receive_pkt(void)
{




	return 0;
}

/*
 * Send a packet to be transmitted by the radio.
 * The radio changes to TX state and sends the packet, then
 * returns to RX state.
 *
 *
 */
int cc_send_pkt(void)
{

	return 0;
}


/* Convert a string to a command value
 *
 * *cmd		Command string
 *
 * Returns
 * command value
 *
 * Uses a lookup table to find a matching command string and return
 * the associated command value.
 * If command is not found the return value is 0.
 */
uint8_t StrToCmd(char *cmd)
{
	int i;
	uint8_t cmd_value;
	CMD_TBL_ENTRY *pcmdtbl;


	pcmdtbl = (CMD_TBL_ENTRY *)(cmd_table);
	cmd_value = 0;

	// Search for a matching command string in command table
	for(i=0; i<N_COMMANDS; i++)
	{
		if(strcmp(cmd, pcmdtbl->txt  ) == 0)
		{
			cmd_value = pcmdtbl->value;
			break;
		}

		pcmdtbl++;
	}

	return cmd_value;
}



/*
 * Convert a state value to a text string.
 */
int cc_state_to_str(uint8_t state, char *s)
{
	if(state > 7)
		return 0;

	strcpy(s, radio_states[state]);

	return 0;
}
