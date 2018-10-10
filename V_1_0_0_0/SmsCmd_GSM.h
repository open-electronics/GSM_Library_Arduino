/*********************************************************************
 *
 *       Sms command
 *
 *********************************************************************
 * FileName:        SmsCmd_GSM.h
 * Revision:        1.0.0
 * Date:			01/10/2016
 * Dependencies:	Arduino.h
 *					Uart_GSM.h
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
 * Copyright (c) 2016, Futura Group srl 
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

#ifndef _SmsCmd_GSM_H_INCLUDED__
#define _SmsCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
const char AT_CPMS[]      PROGMEM = "AT+CPMS=";							//	Command - Preferred SMS Message Storage
const char AT_CMGD[]      PROGMEM = "AT+CMGD=";							//	Command - Delete SMS to memory
const char AT_CMGR[]      PROGMEM = "AT+CMGR=";							//	Command - Read SMS to memory
const char AT_CMGS[]      PROGMEM = "AT+CMGS=";							//	Command - Sends SMS
const char AT_CMGW[]      PROGMEM = "AT+CMGW=";							//	Command - Write SMS to memory
const char AT_CMSS[]      PROGMEM = "AT+CMSS=";							//	Commadn - Send SMS by memory

const char ATQ_CPMS[]     PROGMEM = "AT+CPMS?\r\n";						//	Query - Preferred SMS Message Storage
//======================================================================

//======================================================================
//	AT COMMAND ANSWER - STORED IN FLASH MEMORY
const char AT_ANSW_CPMS[]			PROGMEM = "+CPMS:";					// Key word for answer at the query CPMS

const char AT_ANSW_CMGR_RU[]		PROGMEM = "\"REC UNREAD\"";			// Key Word for SMS IN not yet read
const char AT_ANSW_CMGR_RR[]		PROGMEM = "\"REC READ\"";			// Key Word for SMS IN already read
const char AT_ANSW_CMGR_SU[]		PROGMEM = "\"STO UNSENT\"";			// Key Word for SMS OUT saved and not sent
const char AT_ANSW_CMGR_SS[]		PROGMEM = "\"STO SENT\"";			// Key Word for SMS OUT saved and sent

const char AT_ANSW_CMGS[]			PROGMEM = "+CMGS:";					// Key word for answer at the command CMGS
const char AT_ANSW_CMGW[]			PROGMEM = "+CMGW:";					// Key word for answer at the command CMGW
const char AT_ANSW_CMSS[]			PROGMEM = "+CMSS:";					// Key word for answer at the command CMSS
//======================================================================

//======================================================================
//	SMS Memory storage available
const char SMS_SM[]  PROGMEM = "\"SM\"";
const char SMS_ME[]  PROGMEM = "\"ME\"";
const char SMS_MT[]  PROGMEM = "\"MT\"";

#define CODE_SMS_SM		0x01
#define CODE_SMS_ME		0x02
#define CODE_SMS_MT		0x03
//======================================================================

//======================================================================
//	AT+CPMS OFFSET
#define	CPMS_MEM1		8
#define	CPMS_MEM2		13
#define	CPMS_MEM3		18
//======================================================================

//======================================================================
//	AT+CMGR OFFSET
#define	CMGR_INDEX		8

#define SMS_REC_UNREAD		0
#define SMS_REC_READ		1
#define SMS_STO_UNSENT		2
#define SMS_STO_SENT		3
#define SMS_ALL				4

#define STATE_CHANGE		0
#define STATE_NOT_CHANGE	1
//======================================================================

//======================================================================
//	AT+CMGD OFFSET
#define	CMGD_INDEX						8

#define DEF_FLAG_NORMAL					0	//	"0" Delete the message specified in <index>
#define DEF_FLAG_ALL_READ				1   // 	"1" Delete all read messages from preferred message storage, leaving unread messages and stored
											//	mobile originated messages (whether sent or not) untouched
#define DEF_FLAG_ALL_READ_SENT			2	//	"2" Delete all read messages from preferred message storage and sent mobile originated messages,
											//	leaving unread messages and unsent mobile originated messages untouched
#define DEF_FLAG_ALL_READ_SENT_UNSENT	3	//	"3" Delete all read messages from preferred message storage, sent and unsent mobile originated
											//	messages leaving unread messages untouched
#define DEF_FLAG_ALL					4	//	"4" Delete all messages from preferred message storage including unread messages
//======================================================================

//======================================================================
//	AT+CMGS OFFSET
#define	CMGS_INDEX		8
//======================================================================

//======================================================================
//	AT+CMGW OFFSET
#define	CMGW_INDEX		8
//======================================================================

//======================================================================
//	AT+CMSS OFFSET
#define	CMSS_INDEX		8
//======================================================================

//======================================================================
//	SEND CMD STATE STEP
#define	CMD_SMS_IDLE					0x00

#define WAIT_ANSWER_CMD_AT_CPMS			0x01	//	Answer at command - Preferrred Message Storage
#define WAIT_ANSWER_CMD_AT_CMGD			0x02	//	Answer at command - Delete SMS
#define WAIT_ANSWER_CMD_AT_CMGR			0x03	//	Answer at command - Read SMS
#define WAIT_ANSWER_CMD_AT_CMGS_STEP1	0x04	//	Answer at command - Send SMS
#define WAIT_ANSWER_CMD_AT_CMGS_STEP2	0x05	//	Answer at command - Send SMS
#define WAIT_ANSWER_CMD_AT_CMGW_STEP1	0x06	//	Answer at command - Write SMS to Memory
#define WAIT_ANSWER_CMD_AT_CMGW_STEP2	0x07	//	Answer at command - Write SMS to Memory
#define WAIT_ANSWER_CMD_AT_CMSS			0x08
//======================================================================

class SmsCmd_GSM {
	
public:	
	uint8_t SetCmd_AT_CPMS(uint8_t TypeOfMem1, uint8_t TypeOfMem2, uint8_t TypeOfMem3, bool Query);
	uint8_t SetCmd_AT_CMGD(uint8_t Index, uint8_t DelFlag);
	uint8_t SetCmd_AT_CMGR(uint8_t Index, uint8_t Mode);
	uint8_t SetCmd_AT_CMGS(void);
	uint8_t SetCmd_AT_CMGW(void);
	uint8_t SetCmd_AT_CMSS(uint8_t Index);
	
	uint8_t SmsText[160];		//	SMS Text
	uint8_t SmsOutgoingCounter;	//	Indicates the number of SMS sent. This number begins with 0 and is
								//	incremented by one for each outgoing message (Successful and failure cases);
								//	it is cyclic on one byte (0 follows 255). This variable works with the AT command AT+CMGS
	uint8_t SmsWriteMemoryAdd;	//	Indicates memory address where the outgoing SMS was stored. The SMS has not been sent yet
	uint8_t SmsReceivedIndex;	//	Indicates the index of SMS received
	
	struct {
		uint8_t	Type;
		uint8_t Used;
		uint8_t Total;
	} Sms_Mem1_Storage;			//	Messages to be read and deleted from this memory storage

	struct {
		uint8_t	Type;
		uint8_t Used;
		uint8_t Total;
	} Sms_Mem2_Storage;			//	Messages will be written and sent to this memory storage
	
	struct {
		uint8_t	Type;
		uint8_t Used;
		uint8_t Total;
	} Sms_Mem3_Storage;			//	Received messages will be placed in this memory storage
	
	union SmsFlag {
		uint8_t  SmsByte;
		struct {
			uint8_t InvalidInputValue	:1;		//  "1": Inserted invalid phone number
			uint8_t SendSmsInProgress	:1;		//  "1": Sends SMS in progress
			uint8_t WriteSmsInProgress	:1;		//  "1": Writes SMS to memory in progress
			uint8_t SmsReadStat			:2;		//	"0": "REC UNREAD" Received unread messages
												//	"1": "REC READ"   Received read messages
												//	"2": "STO UNSENT" Stored unsent messages
												//	"3": "STO SENT"   Stored sent messages
			uint8_t SmsReadOk			:1;		//  "0": SMS not read, "1" SMS read
			uint8_t Free				:2;		//	Free
		} Bit;	
	} SmsFlag;

	void    GsmSmsWaitAnswer(void);	
	void    GsmSmsRetrySendCmd(void);
	
private:
	uint8_t * FlashSmsMemTypeAdd;

	uint8_t FindSmsMemoryType(void);
	void    FlashAddSmsTypeMem(uint8_t TypeOfMem);
	
	void    FindUsedSmsMemory(uint8_t Mem, uint8_t StartAdd, uint8_t EndAdd);
	void    FindTotalSmsMemory(uint8_t Mem, uint8_t StartAdd, uint8_t EndAdd);
};

extern SmsCmd_GSM Sms;

#endif