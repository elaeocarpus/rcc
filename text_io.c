/*
 * text_io.c
 */


#include "stm32f103xb.h"
#include "textio.h"
#include "uart.h"





/*
 * Print string
 */
void print_str(char *s)
{
	while(*s != '\0')
	{
		kputc(*s++);
	}

}


/*
 *
 */
void kputc(uint8_t c)
{
	uart2_send_byte(c);

	if(c == '\n')
		uart2_send_byte('\r');
}



void ByteToHex(char *s, uint8_t x)
{
	char c;


	c = (x>>4) & 0x0F;
	if(c >= 0x0A)
		*s++ = c + ('A' - 0x0A);
	else
		*s++ = c + '0';

	c = x & 0x0F;
	if(c >= 0x0A)
		*s++ = c + ('A' - 0x0A);
	else
		*s++ = c + '0';

	*s++ = '\0';

}

void IntToHex(char *s, uint16_t x)
{
	ByteToHex(s, (x>>8) & 0x00FF);
	s++;
	s++;

	ByteToHex(s, x & 0x00FF);
	s++;
	s++;

	*s = '\0';
}


void Int32toHex(char *s, uint32_t x)
{

	ByteToHex(s, (uint8_t)(x>>24));
	s++;
	s++;

	ByteToHex(s, (uint8_t)(x>>16));
	s++;
	s++;

	ByteToHex(s, (uint8_t)(x>>8));
	s++;
	s++;

	ByteToHex(s, (uint8_t)(x));
	s++;
	s++;

	*s = '\0';
}
