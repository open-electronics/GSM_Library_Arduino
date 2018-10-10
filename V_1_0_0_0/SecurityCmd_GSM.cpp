/*********************************************************************
 *
 *       Security command
 *
 *********************************************************************
 * FileName:        SecurityCmd_GSM.cpp
 * Revision:        1.0.0
 * Date:			08/05/2016
 * Dependencies:	SecurityCmd_GSM.h
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

#include "SecurityCmd_GSM.h"
#include "GenericCmd_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


/****************************************************************************
 * Function:        SetCmd_ATQ_SPIC or SetCmd_ATQ_TPIN (FIBOCOM G510) or SetCmd_ATQ_QRTPIN (QUECTEL M95)
 *
 * Overview:        This function is used to verify times remained to input SIM PIN/PUK. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	The syntax of command for SIM800C and SIM900 is AT+SPIC and the answer is +SPIC:<pin1>,<pin2>,<puk1>,<puk2>
 *					The syntax of command for G510 is AT+TPIN? and the answer is +TPIN:<pin1>,<pin2>,<puk1>,<puk2>
 *					The syntax of command for M95 is AT+QTRPIN and the answer is +QTRPIN:<pin1>,<pin2>,<puk1>,<puk2>
 *
 * Input:           None
 *
 * Command Note:	None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *					Return -> 1 (Command sent)
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
uint8_t SecurityCmd_GSM::SetCmd_ATQ_SPIC(void) {
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)ATQ_SPIC, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_SPIC));
		Gsm.WritePointer = strlen(ATQ_SPIC);			
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_ATQ_SPIC, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
#endif

#ifdef FIBOCOM_G510
uint8_t SecurityCmd_GSM::SetCmd_ATQ_TPIN(void) {
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)ATQ_TPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_TPIN));
		Gsm.WritePointer = strlen(ATQ_TPIN);			
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_ATQ_TPIN, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
#endif

#ifdef QUECTEL_M95
uint8_t SecurityCmd_GSM::SetCmd_ATQ_QRTPIN(void) {
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)ATQ_QTRPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_QTRPIN));
		Gsm.WritePointer = strlen(ATQ_QTRPIN);			
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_ATQ_QTRPIN, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
#endif

#ifdef AI_THINKER_A9
uint8_t SecurityCmd_GSM::SetCmd_ATQ_CPINC(void) {
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)ATQ_CPINC, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CPINC));
		Gsm.WritePointer = strlen(ATQ_CPINC);			
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_ATQ_CPINC, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_CPIN
 *
 * Overview:        This function is used to verify if PIN is required. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	AT+CPIN? and the answer is +CPIN:<code>
 *
 * Input:           None
 *
 * Command Note:	None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *					Return -> 1 (Command sent)
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t SecurityCmd_GSM::SetCmd_ATQ_CPIN(void) {
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)ATQ_CPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CPIN));
		Gsm.WritePointer = strlen(ATQ_CPIN);			
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_ATQ_CPIN, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_CPIN_PUK
 *
 * Overview:        This function is used to send PIN or PUK if required. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	AT+CPIN=<pin>[,<new pin>]
 *
 * Input:           TypeOfCmd: Type of command to be execute
 *
 * Command Note:	<pin>		String type; password
 *					<new pin>	String type; If the PIN required is SIM PUK or SIMPUK2: new password
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *					Return -> 1 (Command sent)
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t SecurityCmd_GSM::SetCmd_CPIN_PUK(uint8_t TypeOfCmd) {
	uint8_t Count;
	
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.BckCmdData[0] = TypeOfCmd;
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)AT_CPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPIN));
		EepromAddPinPuk(TypeOfCmd);
		switch (TypeOfCmd)
		{
			case SIM_PIN1_REQ:
			case SIM_PIN2_REQ:
			case SIM_PH_PIN_REQ:
				Count = CPIN_PIN_PUK;
				do {
					Gsm.GSM_Data_Array[Count] = eeprom_read_byte((uint8_t *)EepromAddPin++);
				} while (Count++ < (CPIN_PIN_PUK + 5));
				Gsm.GSM_Data_Array[Count++] = ASCII_CARRIAGE_RET;
				Gsm.GSM_Data_Array[Count++] = ASCII_LINE_FEED;
				//Count = 8;
				break;
			case SIM_PUK1_REQ:
			case SIM_PUK2_REQ:
			case SIM_PH_PUK_REQ:							
				Count = CPIN_PIN_PUK;
				do {
					Gsm.GSM_Data_Array[Count] = eeprom_read_byte((uint8_t *)EepromAddPuk++);
				} while (Count++ < (CPIN_PIN_PUK + 9));
				Gsm.GSM_Data_Array[Count++] = ASCII_COMMA;
				//Count = CPIN_PUK_OFFSET;
				do {
					Gsm.GSM_Data_Array[Count] = eeprom_read_byte((uint8_t *)EepromAddPin++);
				} while (Count++ < (CPIN_PUK_OFFSET + 5));
				Gsm.GSM_Data_Array[Count++] = ASCII_CARRIAGE_RET;
				Gsm.GSM_Data_Array[Count++] = ASCII_LINE_FEED;
				//Count = 15;
				break;
		}
		Gsm.WritePointer = Count;
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_CPIN, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_CLCK
 *
 * Overview:        This function is used to lock, unlock or interrogate a ME or a network facility <fac>. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	AT+CLCK=<fac>,<mode>[,<passwd>[,<class>]]
 *
 * Input:           TypeOfFac: Type of facility
 *					Mode:	   0 unlock; 1 lock; 2 Query status
 *
 * Command Note:	<fac>	  "AO" BAOC      (4 Digit PSWD) (Barr All Outgoing Calls) 
 *							  "OI" BOIC      (4 Digit PSWD) (Barr Outgoing International Calls)
 *							  "OX" BOIC-exHC (4 Digit PSWD) (Barr Outgoing International Calls except to Home Country)
 *							  "AI" BAIC      (4 Digit PSWD) (Barr All Incoming Calls)
 *							  "IR" BIC-Roam  (4 Digit PSWD) (Barr Incoming Calls when Roaming outside the home country)
 *							  "FD" 			 (4 Digit PSWD) SIM card or active application in the UICC (GSM or USIM) fixed dialling memory feature (if PIN2 authentication has not been done during the current session, PIN2 is required as <passwd>)
 *							  "SC" SIM 	     (4 Digit PSWD) (lock SIM/UICC card) (SIM/UICC asks password in MT power-up and when this lock command issued) Correspond to PIN1 code
 *						  	  "PN" 		     (8 Digit PSWD) Network Personalization, Correspond to NCK code
 *							  "PU" 		     (8 Digit PSWD) Network subset Personalization Correspond to NSCK code
 *							  "PP" 		     (8 Digit PSWD) Service Provider Personalization Correspond to SPCK code
 *					<mode>	  0 			 unlock
 *							  1 			 lock
 *							  2 			 query status
 *					<passwd>				 String type
 *					<class>   1				 Voice (telephony)
 *							  2				 Data refers to all bearer services; with <mode>=2 this may refer only to some bearer service if TA does not support values 16, 32, 64 and 128)
 *							  4				 Fax
 *							  7              All classes
 *					<status>  0				 Not active
 *							  1				 Active
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *					Return -> 1 (Command sent)
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t SecurityCmd_GSM::SetCmd_CLCK(uint8_t TypeOfFac, uint8_t Mode) {
	uint8_t Count;
	
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.BckCmdData[0] = TypeOfFac;
		Gsm.BckCmdData[1] = Mode;
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)AT_CLCK, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CLCK));
		FlashAddFac(TypeOfFac);
		Gsm.ReadStringFLASH(FlashFacAdd, ((uint8_t *)Gsm.GSM_Data_Array + CLCK_FAC), strlen(FAC_AO));
		Gsm.GSM_Data_Array[CLCK_FAC + 4] = ASCII_COMMA;
		Gsm.GSM_Data_Array[CLCK_MODE] = Mode + 0x30;	
		if (Mode == 2) {
			Gsm.GSM_Data_Array[CLCK_MODE + 1] = ASCII_CARRIAGE_RET;
			Gsm.GSM_Data_Array[CLCK_MODE + 2] = ASCII_LINE_FEED;
			Gsm.WritePointer = (strlen(AT_CLCK) + 8);			
			Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_CLCK, ANSWER_SECURITY_AT_CMD_STATE);			
			return(1);
		}
		Gsm.GSM_Data_Array[CLCK_MODE + 1] = ASCII_COMMA;
 		Count = CLCK_MODE + 2;
		do {
			if (eeprom_read_byte(EepromAddPin) != 0x00) {
				Gsm.GSM_Data_Array[Count] = eeprom_read_byte((uint8_t *)EepromAddPin++);
			} else {
				break;
			}
		} while (Count++ < (CLCK_MODE + 12));	
 		Gsm.GSM_Data_Array[Count++] = ASCII_CARRIAGE_RET;
		Gsm.GSM_Data_Array[Count++] = ASCII_LINE_FEED;
		Gsm.WritePointer = Count;
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_CLCK, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_CPWD
 *
 * Overview:        This function is used to change a password for the facility lock function. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	AT+CPWD=<fac>,<old-passwd>,<new-passwd>]]
 *
 * Input:           TypeOfFac: Type of facility
 *
 * Command Note:	<fac>	  "AO" BAOC      (4 Digit PSWD) (Barr All Outgoing Calls) 
 *							  "OI" BOIC      (4 Digit PSWD) (Barr Outgoing International Calls)
 *							  "OX" BOIC-exHC (4 Digit PSWD) (Barr Outgoing International Calls except to Home Country)
 *							  "AI" BAIC      (4 Digit PSWD) (Barr All Incoming Calls)
 *							  "IR" BIC-Roam  (4 Digit PSWD) (Barr Incoming Calls when Roaming outside the home country)
 *							  "AB" 			 (4 Digit PSWD) All Barring services
 *							  "P2" 	     	 (8 Digit PSWD) SIM PIN2
 *						  	  "SC" 		     (4 Digit PSWD) SIM (lock SIM/UICC card) (SIM/UICC asks password in MT power-up and when this lock command issued) Correspond to PIN1 code
 *					<pldpwd>				 String type (string should be included in quotation marks): password specified for the facility from the user interface or with command. If an old password has not yet been set, <oldpwd> is not to enter
 *					<newpwd>  				 String type (string should be included in quotation marks): new password
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *					Return -> 1 (Command sent)
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t SecurityCmd_GSM::SetCmd_CPWD(uint8_t TypeOfFac) {
	uint8_t Count;

	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.BckCmdData[0] = TypeOfFac;
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		Gsm.ReadStringFLASH((uint8_t *)AT_CPWD, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPWD));
		FlashAddFac(TypeOfFac);
		Gsm.ReadStringFLASH(FlashFacAdd, ((uint8_t *)Gsm.GSM_Data_Array + CLCK_FAC), strlen(FAC_AO));
		Gsm.GSM_Data_Array[CLCK_FAC + 4] = ASCII_COMMA;		
		Count = CLCK_FAC + 5;
		do {	//	Reads from EEPROM the old PIN and puts it on the GSM data array
			if (eeprom_read_byte(EepromAddPin) != 0x00) {
				Gsm.GSM_Data_Array[Count] = eeprom_read_byte((uint8_t *)EepromAddPin++);
			} else {
				break;
			}
		} while (Count++ < (CLCK_FAC + 15));			
		Gsm.GSM_Data_Array[Count++] = ASCII_COMMA;
		EepromAddPin = &NewPin[0];
		do {	//	Reads from SRAM the new PIN and puts it on the GSM data array 
			if ((*(uint8_t *)EepromAddPin) != 0x00) {
				Gsm.GSM_Data_Array[Count] = (*(uint8_t *)EepromAddPin++);
			} else {
				break;
			}
		} while (Count++ < (CLCK_FAC + 22));
		Gsm.GSM_Data_Array[Count] = ASCII_CARRIAGE_RET;
		Gsm.GSM_Data_Array[Count + 1] = ASCII_LINE_FEED;	
		Gsm.WritePointer = Count + 2;	
		Gsm.StartSendData(CMD_SECURITY_IDLE, WAIT_ANSWER_CMD_CPWD, ANSWER_SECURITY_AT_CMD_STATE);
	}
	return(1);			//	Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        GsmSecurityWaitAnswer
 *
 * Overview:        This function process the AT command answer of the command sent.
 *					The answer received and processed by this code regard the Generic Command Functions
 *                  implemented in this library file
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	None
 *
 * Input:           None
 *
 * Command Note:	None
 *
 * Output:          None
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void SecurityCmd_GSM::GsmSecurityWaitAnswer(void) {
	uint8_t StrPointer = 0xFF;
	uint8_t StrPointerEnd = 0xFF;

	if ((Gsm.StateSendCmd != CMD_SECURITY_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
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
		Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
		return;
	}
	
	if (Gsm.ReadPointer > 0) {
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK)) != 0xFF) {
			Gsm.RetryCounter = 0;
			Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
					
			switch (Gsm.StateWaitAnswerCmd)
			{
				case CMD_WAIT_IDLE:
					break;
					
				case WAIT_ANSWER_CMD_ATQ_SPIC:
				case WAIT_ANSWER_CMD_ATQ_TPIN:
				case WAIT_ANSWER_CMD_ATQ_QTRPIN:
				case WAIT_ANSWER_CMD_ATQ_CPINC:
					Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
					#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
						StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_SPIC, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_SPIC));
					#endif
					#ifdef FIBOCOM_G510
						StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_TPIN, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_TPIN));
					#endif
					#ifdef QUECTEL_M95
						StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_QTRPIN, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_QTRPIN));
					#endif
					#ifdef AI_THINKER_A9
						StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPINC, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPINC));
					#endif
					if (StrPointer != 0xFF) {
						if (Gsm.FindColonCommaCarriageRet() != 0xFF) {							
							SecurityFlag.Bit.PinRetry  = Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 1), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 1)));
							SecurityFlag.Bit.Pin2Retry = Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
							SecurityFlag.Bit.PukRetry  = Gsm.ExtractParameterByte((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));
							SecurityFlag.Bit.Puk2Retry = Gsm.ExtractParameterByte((Gsm.CharPointers[3] + 1), (Gsm.CharPointers[4] - (Gsm.CharPointers[3] + 1)));
						}
					}
					break;
					
				case WAIT_ANSWER_CMD_ATQ_CPIN:
					Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
					if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_READY, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_READY)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_READY;			//	SIM READY
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_SIM_READY_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_SIM_READY_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PIN, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PIN)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_PIN1_REQ;			//	SIM PIN1 REQUIRED
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_SIM_PIN_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_SIM_PIN_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PUK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PUK)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_PUK1_REQ;			//	SIM PUK1 REQUIRED
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_SIM_PUK_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_SIM_PUK_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PH_PIN, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PH_PIN)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_PH_PIN_REQ;		//	PH SIM PIN REQUIRED
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_PH_SIM_PIN_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_PH_SIM_PIN_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PH_PUK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PH_PUK)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_PH_PUK_REQ;		//	PH SIM PUK REQUIRED
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_PH_SIM_PUK_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_PH_SIM_PUK_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PIN2, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PIN2)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_PIN2_REQ;			//	SIM PIN2 REQUIRED
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_SIM_PIN2_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_SIM_PIN2_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PUK2, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PUK2)) != 0xFF) {
						SecurityFlag.Bit.SIM_Status = SIM_PUK2_REQ;			//	SIM PUK2 REQUIRED
						#ifdef GSM_SECURITY_DEBUG
							Serial.print("\n");
							Gsm.ReadStringFLASH((uint8_t *)STR_SIM_PUK2_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_SIM_PUK2_DEBUG));
							Gsm.PrintScreenDebugMode();
						#endif
						break;
					}
					break;
					
				case WAIT_ANSWER_CMD_CPIN:
					Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
					break;
					
				case WAIT_ANSWER_CMD_CLCK:
					Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
					if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CLCK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CLCK)) != 0xFF) {
						if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
							SecurityFlag.Bit.ClckStatus = Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 1), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 1)));
						}
					}
					break;
					
				case WAIT_ANSWER_CMD_CPWD:
					Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
					// Save new PIN in Eeprom
					switch (Gsm.BckCmdData[0])
					{
						case CODE_FAC_AO:		//	(4-digit PSWD)
						case CODE_FAC_OI:		//	(4-digit PSWD)
						case CODE_FAC_OX:		//	(4-digit PSWD)
						case CODE_FAC_AI:		//	(4-digit PSWD)
						case CODE_FAC_IR:		//	(4-digit PSWD)
						case CODE_FAC_AB:		//	(4-digit PSWD)
						case CODE_FAC_AG:		//	(4-digit PSWD)
						case CODE_FAC_AC:		//	(4-digit PSWD)
							EepromAddPin = Gsm.EepronAdd.StartAddShortPswdCode;
							StrPointer    = 0;
							StrPointerEnd = 6;
							break;
						case CODE_FAC_SC:
							EepromAddPin = Gsm.EepronAdd.StartAddPin1Code;
							StrPointer    = 0;
							StrPointerEnd = 6;
							break;
						case CODE_FAC_FD:
							EepromAddPin = Gsm.EepronAdd.StartAddPin2Code;
							StrPointer    = 0;
							StrPointerEnd = 6;
							break;		
						case CODE_FAC_PN:		//	(8-digit PSWD)
						case CODE_FAC_PU:		//	(8-digit PSWD)
						case CODE_FAC_PP:		//	(8-digit PSWD)
						case CODE_FAC_P2:		//	(8-digit PSWD)
							EepromAddPin = Gsm.EepronAdd.StartAddLongPswdCode;
							StrPointer    = 0;
							StrPointerEnd = 10;
							break;
						default:
							EepromAddPin = Gsm.EepronAdd.StartAddPin1Code;
							StrPointer = 0;
							StrPointerEnd = 6;
							break;
					}
					do {
						eeprom_write_byte(EepromAddPin++, NewPin[StrPointer++]);
					} while (StrPointer <= StrPointerEnd);
					break;
					
				default:
					break;	
			}
		} else {
			Gsm.ProcessGsmError();
			switch (Gsm.StateWaitAnswerCmd)
			{
				case WAIT_ANSWER_CMD_CPIN:
					if (SecurityFlag.Bit.IncorrectPSW != 1) {
						GsmSecurityRetrySendCmd();
					} else {
						Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
					}
					break;
					
				default:
					GsmSecurityRetrySendCmd();
					break;
			}
		}				
	} else {
		Gsm.InitReset_GSM();
	}
}	
/****************************************************************************/

/****************************************************************************
 * Function:        GsmSecurityRetrySendCmd
 *
 * Overview:        This function retry to send AT command for a maximum of three time
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	None
 *
 * Input:           None
 *
 * Command Note:	None
 *
 * Output:          None
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void SecurityCmd_GSM::GsmSecurityRetrySendCmd(void) {
	if (Gsm.RetryCounter++ < 2) {
		switch (Gsm.StateWaitAnswerCmd)
		{
			case WAIT_ANSWER_CMD_ATQ_CPIN:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				SetCmd_ATQ_CPIN();
				break;
			case WAIT_ANSWER_CMD_ATQ_SPIC:
			case WAIT_ANSWER_CMD_ATQ_TPIN:
			case WAIT_ANSWER_CMD_ATQ_QTRPIN:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
					SetCmd_ATQ_SPIC();
				#endif
				#ifdef FIBOCOM_G510
					SetCmd_ATQ_TPIN();
				#endif
				#ifdef QUECTEL_M95
					SetCmd_ATQ_QRTPIN();
				#endif				
				break;
			case WAIT_ANSWER_CMD_CPIN:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				SetCmd_CPIN_PUK(Gsm.BckCmdData[0]);
				break;	
			case WAIT_ANSWER_CMD_CLCK:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				SetCmd_CLCK(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
				break;	
			case WAIT_ANSWER_CMD_CPWD:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				SetCmd_CPWD(Gsm.BckCmdData[0]);
				break;	
			default:
				break;
		}
	} else {
		Gsm.InitReset_GSM();
	}
}
/****************************************************************************/

/****************************************************************************
 * Function:        EepromAddPinPuk
 *
 * Overview:        This function is used to extract the EEPROM address where the PIN and PUKE code are stored
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	None
 *
 * Input:           TypeOfCmd: Type of password that is necessary to find the EEPROM memory
 *
 * Command Note:	None
 *
 * Output:          None
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
void SecurityCmd_GSM::EepromAddPinPuk(uint8_t TypeOfCmd) {	
	char AddressBuffer[8];
	
	switch (TypeOfCmd)
	{
		case SIM_PIN1_REQ:			//	(4-digit PSWD)
		case SIM_PIN2_REQ:			//	(4-digit PSWD)
		case SIM_PH_PIN_REQ:		//	(4-digit PSWD)
		case SHORT_PSWD_REQ:		//	(4-digit PSWD)
			switch (TypeOfCmd)
			{
				case SIM_PIN1_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddPin1Code;
					break;
				case SIM_PIN2_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddPin2Code;
					break;
				case SIM_PH_PIN_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddPhPinCode;
					break;
				case SHORT_PSWD_REQ:	
					EepromAddPin = Gsm.EepronAdd.StartAddShortPswdCode;
					break;
			}
			break;
		case SIM_PUK1_REQ:			//	(8-digit PSWD)
		case SIM_PUK2_REQ:			//	(8-digit PSWD)
		case SIM_PH_PUK_REQ:		//	(8-digit PSWD)
		case LONG_PSWD_REQ:			//	(8-digit PSWD)
			switch (TypeOfCmd)
			{
				case SIM_PUK1_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddPin1Code;
					EepromAddPuk = Gsm.EepronAdd.StartAddPuk1Code;
					break;
				case SIM_PUK2_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddPin2Code;
					EepromAddPuk = Gsm.EepronAdd.StartAddPuk2Code;
					break;
				case SIM_PH_PUK_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddPhPinCode;
					EepromAddPuk = Gsm.EepronAdd.StartAddPhPukCode;
					break;
				case LONG_PSWD_REQ:
					EepromAddPin = Gsm.EepronAdd.StartAddLongPswdCode;
					break;
			}
			break;
	}	
}
/****************************************************************************/

/****************************************************************************
 * Function:        FlashAddFac
 *
 * Overview:        This function is used to extract the FLASH address where the facility code is stored
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:	None
 *
 * Input:           TypeOfFac: Type of facility that is necessary to find the FLASH memory
 *
 * Command Note:	None
 *
 * Output:          None
 *
 * GSM answer det:	None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
void SecurityCmd_GSM::FlashAddFac(uint8_t TypeOfFac) {
	switch (TypeOfFac)
	{
		case CODE_FAC_AO:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_AO;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_OI:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_OI;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_OX:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_OX;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_AI:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_AI;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_IR:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_IR;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_AB:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_AB;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_AG:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_AG;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_AC:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_AC;
			EepromAddPinPuk(SHORT_PSWD_REQ);
			break;
		case CODE_FAC_FD:	//	(4-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_FD;
			EepromAddPinPuk(SIM_PIN2_REQ);
			break;
		case CODE_FAC_SC:	//	(4-digit PIN1)
			FlashFacAdd = (uint8_t *)FAC_SC;
			EepromAddPinPuk(SIM_PIN1_REQ);
			break;
		case CODE_FAC_PN:	//	(8-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_PN;
			EepromAddPinPuk(LONG_PSWD_REQ);
			break;
		case CODE_FAC_PU:	//	(8-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_PU;
			EepromAddPinPuk(LONG_PSWD_REQ);
			break;
		case CODE_FAC_PP:	//	(8-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_PP;
			EepromAddPinPuk(LONG_PSWD_REQ);
			break;
		case CODE_FAC_P2:	//	(8-digit PSWD)
			FlashFacAdd = (uint8_t *)FAC_P2;
			EepromAddPinPuk(LONG_PSWD_REQ);
			break;	
		default:			//	(4-digit PIN1)
			FlashFacAdd = (uint8_t *)FAC_SC;
			EepromAddPinPuk(SIM_PIN1_REQ);
			break;
	}
}
/****************************************************************************/
