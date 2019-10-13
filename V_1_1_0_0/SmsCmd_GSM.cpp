/*********************************************************************
 *
 *       Sms command
 *
 *********************************************************************
 * FileName:        SmsCmd_GSM.cpp
 * Revision:        1.0.0
 * Date:            01/10/2016
  *
 * Revision:        1.1.0
 *                  01/12/2018
 *                  - Improved code
 *
 * Dependencies:    SmsCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
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

#include "SmsCmd_GSM.h"
#include "PhoneBookCmd_GSM.h"
#include "Isr_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/****************************************************************************
 * Function:        SetCmd_AT_CPMS
 *
 * Overview:        This function is used to Select preferred SMS Message Stored. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CPMS? and the response is: +CPMS:<mem1>,<used1>,<total1>,<mem2>,<used2>,<total2>,<mem3>,<used3>,<total3>
 *                  AT+CPMS=<mem1>,[,<mem2>,[<mem3>]]. The answer at this command is: +CPMS:<used1>,<total1>,<used2>,<total2>,<used3>,<total3>
 *
 * Input:           TypeOfMem1: Memory type for message storage 1
 *                  TypeOfMem2: Memory type for message storage 2
 *                  TypeOfMem3: Memory type for message storage 3
 *                  Query:      "false" Sends command to select preferred SMS Message Stored
 *                              "true"  Sends command to read preferred SMS Message Stored 
 *
 * Command Note:    (The message_storage1 Parameter) -> The first parameter of the +CPMS AT command, message_storage1, specifies the message
 *                                                      storage area that will be used when reading or deleting SMS messages.
 *                                                      (For details about reading SMS messages, see AT commands +CMGR and +CMGL. For details
 *                                                      about deleting SMS messages, see the AT command +CMGD.)
 *                  (The message_storage2 Parameter) -> The second parameter of the +CPMS AT command, message_storage2, specifies the message
 *                                                      storage area that will be used when sending SMS messages from message storage or
 *                                                      writing SMS messages. (For details about sending SMS messages from message storage, see
 *                                                      the AT command +CMSS. For details about writing SMS messages, see the AT command +CMGW.)
 *                  (The message_storage3 Parameter) -> The third parameter of the +CPMS AT command, message_storage3, specifies the preferred
 *                                                      message storage area for storing newly received SMS messages. If you use the +CNMI AT
 *                                                      command (command name in text: New Message Indications to TE) to tell the GSM/GPRS modem
 *                                                      or mobile phone to forward newly received SMS messages directly to the PC instead of
 *                                                      storing them in the message storage area, you do not need to care about the message_storage3
 *                                                      parameter.
 *                  Here are the values defined in the SMS specification that may be assigned to the parameters message_storage1,
 *                  message_storage2 and message_storage3:
 *
 *                  SM -> It refers to the message storage area on the SIM card.
 *                  ME -> It refers to the message storage area on the GSM/GPRS modem or mobile phone. Usually its storage space is larger
 *                        than that of the message storage area on the SIM card.
 *                  MT -> It refers to all message storage areas associated with the GSM/GPRS modem or mobile phone. For example, suppose a
 *                        mobile phone can access two message storage areas: "SM" and "ME". The "MT" message storage area refers to the
 *                        "SM" message storage area and the "ME" message storage area combined together.
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
uint8_t SmsCmd_GSM::SetCmd_AT_CPMS(uint8_t TypeOfMem1, uint8_t TypeOfMem2, uint8_t TypeOfMem3, bool Query) {
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetSmsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.GsmFlag.Bit.UartTimeOutSelect = T_1S_UART_TIMEOUT;
        Gsm.BckCmdData[0] = TypeOfMem1;
        Gsm.BckCmdData[1] = TypeOfMem2;
        Gsm.BckCmdData[2] = TypeOfMem3;
        Gsm.BckCmdData[3] = Query;
        if (Query == false) {
            Gsm.ReadStringFLASH((uint8_t *)AT_CPMS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CPMS));
            Sms.Sms_Mem1_Storage.Type = TypeOfMem1;
            FlashAddSmsTypeMem(TypeOfMem1);
            Gsm.ReadStringFLASH(FlashSmsMemTypeAdd, ((uint8_t *)Gsm.GSM_Data_Array + CPMS_MEM1_OFFSET), strlen(SMS_SM));
            Gsm.GSM_Data_Array[CPMS_MEM1_OFFSET + 4] = ASCII_COMMA;

            Sms.Sms_Mem2_Storage.Type = TypeOfMem2;
            FlashAddSmsTypeMem(TypeOfMem2);
            Gsm.ReadStringFLASH(FlashSmsMemTypeAdd, ((uint8_t *)Gsm.GSM_Data_Array + CPMS_MEM2_OFFSET), strlen(SMS_ME));
            Gsm.GSM_Data_Array[CPMS_MEM2_OFFSET + 4] = ASCII_COMMA;

            Sms.Sms_Mem3_Storage.Type = TypeOfMem3;
            FlashAddSmsTypeMem(TypeOfMem3);
            Gsm.ReadStringFLASH(FlashSmsMemTypeAdd, ((uint8_t *)Gsm.GSM_Data_Array + CPMS_MEM3_OFFSET), strlen(SMS_MT));
            Gsm.GSM_Data_Array[CPMS_MEM3_OFFSET + 4] = ASCII_CARRIAGE_RET;
            Gsm.GSM_Data_Array[CPMS_MEM3_OFFSET + 5] = ASCII_LINE_FEED; 
            Gsm.WritePointer = CPMS_MEM3_OFFSET + 6;            
        } else {
            Gsm.ReadStringFLASH((uint8_t *)ATQ_CPMS, (uint8_t *)Gsm.GSM_Data_Array, strlen(ATQ_CPMS));
            Gsm.WritePointer = strlen(ATQ_CPMS);
        }
        Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CPMS, ANSWER_SMS_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CMGD
 *
 * Overview:        This function is used to Delete SMS. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CMGD=<index>[,<delflag>]
 *
 * Input:           Index:   Location of memory to be delete
 *                  DelFlag: Action to be execute
 *
 * Command Note:    <SmsIndex>  Integer type; value in the range of location numbers supported by the associated memory
 *                  <delflag>   "0" Delete the message specified in <index>
 *                              "1" Delete all read messages from preferred message storage, leaving unread messages and stored
 *                                  mobile originated messages (whether sent or not) untouched
 *                              "2" Delete all read messages from preferred message storage and sent mobile originated messages,
 *                                  leaving unread messages and unsent mobile originated messages untouched
 *                              "3" Delete all read messages from preferred message storage, sent and unsent mobile originated
 *                                  messages leaving unread messages untouched
 *                              "4" Delete all messages from preferred message storage including unread messages
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
uint8_t SmsCmd_GSM::SetCmd_AT_CMGD(uint8_t SmsIndex, uint8_t DelFlag) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetSmsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.GsmFlag.Bit.UartTimeOutSelect = T_1S_UART_TIMEOUT;  
        Gsm.BckCmdData[0] = SmsIndex;
        Gsm.BckCmdData[1] = DelFlag;    
        Gsm.ReadStringFLASH((uint8_t *)AT_CMGD, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMGD));    
        Index = CMGD_INDEX_OFFSET + Gsm.ConvertNumberToChar(SmsIndex, ((uint8_t *)(Gsm.GSM_Data_Array) + CMGD_INDEX_OFFSET), 0);
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;
        Gsm.GSM_Data_Array[Index++] = (DelFlag + 0x30);
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;   
        Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CMGD, ANSWER_SMS_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CMGR
 *
 * Overview:        This function is used to Read SMS. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CMGR=<index>[,<mode>]
 *
 * Input:           Index: Location of memory to be read
 *                  Mode:  Change SMS state
 *
 * Command Note:    <SmsIndex>  Integer type; value in the range of location numbers supported by the associated memory
 *                  <mode>      "0" Normal; "1" Not change status of the specified SMS record
 *
 * Output:          Return unsigned char
 *                  Return -> 0 (System Busy. Command not executed)
 *                  Return -> 1 (Command sent)
 *
 * GSM answer det:  If Text mode (SMS-DELIVER):        +CMGR: <stat>,<oa>[,<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs> ,<sca>,<tosca>,<length>]<CR><LF><data>
 *                  If Text mode (SMS-SUBMIT):         +CMGR: <stat>,<da>[,<alpha>][,<toda>,<fo>,<pid>,<dcs>[,<vp>] ,<sca>,<tosca>,<length>]<CR><LF><data>
 *                  If Text mode (SMS-STATUS-REPORTs): +CMGR: <stat>,<fo>,<mr>[,<ra>][,<tora>],<scts>,<dt>,<st>
 *                  If Text mode (SMS-COMMANDs):       +CMGR: <stat>,<fo>,<ct>[,<pid>[,<mn>][,<da>][,<toda>] ,<length><CR><LF><cdata>]
 *                  If Text mode (CBM storage):        +CMGR: <stat>,<sn>,<mid>,<dcs>,<page>,<pages><CR><LF><data>
 *                                PDU-Mode             +CMGR: <stat>[,<alpha>],<length><CR><LF><pdu>
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t SmsCmd_GSM::SetCmd_AT_CMGR(uint8_t SmsIndex, uint8_t Mode) {
    uint8_t Index;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetSmsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.GsmFlag.Bit.UartTimeOutSelect = T_1S_UART_TIMEOUT;
        Gsm.BckCmdData[0] = SmsIndex;
        Gsm.BckCmdData[1] = Mode;   
        Gsm.ReadStringFLASH((uint8_t *)AT_CMGR, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMGR));
        Index = CMGR_INDEX_OFFSET + Gsm.ConvertNumberToChar(SmsIndex, ((uint8_t *)(Gsm.GSM_Data_Array) + CMGR_INDEX_OFFSET), 0);
        Gsm.GSM_Data_Array[Index++] = ASCII_COMMA;  
        if (Mode == STATE_CHANGE) {
            //  Change SMS State
            Gsm.GSM_Data_Array[Index++] = ASCII_0;
        } else {
            //  No Change SMS State
            Gsm.GSM_Data_Array[Index++] = ASCII_1;
        }
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;   
        Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CMGR, ANSWER_SMS_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CMGS
 *
 * Overview:        This function is used to Send SMS Message. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CMGS=<da>[,<toda>]<CR>text is entered<SUB/ESC>
 *
 * Input:           None
 *
 * Command Note:    This command is splitted in two step:
 *                  1) The first step sends a phone number
 *                  2) The second step sends a text (Max 160 chars)
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
uint8_t SmsCmd_GSM::SetCmd_AT_CMGS(void) {
    uint8_t Index;
    uint8_t StrLenght;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetSmsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        SmsFlag.Bit.SendSmsInProgress        = 1;
        Gsm.ReadStringFLASH((uint8_t *)AT_CMGS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMGS));
        Index = CMGS_INDEX_OFFSET;  
        StrLenght = strlen(PhoneBook.PhoneNumber);
        strncat((char *)(&Gsm.GSM_Data_Array[Index]), (char *)(&PhoneBook.PhoneNumber[0]), StrLenght);
        Index += StrLenght;
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;   
        Gsm.WritePointer = Index;
        Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CMGS_STEP1, ANSWER_SMS_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CMGW
 *
 * Overview:        This function is used to Write SMS Message to Memory. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CMGW=<da>[,<toda>]<CR>text is entered<SUB/ESC>
 *
 * Input:           None
 *
 * Command Note:    This command is splitted in two step:
 *                  1) The first step sends a phone number
 *                  2) The second step sends a text (Max 160 chars)
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
uint8_t SmsCmd_GSM::SetCmd_AT_CMGW(void) {
    uint8_t Index;
    uint8_t StrLenght;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetSmsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        SmsFlag.Bit.WriteSmsInProgress       = 1;
        Gsm.ReadStringFLASH((uint8_t *)AT_CMGW, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMGW));
        Index = CMGW_INDEX_OFFSET;
        StrLenght = strlen(PhoneBook.PhoneNumber);
        strncat((char *)(&Gsm.GSM_Data_Array[Index]), (char *)(&PhoneBook.PhoneNumber[0]), StrLenght);
        Index += StrLenght;
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;   
        Gsm.WritePointer = Index;
        Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CMGW_STEP1, ANSWER_SMS_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetCmd_AT_CMSS
 *
 * Overview:        This function is used to Send SMS Message from Memory. For details see AT commands datasheet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  AT+CMSS=<index>,<da>[,<toda>]
 *
 * Input:           SmsIndex: Memory index
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
uint8_t SmsCmd_GSM::SetCmd_AT_CMSS(uint8_t SmsIndex) {  
    uint8_t Index;
    uint8_t StrLenght;
    
    if ((Gsm.StateWaitAnswerCmd != CMD_WAIT_IDLE) || (Gsm.UartState != UART_IDLE_STATE) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return(0);      //  System Busy
    } else {
        Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
        //Gsm.ResetAllFlags();
        Gsm.ResetGsmFlags();
        Gsm.ResetSmsFlags();
        Gsm.GsmFlag.Bit.GsmSendCmdInProgress = 1;
        Gsm.GsmFlag.Bit.UartTimeOutSelect = T_1S_UART_TIMEOUT;
        Gsm.BckCmdData[0] = SmsIndex;       
        Gsm.ReadStringFLASH((uint8_t *)AT_CMSS, (uint8_t *)Gsm.GSM_Data_Array, strlen(AT_CMSS));
        Index = CMSS_INDEX_OFFSET + Gsm.ConvertNumberToChar(SmsIndex, ((uint8_t *)(Gsm.GSM_Data_Array) + CMSS_INDEX_OFFSET), 0);
        Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
        Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
        Gsm.WritePointer = Index;   
        Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CMSS, ANSWER_SMS_AT_CMD_STATE);
    }
    return(1);          //  Command sent
}
/****************************************************************************/

/****************************************************************************
 * Function:        GsmSmsWaitAnswer
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
void SmsCmd_GSM::GsmSmsWaitAnswer(void) {   
    uint8_t StrPointer = 0xFF;
    uint8_t Offset;
    uint8_t StrLenght;
    uint8_t Counter;
    uint8_t Mul;
    
    if ((Gsm.StateSendCmd != CMD_SMS_IDLE) || (Gsm.UartState != UART_IDLE_STATE)) {
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
                case WAIT_ANSWER_CMD_AT_CPMS:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CPMS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CPMS)) != 0xFF) {
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {                  
                            Sms_Mem1_Storage.Used  = 0;
                            Sms_Mem2_Storage.Used  = 0;
                            Sms_Mem3_Storage.Used  = 0;
                            Sms_Mem1_Storage.Total = 0;
                            Sms_Mem2_Storage.Total = 0;
                            Sms_Mem3_Storage.Total = 0;

                            if (Gsm.BckCmdData[3] == false) {
                                FindUsedSmsMemory(1, 0, 1);
                                FindTotalSmsMemory(1, 1, 2);
                                FindUsedSmsMemory(2, 2, 3);
                                FindTotalSmsMemory(2, 3, 4);                    
                                FindUsedSmsMemory(3, 4, 5);
                                FindTotalSmsMemory(3, 5, 6);                                    
                            } else {        
                                FindUsedSmsMemory(1, 1, 2);
                                FindTotalSmsMemory(1, 2, 3);
                                FindUsedSmsMemory(2, 4, 5);
                                FindTotalSmsMemory(2, 5, 6);                    
                                FindUsedSmsMemory(3, 7, 8);
                                FindTotalSmsMemory(3, 8, 9);    
                            }
                        }
                    }           
                    break;          
                case WAIT_ANSWER_CMD_AT_CMGR:
                    if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMGR_RU, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMGR_RU)) != 0xFF) {
                        //  REC UNREAD
                        SmsFlag.Bit.SmsReadStat = SMS_REC_UNREAD;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMGR_RR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMGR_RR)) != 0xFF) {
                        //  REC READ
                        SmsFlag.Bit.SmsReadStat = SMS_REC_READ;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMGR_SU, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMGR_SU)) != 0xFF) {
                        //  STO UNSENT
                        SmsFlag.Bit.SmsReadStat = SMS_STO_UNSENT;
                    } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMGR_SS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMGR_SS)) != 0xFF) {
                        //  STO SENT
                        SmsFlag.Bit.SmsReadStat = SMS_STO_SENT;
                    }
                    if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                        //  Reads Phone Number
                        Gsm.ExtractCharArray((char *)(&PhoneBook.PhoneNumber[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[1] + 1]), sizeof(PhoneBook.PhoneNumber), ASCII_COMMA);
                        //  Reads Text associated to the Phone Number
                        Gsm.ExtractCharArray((char *)(&PhoneBook.PhoneText[0]),   (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[2] + 1]), sizeof(PhoneBook.PhoneText),   ASCII_COMMA);
                        if ((SmsFlag.Bit.SmsReadStat == SMS_STO_SENT) || (SmsFlag.Bit.SmsReadStat == SMS_STO_UNSENT)) {
                            Offset = 3;                     
                        } else {
                            Offset = 7;                         
                        }
                        //  Reads SMS Text
                        Gsm.ExtractCharArray((char *)(&Sms.SmsText[0]), (char *)(&Gsm.GSM_Data_Array[Gsm.CharPointers[Offset] + 2]), sizeof(Sms.SmsText), ASCII_CARRIAGE_RET);
                    }
                    SmsFlag.Bit.SmsReadOk = 1;
                    break;      
                case WAIT_ANSWER_CMD_AT_CMGW_STEP2:
                case WAIT_ANSWER_CMD_AT_CMGS_STEP2:
                    if (SmsFlag.Bit.SendSmsInProgress == 1) {
                        StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMGS, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMGS));
                    }
                    if (SmsFlag.Bit.WriteSmsInProgress == 1) {
                        StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)AT_ANSW_CMGW, (uint8_t *)Gsm.TempStringCompare, strlen(AT_ANSW_CMGW));
                    }
                    if (StrPointer != 0xFF) {
                        if (Gsm.FindColonCommaCarriageRet() != 0xFF) {
                            if (SmsFlag.Bit.SendSmsInProgress == 1) {
                                SmsOutgoingCounter = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
                            }
                            if (SmsFlag.Bit.WriteSmsInProgress == 1) {
                                SmsWriteMemoryAdd = (uint8_t)(Gsm.ExtractParameterByte((Gsm.CharPointers[0] + 2), (Gsm.CharPointers[1] - (Gsm.CharPointers[0] + 2))));
                            }                       
                        }
                    }
                    SmsFlag.Bit.SendSmsInProgress  = 0;
                    SmsFlag.Bit.WriteSmsInProgress = 0;
                    break;
                default:
                    break;
            }   
            Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;         
        } else if (Gsm.TestAT_Cmd_Answer((uint8_t *)AT_MAJOR, (uint8_t *)Gsm.TempStringCompare, strlen(AT_MAJOR)) != 0xFF) {
            switch (Gsm.StateWaitAnswerCmd)
            {
                case CMD_WAIT_IDLE:
                    break;
                case WAIT_ANSWER_CMD_AT_CMGS_STEP1:
                case WAIT_ANSWER_CMD_AT_CMGW_STEP1:
                    Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
                    StrLenght = strlen(SmsText);
                    strncat((char *)(&Gsm.GSM_Data_Array[0]), (char *)(&Sms.SmsText[0]), StrLenght);
                    Offset = StrLenght;         
                    Gsm.GSM_Data_Array[Offset] = ASCII_SUB;
                    Gsm.WritePointer = Offset + 1;
                    Gsm.StartSendData(CMD_SMS_IDLE, WAIT_ANSWER_CMD_AT_CMGS_STEP2, ANSWER_SMS_AT_CMD_STATE);    
                    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_7S5_UART_TIMEOUT;                 
                    break;          
                default:
                    break;
            }   
        } else {
            Gsm.ProcessGsmError();
            GsmSmsRetrySendCmd();
        }
    } else {
        //  If no answer from GSM module is received, this means that this GSM module is probabily OFF
        Gsm.InitReset_GSM();
    }
}   
/****************************************************************************/

/****************************************************************************
 * Function:        GsmSmsRetrySendCmd
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
void SmsCmd_GSM::GsmSmsRetrySendCmd(void) {
    uint8_t AnswerCmdStateBackup;
    
    if (Gsm.RetryCounter++ < 2) {
        AnswerCmdStateBackup = Gsm.StateWaitAnswerCmd;
        Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
        switch (AnswerCmdStateBackup)
        {
            case WAIT_ANSWER_CMD_AT_CMSS:
                SetCmd_AT_CMSS(Gsm.BckCmdData[0]);
                break;              
             case WAIT_ANSWER_CMD_AT_CPMS:
                SetCmd_AT_CPMS(Gsm.BckCmdData[0], Gsm.BckCmdData[1], Gsm.BckCmdData[2], Gsm.BckCmdData[3]);
                break;          
            case WAIT_ANSWER_CMD_AT_CMGD:
                SetCmd_AT_CMGD(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;          
            case WAIT_ANSWER_CMD_AT_CMGR:
                SetCmd_AT_CMGR(Gsm.BckCmdData[0], Gsm.BckCmdData[1]);
                break;          
            case WAIT_ANSWER_CMD_AT_CMGS_STEP1:
                SetCmd_AT_CMGS();
                break;      
            case WAIT_ANSWER_CMD_AT_CMGW_STEP1:         
                SetCmd_AT_CMGW();
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
 * Function:        FlashAddSmsTypeMem
 *
 * Overview:        This function return the FLASH address pointer for SMS Memory storage available
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           TypeOfMem: Type of memory to be used
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
void SmsCmd_GSM::FlashAddSmsTypeMem(uint8_t TypeOfMem) {
    switch (TypeOfMem)
    {
        case CODE_SMS_SM:   //  SM -> It refers to the message storage area on the SIM card
            FlashSmsMemTypeAdd = (uint8_t *)SMS_SM;
            break;
        case CODE_SMS_ME:   //  ME -> It refers to the message storage area on the GSM/GPRS modem or mobile phone
            FlashSmsMemTypeAdd = (uint8_t *)SMS_ME;
            break;
        case CODE_SMS_MT:   //  MT -> It refers to all message storage areas associated with the GSM/GPRS modem or mobile phone
            FlashSmsMemTypeAdd = (uint8_t *)SMS_MT;
            break;
        default:
            FlashSmsMemTypeAdd = (uint8_t *)SMS_SM;
            break;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        FindSmsMemoryType
 *
 * Overview:        This function return the code of SMS Memory selected
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          SMS Memory storage available code
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
uint8_t SmsCmd_GSM::FindSmsMemoryType(void) {
    uint8_t StrPointer = 0xFF;
    
    StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)SMS_SM, (uint8_t *)Gsm.TempStringCompare, strlen(SMS_SM));
    if (StrPointer != 0xFF) {
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        return(CODE_SMS_SM);
    }
    StrPointer = 0xFF;
    StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)SMS_ME, (uint8_t *)Gsm.TempStringCompare, strlen(SMS_ME));
    if (StrPointer != 0xFF) {
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        return(CODE_SMS_ME);
    }
    StrPointer = 0xFF;
    StrPointer = Gsm.TestAT_Cmd_Answer((uint8_t *)SMS_MT, (uint8_t *)Gsm.TempStringCompare, strlen(SMS_MT));
    if (StrPointer != 0xFF) {
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        Gsm.GSM_Data_Array[StrPointer++] = ASCII_SPACE;
        return(CODE_SMS_MT);
    }
    return(0xFF);
}
/****************************************************************************/

/****************************************************************************
 * Function:        FindUsedSmsMemory
 *
 * Overview:        This function return the code of SMS Memory selected
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Mem:      Memory selected
 *                  StartAdd: Start Address
 *                  EndAdd:   End Address
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
void SmsCmd_GSM::FindUsedSmsMemory(uint8_t Mem, uint8_t StartAdd, uint8_t EndAdd) {
    uint8_t Counter;
    uint8_t Mul;
    uint8_t TmpData = 0xFF;
        
    TmpData = FindSmsMemoryType();
    Counter = Gsm.CharPointers[StartAdd] + 1;
    Mul     = Gsm.CharPointers[EndAdd] - (Gsm.CharPointers[StartAdd] + 1);

    if (Gsm.BckCmdData[3] == false) {
        if (Mem == 1) {
            Counter = Gsm.CharPointers[StartAdd] + 2;
            Mul     = Gsm.CharPointers[EndAdd] - (Gsm.CharPointers[StartAdd] + 2);
        }
    }

    do {
        switch (Mem)
        {
            case 1:
                if (Gsm.BckCmdData[3] == true) {
                    if (TmpData != 0xFF) {
                        Sms_Mem1_Storage.Type = TmpData;
                        TmpData = 0xFF;
                    }
                }
                if ((Gsm.GSM_Data_Array[Counter] >= ASCII_0) && (Gsm.GSM_Data_Array[Counter] <= ASCII_9)) {
                    Sms_Mem1_Storage.Used += (uint8_t(Gsm.GSM_Data_Array[Counter++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
                }
                break;
            case 2:
                if (Gsm.BckCmdData[3] == true) {
                    if (TmpData != 0xFF) {
                        Sms_Mem2_Storage.Type = TmpData;
                        TmpData = 0xFF;
                    }
                }
                if ((Gsm.GSM_Data_Array[Counter] >= ASCII_0) && (Gsm.GSM_Data_Array[Counter] <= ASCII_9)) {
                    Sms_Mem2_Storage.Used += (uint8_t(Gsm.GSM_Data_Array[Counter++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
                }
                break;
            case 3:
                if (Gsm.BckCmdData[3] == true) {
                    if (TmpData != 0xFF) {
                        Sms_Mem3_Storage.Type = TmpData;
                        TmpData = 0xFF;
                    }
                }
                if ((Gsm.GSM_Data_Array[Counter] >= ASCII_0) && (Gsm.GSM_Data_Array[Counter] <= ASCII_9)) {
                    Sms_Mem3_Storage.Used += (uint8_t(Gsm.GSM_Data_Array[Counter++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
                }
                break;
            default:
                break;
        }
    } while (Mul-- > 1);    
}
/****************************************************************************/

/****************************************************************************
 * Function:        FindTotalSmsMemory
 *
 * Overview:        This function check SMS Memory lenght
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Mem:      Memory selected
 *                  StartAdd: Start Address
 *                  EndAdd:   End Address
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
void SmsCmd_GSM::FindTotalSmsMemory(uint8_t Mem, uint8_t StartAdd, uint8_t EndAdd) {
    uint8_t Counter;
    uint8_t Mul;
        
    Counter = Gsm.CharPointers[StartAdd] + 1;
    Mul     = Gsm.CharPointers[EndAdd] - (Gsm.CharPointers[StartAdd] + 1);                          
    do {
        switch (Mem)
        {
            case 1:
                Sms_Mem1_Storage.Total += (uint8_t(Gsm.GSM_Data_Array[Counter++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
                break;
            case 2:
                Sms_Mem2_Storage.Total += (uint8_t(Gsm.GSM_Data_Array[Counter++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
                break;
            case 3:
                Sms_Mem3_Storage.Total += (uint8_t(Gsm.GSM_Data_Array[Counter++] - 0x30)) * uint8_t(Gsm.MyPow(10, (Mul-1)));
                break;
            default:
                break;
        }
    } while (Mul-- > 1);
}
/****************************************************************************/