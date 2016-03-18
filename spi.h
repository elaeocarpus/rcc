/*
 * spi.h
 *
 */

#ifndef _SPI_H
#define _SPI_H


#include "stm32f103xb.h"


// SPI1
void spi_init(void);
uint8_t spi_out(uint8_t d);
void spi_wr(uint8_t addr, uint8_t data);
void spi_wr_array(uint8_t addr, uint8_t *data, uint8_t n);
uint8_t spi_rd(uint8_t addr);

// SPI2
void spi2_init(void);
uint8_t spi2_out(uint8_t d);


#endif
