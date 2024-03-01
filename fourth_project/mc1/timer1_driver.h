/*
 * timer1_driver.h
 *
 *  Created on: Oct 27, 2023
 *      Author: bassant
 */

#ifndef TIMER1_DRIVER_H_
#define TIMER1_DRIVER_H_
#include"std_types.h"
#define CTC_MODE

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
typedef enum{
	NORMAL,COMPARE=4
}Timer1_Mode;

typedef enum{
	NO_CLOCK,CLOCK_1,CLOCK_8,CLOCK_64,CLOCK_256,CLOCK_1024
}Timer1_Prescaler;

typedef struct {
uint16 initial_value;
#ifdef CTC_MODE
uint16 compare_value; // it will be used in compare mode only.
#endif
Timer1_Prescaler prescaler;
Timer1_Mode mode;
} Timer1_ConfigType;


/*******************************************************************************
 *                                Function Prototypes                          *
 *******************************************************************************/
/*
 * description:
 * stop the timer by setting all the registers to zero and disable interrupts and make the call back function point to null
 */

void Timer1_deInit(void);

/*
 * description:make the callback pointer points to given function
 */
 void Timer1_setCallBack(void(*a_ptr)(void));

 /*description:
  * initialize the timer by setting the required mode and prescalar
  */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

#endif /* TIMER1_DRIVER_H_ */
