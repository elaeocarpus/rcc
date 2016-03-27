/*
 * pwm.c
 *
 *  Created on: 9 Mar 2016
 *      Author: vince
 */


#include "pwm.h"
#include "timer.h"


// ---- External functions ----

void delay(int t);



// ---- Local functions ----

void pwm_fwd(uint32_t pwm);
void pwm_rev(uint32_t pwm);



#define PWM_MAX	512
#define PWM_MIN -512


// ---- Private variables ----

static int curr_pwm;





/*
 * set motor speed
 * speed is a number from -maxspeed .. maxspeed)
 * + indicates forwards, - indicates reverse
 */
void pwm_set_speed(int speed)
{
	uint16_t maxspeed = PWM_S_MAX;
	//uint16_t maxspeed = 512;

	if (speed >= 0)
	{
		if (speed > maxspeed)
		{
			speed = maxspeed;
		}
		//PWM_S_FWD &= ~TIM_CCR1_CCR1;
		PWM_S_FWD = (uint16_t)speed;		// load CCR1 with speed -> MD_FORWARD output goes HIGH for part of cycle
		PWM_S_REV = 0x0000;					// clear CCR2 -> MD_REVERSE output goes LOW for full cycle
	}
	else
	{
		if (speed < -maxspeed)
		{
			speed = -maxspeed;
		}
		PWM_S_FWD = 0x0000;				// clear CCR1 -> MD_FORWARD output goes LOW for full cycle
		//PWM_S_REV &= ~TIM_CCR2_CCR2;
		PWM_S_REV = (uint16_t)(-speed);	// load CCR2 with |speed| -> MD_REVERSE output goes HIGH for part of cycle

	}
}


void pwm_brake(void)
{
	PWM_S_FWD = 0xFFFF;			// set CCR1 and CCR2
	PWM_S_REV = 0xFFFF;			// MD_FORWARD and MD_REVERSE both go HIGH for full cycle
}


void pwm_idle(void)
{
	PWM_S_FWD = 0x0000;			// clear CCR1 and CCR2
	PWM_S_REV = 0x0000;			// MD_FORWARD and MD_REVERSE both go LOW for full cycle
}


int pwm_ramp_speed(int s_initial, int s_final, int ramp_time)
{
	int s_current = s_initial;
	int update_rate = 10;		// in milliseconds
	int s_increment = (s_final - s_initial) / (ramp_time / update_rate);

	if (s_initial < s_final)
	{
		if (s_increment == 0)
		{
			s_increment = 1;
			update_rate = ramp_time / (s_final - s_initial) /  s_increment;
		}
		while (s_current <= s_final)
		{
			pwm_set_speed(s_current);
			s_current += s_increment;
			delay(update_rate);
		}
	}
	else
	{
		if (s_increment == 0)
		{
			s_increment = -1;
			update_rate = ramp_time / ( s_final - s_initial) /  s_increment;
		}
		while (s_current >= s_final)
		{
			pwm_set_speed(s_current);
			s_current += s_increment;
			delay(update_rate);
		}
	}
	return s_current;
}



/* pwm_get_speed
 *
 * Returns the current speed value
. */
int pwm_get_speed(void)
{
	return curr_pwm;
}

/*
int pwm_get_speed(void)
{
	uint16_t s_fwd = PWM_S_FWD;
	uint16_t s_rev = PWM_S_REV;
	int speed;

	if (s_rev)
	{
		speed = -s_rev;	// i.e. if s_rev is non-zero then it is the speed and is -ve
	}
	else
	{
		speed = s_fwd;	// otherwise s_fwd is the speed and is +ve
	}
	return speed;
}
*/

void pwm_change_speed(int new_speed, int ramp_time)
{
	int current_speed = pwm_get_speed();

	pwm_ramp_speed(current_speed, new_speed, ramp_time);

	return;
}


/* pwm_fwd
 *
 * Set counter/compare register for PWM forward output.
 */
void pwm_fwd(uint32_t pwm)
{
	TIM4->CCR3 = pwm;
}


/* pwm_rev
 *
 * Set counter/compare register for PWM reverse output.
 */
void pwm_rev(uint32_t pwm)
{
	TIM4->CCR4 = pwm;
}


/* pwm_out
 *
 * pwm		Signed PWM value.
 * 			Range -512 to + 512
 *
 * Sets PWM output.
 * Sign indicates direction.
 * Directly sets the PWM output without any ramp up or down.
 */
void pwm_out(int pwm)
{
	uint32_t pwm_value;


	if(pwm > PWM_MAX)
		pwm = PWM_MAX;
	if(pwm < PWM_MIN)
		pwm = PWM_MIN;

	curr_pwm = pwm;

	if(pwm < 0)
	{
		pwm_value = (uint32_t)(-pwm);

		pwm_fwd(0);
		pwm_rev(pwm_value);

	}
	else
	{
		pwm_value = (uint32_t)pwm;

		pwm_fwd(pwm_value);
		pwm_rev(0);

	}

}



/* pwm_incr
 *
 * Parameters
 * pwm_step		PWM increment amount
 *
 * Increment PWM by the amount in pwm_step.
 * Returns the actual current PWM value.
 *
 */
int pwm_incr(int pwm_step)
{

	curr_pwm += pwm_step;

	if(curr_pwm > PWM_MAX)
		curr_pwm = PWM_MAX;
	if(curr_pwm < PWM_MIN)
		curr_pwm = PWM_MIN;

	pwm_out(curr_pwm);

	return curr_pwm;
}
