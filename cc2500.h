/*
 * cc2500.h
 *
 *  Created on: Feb 28, 2016
 *      Author: pmatthews
 */

#ifndef CC2500_H_
#define CC2500_H_

#include "types.h"





// Configuration Registers
#define IOCFG2		0x00
#define IOCFG1		0x01
#define IOCFG0		0x02
#define FIFOTHR		0x03
#define SYNC1		0x04
#define SYNC2		0x05
#define PKTLEN		0x06
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


#define	TEST2		0x2C	// Various test settings
#define	TEST1		0x2D	// Various test settings
#define	TEST0		0x2E	// Various test settings

// Commands
#define CC_SRES		0x30		// Reset
#define CC_SFSTXON	0x31		//

#define CC_SNOP		0x3D		// No operation


//  Command and register access.
uint8_t cc_write_cmd(uint8_t cmd);
uint8_t cc_read_status(uint8_t reg);
uint8_t cc_read(uint8_t reg);
int cc_read_regs(uint8_t addr, uint8_t *data, uint8_t n);
uint8_t cc_write(uint8_t reg, uint8_t data);
int cc_write_regs(uint8_t addr, uint8_t *data, uint8_t n);



uint8_t cc_status_update(void);
uint8_t cc_reset(void);



#endif /* CC2500_H_ */
