/*
 * adc.h
 *
 *  Created on: Nov 30, 2015
 *      Author: pmatthews
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f103xb.h"


// ADC channels
#define ADC_IN_0	0
#define ADC_IN_1	1
#define ADC_IN_2	2
#define ADC_IN_3	3
#define ADC_IN_4	4
#define ADC_IN_5	5
#define ADC_IN_6	6
#define ADC_IN_7	7
#define ADC_IN_8	8
#define ADC_IN_9	9
#define ADC_IN_10	10
#define ADC_IN_11	11
#define ADC_IN_12	12
#define ADC_IN_13	13
#define ADC_IN_14	14
#define ADC_IN_15	15
#define ADC_IN_TEMP	16			// Internal temperature reference.
#define ADC_IN_VREF	17			// Internal voltage reference.

// ADC Prescaler dividers
#define ADC_PRE_DIV2	0
#define ADC_PRE_DIV4	1
#define ADC_PRE_DIV6	2
#define ADC_PRE_DIV8	3

// ADC External Trigger Sources
#define ADC_TRIG_TIM1_CC1		0
#define ADC_TRIG_TIM1_CC2		1
#define ADC_TRIG_TIM1_CC3		2
#define ADC_TRIG_TIM2_CC2		3		// Timer2 Capture/Compare Event 2
#define ADC_TRIG_TIM3_TRG0		4
#define ADC_TRIG_TIM4_CC4		5
#define ADC_TRIG_EXT1			6
#define ADC_TRIG_SWSTART		7


void ADC_prescaler(uint32_t div);
void adc1_init(void);

uint32_t adc1_read(uint32_t adc_ch);

void ADC_Enable(void);
void ADC_StartConversion(void);
uint32_t ADC_ReadResult(void);

void adc_sample_time(uint16_t adc_ch, uint16_t smp);
void ADC_SetScanGroupChannel(uint16_t sq, uint16_t adc_ch);
void ADC_SetScanGroupSize(uint32_t n);
void ADC_SetScanGroup(uint16_t *adc_group, uint32_t n);

void ADC_SelectExtTrig(uint32_t trig_src);

#endif /* ADC_H_ */
