/*
 * led.c
 *
 *  Created on: Dec 3, 2015
 *      Author: pmatthews
 */



#include "gpio.h"
#include "led.h"



/*
 *
 */
void led_on(void)
{
	GPIO_BitSet(GPIOA, LED_GREEN);
}


/*
 *
 */
void led_off(void)
{
	GPIO_BitReset(GPIOA, LED_GREEN);
}


/*
 *
 */
void led_toggle(void)
{
	GPIO_BitToggle(GPIOA, LED_GREEN);
}





