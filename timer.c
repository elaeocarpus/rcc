/*
 * timer.c
 */

#include "timer.h"



#define TIM2_Enable()	(TIM2->CR1 |= TIM_CR1_CEN)
#define TIM3_Enable()	(TIM3->CR1 |= TIM_CR1_CEN)
#define TIM4_Enable()	(TIM4->CR1 |= TIM_CR1_CEN)


void TIM_SetPrescaler(TIM_TypeDef *timer, uint32_t psc);



/* timer2_init
 *
 * 16-bit timer/counter
 *
 * Clock (TIM2CLK) 32MHz
 * 1msec timer tick
 */
void timer2_init(void)
{

	TIM2->CR1 = (uint16_t)0x0000;
		// CKD		Clock division for input sampling digital filters
		// ARPE		Auto-reload register not buffered
		// CMS		Edge aligned
		// DIR		Count up
		// OPM		Continuous mode
		// URS		Update request source
		// UDIS		Update event disable is off
		// CEN 		Count not enabled

	TIM2->CR2 = (uint16_t)0x0000;
		// TI1S
		// MMS
		// CCDS

	// Slave mode control register
	TIM2->SMCR = (uint16_t)0x0000;
		// SMS		Slave mode disabled

	// DMA/interrupt enable register
	TIM2->DIER = (uint16_t)0x0000;
		// All DMA/interrupts disabled

	// Status register
	TIM2->SR = (uint16_t)0x0000;
		// Clear all status flags

	// Capture/compare mode registers
	TIM2->CCMR1 = (uint16_t)0x0000;
	TIM2->CCMR2 = (uint16_t)0x0000;

	// Prescaler
	TIM_SetPrescaler(TIM2, PSC_VAL);

	// Auto-reload register.
	TIM2->ARR = 32000;
	//TIM2->ARR = (uint16_t)((CK_CNT / PWM_FREQ) - 1);			// set PWM frequency

	// Capture/compare registers
	TIM2->CCR1 = 0;
	TIM2->CCR2 = 0;
	TIM2->CCR3 = 0;
	TIM2->CCR4 = 0;

	// Output Compare (OCx) output enable
	TIM2->CCER  = (uint16_t)0x0000;

	// DMA control
	TIM2->DCR  = (uint16_t)0x0000;			// DMA control register
	TIM2->DMAR = (uint16_t)0x0000;

	TIM2->DIER |= TIM_DIER_UIE;			// TM2 Update Interrupt Enable.

	TIM2_Enable();						// Timer2 Counter enabled

}


/* timer3_init
 *
 * Clock	32MHz
 *
 * TIM3 Capture/Compare4	PWM Mode 1
 *
 *
 */
void timer3_init(void)
{
	TIM3->CR1 = (uint16_t)0x0000;
		// CKD		Clock division for input sampling digital filters
		// ARPE		Auto-reload register not buffered
		// CMS		Edge aligned
		// DIR		Count up
		// OPM		Continuous mode
		// URS		Update request source
		// UDIS		Update event disable is off
		// CEN 		Count not enabled

	TIM3->CR2 = (uint16_t)0x0000;
		// TI1S
		// MMS
		// CCDS

	// Slave mode control register
	TIM3->SMCR = (uint16_t)0x0000;
		// SMS		Slave mode disabled

	// DMA/interrupt enable register
	TIM3->DIER = (uint16_t)0x0000;
		// All DMA/interrupts disabled

	// Status register
	TIM3->SR = (uint16_t)0x0000;
		// Clear all status flags

	// Capture/compare mode registers
	TIM3->CCMR1 = (uint16_t)0x0000;
		// CCR1
		// CCR2

	TIM3->CCMR2 = (uint16_t)(0x06<<12);
		// CCR3
		// CCR4  PWM Mode 1

	// Prescaler
	TIM_SetPrescaler(TIM3, 0);			// Prescaler = 1 -> clk = 32MHz

	// Auto-reload register.
	TIM3->ARR = (uint16_t)1024;			// Timer period for 10-bit PWM

	// Capture/compare registers
	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
	TIM3->CCR3 = 0;
	TIM3->CCR4 = 0;

	// Output Compare (OCx) output enable
	TIM3->CCER = TIM_CCER_CC4E;			// Enable OC4

	// DMA control
	TIM3->DCR = 0;			// DMA control register
	TIM3->DMAR = 0;


	TIM3->DIER |= TIM_DIER_UIE;			// TM2 Update Interrupt Enable.

	TIM3_Enable();						// Timer2 Counter enabled

}



/* Timer4
 *
 * 16-bit timer/counter
 * Clock 32MHz
 * Configured for PWM 32.15kHz
 */
void timer4_init(void)
{
	TIM4->CR1 = (uint16_t)0x0000;
		// CKD		Clock division for input sampling digital filters
		// ARPE		Auto-reload register not buffered
		// CMS		Edge aligned
		// DIR		Count up
		// OPM		Continuous mode
		// URS		Update request source
		// UDIS		Update event disable is off
		// CEN 		Count not enabled

	TIM4->CR2 = (uint16_t)0x0000;
		// TI1S
		// MMS
		// CCDS

	// Slave mode control register
	TIM4->SMCR = (uint16_t)0x0000;
		// SMS		Slave mode disabled

	// DMA/interrupt enable register
	TIM4->DIER = (uint16_t)0x0000;
		// All DMA/interrupts disabled

	// Status register
	TIM4->SR = (uint16_t)0x0000;
		// Clear all status flags

	// Capture/compare mode registers
	TIM4->CCMR2 = (uint16_t)0x0000;
		// CCR3
	TIM4->CCMR2 |= ((TIM_CCMR2_OC3M | (OCM_PWM_MODE_1 << 4)));
		// CCR4
	TIM4->CCMR2 |= ((TIM_CCMR2_OC4M | (OCM_PWM_MODE_1 << 12)));

	// Prescaler
	TIM_SetPrescaler(TIM4, PSC_VAL);

	// Auto-reload register.
	TIM4->ARR = (uint16_t)((CK_CNT / PWM_FREQ) - 1);			// set PWM frequency

	// Capture/compare registers (Range [0-511]
	TIM4->CCR1 = 0;
	TIM4->CCR2 = 0;
	TIM4->CCR3 = 0;
	TIM4->CCR4 = 0;

	// Output Compare (OCx) output enable
	TIM4->CCER  = (uint16_t)0x0000;
	TIM4->CCER |= TIM_CCER_CC3E;			// Output Compare OC3 active
	TIM4->CCER |= TIM_CCER_CC4E;			// Output Compare OC4 active.


	// DMA control
	TIM4->DCR  = (uint16_t)0x0000;			// DMA control register
	TIM4->DMAR = (uint16_t)0x0000;


	TIM4->DIER |= TIM_DIER_UIE;			// TM2 Update Interrupt Enable.

	TIM4_Enable();						// Timer2 Counter enabled


}



/*
 *
 */
void TIM_SetPrescaler(TIM_TypeDef *timer, uint32_t psc)
{
	timer->PSC = psc;
}

/*
uint16_t TIM_ReadConfigReg(TIM_TypeDef *timer, uint32_t reg)
{
	reg &= 0x7f;
	if (reg > 0x50)
	{
		reg = 0x50;
	}
	uint16_t arr = timer->reg;	// read config register
	return arr;
}
*/

