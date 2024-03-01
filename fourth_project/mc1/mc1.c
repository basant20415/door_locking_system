/*
 * mc1.c
 *
 *  Created on: Oct 27, 2023
 *      Author: bassant
 */

#include "lcd.h"/*for displaying on the lcd*/
#include "keypad.h"/*for displaying the keypad*/
#include "uart.h"/*for sending and receiving between the two microcontrollers*/
#include "timer1_driver.h"/*for intializing the timer*/
#include <avr/io.h>/*to intialize the sreg register*/
#include <util/delay.h>/*for using delay function*/

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define EEPROM_ID			0x02/*eeprom start address*/
#define UNLOCKING_DOOR_DURATION  			15/*the door will be unlocked for 15 seconds*/
#define OPENING_DOOR_DURATION 		 		3/*the door will be opened for 3 seconds*/
#define LOCKING_DOOR_DURATION    			15/*the door will be locked for 15 seconds*/
#define NUMBER_OF_WRONG_PASSWORDS        	3/*the wrong passwords allowed to be entered are 3 then buzzer starts*/
#define BUZZER_ALARM			 			60/*the buzzer will be on for 1 min*/
#define ZERO_ASCII_CODE			        	48/*ascii code for zero*/
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static uint8 volatile g_tick = 0;		/* global variable to count Ticks */

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 *function to take two passwords from user and chaeck they are equal or not
 *if they are not the function will repeat asking the user for entering two similar passwords
 */

void password_confirmation(void);

/*
 * Description :
 *after taking two similar passwords from user there are two option either open door or check password
 */
void options(void);
/*
 * Description :
 *callback function every entering to the ISR the ISR calls this function to increment the ticks
 */

void Timer_callBack(void);

int main(void)
{

	SREG |= (1<<7);/*enable the I_bit*/

	UART_ConfigType UART_Config = {Eight_Bit,Disabled,One_Bit,9600};/*to setup the uart configurations*/

	Timer1_setCallBack(Timer_callBack);/*set the calback function*/

	LCD_init();/*intialize the lcd driver*/
	UART_init(&UART_Config);/*intialize the uart driver*/
	password_confirmation();/*step one:check the entered two passwords*/
	options();/*step two: options*/
	while(1)
	{

	}
}

/*step one*/
void password_confirmation(void)
{
	uint8 key;/*variable to take numbers entered by the keypad*/
	uint8 password1[7]={0};/*array to take the first password*/
	uint8 password2[7]={0};/*array to take the second password*/
	uint8 counter=0;/*variable to count number of numbers taken from keypad to enter them in array*/

	while(1)
	{
		LCD_clearScreen();/*first we clear screen*/
		LCD_displayStringRowColumn(0,0,"plz enter pass: ");/*diaplay this string in row and column 0*/
		LCD_moveCursor(1,0);/*move the cursor to the second row*/

		for(counter=0;counter<6;)
		{

			key=KEYPAD_getPressedKey();/* get the pressed key value */
			if((key>=0)&&(key<=9)&&(counter!=5))
			{
				password1[counter]=key+ZERO_ASCII_CODE;
				/*since we use uart_sendstring
				 *so we have to send the asci of the number
				 *so and store the number in the array
				 */
				LCD_displayCharacter('*');/* display '*' every digit in password */
				counter++;/*increment the counter*/
			}
			else if((key=='=')&&(counter==5))
			{
				password1[counter]='#';/*'#' used to tell the uart the i've finished sending the password*/
				counter++;/*increment the counter to go out of the for loop*/
			}
			_delay_ms(500);/* delay to get the next key */
		}
		LCD_clearScreen();/*clear the screen*/
		LCD_displayStringRowColumn(0,0,"plz re-enter the");/*display the string in row and column 0*/
		LCD_displayStringRowColumn(1,0,"same pass: ");/*display the string in row 1*/

		for(counter=0;counter<6;)
		{
			key=KEYPAD_getPressedKey();/*get the pressed key value*/
			if((key>=0)&&(key<=9)&&(counter!=5))
			{
				password2[counter]=key+ZERO_ASCII_CODE;
				/*since we use uart_sendstring
				 *so we have to send the asci of the number
				 *so and store the number in the array
				 */
				LCD_displayCharacter('*');/*display * instead of number as its a password*/
				counter++;/*increment the counter*/
			}
			else if((key=='=')&&(counter==5))
			{
				password2[counter]='#';/*put '#' at the end of the arrey to tell the uart that i've finished sending */

				counter++;/*increment thecounter to go out of the for loop*/
			}
			_delay_ms(500);/*delay to take the second key*/
		}
		UART_sendByte(MC_READY);/*send to the second mc to make sure that its ready to receive*/
		UART_sendString(password1);/*send the first passsword*/
		UART_sendString(password2);/*send the second passsword*/
		while(UART_recieveByte()!=MC_READY){}/*mc2 will check the two passwords and send to mc1 if they are same or not*/
		if(UART_recieveByte()==ACCEPTED_PASSWORDS)
		{
			break;
			/*if the two passwords are the same so we will go to the second step:options
			 *else it will repeat the while loop
			 */
		}
	}
}

/*second step*/
void options(void)
{
	uint8 key;/*variable to get the pressed key*/
	uint8 password[7];/*array to get the password*/
	uint8 counter=0;/*counter to increment the the number of numbers taken by the keypad*/

	Timer1_ConfigType Timer1_Config = {0,31250,CLOCK_256,COMPARE};
	/*Ttimer=(256/(8*10^6))=3.2*(10^(-5)) ,then the timer will count 1 count every 3.2*(10^(-5)) so in 1s i will count 31250 counts*/

	while(1)
	{
		LCD_clearScreen();/*command to clear screen*/
		LCD_displayStringRowColumn(0,0,"+ : Open Door");/*display this string in row and column 0*/
		LCD_displayStringRowColumn(1,0,"- : Change Pass");/*dislpay this string in the second row*/
		do
		{
			key=KEYPAD_getPressedKey();/*keep looping in this do while loop till the pressed key is + or -*/
		}while(!((key=='+')||(key=='-')));
		if(key=='+')/*if the pressed key is '+'*/
		{
			while(1){

				LCD_clearScreen();/*clear the screen*/
				LCD_displayStringRowColumn(0,0,"plz enter pass: ");/*display this string in row and column 0*/
				LCD_moveCursor(1,0);/*move to the second row*/
				for(counter=0;counter<6;)/*first we get the password*/
				{
					key=KEYPAD_getPressedKey();/*get the pressed key*/
					if((key>=0)&&(key<=9)&&(counter!=5))
					{
						password[counter]=key+ZERO_ASCII_CODE;/*put the taken number in the array */
						LCD_displayCharacter('*');/*display * instead of the number in the screen*/
						counter++;/*increment the counter to take the another number till the counter is equal 5*/
					}
					else if((key=='=')&&(counter==5))
					{
						password[counter]='#';/*# is put to till mc2 that the password is finished*/

						counter++;/*increment the counter to go out of the for loop*/
					}

					_delay_ms(500);/*delay to get the second key*/
				}
				UART_sendByte(MC_READY);/*we use this to check if the second mc2 is ready to accept the password*/

				UART_sendString(password);/*we send th password to mc2 to compare it with the one saved in the eeprom*/

				UART_sendByte('+');/*we send this to tell mc2 that the command is + to take actions according to it*/
				while(UART_recieveByte()!=MC_READY){}/*to check if mc is ready to receive */

				if(UART_recieveByte()==ACCEPTED_PASSWORDS)
				{
					/*if the two passwords are the same so we will display on the screen
					 * 'door is unlocking for 15s then 'open ' for 3s then 'door is locking for 15s
					 */
					LCD_clearScreen();/*command to clear the screen*/
					LCD_displayStringRowColumn(0,0,"Door is Unlocking");
					Timer1_init(&Timer1_Config);/*start the timer*/
					while(1)
					{
						if(g_tick==UNLOCKING_DOOR_DURATION)
						{/*when the timer reaches 15s then reintialize the ticks,and go out of the while loop*/
							g_tick=0;
							break;
						}
					}
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Door is Open");
					while(1)
					{/*when the timer reaches 3s then reintialize the ticks,and go out of the while loop*/
						if(g_tick==OPENING_DOOR_DURATION)
						{
							g_tick=0;
							break;
						}
					}
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Door is Locking");
					while(1)
					{/*when the timer reaches 15s then reintialize the ticks,and go out of the while loop*/
						if(g_tick==LOCKING_DOOR_DURATION)
						{
							g_tick=0;
							break;
						}
					}
					Timer1_deInit();		/* stop the timer */
					break;
				}
				else
				{/*if the received message from mc2 is buzzer error so we will take the following actions:
				 * display 'error' message on the screen and start the timer till it reaches 1min then
				 * stop the timer
				 */

					while(UART_recieveByte()!=MC_READY){}
					if(UART_recieveByte()==BUZZER_ERROR)
					{
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"ERROR");
						Timer1_init(&Timer1_Config);
						while(1)
						{
							if(g_tick==BUZZER_ALARM)
							{
								g_tick=0;
								break;
							}
						}
						Timer1_deInit();		/* stop the timer */
						break;
					}
				}
			}
		}
		else if(key=='-')
		{
			while(1)
			{
				LCD_clearScreen();/*command to clear the screen*/
				LCD_displayStringRowColumn(0,0,"plz enter pass: ");/*display this message in the first row and column*/
				LCD_moveCursor(1,0);/*move the cursor to the second row*/
				for(counter=0;counter<6;)
				{
					key=KEYPAD_getPressedKey();/*get the key pressed*/
					if((key>=0)&&(key<=9)&&(counter!=5))
					{
						password[counter]=key+ZERO_ASCII_CODE;/*take the asci of the number and put it in the array*/
						LCD_displayCharacter('*');/*display * instead of the number*/
						counter++;/*increment the counter*/
					}
					else if((key=='=')&&(counter==5))
					{
						password[counter]='#';/*put '#' at the end of the password to tell mc2 that i've finished sending*/


						counter++;/*increment the counter to go out of the loop*/
					}
					_delay_ms(500);/*delay to get the second key*/
				}
				UART_sendByte(MC_READY);/*to check if the second mc is ready to receive the password*/
				UART_sendString(password);/*send the password*/
				UART_sendByte('-');/*send '-' to mc2 to take actions according to it*/
				while(UART_recieveByte()!=MC_READY){}
				if(UART_recieveByte()==ACCEPTED_PASSWORDS)/*if the two passwords are similar then
				go to password confirmation function to put new password*/
				{
					password_confirmation();
					break;
				}
				else
				{
					/*if the two passwords are not equal then mc2 will check if we reached the allowed number of tries then
					 * we will take buzzer action else we will begin the while loop we are in again*/
					while(UART_recieveByte()!=MC_READY){}
					if(UART_recieveByte()==BUZZER_ERROR)
					{
						LCD_clearScreen();/*command to clear the screen*/
						LCD_displayStringRowColumn(0,0,"ERROR");/*command to display this message in the first row and column*/
						Timer1_init(&Timer1_Config);/*start the timer*/
						while(1)
						{
							if(g_tick==BUZZER_ALARM)/*if we reached 60 min then reinitialize the ticks*/
							{
								g_tick=0;
								break;
							}
						}
						Timer1_deInit();/*stop the timer*/
						break;
					}
				}
			}
		}
	}
}


void Timer_callBack(void)
{
	/* Increment ticks */
	g_tick++;
	/* clear time counted */
	TCNT1=0;
}
