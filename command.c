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


void kputc(uint8_t c);
void print_str(char *s);
void led_toggle(void);


#define CMD_BUF_SIZE 256

// Command processor engine
void cmdline_proc(void);
int cmd_parse(char *s);
char *get_token(char *str, const char *delimiters, char *token);
int IsDelimiter(char c, const char *delimiters);

void print_tokens(int argc, char **argv);


// CC2500 command functions
void cmd_state(void);
void cmd_reg(void);
void cmd_wreg(void);
void cmd_command(void);
int cmd_status(void);
void help_txt(void);
void cmd_mot(void);


char cmd_buf[CMD_BUF_SIZE];			// Command line buffer.
uint16_t cmd_index;					// Current position in command buffer.


char token[32];
char *ptok;


#define MAX_ARGS 16
char *args[MAX_ARGS];
int n_args;


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

			cmdline_proc();			// Process the command buffer.

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
		help_txt();
	}
	else if(strcmp(cmd, "ver") == 0)
	{
		print_str("v1.00");
	}
	else if(strcmp(cmd, "state") == 0)
	{
		cmd_state();
	}
	else if(strcmp(cmd, "reg") == 0)
	{
		cmd_reg();
	}
	else if(strcmp(cmd, "wreg") == 0)
	{
		cmd_wreg();
	}
	else if(strcmp(cmd, "cmd") == 0)
	{
		cmd_command();
	}
	else if(strcmp(cmd, "status") == 0)
	{
		cmd_status();
	}
	else if(strcmp(cmd, "mot") == 0)
	{
		cmd_mot();
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


/*
 *
 */
int cmd_status(void)
{
	char s[16];
	uint32_t addr;				// Status register address
	uint8_t status;
	int i;
	int n;
	char *ptr;


	if(n_args == 1)
	{
		// No register specified. Print all the status registers.
		addr = 0x30;								// Status register start address
		n = N_STATUS_REGS;
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

	if(addr < 0x30 || addr > 0x3D)
	{
		print_str("Status register out of range\n");
		return 0;
	}

	// Print status register contents
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


	return 0;
}

/*
 *
 */
void cmd_mot(void)
{
	char s[16];
	int ramp_time;
	int speed;
	int dir = 1;	// +1 for FORWARD, -1 for REVERSE
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
	return 0;
}

/*
 *
 */
void help_txt(void)
{
	print_str("help\n");
	print_str("reg  [addr] [count]    Read config registers\n");
	print_str("wreg <addr>            Write config registers\n");
	print_str("cmd <command>          Send a command\n");
	print_str("status [reg]           Read radio status register\n");
	print_str("state                  Read radio state\n");
	print_str("mot  [speed] [rtime]   set motor speed\n");
}
