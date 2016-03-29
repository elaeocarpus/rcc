/*
 * command.c
 *
 * Command line processor.
 *
 */


#include "stm32f103xb.h"
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "textio.h"
#include "cc2500_regs.h"
#include "cc_hal.h"
#include "pwm.h"


#ifndef NULL
#define NULL  (void *)0
#endif

// External functions
void kputc(uint8_t c);
void print_str(char *s);
void led_toggle(void);


typedef struct {
	char *cmd_txt;
	void (*cmd_func)(void);
	char *help_txt;
} CMD_ITEM;

#define CMD_BUF_SIZE 256

// Command processor engine
int command_process(void);
void cmdline_proc(void);
int cmd_parse(char *s);
char *get_token(char *str, const char *delimiters, char *token);
int IsDelimiter(char c, const char *delimiters);

void print_tokens(int argc, char **argv);


// CC2500 command functions
void cmd_version(void);
void cmd_state(void);
void cmd_reg(void);
void cmd_wreg(void);
void cmd_command(void);
void cmd_status(void);
void help_txt(void);
void cmd_mot(void);
void cmd_speed_mode(void);
void cmd_speed(void);
void cmd_sres(void);
void cmd_tx(void);


char cmd_buf[CMD_BUF_SIZE];			// Command line buffer.
uint16_t cmd_index;					// Current position in command buffer.


char token[32];
char *ptok;


#define MAX_ARGS 16
char *args[MAX_ARGS];
int n_args;

int cmd_mode;


// Array of command structures
const CMD_ITEM cmd_list[] =
{
	{"help", help_txt, "Help text"},
	{"ver", cmd_version, "Firmware version"},
	{"state", cmd_state, "RF state"},
	{"reg", cmd_reg, "Read registers"},
	{"wreg", cmd_wreg, "Write registers"},
	{"status", cmd_status, "Status registers"},
	{"cmd", cmd_command, "Send command to radio"},
	{"ctrl", cmd_speed_mode, "Go to speed control mode"},
	{"speed", cmd_speed, "Set speed"},
	{"sres", cmd_sres, "RF Reset"},
	{"tx", cmd_tx, "Transmit a string"}
};

#define N_CMDS ((sizeof(cmd_list))/(sizeof(CMD_ITEM)))

/* cmd_proc_init
 *
 * Initialise the command processor.
 *
 */
void cmd_proc_init(void)
{
	int i;

	cmd_index = 0;					// Set position to start of command buffer.

	for(i=0; i<CMD_BUF_SIZE; i++)
		cmd_buf[i] = '\0';			// Clear the command buffer

	kputc('>');						// Command prompt

}


/* cmd_proc
 *
 * c	character passed to command processor.
 *
 * Process the next character.
 */
void cmd_proc(uint8_t c)
{
	if(c == '\r')					// CR, 'Enter' pressed.
	{
		kputc('\n');				// Send CR+LF.  Command output on next line.


		cmd_buf[cmd_index] = '\0';	// Null terminate the command line

		// Process the command line.
		if(cmd_buf[0] != '\0')					// If command line is not empty
		{
			n_args = cmd_parse(cmd_buf);		// Parse the command line into tokens

			//print_tokens(n_args, args);

			command_process();					// Process the command.
			//cmdline_proc();			// Process the command buffer.

			kputc('\n');			// Put command prompt on a new line.
		}

		cmd_index = 0;				// Reset command buffer index.
		kputc('>');					// Command prompt
	}
	else
	{
		if(cmd_index < (CMD_BUF_SIZE-2))	// Allow space for null terminate char.
		{
			cmd_buf[cmd_index++] = c;	// Put char into command buffer.
			kputc(c);					// Echo received character
		}
	}

}


/* cmd_parse
 *
 * Parse a string.
 * Creates an array of pointers to tokens.
 * The string being parsed is modified with '\0' replacing
 * the first whitespace after each token.
 * Returns the number of tokens found in the string.
 *
 */
int cmd_parse(char *s)
{
	char *pch;
	int i;


	i = 0;
	pch = strtok(s, " ,\t");
	while(pch != NULL)
	{
		if(i >= MAX_ARGS)
			break;

		args[i++] = pch;				// Save pointer to token
		pch = strtok(NULL, " ,\t");
	}
	return i;
}



/* command_process
 *
 * Uses the array of command structures to process the command.
 */
int command_process(void)
{
	char *cmd;
	CMD_ITEM *pcmd_item;			// Pointer to command structures
	int i;


	if(!n_args)									//
		return 0;

	cmd = args[0];								// Command is the first argument

	pcmd_item = (CMD_ITEM *)cmd_list;			// Point to the first item in command structure array.

	// Go through the array of command structures
	for(i=0; i<N_CMDS; i++)
	{
		if(strcmp(cmd, pcmd_item->cmd_txt) == 0)			// If first arg matches command text
		{
			pcmd_item->cmd_func();							// Run the associated function.
			return 1;										// Command was executed
		}
		pcmd_item++;
	}

	print_str("Unknown command: ");									// Command not found
	print_str(cmd);
	print_str("\n");

	return 0;
}


/* cmdline_proc
 *
 * Process the command line.
 *
 */
void cmdline_proc(void)
{
	char *cmd;


	if(!n_args)
		return;

	cmd = args[0];

	if(strcmp(cmd, "help") == 0)
	{
	}
	else if(strcmp(cmd, "cmd") == 0)
	{
		cmd_command();
	}
	else if(strcmp(cmd, "mot") == 0)
	{
	}
	else
	{
		print_str("?");
	}

}


/* get_token
 *
 * Parameters
 * src			Source string containing tokens.
 * delimiters	Null terminated string defining token separators.
 * token		Output buffer to hold extracted token.
 *
 * Returns
 * Pointer to the first byte after the token.
 * NULL if there are no more tokens.
 *
 * Remarks
 * -------
 * Finds the next token in a string.
 * The token is copied out to a token buffer.
 * The source string should be null terminated.
 * This function doesn't modify the source string.
 *
 * The return value is the start point for the next token search.
 */
char *get_token(char *str, const char *delimiters, char *token)
{
	char *p;


	p = str;

	// Find the first character that is not a delimiter
	while(*p != '\0')
	{
		if(!IsDelimiter(*p, delimiters))
			break;
		p++;
	}

	// If null char was found then there was no token, only delimiters.
	if(*p == '\0')
	{
		*token = '\0';					// Output token is empty string
		return NULL;					//
	}

	// Copy chars until delimiter or null terminate is found
	while(*p != '\0')
	{
		if(IsDelimiter(*p, delimiters))		// Stop at the first delimiter
			break;

		*token++ = *p++;
	}

	*token = '\0';						// Null terminate the output token buffer.

	if(*p == '\0')						// End of string, there are no more tokens.
		p = NULL;

	return p;
}


/* IsDelimiter
 *
 *
 */
int IsDelimiter(char c, const char *delimiters)
{
	while(*delimiters != '\0')
	{
		if(c == *delimiters++)
			return 1;			// c is a delimiter char
	}

	return 0;					// c is not a delimiter
}


/*
 * Debug function.
 * Print the tokens.
 */
void print_tokens(int argc, char **argv)
{
	int i;
	char **token;


	token = argv;
	for(i=0; i<argc; i++)
	{
		print_str(*token);
		print_str("\n");

		token++;
	}


}




/******************************************************************************
 *
 *
 ******************************************************************************/

/* cmd_state
 *
 * Get the CC2500 state
 */
void cmd_state(void)
{
	char s[32];
	uint8_t state;


	// Get the chip state
	cc_status_update();				// Send NOP command to return the status byte
	state = cc_get_state();			// Get the updated state

	// Print the state value (hex)
	ByteToHex(s, state);
	print_str(s);
	print_str(" ");

	// Print state name
	cc_state_to_str(state, s);
	print_str(s);
	print_str("\n");


}


/* Read CC2500 configuration registers
 *
 * Usage:
 * > reg [addr] [count]
 *
 *		addr	The register address [00-2E] in hex.
 *		count	No. of registers to read in decimal
 *
 * Examples
 *	reg			Print all registers
 *	reg 1e		Print register 0x1E
 *	reg 1e 3	Print three registers from 0x1E to 0x20
 *
 *
 */
void cmd_reg(void)
{
	char s[16];
	uint32_t addr;
	uint8_t value;
	int i, n;
	char *ptr;


	if(n_args == 1)
	{
		// No register specified. Print all the configuration registers.
		addr = 0;
		n = N_CONFIG_REGS;
	}
	else
	{
		// Get the configuration register address
		addr = strtol(args[1], &ptr, 16);			// Hex

		// Get the register count
		if(n_args == 2)								// No count specified.
		{
			n = 1;
		}
		else
		{
			n = strtol(args[2], &ptr, 10);			// Get count in decimal.
		}
	}

	// Check the start register address
	if(addr >= N_CONFIG_REGS)
	{
		print_str("Register address out of range\n");
		return;
	}

	// Print configuration register contents
	for(i=0; i<n; i++)
	{
		if(addr >= N_CONFIG_REGS)
			break;

		value = cc_read(addr);

		ByteToHex(s, addr);
		print_str(s);
		print_str(": ");

		ByteToHex(s, value);
		print_str(s);
		print_str("\n");

		addr++;
	}

}


/* Write to CC2500 configuration register
 *
 * Write to one configuration register.
 *
 * Value is read back and displayed.
 */
void cmd_wreg(void)
{
	uint32_t addr;
	uint32_t value;
	char s[16];
	char *ptr;


	if(n_args < 3 || n_args > 3)
	{
		print_str("Usage: wreg <addr> <value>\n");
		return;
	}

	// Get the register address
	addr = strtol(args[1], &ptr, 16);			// Hex

	value = strtol(args[2], &ptr, 16);			// Hex

	if(addr >= N_CONFIG_REGS)
	{
		print_str("Address range [00-2E]");
		return;
	}

	if(value >= 256)
	{
		print_str("Value range [00-FF]");
		return;
	}

	// Write to config register.
	cc_write(addr, value);

	value = cc_read(addr);

	ByteToHex(s, (uint8_t)addr);
	print_str(s);
	print_str(": ");
	ByteToHex(s, (uint8_t)value);
	print_str(s);
	print_str("\n");

}


/* Send a command to CC2500.
 *
 */
void cmd_command(void)
{
	uint8_t cmd_value;
	char s[16];


	if(n_args < 2 || n_args > 2)
	{
		print_str("Usage: cmd <command>\n");
		return;
	}

	// Convert command string to command value
	cmd_value = StrToCmd(args[1]);

	if(cmd_value == 0)
	{
		print_str("Invalid command\n");
		return;
	}

	// Print command value
	print_str("Command ");
	ByteToHex(s, cmd_value);
	print_str(s);
	print_str("\n");


	// Send command to radio.
	cc_write_cmd(cmd_value);
}


/* cmd_status
 *
 * Read status register
 *
 * Example:
 *  status				Read all status registers
 *  status 3A			Read status register 0x3A (TXBYTES)
 *  status 30 5			Read the first 5 status registers
 *
 */
void cmd_status(void)
{
	char s[16];
	uint32_t addr;				// Status register address
	uint8_t status;
	int i;
	int n;
	char *ptr;


	// This first part is to determine the register start address
	// and the number of registers to display.
	if(n_args == 1)
	{
		// No register specified. Print all the status registers.
		addr = 0x30;								// Status register start address
		n = N_STATUS_REGS;							// Set no. of registers to print.
	}
	else
	{
		// Get the configuration register address
		addr = strtol(args[1], &ptr, 16);			// Hex

		// Get the register count
		if(n_args == 2)								// No count specified.
		{
			n = 1;									// Print one register
		}
		else										// Print n-registers
		{
			n = strtol(args[2], &ptr, 10);			// Get count in decimal.
		}

	}

	// Check register address.
	if(addr < 0x30 || addr > 0x3D)
	{
		print_str("Invalid status register address\n");
		return;
	}


	// Print the status registers.
	for(i=0; i<n; i++)
	{
		if(addr > 0x3D)
			break;

		status = cc_read_status(addr);

		ByteToHex(s, addr);
		print_str(s);
		print_str(": ");

		ByteToHex(s, status);
		print_str(s);
		print_str("\n");

		addr++;

	}

	return;
}


/*
 * Change to speed control mode.
 *
 */
void cmd_speed_mode(void)
{
	cmd_mode = 1;
	print_str("Speed adjust mode\n");
	print_str("Use +/- to adjust speed\n");

}

void cmd_mot(void)
{
	char s[16];
	int ramp_time;
	int speed;
	//int dir = 1;	// +1 for FORWARD, -1 for REVERSE
	//int i, n;
	char *ptr;


	if(n_args == 1)
	{
		// no arguments -> print current value of speed
		speed = pwm_get_speed();

	}
	else
	{
		// Get the new speed setting
		speed = strtol(args[1], &ptr, 10);			// dec

		// Get the register count
		if(n_args == 2)					// No ramp time specified. Use default or previous value?.
		{
			ramp_time = 1000;
		}
		else
		{
			ramp_time = strtol(args[2], &ptr, 10);			// Get ramp time in decimal.
		}
		pwm_change_speed(speed, ramp_time);
	}
	IntToHex(s, speed);
	print_str("current speed : ");
	print_str(s);
	print_str("\n");
	return;
}


/* cmd_speed
 *
 * Set the speed or read the current speed.
 *
 * Usage:
 * speed			Get the current speed
 * speed <val>		Set the current speed
 *
 * Displays the current speed in all cases.
 *
 */
void cmd_speed(void)
{
	int speed;
	char s[16];
	char *ptr;


	if(n_args == 2)
	{
		// Set the speed
		speed = strtol(args[1], &ptr, 10);			// dec
		pwm_out(speed);
	}

	// Get the new speed.
	speed = pwm_get_speed();

	// Print the current speed in hex.
	IntToHex(s, speed);
	print_str("current speed : ");
	print_str(s);
	print_str("\n");

}


/* speed_adjust_mode
 *
 * When in this mode the speed can be adjusted with +/- keys.
 * Use 'q' to quit and return to the command processor.
 */
void speed_adjust_mode(char c)
{
	int speed;
	char s[16];


	// Adjust speed using +/- keys
	switch(c)
	{
		case '+':
			pwm_incr(16);
			break;
		case '-':
			pwm_incr(-16);
			break;
		case 'q':
			cmd_mode = 0;
			print_str("Exit speed adjust mode\n");
			return;
	}



	// Print the current speed.
	speed = pwm_get_speed();
//	itoa(speed, s, 10);
	IntToHex(s, speed);
	print_str(s);
	print_str("\n");


}


/* help_txt
 *
 * Prints the help text from information in the command structure array.
 */
void help_txt(void)
{
	CMD_ITEM *pcmd_item;			// Pointer to command structures
	int i;

	// Go through the array of command structures
	pcmd_item = (CMD_ITEM *)cmd_list;			// Point to the first item in command structure array.
	for(i=0; i<N_CMDS; i++)
	{
		print_str(pcmd_item->cmd_txt);			// Print the command text
		print_str("\t\t");						// Print some spaces
		print_str(pcmd_item->help_txt);			// Print help text for the command.
		print_str("\n");

		pcmd_item++;
	}

/*
	print_str("help\n");
	print_str("reg  [addr] [count]    Read config registers\n");
	print_str("wreg <addr>            Write config registers\n");
	print_str("cmd <command>          Send a command\n");
	print_str("status [reg]           Read radio status register\n");
	print_str("state                  Read radio state\n");
	print_str("mot  [speed] [rtime]   set motor speed\n");
	print_str("speed <val>\n");
*/
}

/*
 *
 */
void cmd_version(void)
{
	print_str("v1.00");
}


/*
 * SRES		 CC2500 Reset
 */
void cmd_sres(void)
{
	cc_write_cmd(SRES);
	print_str("SRES\n");
}


/*
 * tx
 *
 * Send a string to radio for transmit
 */
void cmd_tx(void)
{
	char *str;
	int n;
	char s[16];


	// Check there is a string
	if(n_args < 2)
	{
		print_str("No string\n");
		return;
	}

	str = (char *)args[1];			// Point to the start of the string
	n = strlen(str);				// Get the string length

	// Message echo
	print_str("Sending ");
	print_str(" ");
	IntToHex(s, n);
	print_str(s);
	print_str(" bytes\n");

	// Write the string into the TX FIFO
	rf_write_tx_fifo((uint8_t *)str, n);

}
