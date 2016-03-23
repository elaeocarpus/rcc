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
#define SPI2_clk_enable()	(RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)


#define GPIOA_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIOB_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIOC_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define SPI1_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define ADC1_clk_enable()	(RCC->APB2ENR |= RCC_APB2ENR_ADC1EN)



void dma1_init(void);


void cmd_proc_init(void);
void cmd_proc(uint8_t c);

void gpio_init(void);
void delay(int t);
uint16_t read_key(void);



#define USER_PB		GPIO_IDR_IDR13		// PC13
#define KEY_MASK  (1<<13)



char c;

char s[16];
uint16_t adc_sample;
uint16_t tick_msec;
uint32_t adc_count;

uint8_t reg;
char x;

uint8_t freq[3];
uint8_t regs[0x30];

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

	SPI1_clk_enable();		// Enable SPI1 clock.
		// SPI1 clock source is PCKL2 for APB2 peripherals.

	SPI2_clk_enable();

	ADC_prescaler(ADC_PRE_DIV4);		// ADCCLK = PCLK2/4 = 8MHz
	ADC1_clk_enable();					// Enable ADC1 clock.


	// Hardware initialization
	gpio_init();
	uart2_init();
	timer2_init();
	timer3_init();
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
/*	// Write one byte out SPI2
		GPIO_BitReset(GPIOB, GPIO_PIN12);			// CSn low
		x = spi_out(0x3D);							// NOP
		delay(10);
		GPIO_BitSet(GPIOB, GPIO_PIN12);				// CSn high

		// Print the received status byte out the serial port
		ByteToHex(s, x);
		print_str(s);
		print_str(" ");
		print_str("\n");
*/
	cc_reset();										// Send reset command to CC2500
	print_str("CC2500 Reset\n");
	delay(1000);
	pwm_set_speed(200);

/*
	// Read all the config registers
	cc_read_b(0, regs, 0x2F);

	// Print the configuration registers
	print_str("Registers:\n");
	for(reg=0; reg<0x2E; reg++)
	{
		ByteToHex(s, reg);
		print_str(s);
		print_str(" ");

		x = regs[reg];
		ByteToHex(s, x);
		print_str(s);
		print_str("\n");
	}

*/

	cmd_proc_init();

	while(1)
	{
		if(usart2_rxdata_rdy())
		{
			c = usart2_read();

			cmd_proc(c);



/*			else if(c == 'r')				// RX
			{
				cc_cmd_rx();				// SRX
				print_str("RX\n");

			}
			else if(c == 't')				// TX
			{
				cc_write_cmd(SFSTXON);
				//cc_write_cmd(STX);
				print_str("TX\n");
			}
			else if(c == 'i')				// Idle
			{
				cc_write_cmd(SIDLE);
				print_str("IDLE\n");
			}
*/

		}


//		led_toggle();

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
	GPIOA_clk_enable();											// Enable clock to GPIOA


	// PA0 - TIM2_CCO1
	//GPIO_Config(GPIOA, GPIO_PIN0, GPIO_PP, GPIO_OUT_10MHz);	// Output Push-pull.
	GPIO_Config(GPIOA, GPIO_PIN0, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PWM output

	// PA1 - TIM2_CCO2
	GPIO_Config(GPIOA, GPIO_PIN1, ALT_FUNC_PP, GPIO_OUT_10MHz);		// PWM output


	// PA2/TIM2_CCO3 set to alternate function output
	GPIO_Config(GPIOA, GPIO_PIN2, ALT_FUNC_PP, GPIO_OUT_10MHz);

	// PA3/TIM2_CCO4 set to alternate function output
	GPIO_Config(GPIOA, GPIO_PIN3, ALT_FUNC_PP, GPIO_OUT_10MHz);

//	GPIO_Config(GPIOA, GPIO_PIN5, GPIO_PP, GPIO_OUT_10MHz);				// PA5 GPIO output. (LED2)
	GPIO_Config(GPIOA, GPIO_PIN5, ALT_FUNC_PP, GPIO_OUT_10MHz);			// PA5 alternate function output (SPI1_SCK)
	GPIO_Config(GPIOA, GPIO_PIN6, GPIO_FLOAT, GPIO_IN);					// PA6 alternate function input (SPI1_MISO)
	GPIO_Config(GPIOA, GPIO_PIN7, ALT_FUNC_PP, GPIO_OUT_10MHz);			// PA7 alternate function output (SPI1_MOSI)


	// GPIOB
	GPIOB_clk_enable();

	// PB1
	GPIO_Config(GPIOB, GPIO_PIN1, GPIO_PULL, GPIO_IN);		// Input,pull-up
	GPIOB->ODR |= (1UL<<1);

	// SPI2
	GPIO_Config(GPIOB, GPIO_PIN12, GPIO_PP, GPIO_OUT_10MHz);		// PB12 SPI2_NSS
	GPIO_Config(GPIOB, GPIO_PIN13, ALT_FUNC_PP, GPIO_OUT_10MHz);	// PB13 SPI2_SCK
	GPIO_Config(GPIOB, GPIO_PIN14, GPIO_FLOAT, GPIO_IN);			// PB14 SPI2_MISO
	GPIO_Config(GPIOB, GPIO_PIN15, ALT_FUNC_PP, GPIO_OUT_10MHz);	// PB15 SPI2_MOSI


	// GPIOC
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
 */
void __attribute__((interrupt("IRQ")))TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;			// Clear timer update interrupt flag.

	tick_msec = 1;						// Set 1msec tick flag

}






