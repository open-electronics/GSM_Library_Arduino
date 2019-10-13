/*********************************************************************
 *
 *       Gprs command for GSM
 *
 *********************************************************************
 * FileName:        GprsCmd_GSM.cpp
 * Revision:        1.0.0 (First issue)
 * Date:            15/10/2018
 * Dependencies:    GprsCmd_GSM.h
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

/****************************************************************************
 * Function:        SetCmd_AT_CGATT
 *
 * Overview:        This function is used to Attach or Detach from GPRS Service. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  To Attach od Detach GPRS Service use the AT command AT+CGATT=<state>
 *                  To query the state of GPRS Service use the AT command AT+CGATT? and the answer is +CGATT:<state>
 *
 * Input:           Query: Identifies if the command must set GPRS Service or to read its state
 *                         If "False" Sends command to Attach or Detach from GPRS Service
 *                         If "True"  Sends command to read GPRS Service State 
 *                  State: Identifies if you want Attach or Detach GPRS Service. "1" Attach; "0" Detach
 *
 * Command Note:    <State>: Indicates the state of GPRS attachment -> "1" Attached; "0" Detached (Other values are reserved and will result in an ERROR response to the Write Command)
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
uint8_t GprsCmd_GSM::SetCmd_AT_CGATT(bool Query, uint8_t State) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetGprsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Query;
        Gsm.BckCmdData[1] = State;
        if (Query == false) {
            Gsm.ReadStringFLASH((uint8_t *)AT_CGATT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGATT));
            Index = CGATT_STATE_OFFSET;
            Gsm.GSM_Data_Array[Index++] = (State + 0x30);
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
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_CGREG
 *
 * Overview:        This function is used to query Network Registration Status or to set/reset the network
 *                  registration unsolicited result code. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  To Set/Reset unsolicited result code sends the AT command AT+CGREG=<n>
 *                  To query the Network Registration Status sends the AT command AT+CGREG? and the answer is +CGREG:<n>,<stat>[,<lac>,<ci>]
 *
 * Input:           Query: Identifies if the command must Enable/Disable network registration unsolicited result
 *                         code or reads the Network Registration Status.
 *                         If "False" Sends command to Enable/Disable network registration unsolicited result code
 *                         If "True"  Sends command to reads the Network Registration Status
 *                  n:     Identifies if you want Enable/Disable network registration unsolicited result code. "1" Enable; "0" Disable
 *
 * Command Note:    <n>:   "0" Disable network registration unsolicited result code
 *                         "1" Enable network registration unsolicited result code  +CGREG:<stat>
 *                         "2" Enable network registration and location information unsolicited result code +CGREG: <stat>[,<lac>,<ci>]
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <n>     0   Disable network registration
 *                          1   Enable network registration unsolicited result code +CREG:<stat>
 *                          2   Enable network registration unsolicited result code with location information
 *                  <stat>  0   Not registered, MT is not currently searching a new operator to register to. the GPRS service is disabled
 *                          1   Registered, home network
 *                          2   Not registered, but MT is currently try to attach or searching an operator to register to. The GPRS service is enabled, but an allowable PLMN is currently not available
 *                          3   Registration denied. The GPRS service is disabled, the UE is not allowed to attach for GPRS if it is requested by the user
 *                          4   Unknown
 *                          5   Registered, roaming
 *                  <lac>       String type; two byte location area code in hexadecimal format
 *                  <ci>        String type; two byte cell ID in hexadecimal format
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GprsCmd_GSM::SetCmd_AT_CGREG(bool Query, uint8_t n) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetGprsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Query;
        Gsm.BckCmdData[1] = n;
        if (Query == false) {
            Gsm.ReadStringFLASH((uint8_t *)AT_CGREG, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGREG));
            Index = CGREG_PAR_OFFSET;
            Gsm.GSM_Data_Array[Index++] = (n + 0x30);
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
void GprsCmd_GSM::GprsWaitAnswer(void) {

    if ((Gsm.StateSendCmd != CMD_GPRS_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
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
                case WAIT_ANSWER_CMD_ATQ_CGREG:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CGREG, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CGREG)) != 0xFF) {
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                            Gsm.CREG_Info.Flag.Bit.n    = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
                            Gsm.CREG_Info.Flag.Bit.Stat = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1))));
                            if (Gsm.CREG_Info.Flag.Bit.Stat != 0) {
                                Gsm.ExtractCharArray((char *)(&Gsm.CREG_Info.LAC_Info[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[2] + 1]), sizeof(Gsm.CREG_Info.LAC_Info), ASCII_COMMA);
                                Gsm.ExtractCharArray((char *)(&Gsm.CREG_Info.CI_Info[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[3] + 1]), sizeof(Gsm.CREG_Info.CI_Info), ASCII_CARRIAGE_RET);
                            }
                        }
                    }
                    break;                  
                default:
                    break;  
            }
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        } else {
            Gsm.ProcessGsmError();
            GprsRetrySendCmd();
        }               
    } else {
        //  If no answer from GSM module was received, this means that the module is switched off
        Gsm.InitReset_GSM();
    }
}   

void GprsCmd_GSM::GprsRetrySendCmd(void) {
    uint8_t AnswerCmdStateBackup;
    
    if (Gsm.RetryCounter++ < 2) {
        AnswerCmdStateBackup = Gsm.StateWaitAnswerCmd;
        Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        switch (AnswerCmdStateBackup)
        {
            case WAIT_ANSWER_CMD_AT_CGATT:
            case WAIT_ANSWER_CMD_ATQ_CGATT:
                SetCmd_AT_CGATT(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;
                
            case WAIT_ANSWER_CMD_AT_CGREG:
            case WAIT_ANSWER_CMD_ATQ_CGREG:
                SetCmd_AT_CGREG(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
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
 * Function:        InitGprsSendCmd
 *
 * Overview:        This function is used to send the initialization commands for GPRS function.
 *                  The states machine is realized with a Switch statement
 *                  The complementary function of this is the "InitGprsWaitAnswer" function
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
void GprsCmd_GSM::InitGprsSendCmd(void) {
    uint8_t Index;
    
    if ((Gsm.StateInitWaitAnswerCmd != INIT_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }
    
    Gsm.GsmFlag.Bit.GsmPowerOn = 0;
    Gsm.GsmFlag.Bit.GsmReset   = 0;
    
    Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
    switch (Gsm.StateInitSendCmd)
    {
        case INIT_IDLE:
            Gsm.RetryCounter = 0;
            break;  
        case INIT_CMD_QUERY_CGATT:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)ATQ_CGATT, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CGATT));
            Gsm.WritePointer = strlen(ATQ_CGATT);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_QUERY_CGATT, INIT_CMD_AT_NOTHING);        
            break;
        case INIT_CMD_CGATT:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)AT_CGATT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGATT));
            Index = CGATT_STATE_OFFSET;
            Gsm.GSM_Data_Array[Index++] = 0x31;
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CGATT, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CIPMUX:
            Gsm.ReadStringFLASH((uint8_t *)AT_CIPMUX, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPMUX));
            if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
                Gsm.GSM_Data_Array[CIPMUX_OFFSET] = 0x31;
            }
            Gsm.WritePointer = strlen(AT_CIPMUX);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CIPMUX, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CGEREP:
            Gsm.ReadStringFLASH((uint8_t *)AT_CGEREP, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGEREP));
            Gsm.WritePointer = strlen(AT_CGEREP);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CGEREP, INIT_CMD_AT_NOTHING);     
            break;
        case INIT_CMD_CSTT:
            Gsm.ReadStringFLASH((uint8_t *)AT_CSTT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CSTT));
            Index = CSTT_APN_OFFSET;
            Index = Security.LoadDataFromEepromIntoGsmDataArray(Gprs.GprsEepromAdd.StartAddApn, (Index + sizeof(APN)), Index);
            if (TcpIp.TcpIpFlag.Bit.ApnUserNamePassword != 0) {
                Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
                Index = Security.LoadDataFromEepromIntoGsmDataArray(Gprs.GprsEepromAdd.StartAddApnUserName, (Index + sizeof(APN_USER_NAME)), Index);
                Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
                Index = Security.LoadDataFromEepromIntoGsmDataArray(Gprs.GprsEepromAdd.StartAddApnPassword, (Index + sizeof(APN_PASSWORD)), Index);
            }
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;       
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CSTT, INIT_CMD_AT_NOTHING);           
            break;
        case INIT_CMD_CIICR:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)AT_CIICR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIICR));
            Gsm.WritePointer = strlen(AT_CIICR);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CIICR, INIT_CMD_AT_NOTHING);  
            break;
        case INIT_CMD_CIFSR:
            Gsm.ReadStringFLASH((uint8_t *)AT_CIFSR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIFSR));
            Gsm.WritePointer = strlen(AT_CIFSR);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CIFSR, INIT_CMD_AT_NOTHING);  
            break;
        case INIT_CMD_CGDCONT:
            Gsm.ReadStringFLASH((uint8_t *)AT_CGDCONT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGDCONT));
            Index = CGDCONT_OFFSET;
            Index = Security.LoadDataFromEepromIntoGsmDataArray(Gprs.GprsEepromAdd.StartAddApn, (Index + sizeof(APN)), Index);
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;   
            Gsm.StartSendData(INIT_IDLE, INIT_CMD_CGDCONT, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CGACT:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)AT_CGACT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGACT));
            Gsm.WritePointer = strlen(AT_CGACT);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CGACT, INIT_CMD_AT_NOTHING);  
            break;
        case INIT_CMD_CGPADDR:
            Gsm.ReadStringFLASH((uint8_t *)AT_CGPADDR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGPADDR));
            Gsm.WritePointer = strlen(AT_CGPADDR);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CGPADDR, INIT_CMD_AT_NOTHING);            
            break;
        case INIT_CMD_CIPHEAD:
            Gsm.ReadStringFLASH((uint8_t *)AT_CIPHEAD, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPHEAD));
            Gsm.WritePointer = strlen(AT_CIPHEAD);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CIPHEAD, INIT_CMD_AT_NOTHING);    
            break;
        case INIT_CMD_CIPSRIP:
            Gsm.ReadStringFLASH((uint8_t *)AT_CIPSRIP, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CIPSRIP));
            Gsm.WritePointer = strlen(AT_CIPSRIP);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CIPSRIP, INIT_CMD_AT_NOTHING);    
            break;          
        default:
            break;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitGprsWaitAnswer
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
void GprsCmd_GSM::InitGprsWaitAnswer(void) {
    
    if ((Gsm.StateInitSendCmd != INIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }
    if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) {
        return;
    }
    Gsm.UartFlag.Bit.ReceivedAnswer = 0;
    
    switch (Gsm.StateInitWaitAnswerCmd)
    {
        case INIT_WAIT_ANSWER_CMD_QUERY_CGATT:
        case INIT_WAIT_ANSWER_CMD_CGATT:
        case INIT_WAIT_ANSWER_CMD_CIPMUX:
        case INIT_WAIT_ANSWER_CMD_CGEREP:
        case INIT_WAIT_ANSWER_CMD_CSTT:
        case INIT_WAIT_ANSWER_CMD_CIICR:
        case INIT_WAIT_ANSWER_CMD_CIFSR:
        case INIT_WAIT_ANSWER_CMD_CGDCONT:
        case INIT_WAIT_ANSWER_CMD_CGACT:
        case INIT_WAIT_ANSWER_CMD_CGPADDR:
        case INIT_WAIT_ANSWER_CMD_CIPHEAD:
        case INIT_WAIT_ANSWER_CMD_CIPSRIP:
            if (Gsm.ReadPointer > 0) {
                if (Gsm.StateInitWaitAnswerCmd == INIT_WAIT_ANSWER_CMD_CIFSR) {
                    //  Only for command AT+CIFS the GSM engine do not answer with the "OK" string
                    if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                        Gsm.ExtractCharArray((char *)(&TcpIp.Local_IP_Address[0]), (char *)(&Gsm.GSM_Data_Array[2]), sizeof(TcpIp.Local_IP_Address), ASCII_CARRIAGE_RET);
                    }
                    //  CMD AT+CIIR - OK (SET NEXT STEP -> CMD AT+CGDCONT)
                    Gsm.StateInitSendCmd       = INIT_CMD_CGDCONT;
                    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
                    break;                  
                }
                
                if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK)) != 0xFF) {
                    Gsm.RetryCounter = 0;
                    
                    switch (Gsm.StateInitWaitAnswerCmd)
                    {
                        case INIT_WAIT_ANSWER_CMD_QUERY_CGATT:
                            if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CGATT, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CGATT)) != 0xFF) {
                                if (Gsm.FindColonCommaCarriageRet() != 0xFF) {                      
                                    Gprs.GprsFlag.Bit.GprsService = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
                                }
                            }                       
                            if (Gprs.GprsFlag.Bit.GprsService == 0) {
                                Gsm.StateInitSendCmd = INIT_CMD_CGATT;      //  GPRS OFF - (SET NEXT STEP -> CMD AT+CGATT)
                            } else {
                                Gsm.StateInitSendCmd = INIT_CMD_CIPMUX;     //  GPRS ON - (SET NEXT STEP -> CMD AT+CIPMUX)
                            }
                            break;
                        case INIT_WAIT_ANSWER_CMD_CGATT:
                            Gsm.StateInitSendCmd = INIT_CMD_CIPMUX;         //  CMD AT+CGATT - OK (SET NEXT STEP -> CMD AT+CIPMUX)
                            break;
                        case INIT_WAIT_ANSWER_CMD_CIPMUX:                   //  CMD AT+CIPMUX - OK (SET NEXT STEP -> CMD AT+CGEREP)
                            Gsm.StateInitSendCmd = INIT_CMD_CGEREP;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CGEREP:                   //  CMD AT+CGEREP - OK (SET NEXT STEP -> CMD AT+CSTT)
                            Gsm.StateInitSendCmd = INIT_CMD_CSTT;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CSTT:                     //  CMD AT+CGEREP - OK (SET NEXT STEP -> CMD AT+CIIR)
                            //delay(2000);  
                            Gsm.StateInitSendCmd = INIT_CMD_CIICR;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CIICR:                    //  CMD AT+CIICR - OK (SET NEXT STEP -> CMD AT+CIFSR)
                            //delay(2000);  
                            Gsm.StateInitSendCmd = INIT_CMD_CIFSR;          
                            break;
                        case INIT_WAIT_ANSWER_CMD_CGDCONT:                  //  CMD AT+CGDCONT - OK (SET NEXT STEP -> CMD AT+CGACT)
                            Gsm.StateInitSendCmd = INIT_CMD_CGACT;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CGACT:                    //  CMD AT+CACT - OK (SET NEXT STEP -> CMD AT+CGPADDR)
                            Gsm.StateInitSendCmd = INIT_CMD_CGPADDR;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CGPADDR:                  //  CMD AT+CGPADDR - OK (SET NEXT STEP -> CMD AT+CIPHEAD)
                            if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                                Gsm.ExtractCharArray((char *)(&Gprs.PDP_IP_Address[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[1] + 2]), sizeof(Gprs.PDP_IP_Address), ASCII_QUOTATION_MARKS);
                            }
                            Gsm.StateInitSendCmd = INIT_CMD_CIPHEAD;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CIPHEAD:                  //  CMD AT+CIPHEAD - OK (SET NEXT STEP -> CMD AT+CIPSRIP)
                            Gsm.StateInitSendCmd = INIT_CMD_CIPSRIP;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CIPSRIP:                  //  CMD AT+CIPSRIP - OK (SET NEXT STEP -> INIT IDLE)
                            Gsm.StateInitSendCmd = INIT_IDLE;
                            Gsm.GsmFlag.Bit.GprsInitInProgress = 0;
                            if (Gsm.GsmFlag.Bit.HttpInitInProgress == 1) {
                                //  Starts HTTP Initialization
                                Gsm.StateInitSendCmd = INIT_CMD_AT_SAPBR_1;
                                break;
                            }
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
                            case INIT_WAIT_ANSWER_CMD_QUERY_CGATT:
                                Gsm.StateInitSendCmd = INIT_CMD_QUERY_CGATT;    //  Retry to send command ATQ+CGATT
                                break;
                            case INIT_WAIT_ANSWER_CMD_CGATT:                    //  Retry to send command AT+CGATT
                                Gsm.StateInitSendCmd = INIT_CMD_CGATT;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CIPMUX:                   //  Retry to send command AT+CMUX
                                Gsm.StateInitSendCmd = INIT_CMD_ATF0;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CGEREP:                   //  Retry to send command AT+CGEREP
                                Gsm.StateInitSendCmd = INIT_CMD_CGEREP;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CSTT:                     //  Retry to send command AT+CSTT
                                Gsm.StateInitSendCmd = INIT_CMD_CSTT;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CIICR:                    //  Retry to send command AT+CIICR
                                Gsm.StateInitSendCmd = INIT_CMD_CIICR;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CIFSR:                    //  Retry to send command AT+CIFSR
                                Gsm.StateInitSendCmd = INIT_CMD_CIFSR;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CGDCONT:                  //  Retry to send command AT+CGDCONT
                                Gsm.StateInitSendCmd = INIT_CMD_CGDCONT;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CGACT:                    //  Retry to send command AT+CGACT
                                Gsm.StateInitSendCmd = INIT_CMD_CGACT;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CGPADDR:                  //  Retry to send command AT+CGPADDR
                                Gsm.StateInitSendCmd = INIT_CMD_CGPADDR;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CIPHEAD:                  //  Retry to send command AT+CIPHEAD
                                Gsm.StateInitSendCmd = INIT_CMD_CIPHEAD;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CIPSRIP:                  //  Retry to send command AT+CIPSRIP
                                Gsm.StateInitSendCmd = INIT_CMD_CIPSRIP;
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
 * Function:        GprsEepromStartAddSetup
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
 void GprsCmd_GSM::GprsEepromStartAddSetup(void) {
    Gprs.GprsEepromAdd.StartAddApn         = &APN[0];
    Gprs.GprsEepromAdd.StartAddApnUserName = &APN_USER_NAME[0];
    Gprs.GprsEepromAdd.StartAddApnPassword = &APN_PASSWORD[0];
    Gprs.GprsEepromAdd.StartAddPdpType     = &PDP_TYPE[0];
 }
/****************************************************************************/
