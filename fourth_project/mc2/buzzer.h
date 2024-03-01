/*
 * buzzer.h
 *
 *  Created on: Oct 27, 2023
 *      Author: bassant
 */

#ifndef BUZZER_H_
#define BUZZER_H_
#include"std_types.h"
#include"gpio.h"
/*******************************************************************************
 *                      Buzzer Macros                                          *
 *******************************************************************************/
#define BUZZER_PORT PORTA_ID/*set buzzer port to PORTA*/
#define BUZZER_PIN PIN0_ID/*set the buzzer pin to pin0*/

/*******************************************************************************
 *                     Functions Description                                          *
 *******************************************************************************/
/*
 * description:
 * initialize the buzzer by setting its port and pin
 */
void Buzzer_init();
/*
 * description:
 * open the buzzer by setting its pin logic high
 */
void Buzzer_on(void);
/*
 * description:
 * close the buzzer by setting its pin logic low
 */
void Buzzer_off(void);
#endif /* BUZZER_H_ */
