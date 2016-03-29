/*
 * cc2500.h
 *
 *  Created on: Feb 28, 2016
 *      Author: pmatthews
 */

#ifndef CC2500_H_
#define CC2500_H_

#include "types.h"


#define N_CONFIG_REGS 47
#define N_COMMANDS 13
#define N_STATUS_REGS 14


// Configuration Registers
#define IOCFG2		0x00	// GPO0 configuration
#define IOCFG1		0x01	// GPO1 configuration
#define IOCFG0		0x02	// GPO2 configuration
#define FIFOTHR		0x03	// FIFO Threshold
#define SYNC1		0x04	// Sync word hi-byte
#define SYNC2		0x05	// Sync word lo-byte
#define PKTLEN		0x06	// Packet length
#define PKTCTRL1	0x07	// Packet automation control
#define PKTCTRL0	0x08	// Packet automation control
#define ADDR		0x09	// Device Address
#define CHANNR		0x0A	// Channel number
#define FSCTRL1		0x0B	// Frequency synthesiser control
#define FSCTRL0		0x0C	// Frequency synthesiser control
#define FREQ2		0x0D	// Frequency control word, high byte
#define FREQ1		0x0E	// Frequency control word, middle byte
#define FREQ0		0x0F	// Frequency control word, low byte
#define MDMCFG4		0x10	// Modem configuration
#define MDMCFG3		0x11	// Modem configuration
#define MDMCFG2		0x12	// Modem configuration
#define MDMCFG1		0x13	// Modem configuration
#define MDMCFG0		0x14	// Modem configuration
#define DEVIATN		0x15	// Modem deviation setting
#define MCSM2		0x16	// Main Radio Control State Machine configuration
#define MCSM1		0x17	// Main Radio Control State Machine configuration
#define MCSM0		0x18	// Main Radio Control State Machine configuration
#define FOCCFG		0x19	// Frequency Offset Compensation configuration
#define BSCFG		0x1A	// Bit Synchronization configuration
#define AGCTRL2		0x1B	// AGC Control
#define AGCTRL1		0x1C	// AGC Control
#define AGCTRL0		0x1D	// AGC Control
#define WOREVT1		0x1E	// High byte Event 0 timeout
#define WOREVT0		0x1F	// Low byte Event 0 timeout
#define	WORCTRL		0x20	// Wake On Radio control
#define FREND1		0x21	//
#define FREND0		0x22	//
#define FSCAL3		0x23	//
#define FSCAL2		0x24	//
#define FSCAL1		0x25	//
#define FSCAL0		0x26	//
#define RCCTRL1		0x27	// RC Oscillator configuration
#define RCCTRL0		0x28	// RC Oscillator configuration
#define FSTEST		0x29	// Frequency synthesizer calibration control
#define PTEST		0x2A	// Production test
#define AGCTEST		0x2B	// AGC test
#define	TEST2		0x2C	// Various test settings
#define	TEST1		0x2D	// Various test settings
#define	TEST0		0x2E	// Various test settings

// Commands
#define SRES		0x30		// Reset
#define SFSTXON		0x31		//
#define SXOFF		0x32		// Turn off crystal oscillator
#define SCAL		0x33		// Calibrate frequency synthesizer
#define	SRX			0x34		// Enable RX
#define STX			0x35		// Enable TX
#define SIDLE		0x36		// Idle
#define SWOR		0x38		// Wake-on-Radio
#define SPWD		0x39		// Power down mode
#define SFRX		0x3A		// Flush the RX FIFO buffer.
#define SFTX		0x3B		// Flush the TX FIFI buffer.
#define SWORRST		0x3C		// Reset reeal time clock to Event1 value.
#define SNOP		0x3D		// No operation.  May be used to get access the the chip status byte.

// Status registers
#define	PARTNUM			0x30	// CC2500 part number
#define VERSION			0x31	// Current version number
#define FREQUEST		0x32	// Frequency offset estimate
#define LQI				0x33	// Demodulator estimate for Link Quality
#define RSSI			0x34	// Received signal strength indication
#define MARCSTATE		0x35	// Control state machine state
#define WORTIME1		0x36	// High byte of WOR timer
#define WORTIME0		0x37	// Low byte of WOR timer
#define PKTSTATUS		0x38	// Current GDOx status and packet status
#define VCO_VC_DAC		0x39	// Current setting from PLL calibration module
#define TXBYTES			0x3A	// Underflow and number of bytes in the TX FIFIO
#define RXBYTES			0x3B	// Overflow and number of bytes in RX FIFO
#define RCCTRL1_STATUS	0x3C	// Last RC oscillator calibration result
#define RCCTRL0_STATUS	0x3D	// Last RC oscillator calibration result

#define TX_FIFO			0x3F	//
#define RX_FIFO			0x3F	//

// CC2500 States
#define	IDLE_STATE  			0
#define RX_STATE 				1
#define TX_STATE				2
#define FSTXON_STATE			3		// Frequency Synthesizer on
#define CALIBRATE_STATE			4
#define SETTLING_STATE			5
#define RXFIFO_OVERFLOW_STATE	6
#define TXFIFO_UNDERFLOW_STATE	7



//  Command and status register access.
uint8_t cc_write_cmd(uint8_t cmd);
uint8_t cc_read_status(uint8_t reg);

// Configuration Register access
uint8_t cc_read(uint8_t reg);
int cc_read_b(uint8_t addr, uint8_t *data, uint8_t n);
uint8_t cc_write(uint8_t reg, uint8_t data);
int cc_write_b(uint8_t addr, uint8_t *data, uint8_t n);

// FIFO buffer access
int cc_write_fifo(uint8_t *data, uint8_t n);

// Chip state
uint8_t cc_get_state(void);



#endif /* CC2500_H_ */
