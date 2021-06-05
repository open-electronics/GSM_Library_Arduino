/*********************************************************************
 *
 *       TcpIp command for GSM
 *
 *********************************************************************
 * FileName:        TcpIpCmd_GSM.cpp
 * Revision:        1.0.0 (First issue)
 * Date:            15/10/2018
 *
 * Revision:        1.2.0
 *                  30/11/2019
 *                  - Fixed bug "ClearBuffer" function [sizeof parameter]
 * 
 * Revision:        1.3.0
 *                  06/07/2020
 *					- Added new AT command AT+QIDNSIP=<n>    (Quectel M95)
 *					- Added new AT command AT+CIPMODE=<Mode> (SIMCOM SIM800C; SIM900; SIM928A) or AT+QIMODE=<Mode> (Quectel M95)
 *                  - Added new AT command AT+CIPACK         (SIMCOM SIM800C; SIM900; SIM928A) or AT+QISACK        (Quectel M95)
 *					- Changed ChipStatus processing code
 *                  - Added new init command AT+CIPRXGET=<n> (SIMCOM SIM800C; SIM900; SIM928A) or AT+QINDI=<n>     (Quectel M95)
 *					- Added new AT command AT+CIPPING=...    (SIMCOM SIM800C; SIM900; SIM928A) or AT+QPING=...     (Quectel M95)
 *					- Changed function name "TcpIpEepromStartAddSetup" to "EepromStartAddSetup"
 *					- Changed array name "TcpIpEepromAdd" to "EepromAdd"
 * 
 * Dependencies:    TcpIpCmd_GSM.h
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
#include "GenericCmd_GSM.h"
#include "SecurityCmd_GSM.h"
#include "GprsCmd_GSM.h"
#include "TcpIpCmd_GSM.h"

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
 * Function:        SetCmd_AT_CIPACK
 *
 * Overview:        This function is used to Select TCP/IP Transfer Mode. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is: AT+QISACK for Quectel M95
 *					The syntax for this command is: AT+CIPACK fo Simcom SIM800C; SIM900 or SIM928A
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <n>
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPACK(void) {
        
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.ReadStringFLASH((uint8_t *)ATQ_CIPACK, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CIPACK));
        Gsm.WritePointer = strlen(ATQ_CIPACK);
		Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_ATQ_CIPACK, ANSWER_TCP_IP_AT_CMD_STATE);
    }
    return(1);      //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPMODE
 *
 * Overview:        This function is used to Select TCP/IP Transfer Mode. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is: AT+QIMODE=<Mode> for Quectel M95
 *					The syntax for this command is: AT+CIPMODE=<Mode> for Simcom SIM800C; SIM900 or SIM928A
 *
 * Input:           Mode: 0 to 1. A numeric parameter which indicates:  "0" Normal mode. In this mode, the data should be sent by the command AT+QISEND
 *																		"1" Transparent mode. In this mode, UART will enter data mode after TCP/UDP
 *																			connection has been established. In data mode, all input data from UART will be
 *																			sent to the remote end. +++ can help to switch data mode to command mode. And
 *																			then ATO can help to switch command mode to data mode
 *
 * Command Note:    <Mode>: A numeric parameter select TCP/IP transfer mode
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <n>
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPMODE(uint8_t Mode) {
    uint8_t Index;
        
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Mode;
        Gsm.ReadStringFLASH((uint8_t *)AT_CIPMODE, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPMODE));
		Index = CIPMODE_N_OFFSET;
		Gsm.GSM_Data_Array[Index++] = (Mode + 0x30);
		Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
        Gsm.WritePointer = Index;
		Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPMODE, ANSWER_TCP_IP_AT_CMD_STATE);
    }
    return(1);      //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_QIDNSIP
 *
 * Overview:        This function is used to set if connect with IP Address or Domain Name Server. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is: AT+QIDNSIP=<n>
 *
 * Input:           n: 0 to 1. A numeric parameter which indicates: "0" The address of the remote server is a dotted decimal IP address;
 *																	"1" The address of the remote server is a domain name
 *
 * Command Note:    <n>: A numeric parameter which indicates if connect with IP Address or Domain Name Server
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <n>
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#ifdef QUECTEL_M95
uint8_t TcpIpCmd_GSM::SetCmd_AT_QIDNSIP(uint8_t n) {
    uint8_t Index;
        
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = n;
        Gsm.ReadStringFLASH((uint8_t *)AT_QIDNSIP, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_QIDNSIP));
		Index = QIDNSIP_N_OFFSET;
		Gsm.GSM_Data_Array[Index++] = (n + 0x30);
		Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
        Gsm.WritePointer = Index;
		Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_QIDNSIP, ANSWER_TCP_IP_AT_CMD_STATE);
    }
    return(1);      //  Command sent
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPSTATUS
 *
 * Overview:        This function is used to Query Current Connection Status. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is: AT+CIPSTATUS
 *                  The answer in Single IP mode is: STATE:<state>
 *                  The answer in Multi IP mode is: 
 *                  If the module is set as Server -> S: 0, <bearer>, <port>, <server state>
 *                  If the module is set as Client -> C: <n>,<bearer>, <TCP/UDP>, <IP address>, <port>, <client state>
 *
 *                  The alternartive syntax for this command is: AT+CIPSTATUS=<n>
 *                  The answer is: +CIPSTATUS: <n>,<bearer>, <TCP/UDP>, <IP address>, <port>, <client state>
 *
 * Input:           n: 0 to 2. A numeric parameter which indicates the connection used. Only in Multi IP mode
 *
 * Command Note:    <n>: A numeric parameter which indicates the connection used
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <n>             0-5 A numeric parameter which indicates the connection number. Supported 0 to 2 connections
 *                  <bearer>        0-1 GPRS bearer, default is 0
 *                  <Server State>  OPENING
 *                                  LISTENING
 *                                  CLOSING
 *                  <Client State>  INITIAL
 *                                  CONNECTING
 *                                  CONNECTED
 *                                  REMOTE CLOSING
 *                                  CLOSING
 *                                  CLOSED
 *                  <State>         A string parameter which indicates the progress of connecting
 *                                  0 IP INITIAL 
 *                                  1 IP START
 *                                  2 IP CONFIG
 *                                  3 IP GPRSACT
 *                                  4 IP STATUS
 *                                  5 TCP CONNECTING/UDP CONNECTING /SERVER LISTENING
 *                                  6 CONNECT OK
 *                                  7 TCP CLOSING/UDP CLOSING
 *                                  8 TCP CLOSED/UDP CLOSED
 *                                  9 PDP DEACT
 *                                  In Multi-IP state:
 *                                  0 IP INITIAL 
 *                                  1 IP START
 *                                  2 IP CONFIG
 *                                  3 IP GPRSACT
 *                                  4 IP STATUS
 *                                  5 IP PROCESSING
 *                                  9 PDP DEACT
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPSTATUS(uint8_t n) {
    uint8_t Index;
        
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
		Gsm.GsmFlag.Bit.UartTimeOutSelect    = T_5S_UART_TIMEOUT;
        Gsm.BckCmdData[0] = n;
        Gsm.ReadStringFLASH((uint8_t *)ATQ_CIPSTATUS, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CIPSTATUS));
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP connection
            Index = CIPSTATUS_OFFSET;
            Gsm.GSM_Data_Array[Index++] = ASCII_EQUAL;
            Gsm.GSM_Data_Array[Index++] = (n + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
            Gsm.WritePointer = Index;
        } else {
            Index = CIPSTATUS_OFFSET;
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;
        }
        Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_ATQ_CIPSTATUS, ANSWER_TCP_IP_AT_CMD_STATE);
#endif
#ifdef QUECTEL_M95
        Gsm.ReadStringFLASH((uint8_t *)ATQ_CIPSTATUS, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CIPSTATUS));
        Gsm.WritePointer = strlen(ATQ_CIPSTATUS);
        Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_ATQ_CIPSTATUS, ANSWER_TCP_IP_AT_CMD_STATE);
#endif
    }
    return(1);      //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPSTART
 *
 * Overview:        This function is used to Start Up TCP or UDP Connection. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is: 
 *                  Single IP connection -> AT+CIPSTART=<mode>,<IP address>,<port> or AT+CIPSTART=<mode>,<domain name>,<port>
 *                  Multi  IP connection -> AT+CIPSTART=<n>,<mode>,<IP address>,<port> or AT+CIPSTART=<n>,<mode>,<domain name>,<port>
 *                  The answers for Single IP connection are: OK; +CME ERROR<err>; ALREADY CONNECT; CONNECT OK; CONNECTED FAIL or STATE:<state>
 *                  The answers for Multi  IP connection are: OK; +CME ERROR<err>; <n>,ALREADY CONNECT; <n>,CONNECT OK; <n>,CONNECTED FAIL or STATE:<state>
 *                  The state values for single IP connection are: 0 IP INITIAL
 *                                                                 1 IP START
 *                                                                 2 IP CONFIG
 *                                                                 3 IP GPRSACT
 *                                                                 4 IP STATUS
 *                                                                 5 TCP CONNECTING/UDP CONNECTING/SERVER LISTENING
 *                                                                 6 CONNECT OK
 *                                                                 7 TCP CLOSING/UDP CLOSING
 *                                                                 8 TCP CLOSED/UDP CLOSED
 *                                                                 9 PDP DEACT
 *                  The state values for multi IP connection are:  0 IP INITIAL
 *                                                                 1 IP START
 *                                                                 2 IP CONFIG
 *                                                                 3 IP GPRSACT
 *                                                                 4 IP STATUS
 *                                                                 5 IP PROCESSING
 *                                                                 9 PDP DEACT
 * Input:           n:              Integer value 0 to 2. A numeric parameter which indicates the connection number
 *                  ConnectionMode: Type of connection: "0" TCP Connection; "1" UDP Connection
 *                  IPorDomainName:	if "0" use IP address setted; if "1" use domain name setted
 *
 * Command Note:    <n>             Integer value 0 to 5. A numeric parameter which indicates the connection number
 *                  <mode>          A string parameter which indicates the connection type: "TCP" o "UDP"
 *                  <IP Address>    A string parameter which indicates remote server IP address
 *                  <port>          Remote server port
 *                  <domain name>   A string parameter which indicates remote server domain name
 *                  <state>         A string parameter which indicates the progress of connecting
 *                                  0 IP INITIAL 
 *                                  1 IP START
 *                                  2 IP CONFIG
 *                                  3 IP GPRSACT
 *                                  4 IP STATUS
 *                                  5 TCP CONNECTING/UDP CONNECTING /SERVER LISTENING
 *                                  6 CONNECT OK
 *                                  7 TCP CLOSING/UDP CLOSING
 *                                  8 TCP CLOSED/UDP CLOSED
 *                                  9 PDP DEACT
 *                                  In Multi-IP state:
 *                                  0 IP INITIAL 
 *                                  1 IP START
 *                                  2 IP CONFIG
 *                                  3 IP GPRSACT
 *                                  4 IP STATUS
 *                                  5 IP PROCESSING
 *                                  9 PDP DEACT
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
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPSTART(uint8_t n, uint8_t ConnectionMode, uint8_t IPorDomainName) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
        Gsm.GsmFlag.Bit.UartTimeOutSelect    = T_5S_UART_TIMEOUT;
        Gsm.BckCmdData[0] = n;
        Gsm.BckCmdData[1] = ConnectionMode;
        Gsm.BckCmdData[2] = IPorDomainName;
        Gsm.ReadStringFLASH((uint8_t *)AT_CIPSTART, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPSTART));
        Index = CIPSTART_N_OFFSET;
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP connection
            Gsm.GSM_Data_Array[Index++] = (n + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
        }
        if (ConnectionMode == TCP_CONNECTION) {
            //  TCP Connection
            Gsm.ReadStringFLASH((uint8_t *)TCP_STR, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(TCP_STR));
            Index += strlen(TCP_STR);
        } else {
            //  UDP Connection
            Gsm.ReadStringFLASH((uint8_t *)UDP_STR, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(UDP_STR));
            Index += strlen(UDP_STR);
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
        if (IPorDomainName == IP_ADDRESS) {
            //  IP address
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteServerIp[n], sizeof(REMOTE_SERVER_IP_0));
        } else {
            //  Domain Server
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteDomainServer[n], sizeof(REMOTE_DOMAIN_SERVER_0));
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
		Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteServerPort[n], sizeof(REMOTE_SERVER_PORT_0));
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
        Gsm.WritePointer = Index;
        Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPSTART, ANSWER_TCP_IP_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPCLOSE
 *
 * Overview:        This function is used to Close TCP or UDP Connection. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is:
 *                  Single IP connection -> AT+CIPCLOSE=<n>
 *                  Multi  IP connection -> AT+CIPCLOSE=<id>,[<n>]
 *
 * Input:           n:  "0" Slow close; "1" Quick close (Only for SIMCOM)
 *                  id: A numeric parameter which indicates the connection number
 *
 * Command Note:    <n>     "0" Slow close; "1" Quick close (Only for SIMCOM)
 *                  <id>    A numeric parameter which indicates the connection number
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
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPCLOSE(uint8_t n, uint8_t id) {
    uint8_t Index;
        
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = n;
        Gsm.BckCmdData[1] = id;
        Gsm.ReadStringFLASH((uint8_t *)AT_CIPCLOSE, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPCLOSE));
        Index = CIPCLOSE_OFFSET;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP connection
			Gsm.GSM_Data_Array[Index++] = ASCII_EQUAL;
            Gsm.GSM_Data_Array[Index++] = (id + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = (n + 0x30);
        } else {
			Gsm.GSM_Data_Array[Index++] = ASCII_EQUAL;
            Gsm.GSM_Data_Array[Index++] = (n + 0x30);
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;
        Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPCLOSE, ANSWER_TCP_IP_AT_CMD_STATE);        
#endif
#ifdef QUECTEL_M95
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP connection
            Gsm.GSM_Data_Array[Index++] = ASCII_EQUAL;
            Gsm.GSM_Data_Array[Index++] = (id + 0x30);
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;
        Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPCLOSE, ANSWER_TCP_IP_AT_CMD_STATE);
#endif      
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPSEND
 *
 * Overview:        This function is used to Send Data Through TCP or UDP Connection. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is:
 *                  Single IP connection -> AT+CIPSEND
 *                  Multi  IP connection -> AT+CIPSEND=<n> (If AT+CMUX = 1)
 *
 * Input:           Query: Identifies if the command must send data or to read the max length for data
 *                         If "False" Sends data trough TCP or UDP connection
 *                         If "True"  Reads the max length for data
 *                  n      A numeric parameter which indicates the connection number
 *
 * Command Note:    <n>    A numeric parameters which indicates the connection number
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  response ">", then type data for send. This command is split in two steps
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPSEND(bool Query, uint8_t n) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
        Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
        Gsm.BckCmdData[0] = Query;
        Gsm.BckCmdData[1] = n;
        if (Query == false) {
            Gsm.ReadStringFLASH((uint8_t *)AT_CIPSEND, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPSEND));
            Index = CIPSEND_N_OFFSET;
            if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
                //  Multi IP connection
                Gsm.GSM_Data_Array[Index++] = ASCII_EQUAL;
                Gsm.GSM_Data_Array[Index++] = (n + 0x30);
            }
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;
            Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPSEND_STEP1, ANSWER_TCP_IP_AT_CMD_STATE);
        } else {
            Gsm.ReadStringFLASH((uint8_t *)ATQ_CIPSEND, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CIPSEND));
            Gsm.WritePointer = strlen(ATQ_CIPSEND);
            Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_ATQ_CIPSEND, ANSWER_TCP_IP_AT_CMD_STATE);
        }   
    }
    return(1);          //  Command sent
}

void TcpIpCmd_GSM::SetCmd_AT_CIPSEND_Step2(uint8_t Index) {
    Gsm.ResetAllFlags();
    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_1S5_UART_TIMEOUT;	//T_2S5_UART_TIMEOUT;
#endif
    Gsm.WritePointer = Index;
    Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPSEND_STEP2, ANSWER_TCP_IP_AT_CMD_STATE);
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPRXGET
 *
 * Overview:        This function is used to retrieve the Received TCP/IP Data. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is:
 *                  Single IP connection -> AT+CIPRXGET=<mode>[,<reqlength>]		for SIMCOM SIM800C; SIM900; SIM928A
 *                  Multi  IP connection -> AT+CIPRXGET=<mode>[,<id>,<reqlength>]	for SIMCOM SIM800C; SIM900; SIM928A
 *											AT+QIRD=<id>,<sc>,<sid>,<len>           for Quectel M95
 *
 * Input:           Mode:    Parameter used only for SIMCOM SIM800C; SIM900; SIM928A
 *                           0 - Disable getting data from network manually, the module is set to normal mode, data will be pushed to TE directly.
 *                           1 - Enable getting data from network manually.
 *                           2 - The module can get data, but the length of output data can not exceed 1460 bytes at a time.
 *                           3 - Similar to mode 2, but in HEX mode, which means the module can get 730 bytes maximum at a time.
 *                           4 - Query how many data are not read with a given ID
 *                  n:       A numeric parameter which indicates the connection numbe. Used only for SIMCOM SIM800C; SIM900; SIM928A
 *                  DataLen: Requested number of data bytes (1-1460 bytes)to be read
 *
 * Command Note:    <n>    A numeric parameters which indicates the connection number
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
	uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPRXGET(uint8_t Mode, uint8_t n, uint16_t DataLen) {		
		uint8_t Index;
		
		if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
			return(0);      //  System Busy
		} else {
			Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
			Gsm.ResetGsmFlags();
			Gsm.ResetTcpIpFlags(n);
			Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
			TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
			Gsm.BckCmdData[0] = Mode;
			Gsm.BckCmdData[1] = n;
			Gsm.BckCmdData[2] = (uint8_t)(DataLen >> 8);      //	MSB Byte
			Gsm.BckCmdData[3] = (uint8_t)(DataLen & 0x00FF);  //	LSB Byte

            Gsm.ReadStringFLASH((uint8_t *)AT_CIPRXGET, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPRXGET));
            Index = CIPRXGET_OFFSET;
            Gsm.GSM_Data_Array[Index++] = (Mode + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
				Gsm.GSM_Data_Array[Index++] = (n + 0x30);
				Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			}
			Index += Gsm.ConvertNumberToString(DataLen, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);      
			Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
			Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
			Gsm.WritePointer = Index;
			Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPRXGET, ANSWER_TCP_IP_AT_CMD_STATE);
		}
		return(1);          //  Command sent
	}
#endif
#ifdef QUECTEL_M95
	uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPRXGET(uint16_t DataLen) {
		uint8_t Index;
		
		if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
			return(0);      //  System Busy
		} else {
			Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
			Gsm.ResetGsmFlags();
			Gsm.ResetTcpIpFlags(TcpIp.TcpIpHandleReceivedDataFlag.Bit.Sid);
			Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
			TcpIp.TcpIpFlag.Bit.IndexConnection  = TcpIp.TcpIpHandleReceivedDataFlag.Bit.Sid;
			Gsm.BckCmdData[0] = (uint8_t)(DataLen >> 8);      //	MSB Byte
			Gsm.BckCmdData[1] = (uint8_t)(DataLen & 0x00FF);  //	LSB Byte

            Gsm.ReadStringFLASH((uint8_t *)AT_CIPRXGET, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPRXGET));
            Index = CIPRXGET_OFFSET;
            Gsm.GSM_Data_Array[Index++] = (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Id + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Sc + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Sid + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
			Index += Gsm.ConvertNumberToString(DataLen, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);      
			
			Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
			Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
			Gsm.WritePointer = Index;
			Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPRXGET, ANSWER_TCP_IP_AT_CMD_STATE);
		}
		return(1);          //  Command sent
	}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CIPPING
 *
 * Overview:        This function is used to send ping request. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax for this command is: 
 *                  AT+CIPPING=<IP address>,[,<retryNumber>[,<dataLen>[,<timeout>]]] for SIMCOM SIM800C; SIM900; SIM928A
 *                  AT+QPING="<host>"[,[<timeout][,<pingNum>]] for (Quectel M95)
 *                  
 * Input:           n               Integer value 1 to 3     A numeric parameter which indicates the connection number
 *					RetryNum:		Integer value 1 to 100.  The number of Ping Echo Requset to send. Default 4 (For M95 Max 10 Retry)
 *					EchoDataLen:	Integer value 0 to 1024. The length of Ping Echo Request data. Default 32
 *					TimeOut:		Integer value 1 to 600.  The timeout,in units of 100 ms,waiting for a single Echo Reply. Default: 100 (10 seconds) (For M95 Max TimeOut 255)
 *					IPorDomainName:	Integer value 0 or 1.    If "0" use IP address setted; if "1" use domain name setted
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
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPPING(uint8_t n, uint8_t RetryNum, uint16_t EchoDataLen, uint16_t TimeOut, uint8_t Ttl, uint8_t IPorDomainName) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
        Gsm.ResetGsmFlags();
		Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
		TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
        Gsm.GsmFlag.Bit.UartTimeOutSelect    = T_15S_UART_TIMEOUT;
		if (RetryNum > MAX_PING_NUM)         { RetryNum    = MAX_PING_NUM;      }		
		if (EchoDataLen > MAX_ECHO_DATA_LEN) { EchoDataLen = MAX_ECHO_DATA_LEN; }		
		if (TimeOut > MAX_TIME_OUT)          { TimeOut     = MAX_TIME_OUT;      }
		if (Ttl > MAX_TIME_OUT)              { Ttl         = MAX_TTL;           }
		Gsm.BckCmdData[0] = n;
        Gsm.BckCmdData[1] = RetryNum;
        Gsm.BckCmdData[2] = (uint8_t)(EchoDataLen >> 8);      	//	MSB Byte
		Gsm.BckCmdData[3] = (uint8_t)(EchoDataLen & 0x00FF);  	//	LSB Byte
        Gsm.BckCmdData[4] = (uint8_t)(TimeOut >> 8);      		//	MSB Byte
		Gsm.BckCmdData[5] = (uint8_t)(TimeOut & 0x00FF);  		//	LSB Byte
		Gsm.BckCmdData[6] = Ttl;
		Gsm.BckCmdData[7] = IPorDomainName;
        Gsm.ReadStringFLASH((uint8_t *)AT_CIPPING, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPPING));
        Index = CIPPING_OFFSET;
        if (IPorDomainName == IP_ADDRESS) {
            //  Use IP address
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteServerIp[n], sizeof(REMOTE_SERVER_IP_0));
        } else {
            //  Use Domain Server
			Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteDomainServer[n], sizeof(REMOTE_DOMAIN_SERVER_0));
        }
		Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
		Index += Gsm.ConvertNumberToString(RetryNum, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);
		Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
		Index += Gsm.ConvertNumberToString(EchoDataLen, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);
		Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
		Index += Gsm.ConvertNumberToString(TimeOut, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);
		Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
		Index += Gsm.ConvertNumberToString(Ttl, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);
#endif
#ifdef QUECTEL_M95
		Index += Gsm.ConvertNumberToString(TimeOut, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);
		Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
		Index += Gsm.ConvertNumberToString(RetryNum, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0);
#endif
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
        Gsm.WritePointer = Index;
        Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPPING, ANSWER_TCP_IP_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        GprsWaitAnswer
 *
 * Overview:        This function process the AT command answer of the command sent.
 *                  The answer received and processed by this code regard the GPRS Command Functions
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
void TcpIpCmd_GSM::TcpIpWaitAnswer(void) {
    uint8_t  Index        = 0;
	uint16_t ArrayPointer = 0;
    
	uint16_t ArrayPointer2 = 0;
	
    if ((Gsm.StateSendCmd != CMD_TCP_IP_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) { return; }
    if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) { return; }	
    if (Gsm.UartFlag.Bit.ReceivedAnswer    == 0) { return; }

    Gsm.UartFlag.Bit.ReceivedAnswer = 0;
    if (Gsm.GsmFlag.Bit.CringOccurred == 1) {
        //  CRING OCCURRED. CMD SEND ABORTED
        Gsm.RetryCounter = 0;
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
        Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        return;
    }

    if (Gsm.ReadPointer > 0) {
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPSEND, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPSEND), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            switch (Gsm.StateWaitAnswerCmd)
            {
                case WAIT_ANSWER_CMD_AT_CIPSEND_STEP2:
					Gsm.RetryCounter = 0;
                    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
                    Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE; 
                    break;
                default:
                    break;
            }
			Index = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPRXGET, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPRXGET), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1));
			if (Index != 0xFF) {
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
				TcpIp.ProcessSimcomCipRxGetAnswer(Index);
#endif
			}
		} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPRXGET, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPRXGET), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            switch (Gsm.StateWaitAnswerCmd)
            {
                case WAIT_ANSWER_CMD_AT_CIPRXGET:
                    Gsm.RetryCounter = 0;
                    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
					Index = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPRXGET, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPRXGET), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1));
					if (Index != 0xFF) {
						TcpIp.ProcessSimcomCipRxGetAnswer(Index);
					}
#endif
#ifdef QUECTEL_M95
					TcpIp.ProcessQuectelCipRxGetAnswer();
#endif
                    Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE; 
                    break;
                default:
                    break;
            }
		} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            Gsm.RetryCounter = 0;
            Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;			
            switch (Gsm.StateWaitAnswerCmd)
            {
                case WAIT_ANSWER_CMD_AT_CIPSTART:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_ALREADY_CONNECT, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_ALREADY_CONNECT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_ALREADY_CONNECT_CODE;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CONNECT_OK, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CONNECT_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_CONNECT_OK_CODE;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CONNECT_FAIL, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CONNECT_FAIL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_CONNECT_FAIL_CODE;
                    }
                    break;
                case WAIT_ANSWER_CMD_AT_CIPCLOSE:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CLOSE_OK, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CLOSE_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_NOT_CONNECTED_CODE;
                    }
                    break;                  
                case WAIT_ANSWER_CMD_ATQ_CIPSTATUS:
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
                    ProcessSimcomCipStatusAnswer();
#endif
#ifdef QUECTEL_M95
                    ProcessQuectelCipStatusAnswer();
#endif
                    break;
				case WAIT_ANSWER_CMD_ATQ_CIPACK:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPACK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPACK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
						if (Gsm.FindColonCommaCarriageRet(0) != 0xFF) {
							TcpIp.QueryPrevDataTrasmittedState.DataSent      = Gsm.ExtractParameterWord((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2)));
							TcpIp.QueryPrevDataTrasmittedState.DataSentAck   = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 2), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 2)));
							TcpIp.QueryPrevDataTrasmittedState.DataSentNoAck = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 2), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 2)));
						}
					}
					break;
				case WAIT_ANSWER_CMD_AT_CIPPING:
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
					Gsm.ClearBuffer((uint8_t *)Gsm.TempStringCompare, ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1));
					Gsm.ReadStringFLASH((uint8_t *)AT_ANSW_CIPPING, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPPING));
					do {
						TcpIp.TcpIpPing[Index].Flags.FlagByte = 0;
						TcpIp.TcpIpPing[Index].PingTime       = 0;
						TcpIp.TcpIpPing[Index].Ttl            = 0;
						
						Gsm.TempStringCompare[ANSW_CIPPING_OFFSET] = ASCII_SPACE;
						Gsm.ConvertNumberToString(Index, ((uint8_t *)(Gsm.TempStringCompare) + ANSW_CIPPING_OFFSET + 1), 0);
						ArrayPointer = strstr((char *)(&Gsm.GSM_Data_Array[0]), (char *)(Gsm.TempStringCompare));
						if (ArrayPointer > 0) {
							if (Gsm.FindColonCommaCarriageRet(ArrayPointer - (uint16_t)((uint8_t *)(Gsm.GSM_Data_Array))) != 0xFF) {
								TcpIp.TcpIpPing[Index].Flags.Bit.DataOk = 1;								
								TcpIp.TcpIpPing[Index].PingTime = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));
								TcpIp.TcpIpPing[Index].Ttl      = Gsm.ExtractParameterWord((Gsm.CharPointers[3] + 1), (Gsm.CharPointers[4] - (Gsm.CharPointers[3] + 1)));
								if ((TcpIp.TcpIpPing[Index].PingTime == MAX_TIME_OUT) && (TcpIp.TcpIpPing[Index].Ttl == MAX_TTL)) {
									TcpIp.TcpIpPing[Index].Flags.Bit.PingTimeOut = 1;
								} else {
									TcpIp.TcpIpPing[Index].Flags.Bit.PingOk = 1;
								}

							}
						}
					} while (Index++ < MAX_PING_NUM);
#endif
					break;
                default:
                    break;  
            }
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_MAJOR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_MAJOR), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) { 
            switch (Gsm.StateWaitAnswerCmd)
            {
                case WAIT_ANSWER_CMD_AT_CIPSEND_STEP1:
                    Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], ((sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])) - 1));
                    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
                    TcpIp.TcpIpFlag.Bit.SetsDataToSent   = 1;
                    break;
                default:
                    break;
            }
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        } else {
            Gsm.ProcessGsmError();
            TcpIpRetrySendCmd();
        }               
    } else {
        //  If no answer from GSM module was received, this means that the module is switched off
        Gsm.InitReset_GSM();
    }
}   

void TcpIpCmd_GSM::TcpIpRetrySendCmd(void) {
    uint8_t  AnswerCmdStateBackup;
    uint16_t TempData;
	
    if (Gsm.RetryCounter++ < 2) {
        AnswerCmdStateBackup = Gsm.StateWaitAnswerCmd;
        Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        switch (AnswerCmdStateBackup)
        {
#ifdef QUECTEL_M95				
			case WAIT_ANSWER_CMD_AT_QIDNSIP:
				SetCmd_AT_QIDNSIP(Gsm.BckCmdData[0]);
				break;
			case WAIT_ANSWER_CMD_AT_CIPMODE:
				SetCmd_AT_CIPMODE(Gsm.BckCmdData[0]);
				break;
#endif
            case WAIT_ANSWER_CMD_AT_CIPSTART:
                SetCmd_AT_CIPSTART(Gsm.BckCmdData[0], Gsm.BckCmdData[1], Gsm.BckCmdData[2]);
                break;              
            case WAIT_ANSWER_CMD_AT_CIPCLOSE:
                SetCmd_AT_CIPCLOSE(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;
                
            case WAIT_ANSWER_CMD_AT_CIPSEND_STEP1:
                SetCmd_AT_CIPSEND(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;
                
            case WAIT_ANSWER_CMD_AT_CIPSEND_STEP2:
                break;
                
            case WAIT_ANSWER_CMD_ATQ_CIPSEND:
                break;
                
            case WAIT_ANSWER_CMD_ATQ_CIPSTATUS:
                SetCmd_AT_CIPSTATUS(Gsm.BckCmdData[0]);
                break;
			case WAIT_ANSWER_CMD_ATQ_CIPACK:
				SetCmd_AT_CIPACK();
				break;
			case WAIT_ANSWER_CMD_AT_CIPPING:
				SetCmd_AT_CIPPING(Gsm.BckCmdData[0], Gsm.BckCmdData[1], ((Gsm.BckCmdData[2] << 8) | Gsm.BckCmdData[3]), ((Gsm.BckCmdData[4] << 8) | Gsm.BckCmdData[5]), Gsm.BckCmdData[6], Gsm.BckCmdData[7]);
				break;
			case WAIT_ANSWER_CMD_AT_CIPRXGET:
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
				SetCmd_AT_CIPRXGET(Gsm.BckCmdData[0], Gsm.BckCmdData[1], ((Gsm.BckCmdData[2] << 8) | Gsm.BckCmdData[3]));
#endif
#ifdef QUECTEL_M95
				SetCmd_AT_CIPRXGET(((Gsm.BckCmdData[0] << 8) | Gsm.BckCmdData[1]));
#endif
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
/****************************************************************************/

/****************************************************************************
 * Function:        ProcessSimcomCipStatusAnswer
 *
 * Overview:        This function process the answer received by the AT command AT+CIPSTATUS
 *                  Note: This function is usefull for SIMCOM modules only
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
 * Note:            This is a private function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
void TcpIpCmd_GSM::ProcessSimcomCipStatusAnswer(void) {
    if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
        //  Multi IP
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPSTATUS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPSTATUS), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            if (Gsm.FindColonCommaCarriageRet(0) != 0xFF) {
                TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterWord((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
				if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_INITIAL_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_START, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_START), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_START_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CONFIG, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CONFIG), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CONFIG_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_GPRSACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_GPRSACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_GPRSACT_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_STATUS, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_STATUS), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_STATUS_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_PROCESSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_PROCESSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_PROCESSING_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_PDP_DEACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_PDP_DEACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_PDP_DEACT_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_INITIAL_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTING_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTED_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_REMOTE_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_REMOTE_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_REMOTE_CLOSING_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSING_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSED_CODE;
                }
			}
		}
/*		
        if (Gsm.TestAT_Cmd_Answer((uint8_t *)GSM_SET_AS_SERVER, (uint8_t *)Gsm.TempStringCompare, strlen(GSM_SET_AS_SERVER), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            //  Server
            if (Gsm.TestAT_Cmd_Answer((uint8_t *)SERVER_OPENING_CODE, (uint8_t *)Gsm.TempStringCompare, strlen(SERVER_OPENING_CODE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                TcpIp.TcpIpStateFlag.BitState[0].ServerState = SERVER_OPENING_CODE;
            } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)SERVER_LISTENING_CODE, (uint8_t *)Gsm.TempStringCompare, strlen(SERVER_LISTENING_CODE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                TcpIp.TcpIpStateFlag.BitState[0].ServerState = SERVER_LISTENING_CODE;
            } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)SERVER_CLOSING_CODE, (uint8_t *)Gsm.TempStringCompare, strlen(SERVER_CLOSING_CODE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {        
                TcpIp.TcpIpStateFlag.BitState[0].ServerState = SERVER_CLOSING_CODE;
            }         
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)GSM_SET_AS_CLIENT, (uint8_t *)Gsm.TempStringCompare, strlen(GSM_SET_AS_CLIENT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            //  Client
            if (Gsm.FindColonCommaCarriageRet(0) != 0xFF) {
                TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
                if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_INITIAL_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTING_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTED_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_REMOTE_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_REMOTE_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_REMOTE_CLOSING_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSING_CODE;
                } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
                    TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSED_CODE;
                }
            }            
        }
*/
    } else {
        //  Single IP
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_STATE, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_STATE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
			if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_INITIAL_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_START, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_START), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_START_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CONFIG, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CONFIG), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CONFIG_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_GPRSACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_GPRSACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_GPRSACT_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_STATUS, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_STATUS), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_STATUS_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CONNECTING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CONNECTING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_SERVER_LISTENING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_SERVER_LISTENING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_SERVER_LISTENING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_CONNECT_OK, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_CONNECT_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_CONNECT_OK_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CLOSING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CLOSING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CLOSED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CLOSED_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CLOSED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CLOSED_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_PDP_DEACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_PDP_DEACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_PDP_DEACT_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_INITIAL_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTED_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_REMOTE_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_REMOTE_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_REMOTE_CLOSING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSED), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSED_CODE;
			}
		}
    }
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        ProcessQuectelCipStatusAnswer
 *
 * Overview:        This function process the answer received by the AT command AT+CIPSTATUS
 *                  Note: This function is usefull for QUECTEL modules only
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
 * Note:            This is a private function
 *****************************************************************************/
#ifdef QUECTEL_M95
void TcpIpCmd_GSM::ProcessQuectelCipStatusAnswer(void) {
    if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
        //  Multi IP
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_QISTATE, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_QISTATE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
            if (Gsm.FindColonCommaCarriageRet(0) != 0xFF) {
                TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterWord((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));			
				if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_INITIAL_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_START, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_START), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_START_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CONFIG, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CONFIG), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CONFIG_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IND, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IND), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_IND_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_GPRSACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_GPRSACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_GPRSACT_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_STATUS, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_STATUS), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_STATUS_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_PROCESSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_PROCESSING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_PROCESSING_CODE;
				} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_PDP_DEACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_PDP_DEACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
					TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_PDP_DEACT_CODE;
				}
			}
		}
    } else {
        //  Single IP
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_STATE, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_STATE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
			if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_INITIAL), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_INITIAL_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_START, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_START), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_START_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CONFIG, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CONFIG), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CONFIG_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IND, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IND), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_IND_CODE;            
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_GPRSACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_GPRSACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_GPRSACT_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_STATUS, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_STATUS), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_STATUS_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CONNECTING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CONNECTING), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CONNECTING_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CLOSE, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CLOSE), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CLOSE_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_CONNECT_OK, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_CONNECT_OK), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_CONNECT_OK_CODE;
			} else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_PDP_DEACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_PDP_DEACT), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
				TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_PDP_DEACT_CODE;
			}			
		}
    }
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        ProcessSimcomCipRxGetAnswer
 *
 * Overview:        This function process the answer received by the AT command AT+CIPRXGET
 *                  Note: This function is usefull for SIMCOM modules only
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
 * Note:            This is a private function
 *****************************************************************************/
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
void TcpIpCmd_GSM::ProcessSimcomCipRxGetAnswer(uint8_t Index) {
	if (Gsm.FindColonCommaCarriageRet(Index) != 0xFF) {
		Gsm.ClearBuffer(&TcpIp.CipRxGetAnswer.IpAddress[0], ((sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])) - 1));
		Gsm.ClearBuffer(&TcpIp.CipRxGetAnswer.Port[0], ((sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])) - 1));
		
		TcpIp.CipRxGetAnswer.IpAddress[0] = ASCII_QUOTATION_MARKS;
		TcpIp.CipRxGetAnswer.Port[0]      = ASCII_QUOTATION_MARKS;
		
		TcpIp.TcpIpHandleReceivedDataFlag.TcpIpByte = 0;
		TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode = (uint8_t)(Gsm.ExtractParameterWord((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
		
		if (TcpIp.TcpIpFlag.Bit.MultiIp == 1) {
			//	Multi IP connection
			if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode == CIP_RX_MODE_1) {
				TcpIp.TcpIpHandleReceivedDataFlag.Bit.DataReadyToRead = 1;
				TcpIp.TcpIpHandleReceivedDataFlag.Bit.Id = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[2] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[3] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);
			} else if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode == CIP_RX_MODE_2) {
				TcpIp.TcpIpHandleReceivedDataFlag.Bit.Id = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
				TcpIp.CipRxReqLen = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));
				TcpIp.CipRxCnfLen = Gsm.ExtractParameterWord((Gsm.CharPointers[3] + 1), (Gsm.CharPointers[4] - (Gsm.CharPointers[3] + 1)));
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[4] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[5] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);

				TcpIp.CipRxGetAnswer.DataStartPointer = Gsm.CharPointers[6] + 2;
				CipRxGetAnswer.Flags.Bit.DataReady = 1;
			} else if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode == CIP_RX_MODE_3) {
				TcpIp.TcpIpHandleReceivedDataFlag.Bit.Id = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
				TcpIp.CipRxReqLen = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));
				TcpIp.CipRxCnfLen = Gsm.ExtractParameterWord((Gsm.CharPointers[3] + 1), (Gsm.CharPointers[4] - (Gsm.CharPointers[3] + 1)));
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[4] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[5] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);
				
				TcpIp.CipRxGetAnswer.DataStartPointer = Gsm.CharPointers[6] + 2;
				CipRxGetAnswer.Flags.Bit.DataReady = 1;
			} else {
				TcpIp.TcpIpHandleReceivedDataFlag.Bit.Id = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
				TcpIp.CipRxCnfLen = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));					
			}					
		} else {
			//	Single IP connection
			if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode == CIP_RX_MODE_1) {
				TcpIp.TcpIpHandleReceivedDataFlag.Bit.DataReadyToRead = 1;
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[1] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[2] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);
			} else if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode == CIP_RX_MODE_2) {
				TcpIp.CipRxReqLen = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
				TcpIp.CipRxCnfLen = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));				
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[3] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[4] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);
				
				TcpIp.CipRxGetAnswer.DataStartPointer = Gsm.CharPointers[5] + 2;
				CipRxGetAnswer.Flags.Bit.DataReady = 1;
				
			} else if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.Mode == CIP_RX_MODE_3) {
				TcpIp.CipRxReqLen = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
				TcpIp.CipRxCnfLen = Gsm.ExtractParameterWord((Gsm.CharPointers[2] + 1), (Gsm.CharPointers[3] - (Gsm.CharPointers[2] + 1)));
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[3] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
				Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[4] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);

				TcpIp.CipRxGetAnswer.DataStartPointer = Gsm.CharPointers[5] + 2;
				CipRxGetAnswer.Flags.Bit.DataReady = 1;
			} else {
				TcpIp.CipRxCnfLen = Gsm.ExtractParameterWord((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
			}									
		}
		TcpIp.CipRxGetAnswer.IpAddress[strlen(TcpIp.CipRxGetAnswer.IpAddress)] = ASCII_QUOTATION_MARKS;			
		TcpIp.CipRxGetAnswer.Port[strlen(TcpIp.CipRxGetAnswer.Port)]           = ASCII_QUOTATION_MARKS;
	}
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        ProcessQuectelCipRxGetAnswer
 *
 * Overview:        This function process the answer received by the AT command AT+QIRD
 *                  Note: This function is usefull for QUECTEL modules only
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
 * Note:            This is a private function
 *****************************************************************************/
#ifdef QUECTEL_M95
void TcpIpCmd_GSM::ProcessQuectelCipRxGetAnswer(void) {
	if (Gsm.FindColonCommaCarriageRet(0) != 0xFF) {
		Gsm.ClearBuffer(&TcpIp.CipRxGetAnswer.IpAddress[0], ((sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])) - 1));
		Gsm.ClearBuffer(&TcpIp.CipRxGetAnswer.Port[0], ((sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])) - 1));
		
		CipRxGetAnswer.Flags.FlagByte = 0;
		if (Gsm.TestAT_Cmd_Answer((uint8_t *)UDP_STR_2, (uint8_t *)Gsm.TempStringCompare, strlen(UDP_STR_2), ((sizeof(Gsm.TempStringCompare)/sizeof(Gsm.TempStringCompare[0])) - 1)) != 0xFF) {
			CipRxGetAnswer.Flags.Bit.TcpOrUdp = 1;
		}
		
		TcpIp.CipRxGetAnswer.IpAddress[0] = ASCII_QUOTATION_MARKS;
		Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.IpAddress[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[0] + 2]), (sizeof(TcpIp.CipRxGetAnswer.IpAddress)/sizeof(TcpIp.CipRxGetAnswer.IpAddress[0])), ASCII_COLON);
		TcpIp.CipRxGetAnswer.IpAddress[strlen(TcpIp.CipRxGetAnswer.IpAddress)] = ASCII_QUOTATION_MARKS;
		
		TcpIp.CipRxGetAnswer.Port[0] = ASCII_QUOTATION_MARKS;
		Gsm.ExtractCharArray((char *)(&TcpIp.CipRxGetAnswer.Port[1]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[1] + 1]), (sizeof(TcpIp.CipRxGetAnswer.Port)/sizeof(TcpIp.CipRxGetAnswer.Port[0])), ASCII_QUOTATION_MARKS);
		TcpIp.CipRxGetAnswer.Port[strlen(TcpIp.CipRxGetAnswer.Port)] = ASCII_QUOTATION_MARKS;
		
		TcpIp.CipRxGetAnswer.DataLen = Gsm.ExtractParameterWord((Gsm.CharPointers[3] + 1), (Gsm.CharPointers[4] - (Gsm.CharPointers[3] + 1)));
		
		TcpIp.CipRxGetAnswer.DataStartPointer = Gsm.CharPointers[4] + 2;
		CipRxGetAnswer.Flags.Bit.DataReady = 1;
	}
}
#endif
/****************************************************************************/
	
/****************************************************************************
 * Function:        EepromStartAddSetup
 *
 * Overview:        This function sets the Start address for APN, PDP_Type ecc saved on the EEPROM memory
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
 void TcpIpCmd_GSM::EepromStartAddSetup(void) {
    TcpIp.EepromAdd.StartAddRemoteServerIp[0]     = &REMOTE_SERVER_IP_0[0];
    TcpIp.EepromAdd.StartAddRemoteServerIp[1]     = &REMOTE_SERVER_IP_1[0];
    TcpIp.EepromAdd.StartAddRemoteServerIp[2]     = &REMOTE_SERVER_IP_2[0];
    TcpIp.EepromAdd.StartAddRemoteDomainServer[0] = &REMOTE_DOMAIN_SERVER_0[0];
    TcpIp.EepromAdd.StartAddRemoteDomainServer[1] = &REMOTE_DOMAIN_SERVER_1[0];
    TcpIp.EepromAdd.StartAddRemoteDomainServer[2] = &REMOTE_DOMAIN_SERVER_2[0];
    TcpIp.EepromAdd.StartAddRemoteServerPort[0]   = &REMOTE_SERVER_PORT_0[0];
    TcpIp.EepromAdd.StartAddRemoteServerPort[1]   = &REMOTE_SERVER_PORT_1[0];
    TcpIp.EepromAdd.StartAddRemoteServerPort[2]   = &REMOTE_SERVER_PORT_2[0];
 }
/****************************************************************************/
