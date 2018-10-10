/*********************************************************************
 *
 *       Https command for GSM
 *
 *********************************************************************
 * FileName:        HttpCmd_GSM.h
 * Revision:        1.0.0
 * Date:			04/02/2018
 * Dependencies:	Arduino.h
 *					Uart_Gsm.h
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

#ifndef _HttpCmd_GSM_H_INCLUDED__
#define _HttpCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
//const char AT_CPIN[]     PROGMEM = "AT+CPIN=";							//	Command - Set PIN (Add via code pin and \r\n char)

#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)

#endif
#ifdef FIBOCOM_G510

#endif
#ifdef QUECTEL_M95

#endif

//======================================================================

//======================================================================
//	AT COMMAND ANSWER - STORED IN FLASH MEMORY

//const char AT_ANSW_CLCK[]			PROGMEM = "+CLCK:";					// Key word for answer at command CLCK

//======================================================================

//======================================================================
//	AT+CPIN OFFSET
//#define	CPIN_PIN_PUK	8
//#define	CPIN_PUK_OFFSET	19
//======================================================================

//======================================================================
//	SEND CMD STATE STEP
#define	CMD_HTTP_IDLE			0x00

//#define WAIT_ANSWER_CMD_ATQ_CPIN	0x01

//======================================================================

class HttpCmd_GSM {
	
public:
	#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
		
	#endif
	#ifdef FIBOCOM_G510
		
	#endif
	#ifdef QUECTEL_M95
		
	#endif
	
	//uint8_t SetCmd_ATQ_CPIN(void);
	
	union HttpFlag {
		uint32_t  HttpDoubleWord;
		struct {
			uint32_t Free					:32;	//	Free
		} Bit;	
	} HttpFlag;
	//-------------------------------------------------------------------------------------
	
	void    HttpWaitAnswer(void);	
	void    HttpRetrySendCmd(void);
	
private:

};

extern HttpCmd_GSM Http;

#endif