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
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
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
 *                  IP_Domain:
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
uint8_t TcpIpCmd_GSM::SetCmd_AT_CIPSTART(uint8_t n, uint8_t ConnectionMode, uint8_t IP_Domain) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
        Gsm.GsmFlag.Bit.UartTimeOutSelect    = T_5S_UART_TIMEOUT;
        Gsm.BckCmdData[0] = n;
        Gsm.BckCmdData[1] = ConnectionMode;
        Gsm.BckCmdData[2] = IP_Domain;
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
        if (IP_Domain == 0) {
            //  Use IP address
            Index = Security.LoadDataFromEepromIntoGsmDataArray(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[n], (Index + sizeof(REMOTE_SERVER_IP_0)), Index);
        } else {
            //  Use Domain Server
            Index = Security.LoadDataFromEepromIntoGsmDataArray(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[n], (Index + sizeof(REMOTE_DOMAIN_SERVER_0)), Index);
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
        Index = Security.LoadDataFromEepromIntoGsmDataArray(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[n], (Index + sizeof(REMOTE_SERVER_PORT_0)), Index);
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
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.ResetTcpIpFlags(n);
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        TcpIp.TcpIpFlag.Bit.IndexConnection  = n;
        Gsm.BckCmdData[0] = n;
        Gsm.BckCmdData[1] = id;
        Gsm.ReadStringFLASH((uint8_t *)AT_CIPCLOSE, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPCLOSE));
        Index = CIPCLOSE_OFFSET;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP connection
            Gsm.GSM_Data_Array[Index++] = (id + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = (n + 0x30);
        } else {
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
 * Input:           Query: Identifies if the command must send data or to read the max lenght for data
 *                         If "False" Sends data trough TCP or UDP connection
 *                         If "True"  Reads the max lenght for data
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
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
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
    Gsm.GsmFlag.Bit.UartTimeOutSelect    = T_2S5_UART_TIMEOUT;
    
    Gsm.WritePointer = Index;
    Gsm.StartSendData(CMD_TCP_IP_IDLE, WAIT_ANSWER_CMD_AT_CIPSEND_STEP2, ANSWER_TCP_IP_AT_CMD_STATE);
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
    uint8_t Index;
    
    if ((Gsm.StateSendCmd != CMD_TCP_IP_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }

    if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
        //  System waits data to send
        return;
    }   
    if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) {
        return;
    }
    Gsm.UartFlag.Bit.ReceivedAnswer = 0;
    if (Gsm.GsmFlag.Bit.CringOccurred == 1) {
        //  CRING OCCURRED. CMD SEND ABORTED
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
                case WAIT_ANSWER_CMD_AT_CIPSTART:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_ALREADY_CONNECT, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_ALREADY_CONNECT)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_ALREADY_CONNECT_CODE;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CONNECT_OK, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CONNECT_OK)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_CONNECT_OK_CODE;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CONNECT_FAIL, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CONNECT_FAIL)) != 0xFF) {
                        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_CONNECT_FAIL_CODE;
                    }
                    break;
                case WAIT_ANSWER_CMD_AT_CIPCLOSE:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CLOSE_OK, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CLOSE_OK)) != 0xFF) {
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
                default:
                    break;  
            }
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_MAJOR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_MAJOR)) != 0xFF) { 
            switch (Gsm.StateWaitAnswerCmd)
            {
                case WAIT_ANSWER_CMD_AT_CIPSEND_STEP1:
                    Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
                    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
                    TcpIp.TcpIpFlag.Bit.SetsDataToSent   = 1;
                    break;
                default:
                    break;
            }
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPSEND, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPSEND)) != 0xFF) {
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
#endif
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
    uint8_t AnswerCmdStateBackup;
    
    if (Gsm.RetryCounter++ < 2) {
        AnswerCmdStateBackup = Gsm.StateWaitAnswerCmd;
        Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        switch (AnswerCmdStateBackup)
        {
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
        if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CIPSTATUS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CIPSTATUS)) != 0xFF) {
            if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
            }
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_INITIAL)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_INITIAL_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CONNECTED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CONNECTED)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CONNECTED_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_REMOTE_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_REMOTE_CLOSING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_REMOTE_CLOSING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSED)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState = CLIENT_CLOSED_CODE;
        }
    } else {
        //  Single IP
        if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_INITIAL, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_INITIAL)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_INITIAL_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_START, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_START)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_START_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CONFIG, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CONFIG)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CONFIG_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_GPRSACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_GPRSACT)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_GPRSACT_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_CLOSE, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_CLOSE)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_CLOSE_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_STATUS, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_STATUS)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_STATUS_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_IP_PROCESSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_IP_PROCESSING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_IP_PROCESSING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CONNECTING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CONNECTING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CONNECTING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CONNECTING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CONNECTING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_SERVER_LISTENING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_SERVER_LISTENING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_SERVER_LISTENING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_CONNECT_OK, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_CONNECT_OK)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_CONNECT_OK_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CLOSING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CLOSING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CLOSING, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CLOSING)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CLOSING_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_TCP_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_TCP_CLOSED)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_TCP_CLOSED_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_UDP_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_UDP_CLOSED)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_UDP_CLOSED_CODE;
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)STATE_PDP_DEACT, (uint8_t *)Gsm.TempStringCompare, strlen(STATE_PDP_DEACT)) != 0xFF) {
            TcpIp.TcpIpStateFlag.BitState[0].State_Conn = STATE_PDP_DEACT_CODE;
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
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_QISTATE, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_QISTATE)) != 0xFF) {
        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
            //TcpIp.TcpIpStateFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
        }
    }
    //  TO DO !!!!!!!!!!!!!!!!  
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        TcpIpEepromStartAddSetup
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
 void TcpIpCmd_GSM::TcpIpEepromStartAddSetup(void) {
    TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[0]     = &REMOTE_SERVER_IP_0[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[1]     = &REMOTE_SERVER_IP_1[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[2]     = &REMOTE_SERVER_IP_2[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[0] = &REMOTE_DOMAIN_SERVER_0[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[1] = &REMOTE_DOMAIN_SERVER_1[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[2] = &REMOTE_DOMAIN_SERVER_2[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[0]   = &REMOTE_SERVER_PORT_0[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[1]   = &REMOTE_SERVER_PORT_1[0];
    TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[2]   = &REMOTE_SERVER_PORT_2[0];
 }
/****************************************************************************/
