/*
 * keyscan.c
 *
 *  Created on: Dec 2, 2015
 *      Author: pmatthews
 */


#include "stm32f103xb.h"



uint16_t key_sample_last;
uint16_t key_rslt;


#define KEY_MASK  (1<<13)			// User PB  (PC13)



/* keyscan_init
 *
 */
void keyscan_init(void)
{


}


/* read_key
 *
 *
 */
uint16_t read_key(void)
{
	uint16_t key_in;


	key_in = (GPIOC->IDR);
	key_in &= KEY_MASK;				// Mask off the keypad port bits.

	return key_in;
}


/*
 *
 */
void keyscan(void)
{
	uint16_t key_sample;


	key_sample = read_key();


	if(key_sample ^ key_sample_last)		// Key changed
	{
		key_sample_last = key_sample;

		if(key_sample & GPIO_IDR_IDR13)		// USER_PB
		{
			// Release
			key_rslt = (uint16_t)(0x0001);
		}
		else
		{
			// Press
			key_rslt = (uint16_t)(0x8001);
		}

	}

}

