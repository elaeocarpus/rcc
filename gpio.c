/*
 * gpio.c
 *
 * General Purpose I/O
 *
 *  Created on: Nov 27, 2015
 *      Author: pmatthews
 */



#include "gpio.h"


#define uint16	uint16_t
#define uint32	uint32_t


/*
 * Configure GPIO pin
 *
 * gpio		Pointer to GPIO registers
 * pin		0-15
 * cnf		CNFy[1:0]
 * mode		MODEy[1:0]
 *
 * Configures one GPIO pin in a port.
 *
 */
void GPIO_Config(GPIO_TypeDef *gpio, uint16_t pin, uint16_t cnf, uint16_t mode)
{
	uint32 config;
	uint32 y;


	config = ((uint32)(cnf & 0x00000003) << 2) | ((uint32)(mode & 0x00000003));

	if(pin < 8)
	{
		y = pin<<2;
		gpio->CRL &= ~(0x0000000F << y);			// Clear CNFy[1:0] and MODEy[1:0] bits
		gpio->CRL |= config << y;
	}
	else
	{
		y = (pin-8)<<2;
		gpio->CRH &= ~(0x0000000F << y);			// Clear CNFy[1:0] and MODEy[1:0] bits
		gpio->CRH |= config << y;
	}
}


/* GPIO_BitSet
 *
 * gpio		Pointer to GPIO registers
 * pin		Pin to set [0-15]
 *
 * Writes to the Port bit set/reset register.
 * Atomically sets a GPIO port bit.
 * This function sets only one port bit.
 *
 */
void GPIO_BitSet(GPIO_TypeDef *gpio, uint16_t pin)
{
	gpio->BSRR = (uint32)((1<<pin) & 0x0000FFFF);

}


/* GPIO_BitReset
 *
 * gpio		Pointer to GPIO registers
 * pin		Pin to reset [0-15]
 *
 * Writes to the Port bit reset register.
 * Atomically resets (clears) a GPIO port bit.
 * This function resets only one port bit.
 *
 */
void GPIO_BitReset(GPIO_TypeDef *gpio, uint16_t pin)
{
	gpio->BRR = (uint32)((1<<pin) & 0x0000FFFF);

}


/* GPIO_BitToggle
 *
 * gpio		Pointer to GPIO registers
 * pin		Pin to toggle [0-15]
 *
 * Toggle GPIO Port bit.
 * This function toggles only one port bit.
 * Bit is toggled in GPIO ODR register and is not atomic.
 */
void GPIO_BitToggle(GPIO_TypeDef *gpio, uint16_t pin)
{
	gpio->ODR ^= (uint32)((1<<pin) & 0x0000FFFF);

}


