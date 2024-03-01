/*
 * buzzer.c
 *
 *  Created on: Oct 27, 2023
 *      Author: bassant
 */

#include"buzzer.h"/*used for the defined macros in the .h file*/
#include"gpio.h"/*used for the pins and ports*/
/*
 * description:
 * initialize the buzzer by setting its port and pin
 */
void Buzzer_init(){
	GPIO_setupPinDirection(BUZZER_PORT,BUZZER_PIN,PIN_OUTPUT);/*setup the buzzer pin as an output pin*/
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);/*stop the buzzer at first*/
}
/*
 * description:
 * open the buzzer by setting its pin logic high
 */
void Buzzer_on(void){

	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_HIGH);/*open the buzzer*/

}
/*
 * description:
 * close the buzzer by setting its pin logic low
 */
void Buzzer_off(void){
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);/*stop the buzzer*/
}
