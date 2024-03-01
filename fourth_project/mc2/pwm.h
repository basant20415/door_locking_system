/*
 * pwm.h
 *
 *  Created on: Oct 9, 2023
 *      Author: bassant
 */

#ifndef PWM_H_
#define PWM_H_

#include"std_types.h"/*To Use uint8*/
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for initialize the PWM driver.
 */
void PWM_Timer0_Start(uint8 duty_cycle);
#endif /* PWM_H_ */
