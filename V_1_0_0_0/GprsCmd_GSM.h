/*********************************************************************
 *
 *       Gprs command for GSM
 *
 *********************************************************************
 * FileName:        GprsCmd_GSM.h
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

#ifndef _GprsCmd_GSM_H_INCLUDED__
#define _GprsCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
const char AT_CGATT[]     PROGMEM = "AT+CGATT=";							//	Command - Attach or Detach from GPRS Service
const char AT_CGREG[]     PROGMEM = "AT+CGREG=";							//	Command - Set Network Registration (if <n> = 2 -> Enable network registration unsolicited result code with location information)

const char ATQ_CGATT[]    PROGMEM = "AT+CGATT?\r\n";			            //	Query - Signal quantity report
const char ATQ_CGREG[]    PROGMEM = "AT+CGREG?\r\n";			            //  Query - Network registration

#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)

#endif

#ifdef FIBOCOM_G510

#endif
#ifdef QUECTEL_M95

#endif

//======================================================================

//======================================================================
//	AT COMMAND ANSWER - STORED IN FLASH MEMORY

const char AT_ANSW_CGREG[]			PROGMEM = "+CGREG:";					// Key word for answer at command CLCK

//======================================================================

//======================================================================
//	AT+CGATT OFFSET
#define	CGATT_STATE		9

#define	CGATT_DETACH	0
#define	CGATT_ATTACH	1
//======================================================================

//======================================================================
//	AT+CGREG OFFSET
#define	CGREG_PAR			9

#define	CGREG_DISABLE		0
#define	CGREG_ENABLE_NRURC	1	//	Enable network registration unsolicited result code
#define	CGREG_ENABLE_NRLURC	1	//	Enable network registration and location information unsolicited result code
//======================================================================

//======================================================================
//	SEND CMD STATE STEP
#define	CMD_GPRS_IDLE				0x00

#define WAIT_ANSWER_CMD_AT_CGATT	0x01
#define WAIT_ANSWER_CMD_ATQ_CGATT	0x02
#define WAIT_ANSWER_CMD_AT_CGREG	0x03
#define WAIT_ANSWER_CMD_ATQ_CGREG	0x04

//======================================================================

class GprsCmd_GSM {
	
public:
	#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
		
	#endif
	#ifdef FIBOCOM_G510
		
	#endif
	#ifdef QUECTEL_M95
		
	#endif
	
	uint8_t SetCmd_AT_CGATT(bool,uint8_t);
	uint8_t SetCmd_ATQ_CGREG(bool,uint8_t);
	struct {
		union Flag {
			uint8_t  FlagByte;
			struct {
				uint8_t n 		:2;		//	"0": Disable network registration unsollecited result code
										//	"1": Enable network registration unsollecited result code +CREG:<stat>
										//	"2": Enable network registration unsollecited result code with location information +CREG:<stat>[,<lac>,<ci>]
				uint8_t Stat	:3;		//	"0": Not registered, MT is not currently searching a new operator to register to
										//	"1": Registered, home network
										//	"2": Not registered, but MT is currently searching a nesw operator to register to
										//	"3": Registration denied
										//	"4": Unknown
										//	"5": Registered, roaming
				uint8_t Free	:3;		//	Free
			} Bit;	
		} Flag;			
		uint8_t LAC_Info[7];
		uint8_t CI_Info[7];
	} CGREG_Info;							
	//-------------------------------------------------------------------------------------
	
	union GprsFlag {
		uint32_t  GprsDoubleWord;
		struct {
			uint8_t Free					:32;	//	Free
		} Bit;	
	} GprsFlag;
	//-------------------------------------------------------------------------------------
	
	void    GprsWaitAnswer(void);	
	void    GprsRetrySendCmd(void);
	
private:

};

extern GprsCmd_GSM Gprs;

#endif