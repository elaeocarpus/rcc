/*
 * adc.c
 *
 *
 * TIMx ADC External Trigger
 * ------------------------------
 * The external trigger source for A/D conversion is timer output compare OCx bit.
 * If the trigger source (ADCx_CR2.EXtSEL[2:0]) is Timer2 Capture/CompareEvent 2,
 * then the trigger event is when OC2 goes high.
 *
 * The trigger event is not the output compare event, or the CCxIF interrupt flag.
 *
 * Configure TIM2 to be up-counter and reset at TM2_ARR value.
 * Configure Timer 2 Capture/Compare mode to be Output Compare PWM mode 1.
 * Set register TIM2_CCR2 <= TM2_ARR, so that a compare does occur.
 * TIM2_CCR2 must not be zero, otherwise an output compare event will not occur.
 * PWM is needed to make OC2 pin set and reset automatically
 * Enable the OC2 output.
 * Note: even though OC2 output is enabled, it doesn't have to actually
 * drive a pin  GPIO setup can select GPIO pin instead of OC2 output.
 *
 *
 */


#include "adc.h"



uint16_t adc_scan_group[16];



#define adc1_start()			(ADC1->CR2 |= ADC_CR2_ADON)
#define adc1_data_read()		(ADC1->DR)
#define ADC_EnableScanMode()	(ADC1->CR1 |= ADC_CR1_SCAN)
#define ADC_DisableScanMode()	(ADC1->CR1 &= ~ADC_CR1_SCAN)
#define ADC_EnableExtTrig()		(ADC1->CR2 |= ADC_CR2_EXTTRIG)
#define ADC_DisableExtTrig()	(ADC1->CR2 &= ~ADC_CR2_EXTTRIG)




/* ADC_Enable
 *
 * Writing 1 to ADON when ADON is 0 wakes ADC from low-power mode.
 * Writing 1 to ADON when ADON already set triggers an A/D conversion.
 */
void ADC_Enable(void)
{
	if(!(ADC1->CR2 & ADC_CR2_ADON))			// If ADON not set, then ADC is not enabled.
		ADC1->CR2 |= ADC_CR2_ADON;			// Wake ADC.

}


/* ADC_StartConversion
 *
 */
void ADC_StartConversion(void)
{
	if(!(ADC1->CR2 & ADC_CR2_ADON))			// If ADON not set, then ADC is not enabled.
		return;								// Don't write to ADON.

	ADC1->CR2 |= ADC_CR2_ADON;
}


/* ADC_ReadResult
 *
 * Reads the ADC Data Register (ADC_DR).
 * A/D conversion is 12-bit.
 * ADC Data register holds the result of the last A/D conversion.
 *
 */
uint32_t ADC_ReadResult(void)
{
	return adc1_data_read();
}


/* ADC_prescaler
 *
 * div		ADC Pre-scaler divide value
 * 			00	PCLK2/2
 * 			01	PCLK2/4
 * 			10	PCLK2/6
 * 			11	PCLK2/8
 *
 * Sets the ADC clock pre-scaler in the RCC sub-system.
 * ADCCLK = PCLK2 / ADC_pre-scaler
 *
 * ADCCLK is 14MHz max.
 */
void ADC_prescaler(uint32_t div)
{

	RCC->CFGR &= ~(0x00000003 << 14);			// Clear ADC_PRE[1:0]
	RCC->CFGR |= (div & 0x00000003) << 14;		// Set ADC prescaler divider

}


/* Initialise ADC1
 *
 * Trigger Source: Timer2 Output Compare 2 (CC2IF)
 *
 *
 */
void adc1_init(void)
{

	ADC_Enable();							// Enable ADC1

	ADC1->CR2 |= ADC_CR2_TSVREFE;			// Enable temperature sense and Vrefint
	ADC_EnableScanMode();
	//ADC_DisableScanMode();					// Single channel conversion.

	ADC1->CR2 &= ~ADC_CR2_CONT;				// Continuous mode disabled.


	// Set ADC sample times.
	adc_sample_time(ADC_IN_0, 7);			// Set sample time for ADC input 0.
	adc_sample_time(ADC_IN_1, 7);			//
	adc_sample_time(ADC_IN_TEMP, 7);		// Set sample time for ADC internal temperature sense.
	adc_sample_time(ADC_IN_VREF, 7);		// Set sample time for ADC internal voltage reference.

	// Set up scan group
	adc_scan_group[0] = ADC_IN_0;
	adc_scan_group[1] = ADC_IN_1;
	ADC_SetScanGroup(adc_scan_group, 1);

	// Set trigger source.
	ADC_SelectExtTrig(ADC_TRIG_TIM2_CC2);	// Trigger on Timer2 CC2 Event
	ADC_EnableExtTrig();

	// Enable DMA transfer
	ADC1->CR2 |= ADC_CR2_DMA;
}


/* adc1_read
 *
 * Performs A/D conversion of one channel.
 * Single conversion mode.
 * Starts A/D conversion of selected channel.
 * Polls until A/D conversion is complete.
 *
 * AD result is 12-bit right justified sample.
 *
 *
 */
uint32_t adc1_read(uint32_t adc_ch)
{
	uint32_t adc_data;


	// Select A/D channel
	ADC_SetScanGroupChannel(1, adc_ch);	// Set 1st regular scan group channel
	ADC_SetScanGroupSize(1);			// Scan group has only one channel.


	adc1_start();						// Start A/D conversion.

	while(!(ADC1->SR & ADC_SR_EOC))		// Poll End Of Conversion (EOC) flag.
	{ }

	adc_data = adc1_data_read();		// Read A/D result.

	return adc_data;
}


/* adc_sample_time
 *
 * Set ADC sample time for one ADC channel.
 *
 * adc_ch		A/D channel [0-17]
 * smp			Sample time
 *
 * Registers:
 *	ADC1_SMPR1
 *	ADC1_SMPR2
 *
 */
void adc_sample_time(uint16_t adc_ch, uint16_t smp)
{
	uint32_t shift;


	if(adc_ch > 17)						// ADC channel out of range.
		return;

	smp &= 0x0007;

	if(adc_ch < 10)						// ADC1_IN[9:0]
	{
		shift = adc_ch * 3;
		ADC1->SMPR2 &= ~(0x00000007 << shift);			// Clear SMP[2:0]
		ADC1->SMPR2 |= smp << shift;
	}
	else								// ADC1_IN[17:10]
	{
		shift = (adc_ch - 10) * 3;
		ADC1->SMPR1 &= ~(0x00000007 << shift);			// Clear SMP[2:0]
		ADC1->SMPR1 |= smp << shift;
	}
}


/* ADC_SetScanGroupChannel
 *
 * Set the A/D channel for one sequence location.
 *
 * sq			A/D scan sequence number [1-16]
 * adc_ch		A/D channel [0-17]
 *
 * The first A/D channel in scan sequence (SQ1) is the channel used in single mode.
 *
 * eg:  ADC_SetScanGroupChannel(2, 14);	The 2nd A/D conversion in the scan sequence is ADC channel 14
 *
 */
void ADC_SetScanGroupChannel(uint16_t sq, uint16_t adc_ch)
{
	uint32_t shift;


	if(adc_ch > 17)						// ADC channel out of range.
		return;

	if(sq <= 6)
	{
		shift = (sq - 1) * 5;
		ADC1->SQR3 &= ~(0x001F << shift);		// Clear the SQx[4:0] bits
		ADC1->SQR3 |= adc_ch << shift;			// Set the ADC channel.
		return;
	}

	if(sq <= 12)
	{
		shift = (sq - 7) * 5;
		ADC1->SQR2 &= ~(0x001F << shift);		// Clear the SQx[4:0] bits
		ADC1->SQR2 |= adc_ch << shift;			// Set the ADC channel.
		return;
	}

	if(sq <= 16)
	{
		shift = (sq - 13) * 5;
		ADC1->SQR1 &= ~(0x001F << shift);		// Clear the SQx[4:0] bits
		ADC1->SQR1 |= adc_ch << shift;			// Set the ADC channel.
	}


}


/* ADC_SetScanGroupSize
 *
 * n	No. of scan channels in regular scan group.
 *
 * Sets the number of scan channels in ADC1 regular scan group.
 * The total number of conversions is written to the L[3:0] bits
 * of the ADC_SQR1 register.
 * 	0000 = 1 conversion
 * 	0001 = 2 conversions
 * 	 ...
 * 	1111 = 16 conversions
 *
 * The maximum number of channels in a scan group is 16.
 *
 */
void ADC_SetScanGroupSize(uint32_t n)
{
	if(n > 16)
		return;

	ADC1->SQR1 &= ~0x00F0000;			// Clear bits L[3:0]
	ADC1->SQR1 |= ((n-1)<<20);			// Set bits L[3:0]

}


/* ADC_SetScanGroup
 *
 * *adc_group		List of channels in the scan group.
 * n				No. of channels in the scan group
 *
 * Initialises an ADC regular scan group.
 * When an A/D conversion is triggered, all the channels in the
 * scan group are converted one after the other. There is only one
 * A/D result register, which is continually overwritten.
 * When using a scan group it is necessary to use DMA to capture
 * all the A/D conversions.
 */
void ADC_SetScanGroup(uint16_t *adc_group, uint32_t n)
{
	uint16_t sq;			// Scan sequence number


	if(n > 16)
		return;

	// Write the scan group list into ADC_SQRx registers.
	for(sq=1; sq<=n; sq++)
	{
		ADC_SetScanGroupChannel(sq, *adc_group++);
	}

	// Set the number of A/D channels in the scan group.
	ADC_SetScanGroupSize(n);
}


/* ADC_SelectExtTrig
 *
 * Select the ADC external trigger source.
 * Writes to EXTSEL[2:0] bits in ADC_CR2 register.
 */
void ADC_SelectExtTrig(uint32_t trig_src)
{
	if(trig_src > 7)
		return;

	ADC1->CR2 &= ~((uint32_t)(0x07)<<17);		// Clear EXTSEL[] bits
	ADC1->CR2 |= (trig_src<<17);				// Set external trigger source

}
