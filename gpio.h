/*
 * gpio.h
 *
 *  Created on: Nov 27, 2015
 *      Author: pmatthews
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f103xb.h"

// GPIO Pin
#define GPIO_PIN0	0
#define GPIO_PIN1	1
#define GPIO_PIN2	2
#define GPIO_PIN3	3
#define GPIO_PIN4	4
#define GPIO_PIN5	5
#define GPIO_PIN6	6
#define GPIO_PIN7	7
#define GPIO_PIN8	8
#define GPIO_PIN9	9
#define GPIO_PIN10	10
#define GPIO_PIN11	11
#define GPIO_PIN12	12
#define GPIO_PIN13	13
#define GPIO_PIN14	14
#define GPIO_PIN15	15



// CNFy[1:0]
// Input mode (MODE[1:0] = 00)
#define GPIO_ANALOG		0x00		// Analog mode
#define GPIO_FLOAT		0x01		// Floating input (reset state)
#define GPIO_PULL		0x02		// Input with pull-up/pull-down
// Output mode (MODE[1:0] > 00)
#define GPIO_PP			0x00		// General purpose output push-pull
#define GPIO_OD			0x01		// General purpose output open-drain
#define ALT_FUNC_PP		0x02		// Alternate function output push-pull
#define ALT_FUNC_OD		0x03		// Alternate function output open-drain


// MODEy[1:0]
#define GPIO_IN			0x00		// Input mode (reset state)
#define GPIO_OUT_10MHz	0x01		// Output mode, 10MHz
#define GPIO_OUT_2MHZ	0x02		// Output mode, 2MHz
#define GPIO_OUT_50MHz	0x03		// Output mode, 50MHz



void GPIO_Config(GPIO_TypeDef *gpio, uint16_t pin, uint16_t cnf, uint16_t mode);
void GPIO_BitSet(GPIO_TypeDef *gpio, uint16_t pin);
void GPIO_BitReset(GPIO_TypeDef *gpio, uint16_t pin);
void GPIO_BitToggle(GPIO_TypeDef *gpio, uint16_t pin);



#endif /* GPIO_H_ */
