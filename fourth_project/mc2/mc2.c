
/*
 * mc2.c
 *
 *  Created on: Oct 27, 2023
 *      Author: bassant
 */
#include "uart.h"/*for sending and receiving between the two microcontrollers*/
#include "twi.h"/*for the external eeprom*/
#include "external_eeprom.h"/*for using it to save passwords*/
#include "timer1_driver.h"/*to initialize the timer*/
#include "motor.h"/*to initialize the motor*/
#include "buzzer.h"/*to initialize the buzzer*/
#include <avr/io.h>/*for SREG register*/
#include<util/delay.h>/*for using delay function*/

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define EEPROM_ID			0x02/*eeprom start address*/
#define UNLOCKING_DOOR_DURATION  			15/*the door will be unlocked for 15 seconds*/
#define OPENING_DOOR_DURATION 		 			3/*the door will be opened for 3 seconds*/
#define LOCKING_DOOR_DURATION    			15/*the door will be locked for 15 seconds*/
#define NUMBER_OF_WRONG_PASSWORDS 	3/*the wrong passwords allowed to be entered are 3 then buzzer starts*/
#define BUZZER_ALARM			 			60/*the buzzer will be on for 1 min*/

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static uint8 volatile g_tick = 0;		/* to count Ticks */

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 *callback function every entering to the ISR the ISR calls this function to increment the ticks
 */
void Timer_callBack(void);

int main(void)
{
	uint8 password1[6]={0};/*array to take the first password*/
	uint8 password2[6]={0};/*array to take the second password*/
	uint8 counter=0;/*counter for getting the password*/
	uint8 flag=0;/*flag to check if the two passwords are equal*/
	uint8 command;/*we use it later in options to check if the command is + or -*/
	uint8 error=0;/*a flag to check if the two passwords are equal or not*/
	uint8 no_of_errors=0;/*counter to count the number of unequal wrong passwords */

	SREG |= (1<<7);/*enable the I_bit*/

	UART_ConfigType UART_Config = {Eight_Bit,Disabled,One_Bit,9600};/*uart configurations*/
	TWI_ConfigType TWI_Config = {0x01,0x02};/*twi configurations*/
	Timer1_ConfigType Timer1_Config = {0,31250,CLOCK_256,COMPARE};
	/*Ttimer=(256/(8*10^6))=3.2*(10^(-5)) ,then the timer will count 1 count every 3.2*(10^(-5)) so in 1s i will count 31250 counts*/

	Timer1_setCallBack(Timer_callBack);/*set the call back function*/

	DcMotor_Init();/*initialize the motor*/
	Buzzer_init();/*initialize the buzzzer*/
	UART_init(&UART_Config);/*initialize the uart*/
	TWI_init(&TWI_Config);/*initialize the uart*/
	while(1)
	{
		while(UART_recieveByte()!=MC_READY){}/*wait till the mc is ready to receive*/
		UART_receiveString(password1);/*take the first password*/
		UART_receiveString(password2);/*take the second password*/
		counter=0;/*set the counter to zero for every loop*/
		flag=0;/*set the flag to zero for every loop*/
		while(password1[counter]!='\0')
		{
			if(password1[counter]!=password2[counter])/*if the two taken passwords are not equal set the flag to 1*/
			{
				flag=1;
				break;
			}
			counter++;/*increment thecounter for the next number*/
		}
		if(1==flag)/*if the flag =1 then send to the another mc that the two passwords are not equal*/
		{

			UART_sendByte(MC_READY);
			UART_sendByte(WRONG_PASSWORDS);
		}
		else
		{
			/*if the two taken passwords are equal then send to the another mc that the two paswords are equal to take actions according to it*/
			UART_sendByte(MC_READY);
			UART_sendByte(ACCEPTED_PASSWORDS);
			counter=0; /*reintialize the counter for the second step:options*/
			do
			{
				_delay_ms(10);/*we use delay in eeprom to take its time for reading and writing*/
				EEPROM_writeByte(EEPROM_ID+counter,password1[counter]);
				/*write the taken number from the password into eeprom address
				 *  and every time incremnt the counter
				 * to take the next number in the password and eeprom address
				 */

				counter++;/*increment the counter for the next number*/
			}while(password1[counter-1]!='\0');
			while(1)/*the next step : options*/
			{
				while(UART_recieveByte()!=MC_READY){}/*wait till the mc is ready to receive*/
				UART_receiveString(password1);/*take the password to compare it with the one saved in the eeprom*/
				command=UART_recieveByte();/*receive the command either + or -*/
				counter=0;/*reinitialize the counter to check the password with the one saved in the eeprom*/
				error=0;/*reinitialize the error flag to check the password */
				while(password1[counter]!='\0')
				{
					_delay_ms(10);/*delay to make the eeprom take its time for reading and writing*/
					EEPROM_readByte(EEPROM_ID+counter,password2+counter);
					_delay_ms(10);
					if(password1[counter]!=password2[counter])
						/*if a number in the first password not equal the number in the second password
					     then set the flag to 1 and break from the loop
						 */
					{
						error=1;
						break;
					}
					counter++;/*increment the counter for checking the next numbers*/
				}
				if(1==error)/*if the two passwords are not equal then take the following actions*/
				{

					UART_sendByte(MC_READY);/*check that mc1 is ready to receive*/
					UART_sendByte(ERROR_NOT_DETECTED);/*tell mc1 that there is an error*/
					no_of_errors++;/*increment the number of errors*/
					if(NUMBER_OF_WRONG_PASSWORDS==no_of_errors)
					{/*if the number of errors reached 3 then initialize the buzzer*/
						UART_sendByte(MC_READY);/*check that mc1 is ready*/
						UART_sendByte(BUZZER_ERROR);/*send to mc1 that there is a buzzer error*/
						Timer1_init(&Timer1_Config);/*initialize the timer*/
						Buzzer_on();/*turn on the buzzer*/

						while(g_tick!=BUZZER_ALARM);/*the tick is incremented in the call back function every interrupt so
					when ticks reaches 30s stop the buzzer and the timer*/
						g_tick=0;/*reinitialize the tick*/
						Buzzer_off();/*turn off the buzzer*/
						Timer1_deInit();/*turn off the timer*/
						no_of_errors=0;/*reinitialize the number of errors*/
					}else
					{/*if there is error and its not the third try then till the other mc that there is an error only   */
						UART_sendByte(MC_READY);/*make sure the other mc is ready to receive*/
						UART_sendByte(ERROR_NOT_DETECTED);
					}
				}
				else
				{/*if the passwords are the same*/
					no_of_errors=0;/*reinitialze the number of errors*/

					UART_sendByte(MC_READY);/*make sure that the other mc is ready to receive*/
					UART_sendByte(ACCEPTED_PASSWORDS);/*send to the other mc that the two passwords are the same to take actions according to it*/
					command=UART_recieveByte();/*receive command if its + or -*/
					if(command=='+')
					{
						DcMotor_Rotate(CLOCKWISE,100);/*rotate the motor clockwise and initialize the timer*/
						Timer1_init(&Timer1_Config);
						while(1)
						{
							if(g_tick==UNLOCKING_DOOR_DURATION)/*if the time reaches 15s then reinitialize the ticks */
							{
								g_tick=0;
								break;
							}
						}
						DcMotor_Rotate(stop,0);/*stop the motor for 3s*/
						while(1)
						{
							if(g_tick==OPENING_DOOR_DURATION)/*when the time reaches 3s then reinitialize the ticks*/
							{
								g_tick=0;
								break;
							}
						}
						DcMotor_Rotate(ANTI_CLOCKWISE,100);/*rotate the motor anticlockwise for 15s*/
						while(1)
						{
							if(g_tick==LOCKING_DOOR_DURATION)/*when the time reaches 15s then reinitialize the ticks*/

							{
								g_tick=0;
								break;
							}
						}
						DcMotor_Rotate(stop,0);/*stop the motor*/
						Timer1_deInit();/*stop the timer*/
					}
					else if(command=='-')/*if the command is - then break from the current loop and go to the biggger loop to take the two passwords again*/
					{
						break;
					}
				}
			}
		}
	}
}

/*
 * Description :
 *callback function every entering to the ISR the ISR calls this function to increment the ticks
 */

void Timer_callBack(void)
{
	/* Increment ticks */
	g_tick++;
	/* clear time counted */
	TCNT1=0;
}
