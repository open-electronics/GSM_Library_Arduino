/*********************************************************************
 *
 *       Gprs command for GSM
 *
 *********************************************************************
 * FileName:        GprsCmd_GSM.cpp
 * Revision:        1.0.0
 * Date:			04/02/2018
 * Dependencies:	GprsCmd_GSM.h
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
 
#include "GprsCmd_GSM.h"

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
//	This function is used to
uint8_t GprsCmd_GSM::SetCmd_AT_CGATT(bool Query, uint8_t ServiceState) {
	uint8_t Index;
	
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.BckCmdData[0] = Query;
		Gsm.BckCmdData[1] = ServiceState;
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		if (Query == false) {
			Gsm.ReadStringFLASH((uint8_t *)AT_CGATT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGATT));
			Index = CGATT_STATE;
			Gsm.GSM_Data_Array[Index++] = ServiceState;
			Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
			Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
			Gsm.WritePointer = Index;
			Gsm.StartSendData(CMD_GPRS_IDLE, WAIT_ANSWER_CMD_AT_CGATT, ANSWER_GPRS_AT_CMD_STATE);
		} else {
			Gsm.ReadStringFLASH((uint8_t *)ATQ_CGATT, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CGATT));
			Gsm.WritePointer = strlen(ATQ_CGATT);
			Gsm.StartSendData(CMD_GPRS_IDLE, WAIT_ANSWER_CMD_ATQ_CGATT, ANSWER_GPRS_AT_CMD_STATE);
		}
	}
	return(1);			//	Command sent
}
//-----------------------------------------------------

//-----------------------------------------------------
//	This function is used to request Network Registration
//	The syntax of command is AT+CGREG? and the answer is +CGREG:<n>,<stat>[,<lac>,<ci>]
//	<n>		0	Disable network registration
//			1	Enable network registration unsolicited result code +CREG:<stat>
//			2   Enable network registration unsolicited result code with location information
//	<stat>  0	Not registered, MT is not currently searching a new operator to register to. the GPRS service is disabled
//			1	Registered, home network
//			2	Not registered, but MT is currently try to attach or searching an operator to register to. The GPRS service is enabled, but an allowable PLMN is currently not available
//			3	Registration denied. The GPRS service is disabled, the UE is not allowed to attach for GPRS if it is requested by the user
//			4	Unknown
//			5 	Registered, roaming
//	<lac>		String type; two byte location area code in hexadecimal format
//	<ci>		String type; two byte cell ID in hexadecimal format
//	For details see AT commands datasheet
uint8_t GprsCmd_GSM::SetCmd_ATQ_CGREG(bool Query, uint8_t Parameter) {
	uint8_t Index;
	
	if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
		return(0);		//	System Busy
	} else {
		Gsm.ClearBuffer();
		Gsm.BckCmdData[0] = Query;
		Gsm.BckCmdData[1] = Parameter;
		Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		Gsm.ResetFlags();
		if (Query == false) {
			Gsm.ReadStringFLASH((uint8_t *)AT_CGREG, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGREG));
			Index = CGREG_PAR;
			Gsm.GSM_Data_Array[Index++] = Parameter;
			Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
			Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
			Gsm.WritePointer = Index;
			Gsm.StartSendData(CMD_GPRS_IDLE, WAIT_ANSWER_CMD_AT_CGREG, ANSWER_GPRS_AT_CMD_STATE);
		} else {
			Gsm.ReadStringFLASH((uint8_t *)ATQ_CGREG, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CGREG));
			Gsm.WritePointer = strlen(ATQ_CGREG);
			Gsm.StartSendData(CMD_GPRS_IDLE, WAIT_ANSWER_CMD_ATQ_CGREG, ANSWER_GPRS_AT_CMD_STATE);
		}
	}
	return(1);			//	Command sent
}
//-----------------------------------------------------

//-----------------------------------------------------
void GprsCmd_GSM::GprsWaitAnswer(void) {

	if ((Gsm.StateSendCmd != CMD_GPRS_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
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
				case CMD_GPRS_IDLE:
					break;

				case WAIT_ANSWER_CMD_AT_CGATT:
					break;
					
				case WAIT_ANSWER_CMD_ATQ_CGATT:
					break;

				case WAIT_ANSWER_CMD_AT_CGREG:
					break;
					
				case WAIT_ANSWER_CMD_ATQ_CGREG:
					if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CGREG, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CGREG)) != 0xFF) {
						if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
							Gprs.CGREG_Info.Flag.Bit.n    = Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2)));
							Gprs.CGREG_Info.Flag.Bit.Stat = Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
							if (Gprs.CGREG_Info.Flag.Bit.Stat != 0) {
								Gsm.ExtractCharArray((char *)(&Gprs.CGREG_Info.LAC_Info[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[2] + 1]), sizeof(Gprs.CGREG_Info.LAC_Info), ASCII_COMMA);
								Gsm.ExtractCharArray((char *)(&Gprs.CGREG_Info.CI_Info[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[3] + 1]), sizeof(Gprs.CGREG_Info.CI_Info), ASCII_CARRIAGE_RET);
							}
						}
					}
					break;
					
				default:
					break;	
			}
		} else {
			Gsm.ProcessGsmError();
			switch (Gsm.StateWaitAnswerCmd)
			{
					
				default:
					GprsRetrySendCmd();
					break;
			}
		}				
	} else {
		Gsm.InitReset_GSM();
	}
}	

void GprsCmd_GSM::GprsRetrySendCmd(void) {
	if (Gsm.RetryCounter++ < 2) {
		switch (Gsm.StateWaitAnswerCmd)
		{
			case WAIT_ANSWER_CMD_AT_CGATT:
			case WAIT_ANSWER_CMD_ATQ_CGATT:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				SetCmd_AT_CGATT(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
				break;
				
			case WAIT_ANSWER_CMD_AT_CGREG:
			case WAIT_ANSWER_CMD_ATQ_CGREG:
				Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
				SetCmd_ATQ_CGREG(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
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
