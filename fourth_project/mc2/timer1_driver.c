/*
 * timer1_driver.c
 *
 *  Created on: Oct 27, 2023
 *      Author: bassant
 */
#include"timer1_driver.h"
#include<avr/io.h>
#include <avr/interrupt.h>
/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
static volatile void (*g_timer1CallBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                                    ISR                                      *
 *******************************************************************************/


ISR(TIMER1_OVF_vect){
	if(g_timer1CallBackPtr != NULL_PTR){
		(*g_timer1CallBackPtr)();/*call the call back function when the time reaches overflow value*/
	}

}

ISR(TIMER1_COMPA_vect){
	if(g_timer1CallBackPtr != NULL_PTR){
		(*g_timer1CallBackPtr)();/*call the call back function when the time reaches overflow value*/
	}
}

/*******************************************************************************
 *                      Function Descriptions                                  *
 *******************************************************************************/
void Timer1_init(const Timer1_ConfigType * Config_Ptr){
	/*
	 * TCCR1A description:
	 * FOC1A,FOC1B are set when working with modes other than pwm mode
	 * WGM10,WGM11 are set based on the required mode
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1A=(TCCR1A&0xFC)|((Config_Ptr->mode)&0x03);/*setting the mode normal or compare by setting the first two bits*/
	TCNT1=Config_Ptr->initial_value;/*for setting the intial value*/
	/*
	 * TCCR1B description:
	 * CS12,CS11,CS10 are set based on the required prescalar
	 * WGM12,WGM13 are set based on the required mode
	 */
	TCCR1B=(TCCR1B&0XF8)|(Config_Ptr->prescaler);
	TCCR1B=(TCCR1B&0xE7)|(((Config_Ptr->mode)&0x0C)<<1);
#ifdef CTC_MODE
	/*
	 * if its compare mode then put the compare value in OCR1A and enable timer1 compare interrupt
	 */
	OCR1A = Config_Ptr->compare_value;
	TIMSK|=(1<<OCIE1A);
#elif OVF_MODE
	/*if the mode is overflow then enable timer1 overflow interrupt*/
	TIMSK|=(1<<TOIE1);
#endif
}
/*
 * description:
 * stop the timer by setting all the registers to zero and disable interrupts and make the call back function point to null
 */
void Timer1_deInit(void){
	TCCR1A=0;
	TCCR1B=0;
	TCNT1=0;
	TIMSK&=~(1<<OCIE1A);
	TIMSK&=~(1<<TOIE1);
	g_timer1CallBackPtr = NULL_PTR;
}
/*
 * description:make the callback pointer points to given function
 */
void Timer1_setCallBack(void(*a_ptr)(void)){
	g_timer1CallBackPtr = a_ptr;
}





