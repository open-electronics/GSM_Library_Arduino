/*********************************************************************
 *
 *       Generic command for GSM
 *
 *********************************************************************
 * FileName:        GenericCmd_GSM.cpp
 * Revision:        1.0.0
 * Date:            01/10/2016
 *
 * Revision:        1.1.0
 *                  15/10/2018
 *                  - The name of function "void EepromStartAddSetup(void)" was changed in "void PswdEepromStartAddSetup(void)"
 *                  - Added TCP/IP management
 *                  - Added new error code: Unspecified GPRS error (0x0094), PDP authentication failure (0x0095) and GPRS is suspended (0x0212)
 *                  - Removed the AT command ECHO CANC in the GDM engine initialization
 *                  - Added the AT command AT+CALM in the GSM engine initialization
 *                  - Added the AT command AT+CRSL in the GSM engine initialization
 *                  - Added the AT command AT+CALM to the available commands list
 *                  - Added the AT command AT+CRSL to the available commands list
 *                  - Added the AT command AT+CALS to the available commands list. SIMCOM Only
 *                  - Added code into "ProcessUnsolicitedCode" function to intercept the Unsolicited Result Code for TCP/IP connection 
 *                  - Added code into "ProcessUnsolicitedCode" function to intercept the Unsolicited Result Code for HTTP connection
 *                  - Added three new error code: Invalid input parameter (323); Invalid input format (324); Invalid input value (325)
 *                  - Renamed function "void ResetFlags(void)" in "void ResetAllFlags(void)"
 *                  - Added new functions to clear flags grouped in categories:
 *                      + "void ResetGsmFlags(void)"                -> Reset Gsm Flags Only
 *                      + "void ResetSecurityFlags(void)"           -> Reset Security Flags Only
 *                      + "void ResetPhoneBookFlags(void)"          -> Reset PhoneBook Flags Only
 *                      + "void ResetSmsFlags(void)"                -> Reset Sms Flags Only
 *                      + "void ResetPhonicCallFlags(void)"         -> Reset Phonic Call Flags Only
 *                      + "void ResetGprsFlags(void)"               -> Reset Gprs Flags Only
 *                      + "void ResetHttpFlags(void)"               -> Reset Http Flags Only
 *                      + "void ResetTcpIpFlags(uint8_t IndexConn)" -> Reset TcpIp Flags Only
 *                  - Added new flag "Gsm.GsmFlag.Bit.NoAutoResetGsmError" to disbale automatic GSM reset
 *                  - Improved code
 *
 * Revision:        1.2.0
 *                  30/11/2019
 *                  - Fixed bug "ClearBuffer" function [sizeof parameter]
 *                  - Changed function name "ConvertNumberTochar" in "ConvertNumberToString"
 *                  - Added new function to convert floating numner in a string "ConvertFloatToString"
 *
 * Dependencies:    SoftwareSerial.h
 *                  GenericCmd_GSM.h
 *                  Isr_GSM.h
 *                  Io_GSM.h
 *                  Uart_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  GprsCmd_GSM.h
 *                  TcpIpCmd_GSM.h
 *                  HttpCmd_GSM.h
 *
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

#include <SoftwareSerial.h>

#include "GenericCmd_GSM.h"
#include "Isr_GSM.h"
#include "Io_GSM.h"
#include "Uart_GSM.h"
#include "SecurityCmd_GSM.h"
#include "PhoneBookCmd_GSM.h"
#include "SmsCmd_GSM.h"
#include "PhonicCallCmd_GSM.h"
#include "GprsCmd_GSM.h"
#include "TcpIpCmd_GSM.h"
#include "HttpCmd_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/****************************************************************************
 * Function:        SetCmd
 *
 * Overview:        This routine is used to send a generic GSM command (Not yet completely implemented)
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           Lenght : Command lenght
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
uint8_t GenericCmd_GSM::SetCmd(uint8_t Lenght) {    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ResetAllFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Lenght;
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_GENERIC, ANSWER_GENERIC_AT_CMD_STATE);
        Gsm.WritePointer  = Lenght;
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_SwPowerDown
 *
 * Overview:        This function is used to perform a GSM Power Down by AT CMD
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           Type : Power down typology (QUECTEL M95 Only)
 *                  Type = 0 -> Power Off urgently
 *                  Type = 1 -> Normal Power Off
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
uint8_t GenericCmd_GSM::SetCmd_SwPowerDown(uint8_t Type) {  
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetAllFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.GsmFlag.Bit.GsmPowerOff          = 1;   //  Power Off in progress
        Gsm.BckCmdData[0] = Type;
        Gsm.ReadStringFLASH((uint8_t *)AT_PW_DOWN, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_PW_DOWN));
#ifndef QUECTEL_M95
        Gsm.GSM_Data_Array[9] = (Type + 0x30);
#endif
        Gsm.WritePointer = strlen(AT_PW_DOWN);
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_PW_DOWN, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_HwPowerDown
 *
 * Overview:        This function is used to perform a GSM Power Down by Hardware
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
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
uint8_t GenericCmd_GSM::SetCmd_HwPowerDown(void) {  
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetAllFlags();
        Gsm.GsmFlag.Bit.GsmPowerOff = 1;    //  Power Off in progress
        Gsm.CommandsClassState = ANSWER_GENERIC_AT_CMD_STATE;
        Gsm.StateSendCmd       = CMD_IDLE;
        Gsm.StateWaitAnswerCmd = WAIT_ANSWER_CMD_PW_DOWN;
        Gsm.UartState          = UART_WAITDATA_STATE;       
        Off_GSM();      
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CFUN
 *
 * Overview:        This function is used to set Phone Functionally. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CFUN=<fun>[,<rst>]   
 *
 * Input:           FUN_Code: Functionality mode
 *                  FUN_Code    "0" Minimum functionality
 *                              "1" Full functionality (Defaul)
 *                              "4" Disable phone both transmit and receive RF circuits
 *
 * Command Note:    <fun>       "0" Minimum functionality
 *                              "1" Full functionality (Defaul)
 *                              "4" Disable phone both transmit and receive RF circuits
 *                  <rst>       "0" Do not reset the MT before setting it to <fun> power level
 *                              "1" Reset the MT before setting it to <fun> power level
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
uint8_t GenericCmd_GSM::SetCmd_AT_CFUN(uint8_t FUN_Code) {  
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();    
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = FUN_Code;
        Gsm.ReadStringFLASH((uint8_t *)AT_CFUN, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CFUN));
        Gsm.GSM_Data_Array[8] = (FUN_Code + 0x30);
        Gsm.WritePointer = strlen(AT_CFUN);
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CFUN, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CCLK
 *
 * Overview:        This function is used to enable "Get Local Timestamp". For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CLCK=<time> to set clock or AT+CLCK? to read clock   
 *
 * Input:           Query: Identifies if the command must set clock or reads it
 *                  Query   "false" Sends command to set clock
 *                          "true"  Sends command to read clock 
 *
 * Command Note:    <time>  String type (String should be included in quotation marks) value; format is "yy/MM/dd,hh:mm:ss±zz",
 *                          where characters indicate year (Two last digit), month, day, hours, minutes, seconds and time zone
 *                          (indicates the difference, expressed in quaters of an hour, between the local time and GMT; range
 *                          -47...+48). E.g 6th of May 2010, 00:01:52 GMT +2 hours equals to "10/05/06,00:01:52+08"
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
uint8_t GenericCmd_GSM::SetCmd_AT_CCLK(bool Query) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Query;
        if (Query == false) {
            Gsm.ReadStringFLASH((uint8_t *)AT_CCLK, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CCLK));
            Index = CCLK_OFFSET + Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.Year, ((uint8_t *)(Gsm.GSM_Data_Array) + CCLK_OFFSET), 2);
            Gsm.GSM_Data_Array[Index++] = ASCII_SLASH;
            Index += Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.Month, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;
            Gsm.GSM_Data_Array[Index++] = ASCII_SLASH;
            Index += Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.Day, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Index += Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.Hours, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;
            Gsm.GSM_Data_Array[Index++] = ASCII_COLON;
            Index += Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.Minutes, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;
            Gsm.GSM_Data_Array[Index++] = ASCII_COLON;
            Index += Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.Seconds, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;       
            if (Gsm.Clock_Info.Clock.Bit.GMT >= 0) {
                Gsm.GSM_Data_Array[Index++] = ASCII_PLUS;
                Index += Gsm.ConvertNumberToString(Gsm.Clock_Info.Clock.Bit.GMT, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;
            } else {
                Gsm.GSM_Data_Array[Index++] = ASCII_MINUS;
                Index += Gsm.ConvertNumberToString((Gsm.Clock_Info.Clock.Bit.GMT & 0x3F), ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 2) + 1;
            }
            Gsm.GSM_Data_Array[Index++] = ASCII_QUOTATION_MARKS;
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;
        } else {
            Gsm.ReadStringFLASH((uint8_t *)ATQ_CCLK, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CCLK));
            Gsm.WritePointer = strlen(ATQ_CCLK);            
        }       
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CCLK, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_GMI
 *
 * Overview:        This function is used to request Manufacturer Identification. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+GMI or AT+CGMI
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
uint8_t GenericCmd_GSM::SetCmd_ATQ_GMI(uint8_t FormatAt) {  
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = FormatAt;
        switch (FormatAt)
        {
            case GMI_V25_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_GMI, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_GMI));
                Gsm.WritePointer = strlen(AT_GMI);
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_GMI, ANSWER_GENERIC_AT_CMD_STATE);
                break;
            case CGMI_3GPP_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_CGMI, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGMI));
                Gsm.WritePointer = strlen(AT_CGMI);
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CGMI, ANSWER_GENERIC_AT_CMD_STATE);
                break;
        }
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_GMM
 *
 * Overview:        This function is used to request TA Model Identification. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+GMM or AT+CGMM
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
uint8_t GenericCmd_GSM::SetCmd_ATQ_GMM(uint8_t FormatAt) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = FormatAt;
        switch (FormatAt)
        {
            case GMM_V25_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_GMM, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_GMM));
                Gsm.WritePointer = strlen(AT_GMM);          
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_GMM, ANSWER_GENERIC_AT_CMD_STATE);
                break;
            case CGMM_3GPP_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_CGMM, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGMM));
                Gsm.WritePointer = strlen(AT_CGMM);         
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CGMM, ANSWER_GENERIC_AT_CMD_STATE);
                break;
        }
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_GMR
 *
 * Overview:        This function is used to request TA Revision Identification of Software Release. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+GMR or AT+CGMR
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
uint8_t GenericCmd_GSM::SetCmd_ATQ_GMR(uint8_t FormatAt) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = FormatAt;
        switch (FormatAt)
        {
            case GMR_V25_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_GMR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_GMR));
                Gsm.WritePointer = strlen(AT_GMR);          
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_GMR, ANSWER_GENERIC_AT_CMD_STATE);
                break;
            case CGMR_3GPP_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_CGMR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGMR));
                Gsm.WritePointer = strlen(AT_CGMR);         
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CGMR, ANSWER_GENERIC_AT_CMD_STATE);
                break;
        }
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_GSN
 *
 * Overview:        This function is used to request TA Serial Number Identification (IMEI). For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+GSN or AT+CGSN
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
uint8_t GenericCmd_GSM::SetCmd_ATQ_GSN(uint8_t FormatAt) {  
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = FormatAt;
        switch (FormatAt)
        {
            case GSN_V25_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_GSN, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_GSN));
                Gsm.WritePointer = strlen(AT_GSN);          
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_GSN, ANSWER_GENERIC_AT_CMD_STATE);
                break;
            case CGSN_3GPP_FORMAT:
                Gsm.ReadStringFLASH((uint8_t *)AT_CGSN, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CGSN));
                Gsm.WritePointer = strlen(AT_CGSN);         
                Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CGSN, ANSWER_GENERIC_AT_CMD_STATE);
                break;
        }       
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_CREG
 *
 * Overview:        This function is used to request Network Registration. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CREG? and the answer is +CREG:<n>,<stat>[,<lac>,<ci>]
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <n>     0   Disable network registration
 *                          1   Enable network registration unsolicited result code +CREG:<stat>
 *                          2   Enable network registration unsolicited result code with location information
 *                  <stat>  0   Not registered, ME is not currently searching a new operator to register to
 *                          1   Registered, home network
 *                          2   Not registered, but ME is currently searching a new operator to register to
 *                          3   Registration denied
 *                          4   Unknown
 *                          5   Registered, roaming
 *                  <lac>       String type; two byte location area code in hexadecimal format
 *                  <ci>        String type; two byte cell ID in hexadecimal format 
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::SetCmd_ATQ_CREG(void) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.ReadStringFLASH((uint8_t *)ATQ_CREG, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CREG));
        Gsm.WritePointer = strlen(ATQ_CREG);            
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CREG, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_ATQ_CSQ
 *
 * Overview:        This function is used to request Signal quality Report. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CSQ and the answer is +CSQ:<rssi>,<ber>
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <rssi>  0       -113dBm or less
 *                          1       -111dBm
 *                          2..30   -109... -53dBm
 *                          31      -51dBm or greater
 *                          99      Not known or not detectable
 *                  <ber>   0..7    As RXQUAL values in the table in GSM 05.08 subclause 8.2.4
 *                          99      Not known or not detectable 
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::SetCmd_ATQ_CSQ(void) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.ReadStringFLASH((uint8_t *)ATQ_CSQ, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CSQ));
        Gsm.WritePointer = strlen(ATQ_CSQ);         
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CSQ, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_COPS
 *
 * Overview:        This function is used to read Operator Name. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+COPS and the answer is +COPS:<mode>[,<format>,<oper>]
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <mode>      0   Automatic mode; <oper> field is ignored
 *                              1   Manual operator selection; <oper> field shall be present
 *                              2   Manual deregister from network
 *                              3   Set only <format> (for read Command +COPS?) – not shown in Read Command response
 *                              4   Manual/automatic selected; if manual selection fails, automatic mode (<mode>=0) is entered
 *                  <format>    0   Long format alphanumeric <oper>; can be up to 16 characters long
 *                              1   Short format alphanumeric <oper>
 *                              2   Numeric <oper>; GSM Location Area Identification number
 *                  <oper>          Operator in format as per <mode>
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::SetCmd_ATQ_COPS(void) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.ReadStringFLASH((uint8_t *)ATQ_COPS, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_COPS));
        Gsm.WritePointer = strlen(ATQ_COPS);            
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_COPS, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CPAS
 *
 * Overview:        This function is used to check Phone Activity Status. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CPAS and the answer is +CPAS:<pas>
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  <pas>   0   Ready (MT allows commands from TA/TE)
 *                          2   Unknown (MT is not guaranteed to respond to instructions)
 *                          3   Ringing (MT is ready for commands from TA/TE, but the ringer is active)
 *                          4   Call in progress (MT is ready for commands from TA/TE, but a call is in progress)
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::SetCmd_ATQ_CPAS(void) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.ReadStringFLASH((uint8_t *)AT_CPAS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPAS));
        Gsm.WritePointer = strlen(AT_CPAS);         
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_ATQ_CPAS, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CALS
 *
 * Overview:        This function is used to select Alert Sound type. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  For SIM800C, SIM900, SIM928A only. This function is supported by SIMCOM device only
 *                  The AT command is -> AT+CALS:<n> or AT+CALS:<n>,<switch>
 *
 * Input:           n: a integer value from 0 to 19
 *                  TestAlertSound: 0 Stop playing ring tone    (SIM800C)
 *                                  1 Start to play ring tone   (SIM800C)
 *                                  1 Stop playing ring tone    (SIM900)
 *                                  0 Start to play ring tone   (SIM900)
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
uint8_t GenericCmd_GSM::SetCmd_AT_CALS(uint8_t n, uint8_t TestAlertSound) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = n;
        Gsm.BckCmdData[1] = TestAlertSound;     
        if (n > MAX_SOUND_TYPE) {
            n = MAX_SOUND_TYPE;
        }
        Gsm.ReadStringFLASH((uint8_t *)AT_CALS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CALS));
        Index = CALS_OFFSET + Gsm.ConvertNumberToString(n, ((uint8_t *)(Gsm.GSM_Data_Array) + CALS_OFFSET), 0);
#ifdef SIMCOM_SIM928A
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;           
#endif
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) 
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
        Gsm.GSM_Data_Array[Index++] = (TestAlertSound + 0x30);
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;                               
#endif
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CALS, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CALM
 *
 * Overview:        This function is used to set Alert Sound Mode. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  For SIM800C, SIM900, SIM928A and M95 -> AT+CALM=<mode>
 *                  For G510 -> AT+CALM=<Call_Mode>,<SMS_Mode>
 *
 * Input:           Mode: 0 Normal mode; 1 Silent  mode (For SIM800C, SIM900, SIM928A and M95)
 *                  Call_Mode: 0 Ring, 1 Silent (For G510 only)
 *                  SMS_Mode:  0 Ring, 1 Silent (For G510 only) 
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
uint8_t GenericCmd_GSM::SetCmd_AT_CALM(uint8_t Mode, uint8_t SmsMode) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Mode;
        Gsm.BckCmdData[1] = SmsMode;
        Index = CALM_OFFSET;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined (QUECTEL_M95)
            Gsm.ReadStringFLASH((uint8_t *)AT_CALM, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CALM));
            Gsm.GSM_Data_Array[Index++] = (Mode + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
            Gsm.WritePointer = Index;                               
#endif
#ifdef FIBOCOM_G510
            Gsm.ReadStringFLASH((uint8_t *)AT_CALM, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CALM));
            Gsm.GSM_Data_Array[Index++] = (Mode + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
            Gsm.GSM_Data_Array[Index++] = (SmsMode + 0x30);
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
            Gsm.WritePointer = Index;                           
#endif
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CALM, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CRSL
 *
 * Overview:        This function is used to set Ringer Sound Level. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CRSL=<level>
 *
 * Input:           SoundLevel: 0-100 unsigned char type value with manufacturer specific range (smallest value represents the lowest sound level)
 *                              For SIM800C and M95 the max value for "level" is 100. For G510 the max value for "level" is 7
 *                              For SIM900 and SIM908A: 0 LEVEL OFF
 *                                                      1 LEVEL LOW
 *                                                      2 LEVEL MEDIUM
 *                                                      3 LEVEL HIGH
 *                                                      4 LEVEL CRESCENDO
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
uint8_t GenericCmd_GSM::SetCmd_AT_CRSL(uint8_t SoundLevel) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = SoundLevel;
        
#if defined(SIMCOM_SIM800C) || defined(QUECTEL_M95) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(FIBOCOM_G510)
        if (SoundLevel > MAX_SOUND_VALUE) {
            SoundLevel = MAX_SOUND_VALUE;
        }
        Gsm.ReadStringFLASH((uint8_t *)AT_CRSL, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CRSL));
        Index = CRSL_LEVEL_OFFSET + Gsm.ConvertNumberToString(SoundLevel, ((uint8_t *)(Gsm.GSM_Data_Array) + CRSL_LEVEL_OFFSET), 0);      
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;               
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CRSL, ANSWER_GENERIC_AT_CMD_STATE);
#endif
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CLVL
 *
 * Overview:        This function is used to set Loud Speaker Volume Level. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CLVL=<level>
 *
 * Input:           VolumeLevel: 0-100 unsigned char type value with manufacturer specific range (smallest value represents the lowest sound level)
 *                               For SIM800C, SIM900, SIM928A and M95 the max value for "level" is 100. For G510 the max value for "level" is 6
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
uint8_t GenericCmd_GSM::SetCmd_AT_CLVL(uint8_t VolumeLevel) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = VolumeLevel;
        if (VolumeLevel > MAX_VOLUME_VALUE) {
            VolumeLevel = MAX_VOLUME_VALUE;
        }
        Gsm.ReadStringFLASH((uint8_t *)AT_CLVL, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CLVL));
        Index = CLVL_LEVEL_OFFSET + Gsm.ConvertNumberToString(VolumeLevel, ((uint8_t *)(Gsm.GSM_Data_Array) + CLVL_LEVEL_OFFSET), 0);     
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;               
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CLVL, ANSWER_GENERIC_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CMIC
 *
 * Overview:        This function is used to change Microphone Gain Level. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  The syntax of command for SIM800C and SIM900 is AT+CMIC=<channel>,<gainlevel>
 *                  The syntax of command for M95 is AT+QMIC=<channel>,<gainlevel>
 *                  The syntax of command for G510 is AT+MMICG=<gainlevel>
 *
 * Input:           Channel:   Selects audio channel
 *                  GainLevel: Sets Microphone gain
 *
 * Command Note:    For SIM800C, SIM900 and SIM928A:
 *                  <channel>   0 Main Audio channel 
 *                              1 Aux Audio channel
 *                              2 Main Audio channel hand free mode
 *                              3 Aux  Audio channel hand free mode
 *                  <gainlevel> 0 - 15 (0 -> 0dB; 15 -> +22.5dB. See datasheet for detailes)
 *
 *                  FOR M95:
 *                  <channel>   0 Normal microphone
 *                              1 Headset microphone
 *                              2 Loudspeaker microphone
 *                  <gainlevel> 0 - 15 (See datasheet for detailes)
 *
 *                  For G510 - No channel parameter required
 *                  <gainlevel> 0 - 15 (See datasheet for detailes)
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
uint8_t GenericCmd_GSM::SetCmd_AT_CMIC(uint8_t Channel, uint8_t GainLevel) {
    uint8_t Index;

    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
        Gsm.ResetGsmFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.BckCmdData[0] = Channel;
        Gsm.BckCmdData[1] = GainLevel;
        if (Channel > MAX_CHANNEL_VALUE) {
            Channel = MAX_CHANNEL_VALUE;
        }
        if (GainLevel > MAX_MIC_GAIN_VALUE) {
            GainLevel = MAX_MIC_GAIN_VALUE;
        }       
#ifdef FIBOCOM_G510
        Gsm.ReadStringFLASH((uint8_t *)AT_CMIC, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMIC));
        Index = CMIC_CHANNEL_OFFSET + Gsm.ConvertNumberToString(GainLevel, ((uint8_t *)(Gsm.GSM_Data_Array) + CMIC_CHANNEL_OFFSET), 0);
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;               
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CMIC, ANSWER_GENERIC_AT_CMD_STATE);      
#else
        Gsm.ReadStringFLASH((uint8_t *)AT_CMIC, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMIC));
        Index = CMIC_CHANNEL_OFFSET;
        Gsm.GSM_Data_Array[Index++] = Channel + 0x30;
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
        if (GainLevel < 10) {
            Index += (Gsm.ConvertNumberToString(GainLevel, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0) + 2);
        } else {
            Index += (Gsm.ConvertNumberToString(GainLevel, ((uint8_t *)(Gsm.GSM_Data_Array) + Index), 0) + 3);
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;               
        Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CMIC, ANSWER_GENERIC_AT_CMD_STATE);          
#endif
    }   
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        GsmAnswerStateProcess
 *
 * Overview:        This function recall the appropriate code associated of the AT command sent. Each "case" of
 *                  this "Switch" can be enabled or disabled using appropriate "define"
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
void GenericCmd_GSM::GsmAnswerStateProcess(void) {
    switch (CommandsClassState)
    {
#ifdef ENABLE_ANSWER_GENERIC_AT_CMD_STATE
        case ANSWER_GENERIC_AT_CMD_STATE:
            Gsm.GsmGenericWaitAnswer();
            break;
#endif
#ifdef ENABLE_ANSWER_SECURITY_AT_CMD_STATE
        case ANSWER_SECURITY_AT_CMD_STATE:
            Security.GsmSecurityWaitAnswer();
            break;
#endif
#ifdef ENABLE_ANSWER_PHONEBOOK_AT_CMD_STATE
        case ANSWER_PHONEBOOK_AT_CMD_STATE:
            PhoneBook.GsmPhoneBookWaitAnswer();
            break;
#endif
#ifdef ENABLE_ANSWER_SMS_AT_CMD_STATE
        case ANSWER_SMS_AT_CMD_STATE:
            Sms.GsmSmsWaitAnswer();
            break;
#endif
#ifdef ENABLE_ANSWER_PHONIC_CALL_AT_CMD_STATE
        case ANSWER_PHONIC_CALL_AT_CMD_STATE:
            PhonicCall.GsmPhonicCallWaitAnswer();
            break;
#endif
#ifdef ENABLE_ANSWER_GPRS_AT_CMD_STATE
        case ANSWER_GPRS_AT_CMD_STATE:
            Gprs.GprsWaitAnswer();
            break;      
#endif
#ifdef ENABLE_ANSWER_TCP_IP_AT_CMD_STATE 
        case ANSWER_TCP_IP_AT_CMD_STATE:
            TcpIp.TcpIpWaitAnswer();
            break;
#endif
#ifdef ENABLE_ANSWER_HTTP_AT_CMD_STATE 
        case ANSWER_HTTP_AT_CMD_STATE:
            Http.HttpWaitAnswer();
            break;
#endif
        default:
            break;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        GsmWaitAnswer
 *
 * Overview:        This function process the AT command answer of the command sent.
 *                  The answer received and processed by this code regard the Generic Command Functions
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
void GenericCmd_GSM::GsmGenericWaitAnswer(void) {
    uint8_t StrPointer = 0xFF;
    
    if ((Gsm.StateSendCmd != CMD_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }

    if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) {
        return;
    }
    Gsm.UartFlag.Bit.ReceivedAnswer = 0;
    Gsm.GsmFlag.Bit.GsmPowerOff     = 0;    //  Reset flag Hardware/Software PowerOff in progress
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
            
            switch (StateWaitAnswerCmd)
            {               
                case WAIT_ANSWER_CMD_PW_DOWN:
                    //  Performed POWER OFF (OK)
                    Gsm.GsmFlag.Bit.GsmModuleOff = 1;       //  GSM Module OFF
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_PWR_OFF, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_PWR_OFF)) != 0xFF) {
                        //  Performed POWER OFF (OK)
                        Gsm.GsmFlag.Bit.GsmModuleOff = 1;   //  GSM Module OFF
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_PWR_ON, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_PWR_ON)) != 0xFF) {
                        //  Performed POWER ON. The GSM module was already OFF              
                        Gsm.GsmFlag.Bit.GsmModuleOff = 0;   //  GSM Module ON
                    }
                    break;
                case WAIT_ANSWER_CMD_GENERIC:
                case WAIT_ANSWER_CMD_AT_CFUN:
                case WAIT_ANSWER_CMD_AT_CALS:
                case WAIT_ANSWER_CMD_AT_CALM:
                case WAIT_ANSWER_CMD_AT_CRSL:
                case WAIT_ANSWER_CMD_AT_CLVL:
                case WAIT_ANSWER_CMD_AT_CMIC:
                    break;              
                case WAIT_ANSWER_CMD_AT_CCLK:
                    StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CCLK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CCLK));
                    if (StrPointer != 0xFF) {
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                            Gsm.Clock_Info.Clock.Bit.Year    = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 3), 2));
                            Gsm.Clock_Info.Clock.Bit.Month   = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 6), 2));
                            Gsm.Clock_Info.Clock.Bit.Day     = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 9), 2));
                            Gsm.Clock_Info.Clock.Bit.Hours   = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), 2));
                            Gsm.Clock_Info.Clock.Bit.Minutes = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 4), 2));
                            Gsm.Clock_Info.Clock.Bit.Seconds = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 7), 2));
                            Gsm.Clock_Info.Clock.Bit.GMT     = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 10), 2));
                        }
                    }
                    break;
                case WAIT_ANSWER_CMD_ATQ_COPS:
                    StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_COPS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_COPS));
                    if (StrPointer != 0xFF) {   
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                            Gsm.SimFlag.Bit.OperatorSelMode   = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 1), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 1))));
                            Gsm.SimFlag.Bit.OperatorSelFormat = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1))));
                        }
                        //  Reads operator name
                        Gsm.ExtractCharArray((char *)(&Gsm.OperatorName[0]), (char *)(&Gsm.GSM_Data_Array[StrPointer + COPS_OPERATOR_OFFSET]), sizeof(Gsm.OperatorName), ASCII_CARRIAGE_RET);
                    }
                    break;
                case WAIT_ANSWER_CMD_ATQ_CPAS:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPAS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPAS)) != 0xFF) {
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                            Gsm.SimFlag.Bit.PhoneActivityStatus = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 1), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 1))));
                        }
                    }
                    break;
                case WAIT_ANSWER_CMD_ATQ_GMI:
                case WAIT_ANSWER_CMD_ATQ_CGMI:
                    Gsm.ExtractCharArray((char *)(&Gsm.ME_Info.GMI_Info[0]), (char *)(&Gsm.GSM_Data_Array[2]), sizeof(Gsm.ME_Info.GMI_Info), ASCII_CARRIAGE_RET);
                    break;
                case WAIT_ANSWER_CMD_ATQ_GMM:
                case WAIT_ANSWER_CMD_ATQ_CGMM:
                    Gsm.ExtractCharArray((char *)(&Gsm.ME_Info.GMM_Info[0]), (char *)(&Gsm.GSM_Data_Array[2]), sizeof(Gsm.ME_Info.GMM_Info), ASCII_CARRIAGE_RET);
                    break;
                case WAIT_ANSWER_CMD_ATQ_GMR:
                case WAIT_ANSWER_CMD_ATQ_CGMR:
                    Gsm.ExtractCharArray((char *)(&Gsm.ME_Info.GMR_Info[0]), (char *)(&Gsm.GSM_Data_Array[2]), sizeof(Gsm.ME_Info.GMR_Info), ASCII_CARRIAGE_RET);
                    break;
                case WAIT_ANSWER_CMD_ATQ_GSN:
                case WAIT_ANSWER_CMD_ATQ_CGSN:
                    Gsm.ExtractCharArray((char *)(&Gsm.ME_Info.IMEI_Info[0]), (char *)(&Gsm.GSM_Data_Array[2]), sizeof(Gsm.ME_Info.IMEI_Info), ASCII_CARRIAGE_RET);
                    break;
                case WAIT_ANSWER_CMD_ATQ_CSQ:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CSQ, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CSQ)) != 0xFF) {
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                            Gsm.CSQ_Info.Rssi = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
                            Gsm.CSQ_Info.Ber  = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1))));
                        }
                        if (Gsm.CSQ_Info.Rssi > MAX_RSSI) {
                            Gsm.CSQ_Info.Rssi = MAX_RSSI;
                        }
                        if (Gsm.CSQ_Info.Ber > MAX_BER) {
                            Gsm.CSQ_Info.Ber = MAX_BER;
                        }
                    }
                    break;
                case WAIT_ANSWER_CMD_ATQ_CREG:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CREG, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CREG)) != 0xFF) {
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
            Gsm.GsmRetrySendCmd();
        }           
    } else {
        //  If no answer from GSM module was received, this means that the module is switched off
        Gsm.InitReset_GSM();
    }
}   
/****************************************************************************/

/****************************************************************************
 * Function:        GsmRetrySendCmd
 *
 * Overview:        This function retry to send AT command for a maximum of three time
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
void GenericCmd_GSM::GsmRetrySendCmd(void) {    
    uint8_t AnswerCmdStateBackup;
    
    if (Gsm.RetryCounter++ < 2) {
        AnswerCmdStateBackup = Gsm.StateWaitAnswerCmd;
        Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        switch (AnswerCmdStateBackup)
        {
            case WAIT_ANSWER_CMD_GENERIC:   
                Gsm.SetCmd(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_PW_DOWN:
                Gsm.SetCmd_SwPowerDown(Gsm.BckCmdData[0]);          
                break;
            case WAIT_ANSWER_CMD_AT_CFUN:
                Gsm.SetCmd_AT_CFUN(Gsm.BckCmdData[0]);
                break;                  
            case WAIT_ANSWER_CMD_ATQ_GMI:
            case WAIT_ANSWER_CMD_ATQ_CGMI:
                Gsm.SetCmd_ATQ_GMI(Gsm.BckCmdData[0]);  
                break;
            case WAIT_ANSWER_CMD_ATQ_GMM:
            case WAIT_ANSWER_CMD_ATQ_CGMM:
                Gsm.SetCmd_ATQ_GMM(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_ATQ_GMR:
            case WAIT_ANSWER_CMD_ATQ_CGMR:
                Gsm.SetCmd_ATQ_GMR(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_ATQ_GSN:
            case WAIT_ANSWER_CMD_ATQ_CGSN:
                Gsm.SetCmd_ATQ_GSN(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_ATQ_CREG:
                Gsm.SetCmd_ATQ_CREG();
                break;
            case WAIT_ANSWER_CMD_AT_CCLK:
                Gsm.SetCmd_AT_CCLK(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_ATQ_CSQ:
                Gsm.SetCmd_ATQ_CSQ();
                break;
            case WAIT_ANSWER_CMD_ATQ_COPS:
                Gsm.SetCmd_ATQ_COPS();
                break;
            case WAIT_ANSWER_CMD_ATQ_CPAS:
                Gsm.SetCmd_ATQ_CPAS();
                break;
            case WAIT_ANSWER_CMD_AT_CALS:
                Gsm.SetCmd_AT_CALS(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;
            case WAIT_ANSWER_CMD_AT_CALM:
                Gsm.SetCmd_AT_CALM(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;
            case WAIT_ANSWER_CMD_AT_CRSL:
                Gsm.SetCmd_AT_CRSL(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_AT_CLVL:
                Gsm.SetCmd_AT_CLVL(Gsm.BckCmdData[0]);
                break;
            case WAIT_ANSWER_CMD_AT_CMIC:
                Gsm.SetCmd_AT_CMIC(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
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
 * Function:        ProcessGsmError
 *
 * Overview:        This function process ther error codes returned from GSM engine
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
void GenericCmd_GSM::ProcessGsmError(void) {
    uint8_t StrPointer = 0xFF;
        
    StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CME_ERROR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CME_ERROR));
    if (StrPointer != 0xFF) {
        StrPointer += ERROR_CODE_OFFSET;
        ErrorCodeStringConverter(StrPointer);
    } else {    
        StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMS_ERROR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMS_ERROR));
        if (StrPointer != 0xFF) {
            StrPointer += ERROR_CODE_OFFSET;
            ErrorCodeStringConverter(StrPointer);       
        } else {
            StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_ERROR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_ERROR));
            if (StrPointer != 0xFF) {
                //  Returned "ERROR" string without code
                Gsm.GsmFlag.Bit.ErrorWithoutCode = 1;
                return;
            } else {
                return;
            }
        }
    }
    
    switch (GsmErrorCode)
    {
        case XXX_ERR_2069:
        case XXX_ERR_2076:
        case XXX_ERR_49155:
        case CMS_BEARER_CAPABILITY:         //  Error -> 058 Dec Fromat
        case CME_NO_NETWORK_SERVICE:        //  Error -> 030 Dec Format
        case CME_NETWORK_TIMOUT:            //  Error -> 031 Dec Format
        case CMS_NO_NETWORK:                //  Error -> 331 Dec Format
        case CMS_NETWORK_TIMEOUT:           //  Error -> 332 Dec Format
        case CME_UNKNOWN:                   //  Error -> 100 Dec Format
        case CMS_UNKNOWN:                   //  Error -> 500 Dec Format
            Gsm.GsmFlag.Bit.UnknownError = 1;
            break;
        case XXX_ERR_2098:                  // Error  -> 2086 Dec Format
        case XXX_ERR_2086:
            Gsm.SimFlag.Bit.SIM_OutOfPhoneCredit = 1;
            break;
        case CME_NOT_SUPPORTED:             //  Error -> 004 Dec Format
        case CMS_NOT_SUPPORTED:             //  Error -> 303 Dec Format
            Gsm.GsmFlag.Bit.OperationNotSupported = 1;
            break;
        case CME_NOT_ALLOWED:               //  Error -> 003 Dec Format
        case CMS_NOT_ALLOWED:               //  Error -> 302 Dec Format
            Gsm.GsmFlag.Bit.OperationNotAllowed = 1;
            break;
        case CME_SIM_NOT_INSERTED:          //  Error -> 010 Dec Format
        case CMS_SIM_NOT_INSERTED:          //  Error -> 310 Dec Format
            Gsm.SimFlag.Bit.SIM_NotInserted = 1;
            break;
        case CME_SIM_FAILURE:               //  Error -> 013 Dec Format
        case CMS_SIM_FAILURE:               //  Error -> 313 Dec Format
            Gsm.SimFlag.Bit.SIM_Failure = 1;
            break;
        case CME_SIM_BUSY:                  //  Error -> 014 Dec Format
        case CMS_SIM_BUSY:                  //  Error -> 314 Dec Format
            Gsm.SimFlag.Bit.SIM_Busy = 1;
            break;
        case CMS_SIM_NOT_READY:             //  Error -> 512 Dec Format
            Gsm.SimFlag.Bit.SIM_NotReady = 1;
            break;
        case CME_SIM_WRONG:                 //  Error -> 015 Dec Format
        case CMS_SIM_WRONG:                 //  Error -> 315 Dec Format
            Gsm.SimFlag.Bit.SIM_Wrong = 1;
            break;
        case CME_INCORRECT_PSW:             //  Error -> 016 Dec Format
            Security.SecurityFlag.Bit.IncorrectPSW = 1;
            break;
        case CME_SIM_PIN_REQ:               //  Error -> 011 Dec Format
        case CMS_SIM_PIN_NECESSARY:         //  Error -> 311 Dec Format
            Security.SecurityFlag.Bit.SIM_Status = SIM_PIN1_REQ;
            break;
        case CME_PH_SIM_PIN_REQ:            //  Error -> 005 Dec Format
        case CME_PH_FSIM_PIN_REQ:           //  Error -> 006 Dec Format
            Security.SecurityFlag.Bit.SIM_Status = SIM_PH_PIN_REQ;
            break;
        case CME_SIM_PIN2_REQ:              //  Error -> 017 Dec Format
            Security.SecurityFlag.Bit.SIM_Status = SIM_PIN2_REQ;
            break;
        case CME_SIM_PUK_REQ:               //  Error -> 012 Dec Format
        case CMS_SIM_PUK_REQ:               //  Error -> 316 Dec Format
            Security.SecurityFlag.Bit.SIM_Status = SIM_PUK1_REQ;
            break;
        case CME_PH_FSIM_PUK_REQ:           //  Error -> 007 Dec Format
            Security.SecurityFlag.Bit.SIM_Status = SIM_PH_PUK_REQ;
            break;
        case CME_SIM_PUK2_REQ:              //  Error -> 018 Dec Format
            Security.SecurityFlag.Bit.SIM_Status = SIM_PUK2_REQ;
            break;
        case CME_CALL_SETUP_INPROGRESS:     //  Error -> 771 Dec Format
            PhonicCall.PhonicCallFlag.Bit.CallSetupInProgress = 1;
            break;
        case CME_MEMORY_FAILURE:            //  Error -> 023 Dec Format
        case CMS_MEMORY_FAILURE:            //  Error -> 320 Dec Format
            PhoneBook.PhoneBookFlag.Bit.MemoryFailure = 1;
            break;
        case CME_NOT_FOUND:                 //  Error -> 022 Dec Format
            PhoneBook.PhoneBookFlag.Bit.NotFound = 1;
            break;
        case CME_INVALID_INDEX:             //  Error -> 021 Dec Format
        case CMS_INVALID_INDEX:             //  Error -> 321 Dec Format
            PhoneBook.PhoneBookFlag.Bit.InvalidIndex = 1;
            break;
        case CME_MEMORY_FULL:               //  Error -> 020 Dec Format
        case CMS_MEMORY_FULL:               //  Error -> 322 Dec Format
            PhoneBook.PhoneBookFlag.Bit.MemoryFull = 1;
            break;      
        case CMS_INVALID_INPUT_PARAMETER:   //  Error -> 323 Dec Format
            PhoneBook.PhoneBookFlag.Bit.InvalidInputParameter = 1;
            break;
        case CMS_INVALID_INPUT_FORMAT:      //  Error -> 324 Dec Format
            PhoneBook.PhoneBookFlag.Bit.InvalidInputFormat = 1;
            break;          
        case CME_TEXT_STR_TOO_LONG:         //  Error -> 024 Dec Format
            PhoneBook.PhoneBookFlag.Bit.TextStrTooLong = 1;
            break;
        case CME_INVALID_TEXT_CHAR:         //  Error -> 025 Dec Format
            PhoneBook.PhoneBookFlag.Bit.InvalidTextChar = 1;
            break;
        case CME_DIAL_STR_TOO_LONG:         //  Error -> 026 Dec Format
            PhoneBook.PhoneBookFlag.Bit.DialStrTooLong = 1;
            break;
        case CME_INVALID_DIAL_CHAR:         //  Error -> 027 Dec Format
            PhoneBook.PhoneBookFlag.Bit.InvalidDialChar = 1;
            break;
        case CME_UNSPEC_GPRS_ERROR:         //  Error -> 148 Dec Format
            Gprs.GprsFlag.Bit.GprsError = 1;
            break;
        case CME_PDP_FAILUR:                //  Error -> 149 Dec Format
            Gprs.GprsFlag.Bit.PdpFailure = 1;   
            break;
        case CMS_GPRS_SUSPENDED:            //  Error -> 530 Dec Format
            Gprs.GprsFlag.Bit.GprsSuspended = 1;
            break;
        case CME_INVALID_PARAMETERS:        //  Error -> 0053 Dec Format
        case CMS_INVALID_INPUT_VALUE:       //  Error -> 0325 Dec Format
        case CME_INVALID_INPUT_VALUE_:      //  Error -> 0765 Dec Format
        case CME_INVALID_INPUT_VALUE:       //  Error -> 3765 Dec Format
            Sms.SmsFlag.Bit.InvalidInputValue = 1;
            break;
        default:
            break;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        ProcessUnsolicitedCode
 *
 * Overview:        This function is used to manage the unsolicited AT string sended by GSM engine.
 *                  Currently supported: SMS         unsolicited string
 *                                       Phonic Call unsolicited string
 *                                       CREG        unsolicited string
 *                                       CLK         unsolicited string only if "Get Local Timestamp" is enable
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
void GenericCmd_GSM::ProcessUnsolicitedCode(void) {
    if ((Gsm.StateSendCmd != CMD_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }
    if (Gsm.UartFlag.Bit.ReceivedSomething == 0) {
        if (Gsm.GsmFlag.Bit.CringOccurred == 0) {
            return;
        }
        if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) {
            return;
        }
        Gsm.UartFlag.Bit.ReceivedAnswer    = 0;
    }
    Gsm.UartFlag.Bit.ReceivedSomething = 0;

    //========================================================================= 
    //  CRING Unsolicited code 
    if (Gsm.GsmFlag.Bit.CringOccurred == 1) {
        Gsm.GsmFlag.Bit.CringOccurred = 0;
        if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMTI, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMTI)) != 0xFF) {
            //  Received New SMS
            Gsm.GsmFlag.Bit.SmsReceived = 1;
            if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                Sms.SmsReceivedIndex = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1))));
            }
            return;
        }
        if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CRING_VOICE, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CRING_VOICE)) != 0xFF) {
            //  Incoming Call
            Gsm.GsmFlag.Bit.IncomingCall = 1;
            if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CLIP, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CLIP)) != 0xFF) {
                if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                    Gsm.ExtractCharArray((char *)(&PhoneBook.PhoneNumber[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[3] + 2]), sizeof(PhoneBook.PhoneNumber), ASCII_COMMA);                      
                    Gsm.ExtractCharArray((char *)(&PhoneBook.PhoneText[0]),   (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[7] + 1]), sizeof(PhoneBook.PhoneText),   ASCII_COMMA);
                }   
            }
            return;
        }
    }
    //=========================================================================
    
    //=========================================================================
    //  Phonic call unsolicited code
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_ATD_NODIALTONE, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_ATD_NODIALTONE)) != 0xFF) {
        PhonicCall.PhonicCallFlag.Bit.NoDialtone = 1;
        PhonicCall.PhonicCallFlag.Bit.OutgoingCallInProgress = 0;
        return;
    }
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_ATD_BUSY, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_ATD_BUSY)) != 0xFF) {
        PhonicCall.PhonicCallFlag.Bit.Busy = 1;
        PhonicCall.PhonicCallFlag.Bit.OutgoingCallInProgress = 0;
        return;
    }
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_ATD_NOCARRIER, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_ATD_NOCARRIER)) != 0xFF) {
        PhonicCall.PhonicCallFlag.Bit.NoCarrier = 1;
        PhonicCall.PhonicCallFlag.Bit.OutgoingCallInProgress = 0;
        PhonicCall.PhonicCallFlag.Bit.IncomingCallInProgress = 0;
        return;
    }
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_ATD_NOANSWER, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_ATD_NOANSWER)) != 0xFF) {
        PhonicCall.PhonicCallFlag.Bit.NoAnswer = 1;
        PhonicCall.PhonicCallFlag.Bit.OutgoingCallInProgress = 0;
        return;
    }
    //=========================================================================

    //=========================================================================
    //  CREG Unsolicited Result Code
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CREG, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CREG)) != 0xFF) {
        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
            Gsm.CREG_Info.Flag.Bit.Stat = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
            if (Gsm.CREG_Info.Flag.Bit.Stat != 0) {
                Gsm.ExtractCharArray((char *)(&Gsm.CREG_Info.LAC_Info[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[1] + 1]), sizeof(Gsm.CREG_Info.LAC_Info), ASCII_COMMA);
                Gsm.ExtractCharArray((char *)(&Gsm.CREG_Info.CI_Info[0]),  (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[2] + 1]), sizeof(Gsm.CREG_Info.CI_Info),  ASCII_CARRIAGE_RET);             
            }
        }
        return;
    }   
    //=========================================================================

    //=========================================================================
    //  TCP/UDP Unsolicited Result Code
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)CLIENT_CLOSED, (uint8_t *)Gsm.TempStringCompare, strlen(CLIENT_CLOSED)) != 0xFF) {
        TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP
            TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte(0, 1));
        }
        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_NOT_CONNECTED_CODE;
        return;
    }
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CONNECT_OK, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CONNECT_OK)) != 0xFF) {
        TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP
            TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte(0, 1));
        }
        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_CONNECT_OK_CODE;
        return;
    }
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_ALREADY_CONNECT, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_ALREADY_CONNECT)) != 0xFF) {
        TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP
            TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte(0, 1));
        }
        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_ALREADY_CONNECT_CODE;
        return;
    }   
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)TCP_UDP_CONNECT_FAIL, (uint8_t *)Gsm.TempStringCompare, strlen(TCP_UDP_CONNECT_FAIL)) != 0xFF) {
        TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
        if (TcpIp.TcpIpFlag.Bit.MultiIp == MULTI_IP_CONN) {
            //  Multi IP
            TcpIp.TcpIpFlag.Bit.IndexConnection = (uint8_t)(Gsm.ExtractParameterByte(0, 1));
        }   
        TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].State_TCP_UPD = TCP_UDP_CONNECT_FAIL_CODE;
        return;
    }
    //========================================================================= 
    
    //=========================================================================
    //  HTTP Unsolicited Result Code
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)    
    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_HTTP_ACTION, (uint8_t *)Gsm.TempStringCompare, strlen(AT_HTTP_ACTION)) != 0xFF) {
        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
            Http.HttpFlag.Bit.HttpActionMethod = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
            Http.HttpFlag.Bit.HttpStatusCode   = 0;
            Http.HttpFlag.Bit.HttpStatusCode   = Gsm.ExtractParameterByte((Gsm.CharPointers[1] + 1), (Gsm.CharPointers[2] - (Gsm.CharPointers[1] + 1)));
        }
        return;
    }
#endif  
    //========================================================================= 
}
/****************************************************************************/

/****************************************************************************
 * Function:        On_GSM
 *
 * Overview:        This function turn ON the GSM module. Hardware command
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
void GenericCmd_GSM::On_GSM(void) {
    Io.TriggerOn(TRIGGER_1);
    pinMode(GSM_ON_OFF, OUTPUT);
    digitalWrite(GSM_ON_OFF, HIGH);
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95)
    delay(1200);
#endif  
#ifdef FIBOCOM_G510
    delay(900);
#endif
    digitalWrite(GSM_ON_OFF, LOW);
    Io.TriggerOff(TRIGGER_1);
}
/****************************************************************************/

/****************************************************************************
 * Function:        Off_GSM
 *
 * Overview:        This function turn OFF the GSM module. Hardware command
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
void GenericCmd_GSM::Off_GSM(void) {
    pinMode(GSM_ON_OFF, OUTPUT);
    digitalWrite(GSM_ON_OFF, HIGH);
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    delay(2000);
#endif
#ifdef FIBOCOM_G510
    delay(3500);
#endif
#ifdef QUECTEL_M95
    delay(850);
#endif
    digitalWrite(GSM_ON_OFF, LOW);
}
/****************************************************************************/

/****************************************************************************
 * Function:        Reset_GSM
 *
 * Overview:        This function Reset the GSM module. Hardware command
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
void GenericCmd_GSM::Reset_GSM(void) {
    pinMode(GSM_RESET, OUTPUT);
    digitalWrite(GSM_RESET, HIGH);
    delay(500);
    digitalWrite(GSM_RESET, LOW);
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitPowerON_GSM
 *
 * Overview:        This function initialize state machine and flags
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
void GenericCmd_GSM::InitPowerON_GSM(void) {
    Gsm.StateInitSendCmd       = INIT_POWER_ON;
    Gsm.StateSendCmd           = CMD_IDLE;
    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
    Gsm.StateWaitAnswerCmd     = CMD_WAIT_IDLE;
    Gsm.GsmFlag.Bit.GsmInitInProgress    = 1;
    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
    if (Gprs.GprsFlag.Bit.EnableGprsFunctions == 1) {
        //  Enable GPRS Initializations
        Gsm.GsmFlag.Bit.GprsInitInProgress = 1; 
    }
    if (Http.HttpFlag.Bit.EnableHttpFunctions == 1) {
        //  Enable HTPP Initializations
        Gsm.GsmFlag.Bit.HttpInitInProgress = 1; 
    }
    Gsm.RetryCounter = 0;
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitReset_GSM
 *
 * Overview:        This function initialize state machine and flags
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
void GenericCmd_GSM::InitReset_GSM(void) {
    Gsm.StateInitSendCmd       = INIT_RESET;
    Gsm.StateSendCmd           = CMD_IDLE;
    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
    Gsm.StateWaitAnswerCmd     = CMD_WAIT_IDLE;
    Gsm.GsmFlag.Bit.GsmInitInProgress    = 1;
    Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 0;
    if (Gprs.GprsFlag.Bit.EnableGprsFunctions == 1) {
        //  Enable GPRS Initializations
        Gsm.GsmFlag.Bit.GprsInitInProgress = 1;
    }
    if (Http.HttpFlag.Bit.EnableHttpFunctions == 1) {
        //  Enable HTPP Initializations
        Gsm.GsmFlag.Bit.HttpInitInProgress = 1; 
    }
    Gsm.RetryCounter = 0;
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitAligneBaudRate_GSM
 *
 * Overview:        This function aligne the Arduino's BaudRate with the GSM's BaudRate
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
void GenericCmd_GSM::InitAligneBaudRate_GSM(void) { 
    if (Gsm.AlignBaudRateSelector > BAUD_115200) {
        Gsm.AlignBaudRateSelector = BAUD_2400;
        Gsm.InitReset_GSM();
        return;
    }   
    //---------------------------------------
    //  If open close the uart
    Gsm.SetBaudRateUart1(false, AlignBaudRateSelector);
    //---------------------------------------

    //---------------------------------------
    //  Sets new UART BaudRate
    Gsm.SetBaudRateUart1(true, AlignBaudRateSelector);
    //---------------------------------------

    //---------------------------------------   
    Gsm.StateInitSendCmd       = INIT_CMD_AT;
    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
    Gsm.RetryCounter = 0;   
    //---------------------------------------

#ifdef UART_FIND_BAUDRATE_DEBUG
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SEP1, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SEP1));
    Gsm.PrintScreenDebugMode();
    Gsm.ReadStringFLASH((uint8_t *)STR_UART_FIND_BAUDRATE_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_FIND_BAUDRATE_DEBUG));
    Gsm.PrintScreenDebugMode();
    switch (AlignBaudRateSelector)
    {
        case BAUD_2400:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_2400_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_2400_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;
        case BAUD_4800:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_4800_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_4800_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;
        case BAUD_9600:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_9600_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_9600_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;
        case BAUD_19200:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_19200_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_19200_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;
        case BAUD_38400:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_38400_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_38400_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;
        case BAUD_57600:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_57600_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_57600_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;
        case BAUD_115200:
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BAUDRATE_115200_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BAUDRATE_115200_DEBUG));
            Gsm.PrintScreenDebugMode();
            break;  
        default:
            
            break;          
    }
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SEP2, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SEP2));
    Gsm.PrintScreenDebugMode();
    Serial.print("\n");
#endif
    ++Gsm.AlignBaudRateSelector;
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitGsmSendCmd
 *
 * Overview:        This function is used to send the initialization commands to GSM engine.
 *                  The states machine is realized with a Switch statement
 *                  The complementary function of this is the "InitGsmWaitAnswer" function
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
void GenericCmd_GSM::InitGsmSendCmd(void) {
    uint8_t Index;
    
    if ((Gsm.StateInitWaitAnswerCmd != INIT_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }
    
    Gsm.GsmFlag.Bit.GsmPowerOn = 0;
    Gsm.GsmFlag.Bit.GsmReset   = 0;
    
    Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array) - 1));
    switch (Gsm.StateInitSendCmd)
    {
        case INIT_IDLE:
            Gsm.RetryCounter = 0;
            break;  
        case INIT_POWER_ON:
#ifdef GSM_ON_OFF_CMD_DEBUG
            Serial.print("\n");
            Gsm.ReadStringFLASH((uint8_t *)STR_PERFORM_POWER_ON_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_PERFORM_POWER_ON_DEBUG));
            Gsm.PrintScreenDebugMode();
#endif
            Gsm.StateInitSendCmd       = INIT_IDLE;
            Gsm.StateInitWaitAnswerCmd = INIT_WAIT_ANSWER_POWER_ON;
            Gsm.UartState              = UART_WAITDATA_STATE;
            Gsm.GsmFlag.Bit.GsmPowerOn = 1;
            On_GSM();
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            break;
        case INIT_RESET:
#ifdef GSM_ON_OFF_CMD_DEBUG
            Serial.print("\n");
            Gsm.ReadStringFLASH((uint8_t *)STR_PERFORM_RESET_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_PERFORM_RESET_DEBUG));
            Gsm.PrintScreenDebugMode();
#endif
            Gsm.StateInitSendCmd       = INIT_IDLE;
            Gsm.StateInitWaitAnswerCmd = INIT_WAIT_ANSWER_RESET;
            Gsm.UartState              = UART_WAITDATA_STATE;
            Gsm.GsmFlag.Bit.GsmReset   = 1;
            Reset_GSM();
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            break;
        case INIT_CMD_AT:
            Gsm.ReadStringFLASH((uint8_t *)AT, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT));
            Gsm.WritePointer = strlen(AT);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_AT, INIT_CMD_AT_NOTHING);
            break;  
        case INIT_CMD_ATF0:
            Gsm.ReadStringFLASH((uint8_t *)ATF0, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATF0));
            Gsm.WritePointer = strlen(ATF0);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_ATF0, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_ATE0:         
            Gsm.ReadStringFLASH((uint8_t *)ATE0, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATE0));
            Gsm.WritePointer = strlen(ATE0);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_ATE0, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_ATQ0: 
            Gsm.ReadStringFLASH((uint8_t *)ATQ0, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ0));
            Gsm.WritePointer = strlen(ATQ0);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_ATQ0, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_ATV1:
            Gsm.ReadStringFLASH((uint8_t *)ATV1, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATV1));
            Gsm.WritePointer = strlen(ATV1);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_ATV1, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CMEE:
            Gsm.ReadStringFLASH((uint8_t *)AT_CMEE, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMEE));
            Gsm.WritePointer = strlen(AT_CMEE);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CMEE, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_IPR:      
            Gsm.ReadStringFLASH((uint8_t *)AT_IPR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_IPR));
            Gsm.WritePointer = strlen(AT_IPR);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_IPR, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_ICF:
            Gsm.ReadStringFLASH((uint8_t *)AT_ICF, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_ICF));
            Gsm.WritePointer = strlen(AT_ICF);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_ICF, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_IFC:
            Gsm.ReadStringFLASH((uint8_t *)AT_IFC, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_IFC));
            Gsm.WritePointer = strlen(AT_IFC);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_IFC, INIT_CMD_AT_NOTHING);
            break;          
        case INIT_CMD_CPINQ:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)ATQ_CPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CPIN));
            Gsm.WritePointer = strlen(ATQ_CPIN);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CPINQ, INIT_CMD_AT_NOTHING);
            break;  
        case INIT_CMD_SPIC:
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
            Gsm.ReadStringFLASH((uint8_t *)ATQ_SPIC, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_SPIC));
            Gsm.WritePointer = strlen(ATQ_SPIC);
#endif
#ifdef FIBOCOM_G510
            Gsm.ReadStringFLASH((uint8_t *)ATQ_TPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_TPIN));
            Gsm.WritePointer = strlen(ATQ_TPIN);
#endif
#ifdef QUECTEL_M95
            Gsm.ReadStringFLASH((uint8_t *)ATQ_QTRPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_QTRPIN));
            Gsm.WritePointer = strlen(ATQ_QTRPIN);
#endif
#ifdef AI_THINKER_A9
            Gsm.ReadStringFLASH((uint8_t *)ATQ_CPINC, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CPINC));
            Gsm.WritePointer = strlen(ATQ_CPINC);
#endif          
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_SPIC, INIT_CMD_AT_NOTHING);
            break;      
        case INIT_CMD_CPIN:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)AT_CPIN, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPIN));
            Security.EepromAddPinPuk(SIM_PIN1_REQ);
            Index = Security.LoadDataFromEepromIntoGsmDataArray(Security.EepromAddPin, (CPIN_OFFSET + 5), CPIN_OFFSET);
            Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;
            Gsm.WritePointer = Index;
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CPIN, INIT_CMD_AT_NOTHING);
            break;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95) || defined(FIBOCOM_G510)
        case INIT_CMD_CALM:
            Gsm.ReadStringFLASH((uint8_t *)AT_CALM, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CALM));
            Gsm.WritePointer = strlen(AT_CALM);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CALM, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CRSL:
            Gsm.ReadStringFLASH((uint8_t *)AT_CRSL, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CRSL));
            Gsm.WritePointer = strlen(AT_CRSL);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CRSL, INIT_CMD_AT_NOTHING);
            break;
#endif          
        case INIT_CMD_CLVL:
            Gsm.ReadStringFLASH((uint8_t *)AT_CLVL, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CLVL));
            Gsm.WritePointer = strlen(AT_CLVL);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CLVL, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CMIC:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_1S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)AT_CMIC, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMIC));
            Gsm.WritePointer = strlen(AT_CMIC);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CMIC, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CRC:
            Gsm.ReadStringFLASH((uint8_t *)AT_CRC, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CRC));
            Gsm.WritePointer = strlen(AT_CRC);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CRC, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CLIP:
            Gsm.GsmFlag.Bit.UartTimeOutSelect = T_5S_UART_TIMEOUT;
            Gsm.ReadStringFLASH((uint8_t *)AT_CLIP, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CLIP));
            Gsm.WritePointer = strlen(AT_CLIP);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CLIP, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CMGF:
            Gsm.ReadStringFLASH((uint8_t *)AT_CMGF, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMGF));
            Gsm.WritePointer = strlen(AT_CMGF);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CMGF, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CSCS:
            Gsm.ReadStringFLASH((uint8_t *)AT_CSCS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CSCS));
            Gsm.WritePointer = strlen(AT_CSCS);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CSCS, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CNMI:
            Gsm.ReadStringFLASH((uint8_t *)AT_CNMI, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CNMI));
            Gsm.WritePointer = strlen(AT_CNMI);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CNMI, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CPMS:
            delay(1000);
            Gsm.ReadStringFLASH((uint8_t *)AT_CPMS_DEF, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPMS_DEF));
            Gsm.WritePointer = strlen(AT_CPMS_DEF);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CPMS, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CSMP:
            Gsm.ReadStringFLASH((uint8_t *)AT_CSMP, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CSMP));
            Gsm.WritePointer = strlen(AT_CSMP);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CSMP, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CPBS:
            Gsm.ReadStringFLASH((uint8_t *)AT_CPBS_DEF, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPBS_DEF));
            Gsm.WritePointer = strlen(AT_CPBS_DEF);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CPBS, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_CREG:
            Gsm.ReadStringFLASH((uint8_t *)AT_CREG, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CREG));
            Gsm.WritePointer = strlen(AT_CREG);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_CREG, INIT_CMD_AT_NOTHING);
            break;
        case INIT_CMD_ATW0:
            Gsm.ReadStringFLASH((uint8_t *)ATW0, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATW0));
            Gsm.WritePointer = strlen(ATW0);
            Gsm.StartSendData(INIT_IDLE, INIT_WAIT_ANSWER_CMD_ATW0, INIT_CMD_AT_NOTHING);
            break;
        default:
            break;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitGsmWaitAnswer
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
void GenericCmd_GSM::InitGsmWaitAnswer(void) {
    uint8_t StrPointer = 0xFF;
    
    if ((Gsm.StateInitSendCmd != INIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
        return;
    }
    if (Gsm.UartFlag.Bit.ReceivedAnswer == 0) {
        return;
    }
    Gsm.UartFlag.Bit.ReceivedAnswer = 0;
    
    switch (Gsm.StateInitWaitAnswerCmd)
    {
        case INIT_WAIT_IDLE:
            break;

        case INIT_WAIT_ANSWER_RESET:
            if (Gsm.ReadPointer > 0) {          
                if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_PWR_ON, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_PWR_ON)) != 0xFF) {
#ifdef AI_THINKER_A9
                    Gsm.StateInitSendCmd = INIT_CMD_ATE0;   //  PERFORMED RESET -  (SET NEXT STEP -> CMD ATE0)
#else
                    Gsm.StateInitSendCmd = INIT_CMD_ATF0;   //  PERFORMED RESET -  (SET NEXT STEP -> CMD ATF0)
#endif
                    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
                    Gsm.RetryCounter = 0;
                }
                else {
                    Gsm.GsmFlag.Bit.GsmModuleOff         = 0;
                    Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect = 1;
                    Gsm.StateInitSendCmd       = INIT_CMD_AT;
                    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
                    Gsm.RetryCounter = 0;
                    Gsm.AlignBaudRateSelector = 0;
                    break;
                }               
            } else {            
                Gsm.InitPowerON_GSM();
            }
            break;
            
        case INIT_WAIT_ANSWER_POWER_ON:     
            if (Gsm.ReadPointer > 0) {
                if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_PWR_ON, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_PWR_ON)) != 0xFF) {
#ifdef AI_THINKER_A9
                    Gsm.StateInitSendCmd = INIT_CMD_ATE0;   //  PERFORMED POWER ON -  (SET NEXT STEP -> CMD ATE0)
#else
                    Gsm.StateInitSendCmd = INIT_CMD_ATF0;   //  PERFORMED POWER ON -  (SET NEXT STEP -> CMD ATF0)
#endif
                    Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
                    Gsm.RetryCounter = 0;
                    Gsm.GsmFlag.Bit.GsmModuleOff = 0;
                } else {
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_PWR_OFF, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_PWR_OFF)) != 0xFF) {
                        //  PERFORMED POWER OFF
                        Gsm.GsmFlag.Bit.GsmModuleOff = 1;
                        Gsm.InitPowerON_GSM();
                        break;
                    } else {
                        //  No answer after Power ON/OFF. Starts procedure to send ripetitive AT command to align the
                        //  UART BaudRate of Arduino with the BaudRate of the GSM module. In this case the system suppose
                        //  that GSM module is sets to work in AutoBaud mode.
                        //  If the GSM board have been set with a fixed baudrate, and this is different from the baudrate
                        //  setted for the Arduino's UART interface, it is necessary execute an alignment of the Arduino's
                        //  UART interface baudrate with the GSM's baudrate
                        Gsm.GsmFlag.Bit.GsmModuleOff         = 0;
                        Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect = 1;
                        Gsm.StateInitSendCmd       = INIT_CMD_AT;
                        Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
                        Gsm.RetryCounter = 0;
                        Gsm.AlignBaudRateSelector = 0;
                        break;
                    }
                }
            } else {
                //  No answer after Power ON/OFF. Starts procedure to send ripetitive AT command to align the
                //  UART BaudRate of Arduino with the BaudRate of the GSM module. In this case the system suppose
                //  that GSM module is sets to work in AutoBaud mode.
                //  If the GSM board have been set with a fixed baudrate, and this is different from the baudrate
                //  setted for the Arduino's UART interface, it is necessary execute an alignment of the Arduino's
                //  UART interface baudrate with the GSM's baudrate
                Gsm.GsmFlag.Bit.GsmModuleOff         = 0;
                Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect = 1;
                Gsm.StateInitSendCmd       = INIT_CMD_AT;
                Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
                Gsm.RetryCounter = 0;
                Gsm.AlignBaudRateSelector = 0;
            }   
            break;
            
        case INIT_WAIT_ANSWER_CMD_AT:
        case INIT_WAIT_ANSWER_CMD_ATF0:
        case INIT_WAIT_ANSWER_CMD_ATE0:
        case INIT_WAIT_ANSWER_CMD_ATQ0:
        case INIT_WAIT_ANSWER_CMD_ATV1:
        case INIT_WAIT_ANSWER_CMD_CMEE:
        case INIT_WAIT_ANSWER_CMD_IPR:
        case INIT_WAIT_ANSWER_CMD_ICF:
        case INIT_WAIT_ANSWER_CMD_IFC:
        case INIT_WAIT_ANSWER_CMD_CPINQ:
        case INIT_WAIT_ANSWER_CMD_SPIC:
        case INIT_WAIT_ANSWER_CMD_CPIN:
        case INIT_WAIT_ANSWER_CMD_CALM:
        case INIT_WAIT_ANSWER_CMD_CRSL:
        case INIT_WAIT_ANSWER_CMD_CLVL:
        case INIT_WAIT_ANSWER_CMD_CMIC:
        case INIT_WAIT_ANSWER_CMD_CRC:
        case INIT_WAIT_ANSWER_CMD_CLIP:
        case INIT_WAIT_ANSWER_CMD_CMGF:
        case INIT_WAIT_ANSWER_CMD_CSCS:
        case INIT_WAIT_ANSWER_CMD_CNMI:
        case INIT_WAIT_ANSWER_CMD_CPMS:
        case INIT_WAIT_ANSWER_CMD_CSMP:
        case INIT_WAIT_ANSWER_CMD_CPBS:
        case INIT_WAIT_ANSWER_CMD_CREG:
        case INIT_WAIT_ANSWER_CMD_ATW0:
            if (Gsm.ReadPointer > 0) {
                if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_OK, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_OK)) != 0xFF) {
                    Gsm.RetryCounter = 0;
                    Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect = 0;
                    
                    switch (Gsm.StateInitWaitAnswerCmd)
                    {
                        case INIT_WAIT_ANSWER_CMD_AT:
                            //--------------------------------------------
                            //  Procedure to find GSM Uart BaudRate inprogress
                            if (Gsm.GsmFlag.Bit.AlignBaudRateProcess != 0) {
                                //  Found GSM BaudRate. Tries to force the GSM UART in Autobaud mode  
                                Gsm.StateInitSendCmd = INIT_CMD_IPR;
                                break;  
                            }
                            //--------------------------------------------
#ifdef AI_THINKER_A9
                            Gsm.StateInitSendCmd = INIT_CMD_ATE0;   //  CMD AT - OK (SET NEXT STEP -> CMD ATE0)
#else
                            Gsm.StateInitSendCmd = INIT_CMD_ATF0;   //  CMD AT - OK (SET NEXT STEP -> CMD ATF0)
#endif
                            break;
                        case INIT_WAIT_ANSWER_CMD_ATF0:                 //  CMD ATF0 - OK (SET NEXT STEP -> CMD ATE0)
                            Gsm.StateInitSendCmd = INIT_CMD_ATE0;
                            break;
                        case INIT_WAIT_ANSWER_CMD_ATE0:                 //  CMD ATE0 - OK (SET NEXT STEP -> CMD ATQ0)
                            Gsm.StateInitSendCmd = INIT_CMD_ATQ0;
                            break;
                        case INIT_WAIT_ANSWER_CMD_ATQ0:                 //  CMD ATQ0 - OK (SET NEXT STEP -> CMD ATV1)
                            Gsm.StateInitSendCmd = INIT_CMD_ATV1;
                            break;
                        case INIT_WAIT_ANSWER_CMD_ATV1:                 //  CMD ATV1 - OK (SET NEXT STEP -> CMD AT+CMEE)
                            Gsm.StateInitSendCmd = INIT_CMD_CMEE;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CMEE:                 //  CMD CMEE - OK (SET NEXT STEP -> CMD AT+IPR)
                            Gsm.StateInitSendCmd = INIT_CMD_IPR;
                            break;
                        case INIT_WAIT_ANSWER_CMD_IPR:
                            //--------------------------------------------
                            //  Procedure to find GSM Uart BaudRate inprogress. Final step 
                            if (Gsm.GsmFlag.Bit.AlignBaudRateProcess != 0) {
                                //  Found GSM BaudRate. Tries to save new profile 
                                Gsm.StateInitSendCmd = INIT_CMD_ATW0;
                                break;
                            }
                            //--------------------------------------------
#ifdef SIMCOM_SIM800C
                            Gsm.StateInitSendCmd = INIT_CMD_IFC;    //  CMD IPR - OK (SET NEXT STEP -> CMD AT+IFC)
#else
                            Gsm.StateInitSendCmd = INIT_CMD_ICF;    //  CMD IPR - OK (SET NEXT STEP -> CMD AT+ICF)
#endif
                            break;
                        case INIT_WAIT_ANSWER_CMD_ICF:
#ifdef AI_THINKER_A9
                            Gsm.StateInitSendCmd = INIT_CMD_CPINQ;  //  CMD ICF - OK (SET NEXT STEP -> CMD AT+CPINQ)
#else
                            Gsm.StateInitSendCmd = INIT_CMD_IFC;    //  CMD ICF - OK (SET NEXT STEP -> CMD AT+IFC)
#endif
                            break;                          
                        case INIT_WAIT_ANSWER_CMD_IFC:                  //  CMD IFC - OK (SET NEXT STEP -> CMD AT+CPINQ)
                            Gsm.StateInitSendCmd = INIT_CMD_CPINQ;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CPINQ:
                            if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_READY, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_READY)) != 0xFF) {
                                //  CMD CPINQ - IF "+CPIN: READY" -> (SET NEXT STEP -> CMD AT+CLVL)
#ifdef AI_THINKER_A9
                                Gsm.StateInitSendCmd = INIT_CMD_CLVL;
#else
                                Gsm.StateInitSendCmd = INIT_CMD_CALM;
#endif
                            } else {
                                if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPIN_PIN, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPIN_PIN)) != 0xFF) {
                                    //  PIN Required
                                    //  First step -> Check times remained to input SIM PIN/PUK
                                    //  CMD CPIN - IF "+CPIN: SIM PIN" -> (SET NEXT STEP -> CMD AT+SPIC)
                                    Gsm.StateInitSendCmd = INIT_CMD_SPIC;
                                    break;
                                } else {
                                    //  Otherwise Reset GSM
                                    Gsm.InitReset_GSM();
                                    break;
                                }
                            }
                            break;
                        case INIT_WAIT_ANSWER_CMD_SPIC:
                            //Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
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
                                    Security.SecurityFlag.Bit.PinRetry = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 1), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 1))));
                                }
                            }
                            if (Security.SecurityFlag.Bit.PinRetry > 1) {
                                //  If PinRetry > 1 then try to send PIN CODE
                                Gsm.StateInitSendCmd = INIT_CMD_CPIN;
                            } else {
                                //  Otherwise Reset GSM
                                Gsm.InitReset_GSM();
                            }
                            break;
                        case INIT_WAIT_ANSWER_CMD_CPIN:                     //  CMD CPIN - OK (SET NEXT STEP -> CMD AT+CLVL)
#ifdef AI_THINKER_A9
                            Gsm.StateInitSendCmd = INIT_CMD_CLVL;
#else
                            Gsm.StateInitSendCmd = INIT_CMD_CALM;
#endif
                            break;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95) || defined(FIBOCOM_G510)                           
                        case INIT_WAIT_ANSWER_CMD_CALM:                     //  CMD CALM - OK (SET NEXT STEP -> CMD AT+CRSL)
                            Gsm.StateInitSendCmd = INIT_CMD_CRSL;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CRSL:                     //  CMD CRSL - OK (SET NEXT STEP -> CMD AT+CLVL)
                            Gsm.StateInitSendCmd = INIT_CMD_CLVL;
                            break;
#endif                          
                        case INIT_WAIT_ANSWER_CMD_CLVL:                     //  CMD CLVL - OK (SET NEXT STEP -> CMD AT+CMIC)
                            Gsm.StateInitSendCmd = INIT_CMD_CMIC;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CMIC:                     //  CMD CMIC - OK (SET NEXT STEP -> CMD AT+CRC)
                            Gsm.StateInitSendCmd = INIT_CMD_CRC;            
                            break;
                        case INIT_WAIT_ANSWER_CMD_CRC:                      //  CMD CRC - OK (SET NEXT STEP -> CMD AT+CLIP)
                            Gsm.StateInitSendCmd = INIT_CMD_CLIP;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CLIP:                     //  CMD CLIP - OK (SET NEXT STEP -> CMD AT+CMGF)
                            Gsm.StateInitSendCmd = INIT_CMD_CMGF;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CMGF:                     //  CMD CMGF - OK (SET NEXT STEP -> CMD AT+CSCS)
                            Gsm.StateInitSendCmd = INIT_CMD_CSCS;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CSCS:                     //  CMD CSCS - OK (SET NEXT STEP -> CMD AT+CNMI)
                            Gsm.StateInitSendCmd = INIT_CMD_CNMI;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CNMI:                     //  CMD CNMI - OK (SET NEXT STEP -> CMD AT+CPMS)
                            Gsm.StateInitSendCmd = INIT_CMD_CPMS;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CPMS:                     //  CMD CPMS - OK (SET NEXT STEP -> CMD AT+CSMP)
                            Gsm.StateInitSendCmd = INIT_CMD_CSMP;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CSMP:                     //  CMD CSMP - OK (SET NEXT STEP -> CMD AT+CPBS)
                            Gsm.StateInitSendCmd = INIT_CMD_CPBS;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CPBS:                     //  CMD CPBS - OK (SET NEXT STEP -> CMD AT+CREG)
                            Gsm.StateInitSendCmd = INIT_CMD_CREG;
                            break;
                        case INIT_WAIT_ANSWER_CMD_CREG:                     //  CMD CREG - OK (SET NEXT STEP -> CMD AT+CFUN)
                            Gsm.StateInitSendCmd = INIT_IDLE;
                            Gsm.GsmFlag.Bit.GsmInitInProgress = 0;
                            if (Gsm.GsmFlag.Bit.GprsInitInProgress == 1) {
                                //  Starts GPRS Initialization
                                Gsm.StateInitSendCmd = INIT_CMD_QUERY_CGATT;
                                break;
                            }
                            Io.LedOff(TRIGGER_3);
                            break;
                        case INIT_WAIT_ANSWER_CMD_ATW0:                     //  CMD ATW0 - OK (Wait that watchdog expired. Arduino reset itself automatically)
                            //--------------------------------------------
                            //  New profile saved. Wait that watchdog expired
                            //  Arduino reset itself automatically
                            if (Gsm.GsmFlag.Bit.AlignBaudRateProcess != 0) {
                                Gsm.GsmFlag.Bit.AlignBaudRateProcess = 0;
                                wdt_enable(WDTO_15MS);
                                while(1) {}     //  Wait Software Arduino Reset
                            }
                            //--------------------------------------------
                            break;
                        default:
                            break;
                    }
                } else {
                    Gsm.ProcessGsmError();
                    if (Gsm.RetryCounter++ < 2) {
                        switch (Gsm.StateInitWaitAnswerCmd)
                        {
                            case INIT_WAIT_ANSWER_CMD_AT:           //  Retry to send command AT
                                Gsm.StateInitSendCmd = INIT_CMD_AT;
                                break;
                            case INIT_WAIT_ANSWER_CMD_ATF0:         //  Retry to send command ATF0
                                Gsm.StateInitSendCmd = INIT_CMD_ATF0;
                                break;
                            case INIT_WAIT_ANSWER_CMD_ATE0:         //  Retry to send command ATE0
                                Gsm.StateInitSendCmd = INIT_CMD_ATE0;
                                break;
                            case INIT_WAIT_ANSWER_CMD_ATQ0:         //  Retry to send command ATQ0
                                Gsm.StateInitSendCmd = INIT_CMD_ATQ0;
                                break;
                            case INIT_WAIT_ANSWER_CMD_ATV1:         //  Retry to send command ATV1
                                Gsm.StateInitSendCmd = INIT_CMD_ATV1;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CMEE:         //  Retry to send command AT+CMEE
                                Gsm.StateInitSendCmd = INIT_CMD_CMEE;
                                break;
                            case INIT_WAIT_ANSWER_CMD_IPR:          //  Retry to send command AT+IPR
                                Gsm.StateInitSendCmd = INIT_CMD_IPR;
                                break;
                            case INIT_WAIT_ANSWER_CMD_ICF:          //  Retry to send command AT+ICF
                                Gsm.StateInitSendCmd = INIT_CMD_ICF;
                                break;
                            case INIT_WAIT_ANSWER_CMD_IFC:          //  Retry to send command AT+IFC
                                Gsm.StateInitSendCmd = INIT_CMD_IFC;
                                break;                                  
                            case INIT_WAIT_ANSWER_CMD_CPINQ:        //  Retry to send command AT+CPINQ
                                Gsm.StateInitSendCmd = INIT_CMD_CPINQ;
                                break;
                            case INIT_WAIT_ANSWER_CMD_SPIC:         //  Retry to send command AT+SPIC
                                Gsm.StateInitSendCmd = INIT_CMD_SPIC;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CPIN:         //  Retry to send command AT+CPIN                       
                                if (Security.SecurityFlag.Bit.IncorrectPSW == 1) {
                                    Security.SecurityFlag.Bit.IncorrectPSW = 0;
                                    Gsm.InitReset_GSM();
                                    break;                                  
                                }
                                Gsm.StateInitSendCmd = INIT_CMD_CPIN;
                                break;
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95) || defined(FIBOCOM_G510)                           
                            case INIT_WAIT_ANSWER_CMD_CALM:         //  Retry to send command AT+CALM
                                Gsm.StateInitSendCmd = INIT_CMD_CALM;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CRSL:         //  Retry to send command AT+CRSL
                                Gsm.StateInitSendCmd = INIT_CMD_CRSL;
                                break;
#endif
                            case INIT_WAIT_ANSWER_CMD_CLVL:         //  Retry to send command AT+CLVL
                                Gsm.StateInitSendCmd = INIT_CMD_CLVL;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CMIC:         //  Retry to send command AT+CMIC
                                Gsm.StateInitSendCmd = INIT_CMD_CMIC;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CRC:          //  Retry to send command AT+CRC
                                Gsm.StateInitSendCmd = INIT_CMD_CRC;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CLIP:         //  Retry to send command AT+CLIP
                                Gsm.StateInitSendCmd = INIT_CMD_CLIP;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CMGF:         //  Retry to send command AT+CMGF
                                Gsm.StateInitSendCmd = INIT_CMD_CMGF;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CSCS:         //  Retry to send command AT+CSCS
                                Gsm.StateInitSendCmd = INIT_CMD_CSCS;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CNMI:         //  Retry to send command AT+CNMI
                                Gsm.StateInitSendCmd = INIT_CMD_CNMI;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CPMS:         //  Retry to send command AT+CPMS
                                Gsm.StateInitSendCmd = INIT_CMD_CPMS;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CSMP:         //  Retry to send command AT+CSMP
                                Gsm.StateInitSendCmd = INIT_CMD_CSMP;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CPBS:         //  Retry to send command AT+CPBS
                                Gsm.StateInitSendCmd = INIT_CMD_CPBS;
                                break;
                            case INIT_WAIT_ANSWER_CMD_CREG:         //  Retry to send command AT+CREG
                                Gsm.StateInitSendCmd = INIT_CMD_CREG;
                                break;
                            case INIT_WAIT_ANSWER_CMD_ATW0:         //  Retry to send command ATW0
                                Gsm.StateInitSendCmd = INIT_CMD_ATW0;
                                break;
                            default:
                                break;
                        }                       
                    } else {
                        if ((Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect != 1) && (Gsm.GsmFlag.Bit.AlignBaudRateProcess != 1)) {
                            Gsm.InitReset_GSM();
                        } else {
                            Gsm.CallAutoBaudeFunctions();
                        }
                        break;
                    }
                }
                Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
            } else {
                Gsm.GsmFlag.Bit.GsmNoAnswer = 1;        //  No answer from GSM module
                if ((Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect != 1) && (Gsm.GsmFlag.Bit.AlignBaudRateProcess != 1)) {
                    Gsm.GsmFlag.Bit.GsmNoAnswer = 0;
                    Gsm.InitReset_GSM();
                } else {
                    Gsm.CallAutoBaudeFunctions();
                }
                break;
            }
            break;
            
        default:
            break;
    }
}

void GenericCmd_GSM::CallAutoBaudeFunctions(void) {
    Gsm.ProcessGsmError();
    if (Gsm.RetryCounter++ < 3) {       
        if (Gsm.StateInitWaitAnswerCmd == INIT_WAIT_ANSWER_CMD_AT) {
            Gsm.StateInitSendCmd = INIT_CMD_AT;
            Gsm.StateInitWaitAnswerCmd = INIT_WAIT_IDLE;
            Gsm.GsmFlag.Bit.GsmNoAnswer = 0;
        }
    } else {
        //---------------------------------------------------
        //  If no answer from GSM module starts reset procedure
        if (Gsm.GsmFlag.Bit.GsmNoAnswer != 0) {
            Gsm.GsmFlag.Bit.GsmNoAnswer = 0;
            Gsm.InitReset_GSM();
            return;
        }
        //---------------------------------------------------
        
        //  GSM AT command AutoBaud test failed !!!
        //  Starts procedure to align Arduino's Baudrate with GSM's Baudrate
        if (Gsm.GsmFlag.Bit.AlignBaudRateProcess != 1) {
            Gsm.AlignBaudRateSelector = BAUD_2400;
        }
        if (Gsm.AlignBaudRateSelector > BAUD_115200) {
            Gsm.AlignBaudRateSelector = BAUD_2400;
        }
        Gsm.GsmFlag.Bit.AT_CmdAutoBaudDetect = 0;
        Gsm.GsmFlag.Bit.AlignBaudRateProcess = 1;
        Gsm.InitAligneBaudRate_GSM();
    }   
}
/****************************************************************************/

/****************************************************************************
 * Function:        ErrorCodeStringConverter
 *
 * Overview:        This function is used to convert string error code in unsigned short error code
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Pointer: Array pointer to the error code
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
void GenericCmd_GSM::ErrorCodeStringConverter(uint8_t Pointer) {
    uint8_t Counter;
    uint8_t Mul;
    
    GsmErrorCode = 0;
    Counter = 0;
    Mul = 1;
    do {
        if (Counter < 4) {
            if ((Gsm.GSM_Data_Array[Pointer] >= ASCII_0) && (Gsm.GSM_Data_Array[Pointer] <= ASCII_9)) {
                GsmErrorCode *= Mul;
                Mul *= 10;
                GsmErrorCode += (Gsm.GSM_Data_Array[Pointer] - 0x30);
#ifdef GSM_ERROR_CODE
                Gsm.ReadStringFLASH((uint8_t *)STR_ERROR_CODE_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_ERROR_CODE_DEBUG));
                Gsm.PrintScreenDebugMode();
                Serial.print(GsmErrorCode);
                Serial.print("\n");             
#endif
            } else {
                break;
            }
        }
    } while (Gsm.GSM_Data_Array[Pointer++] != ASCII_CARRIAGE_RET);
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetAllFlags
 *
 * Overview:        This function is used to reset flags
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
void GenericCmd_GSM::ResetAllFlags(void) {
    Gsm.GsmFlag.Bit.GsmSendCmdInProgress              = 0;
    Gsm.GsmFlag.Bit.CringOccurred                     = 0;
    Gsm.GsmFlag.Bit.ErrorWithoutCode                  = 0;
    Gsm.GsmFlag.Bit.UnknownError                      = 0;
    Gsm.GsmFlag.Bit.OperationNotAllowed               = 0;
    Gsm.GsmFlag.Bit.OperationNotSupported             = 0;
    Gsm.SimFlag.Bit.SIM_OutOfPhoneCredit              = 0;
    Gsm.SimFlag.Bit.SIM_NotInserted                   = 0;
    Gsm.SimFlag.Bit.SIM_Failure                       = 0;
    Gsm.SimFlag.Bit.SIM_Busy                          = 0;
    Gsm.SimFlag.Bit.SIM_NotReady                      = 0;
    Gsm.SimFlag.Bit.SIM_Wrong                         = 0;
    
    Security.SecurityFlag.Bit.IncorrectPSW            = 0;
    
    PhoneBook.PhoneBookFlag.Bit.MemoryFailure         = 0;
    PhoneBook.PhoneBookFlag.Bit.NotFound              = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidIndex          = 0;
    PhoneBook.PhoneBookFlag.Bit.MemoryFull            = 0;
    PhoneBook.PhoneBookFlag.Bit.TextStrTooLong        = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidTextChar       = 0;
    PhoneBook.PhoneBookFlag.Bit.DialStrTooLong        = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidDialChar       = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidInputParameter = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidInputFormat    = 0;

    Sms.SmsFlag.Bit.InvalidInputValue                 = 0;
    Sms.SmsFlag.Bit.SendSmsInProgress                 = 0;
    Sms.SmsFlag.Bit.WriteSmsInProgress                = 0;
    
    PhonicCall.PhonicCallFlag.Bit.NoDialtone          = 0;
    PhonicCall.PhonicCallFlag.Bit.Busy                = 0;
    PhonicCall.PhonicCallFlag.Bit.NoCarrier           = 0;
    PhonicCall.PhonicCallFlag.Bit.NoAnswer            = 0;
    
    Gprs.GprsFlag.Bit.GprsError                       = 0;
    Gprs.GprsFlag.Bit.GprsSuspended                   = 0;
    Gprs.GprsFlag.Bit.PdpFailure                      = 0;
    
    Http.HttpFlag.Bit.HttpStatusMode                  = 0;
    Http.HttpFlag.Bit.HttpStatus                      = 0;
    Http.HttpFlag.Bit.HttpStatusCode                  = 0;
    Http.HttpFlag.Bit.HttpActionMethod                = 0;
    
    TcpIp.TcpIpStateFlag.BitState[0].ServerState      = 0;
    TcpIp.TcpIpStateFlag.BitState[1].ServerState      = 0;
    TcpIp.TcpIpStateFlag.BitState[2].ServerState      = 0;
    TcpIp.TcpIpStateFlag.BitState[0].ClientState      = 0;
    TcpIp.TcpIpStateFlag.BitState[1].ClientState      = 0;
    TcpIp.TcpIpStateFlag.BitState[2].ClientState      = 0;
    TcpIp.TcpIpStateFlag.BitState[0].State_Conn       = 0;
    TcpIp.TcpIpStateFlag.BitState[1].State_Conn       = 0;
    TcpIp.TcpIpStateFlag.BitState[2].State_Conn       = 0;
    TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD    = 0;
    TcpIp.TcpIpStateFlag.BitState[1].State_TCP_UPD    = 0;
    TcpIp.TcpIpStateFlag.BitState[2].State_TCP_UPD    = 0;

    Gsm.GsmFlag.Bit.UartTimeOutSelect                 = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetGsmFlags
 *
 * Overview:        This function is used to reset flags
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
void GenericCmd_GSM::ResetGsmFlags(void) {
    Gsm.GsmFlag.Bit.GsmSendCmdInProgress  = 0;
    Gsm.GsmFlag.Bit.CringOccurred         = 0;
    Gsm.GsmFlag.Bit.ErrorWithoutCode      = 0;
    Gsm.GsmFlag.Bit.UnknownError          = 0;
    Gsm.GsmFlag.Bit.OperationNotAllowed   = 0;
    Gsm.GsmFlag.Bit.OperationNotSupported = 0;
    
    Gsm.SimFlag.Bit.SIM_OutOfPhoneCredit  = 0;
    Gsm.SimFlag.Bit.SIM_NotInserted       = 0;
    Gsm.SimFlag.Bit.SIM_Failure           = 0;
    Gsm.SimFlag.Bit.SIM_Busy              = 0;
    Gsm.SimFlag.Bit.SIM_NotReady          = 0;
    Gsm.SimFlag.Bit.SIM_Wrong             = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetSecurityFlags
 *
 * Overview:        This function is used to reset flags
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
void GenericCmd_GSM::ResetSecurityFlags(void) {
    Security.SecurityFlag.Bit.IncorrectPSW = 0;
        
    Gsm.GsmFlag.Bit.UartTimeOutSelect      = T_250MS_UART_TIMEOUT;  // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetPhoneBookFlags
 *
 * Overview:        This function is used to reset PhoneBook flags
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
void GenericCmd_GSM::ResetPhoneBookFlags(void) {
    PhoneBook.PhoneBookFlag.Bit.MemoryFailure         = 0;
    PhoneBook.PhoneBookFlag.Bit.NotFound              = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidIndex          = 0;
    PhoneBook.PhoneBookFlag.Bit.MemoryFull            = 0;
    PhoneBook.PhoneBookFlag.Bit.TextStrTooLong        = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidTextChar       = 0;
    PhoneBook.PhoneBookFlag.Bit.DialStrTooLong        = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidDialChar       = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidInputParameter = 0;
    PhoneBook.PhoneBookFlag.Bit.InvalidInputFormat    = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetSmsFlags
 *
 * Overview:        This function is used to reset Sms flags
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
void GenericCmd_GSM::ResetSmsFlags(void) {
    Sms.SmsFlag.Bit.InvalidInputValue  = 0;
    Sms.SmsFlag.Bit.SendSmsInProgress  = 0;
    Sms.SmsFlag.Bit.WriteSmsInProgress = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect  = T_250MS_UART_TIMEOUT;  // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetPhonicCallFlags
 *
 * Overview:        This function is used to reset Phonic Call flags
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
void GenericCmd_GSM::ResetPhonicCallFlags(void) {
    PhonicCall.PhonicCallFlag.Bit.NoDialtone          = 0;
    PhonicCall.PhonicCallFlag.Bit.Busy                = 0;
    PhonicCall.PhonicCallFlag.Bit.NoCarrier           = 0;
    PhonicCall.PhonicCallFlag.Bit.NoAnswer            = 0;
    PhonicCall.PhonicCallFlag.Bit.CallSetupInProgress = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetGprsFlags
 *
 * Overview:        This function is used to reset Gprs flags
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
void GenericCmd_GSM::ResetGprsFlags(void) {
    Gprs.GprsFlag.Bit.GprsError     = 0;
    Gprs.GprsFlag.Bit.GprsSuspended = 0;
    Gprs.GprsFlag.Bit.PdpFailure    = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetHttpFlags
 *
 * Overview:        This function is used to reset Http flags
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
void GenericCmd_GSM::ResetHttpFlags(void) {
    Http.HttpFlag.Bit.HttpStatusMode   = 0;
    Http.HttpFlag.Bit.HttpStatus       = 0;
    Http.HttpFlag.Bit.HttpStatusCode   = 0;
    Http.HttpFlag.Bit.HttpActionMethod = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        ResetTcpIpFlags
 *
 * Overview:        This function is used to reset TcpIp flags
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
void GenericCmd_GSM::ResetTcpIpFlags(uint8_t IndexConn) {   
    TcpIp.TcpIpStateFlag.BitState[IndexConn].ServerState   = 0;
    TcpIp.TcpIpStateFlag.BitState[IndexConn].ClientState   = 0;
    TcpIp.TcpIpStateFlag.BitState[IndexConn].State_Conn    = 0;
    TcpIp.TcpIpStateFlag.BitState[IndexConn].State_TCP_UPD = 0;
    
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut 250 mSec
}
/****************************************************************************/

/****************************************************************************
 * Function:        StartSendData
 *
 * Overview:        This function is used to START send GSM command
 *                  Initializes the state machine
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           IdleState:          Identify the IDLE State Code for the Generic command
 *                  WaitState:          Identify the WAIT State Code for the Generic command. This code is associated with the Generic GSM command sent
 *                  ProcessStateClass:  Identify the CLASS state Code for the Generic command. This code is associated with the Generic GSM command sent
 *
 *                  Example: Gsm.StartSendData(CMD_IDLE, WAIT_ANSWER_CMD_AT_CPAS, GENERIC_AT_CMD_STATE);
 *                  See file "GenericCmd_Gsm.h", in the section "SEND CMD STATE STEP", to find supported code
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
void GenericCmd_GSM::StartSendData(uint8_t IdleState, uint8_t WaitState, uint8_t ProcessStateClass) {
    Gsm.UartFlag.Bit.SendData = 1;
    if ((Gsm.GsmFlag.Bit.GsmInitInProgress == 1) || (Gsm.GsmFlag.Bit.GprsInitInProgress == 1) || (Gsm.GsmFlag.Bit.HttpInitInProgress == 1)) {
        Gsm.StateInitSendCmd       = IdleState;
        Gsm.StateInitWaitAnswerCmd = WaitState;
        Gsm.CommandsClassState     = ProcessStateClass;
    } else {
        Gsm.StateSendCmd       = IdleState;
        Gsm.StateWaitAnswerCmd = WaitState;
        Gsm.CommandsClassState = ProcessStateClass;
    }
}   
/****************************************************************************/
    
/****************************************************************************
 * Function:        TestAT_Cmd_Answer
 *
 * Overview:        This function is used to find a string in the command buffer answer
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           FlashPointer:   Pointer in the FLASH memory where is stored the string to find in the received answer by GSM engine
 *                  BufferPointer:  Pointer in the SRAM memory where to save the string read from the FLASH memory
 *                  BufferSize:     String lenght
 *
 * Command Note:    None
 *
 * Output:          Unsigned char pointer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::TestAT_Cmd_Answer(uint8_t *FlashPointer, uint8_t *BufferPointer, uint8_t BufferSize) {  
    uint16_t StrPointer = 0x0000;
    
    Gsm.ClearBuffer(&Gsm.TempStringCompare[0], (sizeof(Gsm.TempStringCompare) - 1));
    Gsm.ReadStringFLASH(FlashPointer, BufferPointer, BufferSize);   
    StrPointer = strstr((char *)(&Gsm.GSM_Data_Array[0]), (char *)(&Gsm.TempStringCompare[0]));
    
#ifdef STRING_COMPARE_DEBUG
    uint8_t Counter = 0;
        
    Serial.println();
    Serial.print("String Compare -> ");
    do {
        if (Gsm.TempStringCompare[Counter] != ASCII_NULL) {
            Serial.print(char(Gsm.TempStringCompare[Counter]));
        } else {
            break;
        }
    } while (Counter++ < (sizeof(Gsm.TempStringCompare) - 1));
    Serial.println();
    Serial.print("String Pointer -> ");
    Serial.print(StrPointer);
    Serial.println();
    Serial.print("GSM String received -> ");
    Counter = 0;
    do {
        if (Gsm.GSM_Data_Array[Counter] != ASCII_NULL) {
            Serial.print(char(Gsm.GSM_Data_Array[Counter]));
        } else {
            break;
        }
    } while (Counter++ < (sizeof(Gsm.GSM_Data_Array) - 1));
    Serial.println();
    Serial.print("GSM Array Pointer -> ");
    Serial.print((uint16_t)(&Gsm.GSM_Data_Array[0]));
    Serial.println();
#endif  
    if (StrPointer > 0x0000) {
        return((uint8_t)(StrPointer - (uint16_t)(&Gsm.GSM_Data_Array[0])));
    } else {
        return(0xFF);
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        FindBracketMinus
 *
 * Overview:        This function is used to find a chars: Brackets and minus into array "Gsm.GSM_Data_Array"
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Unsigned char pointer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::FindBracketMinus(void) {
    uint8_t Counter = 0;
    uint8_t ArrayPointer = 0;

    do {    
        CharPointers[Counter] = ASCII_NULL;
    } while (Counter++ < (sizeof(CharPointers) - 1));
    
    Counter = 0;
    do {
        if (Counter < (sizeof(CharPointers) - 1)) {
            switch (Gsm.GSM_Data_Array[ArrayPointer])
            {
                case ASCII_OPEN_ROUND_BRACKET:                  
                case ASCII_CLOSE_ROUND_BRACKET: 
                case ASCII_MINUS:
                    CharPointers[Counter++] = ArrayPointer;
                    break;
            }           
        }
    } while (ArrayPointer++ < (sizeof(Gsm.GSM_Data_Array) - 1));

    if (CharPointers[0] == 0) {
        return(0xFF);
    } else {
        return(ASCII_NULL);
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        FindColonCommaCarriageRet
 *
 * Overview:        This function is used to find a chars: Colon, Comma and CarriageReturn into array "Gsm.GSM_Data_Array"
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Unsigned char pointer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::FindColonCommaCarriageRet(void) {
    uint8_t Counter = 0;
    uint8_t ArrayPointer = 0;

    do {    
        CharPointers[Counter] = ASCII_NULL;
    } while (Counter++ < (sizeof(CharPointers) - 1));
    
    Counter = 0;
    do {
        if (Counter < (sizeof(CharPointers) - 1)) {
            switch (Gsm.GSM_Data_Array[ArrayPointer])
            {
                case ASCII_COLON:                   
                case ASCII_COMMA:                   
                    CharPointers[Counter++] = ArrayPointer;
                    break;
                    
                case ASCII_CARRIAGE_RET:
                    if (ArrayPointer > 1) {
                        CharPointers[Counter++] = ArrayPointer;
                    }
                    break;
            }           
        }
    } while (ArrayPointer++ < (sizeof(Gsm.GSM_Data_Array) - 1));

    if (CharPointers[0] == 0) {
        return(0xFF);
    } else {
        return(ASCII_NULL);
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        MyPow
 *
 * Overview:        This function is used to calculate an exponentiation
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           x: base
 *                  y: exponent
 *
 * Command Note:    None
 *
 * Output:          Unsigned short
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint16_t GenericCmd_GSM::MyPow(uint16_t x, uint8_t y) {
    uint16_t TempPow = 1;
    
    do {
        if (y > 0) {
            TempPow *= x;
        } else {
            TempPow = 1;
        }
    } while(y-- > 1);
    return(TempPow);
}
/****************************************************************************/

/****************************************************************************
 * Function:        ExtractParameterByte
 *
 * Overview:        This function is used to convert a digit string in a digit number
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Pointer: Array Pointer
 *                  Mul:     Multiplier
 *
 * Command Note:    None
 *
 * Output:          Unsigned integer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint16_t GenericCmd_GSM::ExtractParameterByte(uint8_t Pointer, uint8_t Mul) {
    uint16_t TempData = 0;

    do {
        TempData += (uint8_t(Gsm.GSM_Data_Array[Pointer++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
    } while (Mul-- > 1);
    return(TempData);
}
/****************************************************************************/

/****************************************************************************
 * Function:        ExtractCharArray
 *
 * Overview:        This function is used to extract one or more chars from string array
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           ArrayPointerOut: Pointer in SRAM to save data
 *                  ArrayPointerIn:  Pointer in SRAM where to read data
 *                  ArrayOutLenght:  Max Lenght of output array 
 *                  StopChar:        Char that is used to force exit from function
 *
 * Command Note:    None
 *
 * Output:          Unsigned char
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void GenericCmd_GSM::ExtractCharArray(uint8_t *ArrayPointerOut, uint8_t *ArrayPointerIn, uint8_t ArrayOutLenght, uint8_t StopChar) {
    uint8_t Counter = 0;    
    do {
        if (*(ArrayPointerIn + Counter) == StopChar) {
            break;
        }
        *(ArrayPointerOut + Counter) = *(ArrayPointerIn + Counter);
        *(ArrayPointerOut + (++Counter)) = ASCII_NULL;
    } while (Counter < (ArrayOutLenght - 1));
}
/****************************************************************************/

/****************************************************************************
 * Function:        CountDigit
 *
 * Overview:        This function is used to count a digit numbers
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Number: Data to process
 *
 * Command Note:    None
 *
 * Output:          Unsigned char
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::CountDigit(uint8_t Number) {
    uint8_t  NumberLenght = 0;
    uint8_t  Counter;
    
    Counter = Number;   //  Temp Data
    do {
        NumberLenght++;
        Counter /= 10;
    } while (Counter > 0);  
    return(NumberLenght);
}
/****************************************************************************/

/****************************************************************************
 * Function:        ConvertNumberToString
 *
 * Overview:        This function is used to convert a number to a string
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Number:          Data to process
 *                  ArrayStartIndex: Start index
 *                  HowManyDigit:    Number of digits
 *
 * Command Note:    None
 *
 * Output:          Unsigned integer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::ConvertNumberToString(uint16_t Number, uint8_t *ArrayStartIndex, uint8_t HowManyDigit) {
    uint8_t   NumberLenght = 0;
    uint16_t  Counter;
    uint16_t  Divider;
    
    Counter = Number;   //  Temp Data
    do {
        NumberLenght++;
        Counter /= 10;
    } while (Counter > 0);
    
    Counter = 0;
    do {
        if (HowManyDigit <= NumberLenght) {
            break;
        }
        *(ArrayStartIndex + Counter++) = 0x30;
    } while (HowManyDigit-- > NumberLenght);

    do {
        Divider = MyPow(10, (NumberLenght - 1));
        *(ArrayStartIndex + Counter++) = ((Number / Divider) + 0x30);
        Number %= Divider;
    } while (--NumberLenght > 0);
    return(Counter);
}
/****************************************************************************/

/****************************************************************************
 * Function:        ConvertFloatToString
 *
 * Overview:        This function is used to convert a floating-point/double number to a string
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Number:          Data to process
 *                  ArrayStartIndex: Start index
 *                  HowManyDigit:    Number of digits
 *
 * Command Note:    None
 *
 * Output:          Unsigned integer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t GenericCmd_GSM::ConvertFloatToString(float Number, uint8_t *ArrayStartIndex, uint8_t HowManyDigit, uint8_t HowManyDigitAfterPoint) {
    uint16_t IntegerPart;
    float    FloatPart;
    uint8_t  Index;
    
    IntegerPart = (uint16_t)(Number);
    FloatPart   = Number - (float)(IntegerPart);   
    FloatPart   = FloatPart * pow(10, HowManyDigitAfterPoint);
    
    Index = Gsm.ConvertNumberToString(IntegerPart, ArrayStartIndex, HowManyDigit);
    if (HowManyDigitAfterPoint > 0) {
        *(ArrayStartIndex + Index++) = ASCII_DOT;
        Index += Gsm.ConvertNumberToString(FloatPart, (ArrayStartIndex + Index), HowManyDigitAfterPoint);
    }
    return(Index);
}    
/****************************************************************************/
 
/****************************************************************************
 * Function:        ReadStringFLASH
 *
 * Overview:        This function is used to reads AT commands or text by FLASH
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           FlashPointer:  is a pointer in FLASH
 *                  BufferPointer: is a pointer in SRAM
 *                  BufferSize:    is data lenght
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
void GenericCmd_GSM::ReadStringFLASH(uint8_t *FlashPointer, uint8_t *BufferPointer, uint8_t BufferSize) {
  uint8_t k;
  char    myChar;
 
  for (k = 0; k <= BufferSize; k++) {
    *(BufferPointer + k) = pgm_read_byte_near(FlashPointer + k);
  }
}
/****************************************************************************/

/****************************************************************************
 * Function:        PswdEepromStartAddSetup
 *
 * Overview:        This function sets the Start address for PIN, PUK ecc saved on the EEPROM memory
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
 void GenericCmd_GSM::PswdEepromStartAddSetup(void) {
    Gsm.PswdEepromAdd.StartAddLongPswdCode  = &LONG_GEN_PSWD[0];
    Gsm.PswdEepromAdd.StartAddPhPukCode     = &PH_PUK_CODE[0];
    Gsm.PswdEepromAdd.StartAddPuk2Code      = &PUK2_CODE[0];
    Gsm.PswdEepromAdd.StartAddPuk1Code      = &PUK1_CODE[0];
    Gsm.PswdEepromAdd.StartAddShortPswdCode = &SHORT_GEN_PSWD[0];
    Gsm.PswdEepromAdd.StartAddPhPinCode     = &PH_PIN_CODE[0];
    Gsm.PswdEepromAdd.StartAddPin2Code      = &PIN2_CODE[0];
    Gsm.PswdEepromAdd.StartAddPin1Code      = &PIN1_CODE[0];    
 }
/****************************************************************************/
