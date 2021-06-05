/*********************************************************************
 *
 *       Https command for GSM
 *
 *********************************************************************
 * FileName:        HttpCmd_GSM.cpp
 * Revision:        1.0.0
 * Date:            13/01/2019
 *
 * Revision:        1.2.0
 *                  30/11/2019
 *                  - Fixed bug "ClearBuffer" function [sizeof parameter]
 * 
 * Revision:        1.3.0
 *                  08/12/2019
 *					- Changed function name "HttpEepromStartAddSetup" to "EepromStartAddSetup"
 *					- Changed array name "HttpEepromAdd" to "EepromAdd"
 *					- Changed "sizeof" implementations to calc arrays length
 *
 * Dependencies:    HttpCmd_GSM.h
 * Arduino Board:   Arduino Uno, Arduino Mega 2560, Fishino Uno, Fishino Mega 2560       
 *
 * Company:         Futura Group srl
 *                  www.Futurashop.it
 *                  www.open-electronics.org
 *
 * Developer:       Destro Matteo
 *
 * Support:         info@open-electronics.org
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
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************/

#include "Isr_GSM.h"
#include "Io_GSM.h"
#include "GenericCmd_GSM.h"
#include "SecurityCmd_GSM.h"
#include "GprsCmd_GSM.h"
#include "TcpIpCmd_GSM.h"
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

#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
#endif

/****************************************************************************
 * Function:        SetCmd_AT_HTTP_STATUS
 *
 * Overview:        This function is used to Read HTTP Status. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+HTTPSTATUS?
 *                  The answer of this command is: +HTTPSTATUS: <mode>,<status>,<finish>,<remain>
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <mode>   GET
 *                           POST
 *                           HEAD
 *                  <status> 0 Idle
 *                           1 Receiving
 *                           2 Sending
 *                  <finish> The amount of data which have been transmitted
 *                  <remain> The amount of data remaining to be sent or received
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
	#if ENABLE_ANSWER_HTTP_AT_CMD_STATE
		uint8_t HttpCmd_GSM::SetCmd_AT_HTTP_STATUS(void) {   
			if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
				return(0);      //  System Busy
			} else {
				Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
				Gsm.ResetGsmFlags();
				Gsm.ResetHttpFlags();
				Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
				Gsm.ReadStringFLASH((uint8_t *)AT_HTTPSTATUS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_HTTPSTATUS));
				Gsm.WritePointer = strlen(AT_HTTPSTATUS);
				Gsm.StartSendData(CMD_HTTP_IDLE, WAIT_ANSWER_CMD_AT_HTTP_STATUS, ANSWER_HTTP_AT_CMD_STATE);
			}
			return(1);          //  Command sent	
		}
	#endif
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_HTTPINIT
 *
 * Overview:        This function is used to Initialize HTTP Service. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+HTTPINIT
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
	#if ENABLE_ANSWER_HTTP_AT_CMD_STATE
		uint8_t HttpCmd_GSM::SetCmd_AT_HTTP_INIT(void) {
			if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
				return(0);      //  System Busy
			} else {
				Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
				Gsm.ResetGsmFlags();
				Gsm.ResetHttpFlags();
				Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
				Gsm.ReadStringFLASH((uint8_t *)AT_HTTPINIT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_HTTPINIT));
				Gsm.WritePointer = strlen(AT_HTTPINIT);
				Gsm.StartSendData(CMD_HTTP_IDLE, WAIT_ANSWER_CMD_AT_HTTP_INIT, ANSWER_HTTP_AT_CMD_STATE);
			}
			return(1);          //  Command sent
		}	
	#endif
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_HTTP_TERM
 *
 * Overview:        This function is used to Terminate HTTP Service. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+HTTPTERM
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
	#if ENABLE_ANSWER_HTTP_AT_CMD_STATE
		uint8_t HttpCmd_GSM::SetCmd_AT_HTTP_TERM(void) {
			if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
				return(0);      //  System Busy
			} else {
				Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
				Gsm.ResetGsmFlags();
				Gsm.ResetHttpFlags();
				Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
				Gsm.ReadStringFLASH((uint8_t *)AT_HTTPTERM, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_HTTPTERM));
				Gsm.WritePointer = strlen(AT_HTTPTERM);
				Gsm.StartSendData(CMD_HTTP_IDLE, WAIT_ANSWER_CMD_AT_HTTP_TERM, ANSWER_HTTP_AT_CMD_STATE);
			}
			return(1);          //  Command sent
		}
	#endif	
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_HTTPPARA
 *
 * Overview:        This function is used to Set HTTP Parameters Value. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+HTTPINIT:<HTTPParamTag>,<HTTPParamValue>
 *
 * Input:           ParamTagCode: Identifies the parameter code
 *
 * Command Note:    <HTTPParamTag>      HTTP Parameter
 *                                      "CID"       (Mandatory Parameter) Bearer profile identifier
 *                                      "URL"       (Mandatory Parameter) HTTP client URL "http://'server'/'path':'tcpPort' " "server": FQDN or IP-address "path": path of file or directory "tcpPort": default value is 80. Refer to "IETF-RFC 2616".)
 *                                      "UA"        The user agent string which is set by the application to identify the mobile. Usually this parameter is set as operation system and software version information. Default value is "SIMCom_MODULE"
 *                                      "PROIP"     The IP address of HTTP proxy server
 *                                      "PROPORT"   The port of HTTP proxy server
 *                                      "REDIR"     This flag controls the redirection mechanism of the SIM800 when it is acting as HTTP client (numeric). If the server sends a redirect code (range 30x), the client will automatically send a new HTTP request when the flag is set to (1). Default value is 0 (no redirection)
 *                                      "BREAK"     Parameter for HTTP method "GET", used for resuming broken transfer
 *                                      "BREAKEND"  Parameter for HTTP method "GET", used for resuming broken transfer. which is used together with "BREAK", If the value of "BREAKEND" is bigger than "BREAK", the transfer scope is from "BREAK" to "BREAKEND". If the value of "BREAKEND" is smaller than "BREAK", the transfer scope is from "BREAK" to the end of the file
 *                                      "TIMEOUT"   If both "BREAKEND" and "BREAK" are 0, the resume broken transfer function is disabled. HTTP session timeout value, scope: 30-1000 second. Default value is 120 seconds. HTTP Parameter value. Type and supported content depend on related <HTTPParamTag>
 *                                      "CONTENT"   Used to set the “Content-Type” field in HTTP header
 *                                      "USERDATA"  _User data
 *                  <HTTPParamValue>    HTTP Parameter value.Type and supported content depend on related <HTTPParamTag>.   
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
	#if ENABLE_ANSWER_HTTP_AT_CMD_STATE
		uint8_t HttpCmd_GSM::SetCmd_AT_HTTP_PARA(uint8_t ParamTagCode) {
			uint8_t Index;
			
			if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
				return(0);      //  System Busy
			} else {
				Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
				Gsm.ResetGsmFlags();
				Gsm.ResetHttpFlags();
				Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
				Gsm.BckCmdData[0] = ParamTagCode;
				Gsm.ReadStringFLASH((uint8_t *)AT_HTTPPARA, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_HTTPPARA));
				Index = HTTP_PARA_OFFSET;
				Index = Http.SetHttpParaParam(Index, ParamTagCode);
				Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
				Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
				Gsm.WritePointer = Index;
				Gsm.StartSendData(CMD_HTTP_IDLE, WAIT_ANSWER_CMD_AT_HTTP_PARA, ANSWER_HTTP_AT_CMD_STATE);
			}
			return(1);          //  Command sent
		}
	#endif
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_HTTPACTION
 *
 * Overview:        This function is used to set HTTP Method Action. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+HTTPACTION=<Method>
 *
 * Input:           Method: Insert a code that identify the action: "0" GET; "1" POST; "2" HEAD; "3" DELETE
 *
 * Command Note:    <method>:   HTTP method specification: "0" GET; "1" POST; "2" HEAD; "3" DELETE
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <Method>        HTTP method specification:
 *                                  0   GET
 *                                  1   POST
 *                                  2   HEAD
 *                                  3   DELETE
 *                  <Statuscode>    HTTP Status Code responded by remote server, it identifier refer to HTTP1.1(RFC2616)
 *                                  100 Continue 
 *                                  101 Switching Protocols
 *                                  200 OK 
 *                                  201 Created
 *                                  202 Accepted
 *                                  203 Non-Authoritative Information
 *                                  204 No Content
 *                                  205 Reset Content
 *                                  206 Partial Content
 *                                  300 Multiple Choices
 *                                  301 Moved Permanently
 *                                  302 Found
 *                                  303 See Other
 *                                  304 Not Modified 
 *                                  305 Use Proxy
 *                                  307 Temporary Redirect
 *                                  400 Bad Request
 *                                  401 Unauthorized
 *                                  402 Payment Required
 *                                  403 Forbidden
 *                                  404 Not Found
 *                                  405 Method Not Allowed
 *                                  406 Not Acceptable
 *                                  407 Proxy Authentication Required
 *                                  408 Request Time-out
 *                                  409 Conflict
 *                                  410 Gone 
 *                                  411 Length Required
 *                                  412 Precondition Failed
 *                                  413 Request Entity Too Large
 *                                  414 Request-URI Too Large
 *                                  415 Unsupported Media Type
 *                                  416 Requested range not satisfiable
 *                                  417 Expectation Failed
 *                                  500 Internal Server Error
 *                                  501 Not Implemented
 *                                  502 Bad Gateway
 *                                  503 Service Unavailable
 *                                  504 Gateway Time-out
 *                                  505 HTTP Version not supported
 *                                  600 Not HTTP PDU
 *                                  601 Network Error
 *                                  602 No memory
 *                                  603 DNS Error
 *                                  604 Stack Busy <DataLen> The length of data got
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
	#if ENABLE_ANSWER_HTTP_AT_CMD_STATE
		uint8_t HttpCmd_GSM::SetCmd_AT_HTTP_ACTION(uint8_t Method) {
			uint8_t Index;
			
			if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
				return(0);      //  System Busy
			} else {
				Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
				Gsm.ResetGsmFlags();
				Gsm.ResetHttpFlags();
				Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
				//Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
				Gsm.BckCmdData[0] = Method;
				Gsm.ReadStringFLASH((uint8_t *)AT_HTTPACTION, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_HTTPACTION));
				Index = HTTP_ACTION_OFFSET;
				Gsm.GSM_Data_Array[Index++] = (Method + 0x30);
				Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
				Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
				Gsm.WritePointer = Index;
				Gsm.StartSendData(CMD_HTTP_IDLE, WAIT_ANSWER_CMD_AT_HTTP_ACTION, ANSWER_HTTP_AT_CMD_STATE);
			}
			return(1);          //  Command sent
		}
	#endif
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        HttpWaitAnswer
 *
 * Overview:        This function process the AT command answer of the command sent.
 *                  The answer received and processed by this code regard the HTTP Command Functions
 *                  implemented in this library file
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    void HttpCmd_GSM::HttpWaitAnswer(void) {
        uint8_t StrPointer = 0xFF;
        uint8_t StrPointerEnd = 0xFF;

        if ((Gsm.StateSendCmd != CMD_HTTP_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) { return; }
        if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) { return; }
		
        Gsm.UartFlag.Bit.ReceivedAnswer = 0;
        if (Gsm.GsmFlag.Bit.CringOccurred == 1) {
            //  CRING OCCURRED. CMD SEND ABORTED
            Gsm.RetryCounter = 0;
            Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
            return;
        }
        
        if (Gsm.ReadPointer > 0) {
            if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                Gsm.RetryCounter = 0;
                Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;

                switch (Gsm.StateWaitAnswerCmd)
                {
                    case WAIT_ANSWER_CMD_AT_HTTP_STATUS:
                        if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_HTTP_STATUS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_HTTP_STATUS), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                            if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_HTTP_GET, (uint8_t *)Gsm.TempStringCompare, strlen(AT_HTTP_GET), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                                Http.HttpFlag.Bit.HttpStatusMode = HTTP_ACTION_METHOD_GET;
                            } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_HTTP_POST, (uint8_t *)Gsm.TempStringCompare, strlen(AT_HTTP_POST), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                                Http.HttpFlag.Bit.HttpStatusMode = HTTP_ACTION_METHOD_POST;
                            } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_HTTP_HEAD, (uint8_t *)Gsm.TempStringCompare, strlen(AT_HTTP_HEAD), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                                Http.HttpFlag.Bit.HttpStatusMode = HTTP_ACTION_METHOD_HEAD;
                            }
                            if (Gsm.FindColonCommaCarriageRet(0) != 0xFF) {
                                Http.HttpFlag.Bit.HttpStatus = (uint8_t)(Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1))));
                            }                       
                        }
                        break;
                    default:
                        break;  
                }
                Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
            } else {
                Gsm.ProcessGsmError();
                HttpRetrySendCmd();
            }           
        } else {
            //  If no answer from GSM module was received, this means that the module is switched off
            Gsm.InitReset_GSM();
        }
    }   

    void HttpCmd_GSM::HttpRetrySendCmd(void) {
        uint8_t AnswerCmdStateBackup;
        
        if (Gsm.RetryCounter++ < 2) {
            AnswerCmdStateBackup = Gsm.StateWaitAnswerCmd;
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
            switch (AnswerCmdStateBackup)
            {
                case WAIT_ANSWER_CMD_AT_HTTP_STATUS:
                    Http.SetCmd_AT_HTTP_STATUS();
                    break;
                case WAIT_ANSWER_CMD_AT_HTTP_INIT:
                    Http.SetCmd_AT_HTTP_INIT();
                    break;
                case WAIT_ANSWER_CMD_AT_HTTP_TERM:
                    Http.SetCmd_AT_HTTP_TERM();
                    break;
                case WAIT_ANSWER_CMD_AT_HTTP_PARA:
                    Http.SetCmd_AT_HTTP_PARA(Gsm.BckCmdData[0]);
                    break;
                case WAIT_ANSWER_CMD_AT_HTTP_ACTION:
                    Http.SetCmd_AT_HTTP_ACTION(Gsm.BckCmdData[0]);
                    break;
                default:
                    break;
            }
        } else {
            if (Gsm.GsmFlag.Bit.NoAutoResetGsmError != 1) {
                Gsm.InitReset_GSM();
            }
        }
    }
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        InitHttpSendCmd
 *
 * Overview:        This function is used to send the initialization commands for HTTP function.
 *                  The states machine is realized with a Switch statement
 *                  The complementary function of this is the "InitHttpWaitAnswer" function
 *                  that processes data received from GSM engine (see below)
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    void HttpCmd_GSM::InitHttpSendCmd(void) {
        uint8_t Index;
        
        if ((Gsm.StateInitWaitAnswerCmd != INIT_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) { return; }
        if (Isr.TimeOutInit > 0) { return; }
		
        Gsm.GsmFlag.Bit.GsmPowerOn = 0;
        Gsm.GsmFlag.Bit.GsmReset   = 0;
        
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        switch (Gsm.StateInitSendCmd)
        {
            case INIT_IDLE:
                Gsm.RetryCounter = 0;
                break;  
            case INIT_CMD_AT_SAPBR_1:
                Gsm.ReadStringFLASH((uint8_t *)AT_SAPBR_1, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_SAPBR_1));
                Gsm.WritePointer = strlen(AT_SAPBR_1);
                Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_AT_SAPBR_1, INIT_CMD_AT_NOTHING);     
                break;
            case INIT_CMD_AT_SAPBR_2:       
                Gsm.ReadStringFLASH((uint8_t *)AT_SAPBR_2, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_SAPBR_2));
                Index = SAPBR_2_APN_OFFSET;
                Gsm.ReadStringFLASH((uint8_t *)SAPBR_PARAM_APN, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(SAPBR_PARAM_APN));
                Index = strlen(Gsm.GSM_Data_Array);
                Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
				Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], Gprs.EepromAdd.StartAddApn, sizeof(APN));
                Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
                Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
                Gsm.WritePointer = Index;
                Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_AT_SAPBR_2, INIT_CMD_AT_NOTHING);
                break;
            case INIT_CMD_AT_SAPBR_3:
                Gsm.ReadStringFLASH((uint8_t *)AT_SAPBR_3, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_SAPBR_3));
                Gsm.WritePointer = strlen(AT_SAPBR_3);
                Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_AT_SAPBR_3, INIT_CMD_AT_NOTHING);         
                break;
            case INIT_CMD_AT_SAPBR_4:
                Gsm.ReadStringFLASH((uint8_t *)AT_SAPBR_4, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_SAPBR_4));
                Gsm.WritePointer = strlen(AT_SAPBR_4);
                Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_AT_SAPBR_4, INIT_CMD_AT_NOTHING);             
                break;      
            default:
                break;
        }
    }
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        InitHttpWaitAnswer
 *
 * Overview:        This function is used to proces received data from GSM engine
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void HttpCmd_GSM::InitHttpWaitAnswer(void) {
    uint8_t StrPointer = 0xFF;
        
    if ((Gsm.StateInitSendCmd != INIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) { return; }
    if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) { return; }
	
    Gsm.UartFlag.Bit.ReceivedAnswer = 0;
    
    switch (Gsm.StateInitWaitAnswerCmd)
    {
        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_1:
        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_2:
        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_3:
        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_4:
            if (Gsm.ReadPointer > 0) {
                if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    Gsm.RetryCounter = 0;
                    
                    switch (Gsm.StateInitWaitAnswerCmd)
                    {
                        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_1:               //  CMD AT+SAPBR STEP 1 - OK (SET NEXT STEP -> CMD AT+SAPBR STEP 2)
                            Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_2;     
                            break;
                        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_2:               //  CMD AT+SAPBR STEP 2 - OK (SET NEXT STEP -> CMD AT+SAPBR STEP 3)
                            Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_3;     
                            break;
                        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_3:               //  CMD AT+SAPBR STEP 3 - OK (SET NEXT STEP -> CMD AT+SAPBR STEP 4)
                            Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_4;     
                            break;          
                        case INIT_WAIT_ANSWER_CMD_AT_SAPBR_4:               //  CMD AT+SAPBR STEP 4 - OK (SET NEXT STEP -> INIT IDLE)
                            Gsm.StateInitSendCmd = INIT_IDLE;
                            Gsm.GsmFlag.Bit.HttpInitInProgress = 0;
                            Io.LedOff(TRIGGER_3);
                            break;
                        default:
                            break;
                    }
                } else {
                    Gsm.ProcessGsmError();
                    if (Gsm.RetryCounter++ < 2) {
                        switch (Gsm.StateInitWaitAnswerCmd)
                        {
                            case INIT_WAIT_ANSWER_CMD_AT_SAPBR_1:           //  Retry to send command AT+SAPBR STEP 1
                                Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_1;
                                break;
                            case INIT_WAIT_ANSWER_CMD_AT_SAPBR_2:           //  Retry to send command AT+SAPBR STEP 2
                                Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_2;
                                break;
                            case INIT_WAIT_ANSWER_CMD_AT_SAPBR_3:           //  Retry to send command AT+SAPBR STEP 3
                                Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_3;
                                break;
                            case INIT_WAIT_ANSWER_CMD_AT_SAPBR_4:           //  Retry to send command AT+SAPBR STEP 4
                                Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_4;
                                break;
                            default:
                                break;
                        }                       
                    } else {
                        Gsm.InitReset_GSM();
                        break;
                    }
                }
                Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
            } else {
                Gsm.GsmFlag.Bit.GsmNoAnswer = 1;        //  No answer from GSM module
                Gsm.InitReset_GSM();
                break;
            }
            break; 
        default:
            break;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        SelectParamTag
 *
 * Overview:        This function is used to reads the Parameter Tag strings used in AT+HTTPPARA command
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Index:     Pointer in GSM_Data_Array to write Parameter Tag
 *                  ParamCode: Code to extract the string associated to the Parameter Tag
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
uint16_t HttpCmd_GSM::SetHttpParaParam(uint16_t Index, uint8_t ParamCode) {
    uint16_t Count = 0;
    
    switch (ParamCode)
    {
        case HTTP_PARAM_CID_CODE:   //  AT+HTTPPARA="CID",1
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_CID, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_CID));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = 0x31;
            break;
        case HTTP_PARAM_URL_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_URL, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_URL));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], Http.EepromAdd.StartAddHttpUrlLink, sizeof(HTTP_URL_LINK));
            if (Http.HttpFlag.Bit.AddHttpParameters == 1) {
                Http.HttpFlag.Bit.AddHttpParameters = 0;
                Index--;    //  Overwrite the '"' char
                do {
                    if (Count < ((sizeof(Http.UrlParameters)/sizeof(Http.UrlParameters[0])) && (Http.UrlParameters[Count] != 0))) {
                        Gsm.GSM_Data_Array[Index++] = Http.UrlParameters[Count++];
                    } else {
                        break;
                    }
                } while (Index < ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 5));
                Gsm.GSM_Data_Array[Index++] = ASCII_QUOTATION_MARKS;
            }
            break;
        case HTTP_PARAM_UA_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_UA, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_UA));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], Http.EepromAdd.StartAddHttpUA, sizeof(HTTP_UA));
            break;
        case HTTP_PARAM_PROIP_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_PROIP, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_PROIP));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], Http.EepromAdd.StartAddHttpProxyIp, sizeof(HTTP_PROXY_IP));
            break;
        case HTTP_PARAM_PROPORT_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_PROPORT, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_PROPORT));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], Http.EepromAdd.StartAddHttpProxyPort, sizeof(HTTP_PROXY_PORT));
            break;
        case HTTP_PARAM_REDIR_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_REDIR, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_REDIR));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = (Http.HttpFlag.Bit.HttpRedir + 0x30);
            break;
        case HTTP_PARAM_BREAK_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_BREAK, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_BREAK));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Index += Gsm.ConvertNumberToString(Http.HttpBreak, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 5) + 1;
            break;
        case HTTP_PARAM_BREAKEND_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_BREAKEND, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_BREAKEND));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Index += Gsm.ConvertNumberToString(Http.HttpBreakEnd, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 5) + 1;
            break;
        case HTTP_PARAM_TIMEOUT_CODE:
            Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_TIMEOUT, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_TIMEOUT));
            Index = strlen(Gsm.GSM_Data_Array);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Index += Gsm.ConvertNumberToString(Http.HttpTimeOut, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 5) + 1;
            break;
//      case HTTP_PARAM_CONTENT_CODE:
//          Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_CONTENT, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_CONTENT));
//          break;
//      case HTTP_PARAM_USERDATA_CODE:
//          Gsm.ReadStringFLASH((uint8_t *)HTTP_PARAM_USERDATA, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_PARAM_USERDATA));
//          break;
        default:
            break;
    }
    return(strlen(Gsm.GSM_Data_Array));
}
/****************************************************************************/

/****************************************************************************
 * Function:        EepromStartAddSetup
 *
 * Overview:        This function sets the Start address for URL Link, Proxy IP and Proxy Port saved on the EEPROM memory
 *
 * PreCondition:    This function need Arduino Uno, Fishino Uno, Arduino Mega 2560 or Fishino Mega 2560
 *
 * GSM cmd syntax:  None    
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/ 
 void HttpCmd_GSM::EepromStartAddSetup(void) {
    Http.EepromAdd.StartAddHttpUrlLink   = &HTTP_URL_LINK[0];
    Http.EepromAdd.StartAddHttpProxyIp   = &HTTP_PROXY_IP[0];
    Http.EepromAdd.StartAddHttpProxyPort = &HTTP_PROXY_PORT[0];
    Http.EepromAdd.StartAddHttpUA        = &HTTP_UA[0];
 }
/****************************************************************************/