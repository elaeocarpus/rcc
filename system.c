/*-----------------------------------------------------------------------------
 * system.c
 *
 * System initialisation done the hard way, without any peripheral library.
 *
 *-----------------------------------------------------------------------------
 */


#include "stm32f103xb.h"


void SystemInitError(void);


/* SystemInit
 *
 * Initialise hardware.
 * This function is called from the C-startup code before main().
 *
 * Clock source: HSI 8MHz
 * PLL clock source: HSI/2 = 4MHz
 * PLL enabled: (x8) =>	32MHz
 * SYSCLK set to PLLCLK = 32MHz.
 * HCLK = SYSCLK/1	32MHz
 *
 */
void SystemInit(void)
{
	// Enable Power Control clock
	RCC->APB1ENR = RCC_APB1ENR_PWREN;
		// APB1ENR - APB1 peripheral clock enable register

	// Regulator voltage scaling output selection: Scale 2
	//PWR->CR |= PWR_CR_VOS_1;

	// HSI (RC 8MHz) starts on reset.  Not required to enable HSI.
	// Wait until HSI is stable.
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
		// HSI: High Speed Internal Oscillator.
		// Poll the HSIRDY flag in RCC_CR register until it is set by hardware.

	// Store calibration value.
	PWR->CR |= (uint32_t)(16 << 3);			// HSITRIM[4:0]
		// Set HSI trim value for RC oscillator.


	// Disable PLL
	RCC->CR &= ~(RCC_CR_PLLON);
	while((RCC->CR & RCC_CR_PLLRDY) != 0);			// Wait until PLL is unlocked.

	// Configure PLL
	RCC->CFGR = RCC_CFGR_PLLMULL8;
//	RCC->CFGR |= RCC_CFGR_PLLMULL16;				// *** Doesn't seem to work. ***
		// PLL clock source: HSI/2 (4MHz)
		// PLL Multiplier: x8
		// Note: Can't configure PLL and select SYSCLK source at the same time.

	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0);			// Wait until PLL has locked.


	// Flash configuration
	//FLASH->ACR = FLASH_ACR_ICEN;



	// Set SYSCLK source (SW[1:0])
	RCC->CFGR |= RCC_CFGR_SW_PLL;					// SYSCLK source: PLL (32MHz)

	// Check System Clock Select Status (SWS[1:0])
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);

	// SYSCLK is the input into AHB prescaler.

	// Set AHB prescaler DIV1
	RCC->CFGR &= ~(RCC_CFGR_HPRE);				// HCLK = 32MHz
		// HCLK = SYSCLK/1

	// Set APB1 Low speed prescaler (APB1) DIV1
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);				// Clear PPRE1[2:0]
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;			// PCLK1 = HCLK

	// Set APB2 High speed prescaler (APB2) DIV1
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);				// DIV1


}


/*
 *
 */
void SystemInitError(void)
{
	while(1);
}
