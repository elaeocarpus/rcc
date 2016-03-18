/*
 * timer.h
 */
#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f103xb.h"

// prescaler values
#define PSC_1MHz		31
#define PSC_2MHz		15
#define PSC_4MHz		7
#define PSC_8MHz		3
#define PSC_16MHz		1
#define PSC_32MHz		0
#define PSC_VAL			PSC_32MHz
// CCR modes
#define OCM_FROZEN		0
#define OCM_PWM_MODE_1	6
#define OCM_PWM_MODE_2	7
// clock frequencies
#define CK_INT			32000000					// internal clock 32MHz
#define CK_CNT			(CK_INT / (PSC_VAL + 1))	// counter clock
#define PWM_FREQ		62500						// 62.5kHz


void timer2_init(void);
void timer3_init(void);

//uint16_t TIM_ReadConfigReg(TIM_TypeDef *timer, uint32_t reg);

#endif
