/*
 * led.h
 *
 *  Created on: Dec 3, 2015
 *      Author: pmatthews
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f103xb.h"

#define LED_GREEN	GPIO_PIN5			// PA5


void led_toggle(void);
void led_on(void);
void led_off(void);


#endif /* LED_H_ */
