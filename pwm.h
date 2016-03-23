/*
 * pwm.h
 *
 *  Created on: 9 Mar 2016
 *      Author: vince
 */

#ifndef PWM_H_
#define PWM_H_

#include "stm32f103xb.h"

// read timer registers
//#define PWM_S_FWD 	(TIM2->CCR1)
//#define PWM_S_REV 	(TIM2->CCR2)
#define PWM_S_FWD 	(TIM2->CCR3)
#define PWM_S_REV 	(TIM2->CCR4)
#define PWM_S_MAX	(TIM2->ARR)

void pwm_set_speed(int speed);
void pwm_brake(void);
void pwm_idle(void);
int pwm_ramp_speed(int s_initial, int s_final, int ramp_time);
int pwm_get_speed(void);
void pwm_change_speed(int new_speed, int ramp_time);

#endif /* PWM_H_ */
