/*
 * motor.h
 *
 *  Created on: Oct 10, 2023
 *      Author: bassant
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "gpio.h"
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
#define MOTOR_PORT PORTB_ID
#define MOTOR_FIRST_PIN PIN0_ID
#define MOTOR_SECOND_PIN PIN1_ID

typedef enum{
	stop=0xFC,ANTI_CLOCKWISE=0xFD,CLOCKWISE=0xFE
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for initialize the DC_motor driver.
 */
void DcMotor_Init(void);
/*
 * Description :
 * Function responsible for Taking The State And Speed As AN Input And Adjust The Motor According To Them
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* MOTOR_H_ */
