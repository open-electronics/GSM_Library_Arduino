/*********************************************************************
 *
 *       Https command for GSM
 *
 *********************************************************************
 * FileName:        HttpCmd_GSM.cpp
 * Revision:        1.0.0
 * Date:			04/02/2018
 * Dependencies:	HttpCmd_GSM.h
 * Arduino Board:	Arduino Uno, Arduino Mega 2560, Fishino Uno, Fishino Mega 2560       
 *
 * Company:         Futura Group srl
 *  				www.Futurashop.it
 *  				www.open-electronics.org
 *
 * Developer:		Destro Matteo
 *
 * Support:			info@open-electronics.org
 * 
 * Software License Agreement
 *
 * Copyright (c) 2018, Futura Group srl 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************/

#include "HttpCmd_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//-----------------------------------------------------
//	Public Function

#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)

#endif

#ifdef FIBOCOM_G510

#endif

#ifdef QUECTEL_M95

#endif

//-----------------------------------------------------
//	This function is used to verify if PIN is required
//	The syntax of command is AT+CPIN? and the answer is +CPIN:<code>
//	For details see AT commands datasheet
// uint8_t HttpCmd_GSM::SetCmd_ATQ_CPIN(void) {
	// if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		// return(0);		//	System Busy
	// } else {
		// Gsm.ClearBuffer();
		// Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		// Gsm.ResetFlags();
		// Gsm.ReadStringFLASH((uint8_t *)ATQ_CPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CPIN));
		// Gsm.WritePointer = strlen(ATQ_CPIN);			
		// Gsm.StartSendData(CMD_HTTP_IDLE, WAIT_ANSWER_CMD_ATQ_CPIN, ANSWER_HTTP_AT_CMD_STATE);
	// }
	// return(1);			//	Command sent
// }
//-----------------------------------------------------


//-----------------------------------------------------
void HttpCmd_GSM::HttpWaitAnswer(void) {
	uint8_t StrPointer = 0xFF;
	uint8_t StrPointerEnd = 0xFF;

	if ((Gsm.StateSendCmd != CMD_HTTP_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
		return;
	}
	
	if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) {
		return;
	}
	Gsm.UartFlag.Bit.ReceivedAnswer = 0;
	if (Gsm.GsmFlag.Bit.CringOccurred == 1) {
		//	CRING OCCURRED. CMD SEND ABORTED
		Gsm.RetryCounter = 0;
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
		Gsm.StateWaitAnswerCmd = CMD_HTTP_IDLE;
		return;
	}
	
	if (Gsm.ReadPointer > 0) {
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK)) != 0xFF) {
			Gsm.RetryCounter = 0;
			Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
					
			switch (Gsm.StateWaitAnswerCmd)
			{
				case CMD_HTTP_IDLE:
					break;

					
				default:
					break;	
			}
		} else {
			Gsm.ProcessGsmError();
			switch (Gsm.StateWaitAnswerCmd)
			{
					
				default:
					HttpRetrySendCmd();
					break;
			}
		}				
	} else {
		Gsm.InitReset_GSM();
	}
}	

void HttpCmd_GSM::HttpRetrySendCmd(void) {
	if (Gsm.RetryCounter++ < 2) {
		switch (Gsm.StateWaitAnswerCmd)
		{
			//case WAIT_ANSWER_CMD_ATQ_CPIN:
				//Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				//SetCmd_ATQ_CPIN();
				break;
				
			default:
				break;
		}
	} else {
		Gsm.InitReset_GSM();
	}
}
//-----------------------------------------------------

//-----------------------------------------------------
//	Private functions

//-----------------------------------------------------
