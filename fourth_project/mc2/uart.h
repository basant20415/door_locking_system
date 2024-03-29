/*
 * uart.h
 *
 *  Created on: Oct 25, 2023
 *      Author: bassant
 */

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MC_READY 0xFF
#define ACCEPTED_PASSWORDS 0X01
#define WRONG_PASSWORDS   0X00
#define BUZZER_ERROR 0xFE
#define  ERROR_NOT_DETECTED 0xAC
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	Disabled,Even_Parity=2,Odd_Parity=3
}UART_Parity;

typedef enum{
	One_Bit,Two_Bit
}UART_StopBit;

typedef enum{
	Five_Bit,Six_Bit,Seven_Bit,Eight_Bit,Nine_Bit=7
}UART_BitData;

typedef struct{
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stop_bit;
	uint32 baud_rate;
}UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame : number of data bits, parity bit type , number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *Config_ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
