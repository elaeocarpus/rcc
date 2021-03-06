/* Project: rcc
 *
 * Radio Command Control
 *
 * File: main.c
 *
 */


//#include <stdio.h>
#include "stm32f103xb.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "spi.h"
#include "adc.h"
#include "led.h"
#include "cc2500_regs.h"
#include "cc_hal.h"
#include "textio.h"
#include "pwm.h"


// Peripheral Clock Enable
#define DMA1_clk_enable()	(RCC->AHBENR |= RCC_AHBENR_DMA1EN)

#define PWR_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_PWREN)			// Power Interface Clock Enable
#define BKP_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_BKPEN)			// Backup Interface Clock Enable
#define USART2_clk_enable() (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define TIM2_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIM3_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define TIM4_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_TIM4EN)
#define SPI2_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)


#define GPIOA_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIOB_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIOC_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define SPI1_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define ADC1_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_ADC1EN)



void dma1_init(void);


void cmd_proc_init(void);
void cmd_proc(uint8_t c);
void speed_adjust_mode(char c);


void gpio_init(void);
void delay(int t);
uint16_t read_key(void);



#define USER_PB		GPIO_IDR_IDR13		// PC13
#define KEY_MASK  (1<<13)



char c;

char s[16];
uint16_t adc_sample;
uint32_t tick_msec;
uint32_t adc_count;
uint32_t count_10msec;
uint32_t count_1sec;


uint8_t reg;
char x;

uint8_t freq[3];
uint8_t regs[0x30];

extern int cmd_mode;


/*
 *
 *
 */
int main(void)
{

	PWR_clk_enable();
		// Enable clock to Power Control registers

	BKP_clk_enable();		// Enable clock to Backup domain registers.

	DMA1_clk_enable();		// Enable DMA1 clock.

	USART2_clk_enable();	// Enable USART2 clock.
		// USART2 clock source is PCKL1 for APB1 peripherals.
		// Clock to USART2 must be enabled to be able to access
		// USART2 registers.

	TIM2_clk_enable();		// Enable TIM2CLK clock.
		// TIM2CLK must be enabled to be able to access TIM2 registers.

	TIM3_clk_enable();		// Enable TIM3CLK clock.
		// TIM3CLK must be enabled to be able to access TIM3 registers.

	TIM4_clk_enable();		// Enable TIM4CLK clock.
		// TIM4CLK must be enabled to be able to access TIM4 registers.

	SPI1_clk_enable();		// Enable SPI1 clock.
		// SPI1 clock source is PCKL2 for APB2 peripherals.

	SPI2_clk_enable();

	ADC_prescaler(ADC_PRE_DIV4);		// ADCCLK = PCLK2/4 = 8MHz
	ADC1_clk_enable();					// Enable ADC1 clock.


	// Hardware initialization
	gpio_init();
	uart2_init();
	timer2_init();
	//timer3_init();
	timer4_init();
	spi_init();
	spi2_init();
	adc1_init();

	led_off();

	// Interrupt priorities and enable
	NVIC_SetPriority(TIM2_IRQn, 0);					// Set priority of TIM2 interrupt
	NVIC_EnableIRQ(TIM2_IRQn);						// Enable TIM2 interrupt

	NVIC_SetPriority(USART2_IRQn, 0);				// Set UART2 interrupt priority
	NVIC_EnableIRQ(USART2_IRQn);					// Enable UART2 interrupt.

	__enable_irq();									// Enable interrupts

	GPIO_BitSet(GPIOB, GPIO_PIN12);					// CSn high

	// Reset message.
	print_str("\n\nRCC\n");
	print_str("Reset\n");
	print_str("Last Build: ");
	print_str(__DATE__);
	print_str(" ");
	print_str(__TIME__);
	print_str("\n");


	led_on();

	cc_reset();										// Send reset command to CC2500
	print_str("CC2500 Reset\n");

	pwm_out(0);										// Both FWD and REV PWM output off.

	cmd_proc_init();

	while(1)
	{
		// Serial comms
		if(usart2_rxdata_rdy())
		{
			c = usart2_read();

			if(cmd_mode == 1)
				speed_adjust_mode(c);
			else
				cmd_proc(c);
		}

		// Timer tick 1msec
		if(tick_msec)								// Incremented by timer ISR
		{
			tick_msec--;							// Decrement with atomic operation


			// 10msec
			if(!count_10msec)
			{
				count_10msec = 10;


				// Take A/D conversion.
				// Update speed.


			}
			count_10msec--;

			// 1 sec
			if(!count_1sec)
			{
				count_1sec = 1000;

				//

			}
			count_1sec--;
		}
	}

	return 0;
}




/*
 * Configure GPIO ports.
 * Set mode and directions
 *
 */
void gpio_init(void)
{
	// GPIOA
	GPIOA_clk_enable();												// Enable clock to GPIOA

	GPIO_Config(GPIOA, GPIO_PIN0, GPIO_PP, GPIO_OUT_10MHz);			// PA0 Output Push-pull.
	GPIO_Config(GPIOA, GPIO_PIN1, GPIO_PP, GPIO_OUT_10MHz);			// PA1 Output Push-pull.
	GPIO_Config(GPIOA, GPIO_PIN2, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PA2/UART2_TX set to alternate function output
	GPIO_Config(GPIOA, GPIO_PIN3, GPIO_FLOAT, GPIO_IN);				// PA3/USART2_RX set to floating input

	GPIO_Config(GPIOA, GPIO_PIN5, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PA5 alternate function output (SPI1_SCK)
	GPIO_Config(GPIOA, GPIO_PIN6, GPIO_FLOAT, GPIO_IN);				// PA6 alternate function input (SPI1_MISO)
	GPIO_Config(GPIOA, GPIO_PIN7, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PA7 alternate function output (SPI1_MOSI)


	// GPIOB
	//------
	GPIOB_clk_enable();

	GPIO_Config(GPIOB, GPIO_PIN1, GPIO_PULL, GPIO_IN);				// PB1 Input,pull-up
	GPIOB->ODR |= (1UL<<1);

	// PWM outputs
	GPIO_Config(GPIOB, GPIO_PIN8, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PB8 TIM4_CH3
	GPIO_Config(GPIOB, GPIO_PIN9, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PB9 TIM4_CH4

	// SPI2
	GPIO_Config(GPIOB, GPIO_PIN12, GPIO_PP, GPIO_OUT_10MHz);		// PB12 SPI2_NSS
	GPIO_Config(GPIOB, GPIO_PIN13, ALT_FUNC_PP, GPIO_OUT_10MHz);	// PB13 SPI2_SCK
	GPIO_Config(GPIOB, GPIO_PIN14, GPIO_FLOAT, GPIO_IN);			// PB14 SPI2_MISO
	GPIO_Config(GPIOB, GPIO_PIN15, ALT_FUNC_PP, GPIO_OUT_10MHz);	// PB15 SPI2_MOSI


	// GPIOC
	//------
	GPIOC_clk_enable();								// Enable clock to GPIOC

	// PC13 to GPIO input. (User P/B)
	GPIO_Config(GPIOC, GPIO_PIN13, GPIO_FLOAT, GPIO_IN);


}


/*
 *
 */
void delay(int t)
{
	volatile int x;

	while(t > 0)
	{
		x = 100;
		while(x > 0)
			x--;

		t--;
	}

}






/* TIM2 Interrupt Handler
 *
 * Timer 2		1kHz
 *
 * Global variable tick_ms is incremented in this handler every 1msec.
 * The main loop decrements the variable.
 *
 */
void __attribute__((interrupt("IRQ")))TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;			// Clear timer update interrupt flag.

	tick_msec++;						// Set 1msec tick flag

}






