/*
 * motor.c
 *
 *  Created on: Oct 10, 2023
 *      Author: bassant
 */

#include"gpio.h"/*To Use The Prototypes Of Functions Like GPIO_setupPinDirection,GPIO_writePort*/
#include"std_types.h"/*To Use uint8*/
#include"motor.h"/*To Use The Structure And The Enums*/
#include"pwm.h"/*For Sending The Speed Of The Motor The PWM_driver*/

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void DcMotor_Init(void){
	GPIO_setupPinDirection(MOTOR_PORT,MOTOR_FIRST_PIN,PIN_OUTPUT);/*Setup The Direction Of The First Motor Pin*/
	GPIO_setupPinDirection(MOTOR_PORT,MOTOR_SECOND_PIN,PIN_OUTPUT);/*Setup The Direction Of The Second Motor Pin*/
	GPIO_writePort(MOTOR_PORT,0xFC);/*Stop The DC_motor At The Beggining By Clearing The First Two Bits In PORTB*/
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	PWM_Timer0_Start(speed);/*We Send The Speed To The PWM_driver To Make In It Some Calculations And Put The Result in OCR0 To Generate PWM Signal In OC0*/
	if(state==CLOCKWISE){
		GPIO_writePort(MOTOR_PORT,0xFE);/*PB0=0,PB1=1 To Rotate The Motor ClockWise*/

	}else if(state==ANTI_CLOCKWISE){
		GPIO_writePort(MOTOR_PORT,0xFD);/*PB0=1,PB1=0 To Rotate The Motor AntiClockWise*/

	}else{
		GPIO_writePort(MOTOR_PORT,0xFC);/*PB0 And PB1 =0 To Stop The Motor*/

	}

}

