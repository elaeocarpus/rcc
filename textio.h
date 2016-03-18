/*
 * textio.h
 *
 *  Created on: Feb 27, 2016
 *      Author: pmatthews
 */

#ifndef TEXTIO_H_
#define TEXTIO_H_


void kputc(uint8_t c);
void print_str(char *s);

void ByteToHex(char *s, uint8_t x);
void IntToHex(char *s, uint16_t x);
void Int32toHex(char *s, uint32_t x);


#endif /* TEXTIO_H_ */
