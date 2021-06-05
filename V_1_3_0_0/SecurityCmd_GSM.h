/*********************************************************************
 *
 *       Security command
 *
 *********************************************************************
 * FileName:        SecurityCmd_GSM.h
 * Revision:        1.0.0
 * Date:            08/05/2016
 *
 * Revision:        1.1.0
 *                  15/10/2018 
 *                  - The name of structure "EepronAdd" was changed in "PswdEepronAdd"
 *                  - Added function "LoadDataFromEepromIntoGsmDataArray" used to read data EEPROM
 *                  - Improved code
 *
 * Revision:        1.2.0
 *                  30/11/2019
 *                  - Fixed bug "ClearBuffer" function [sizeof parameter]
 * 
 * Revision:        1.3.0
 *                  07/07/2020
 *                  - Removed function "LoadDataFromEepromIntoGsmDataArray" used to read data EEPROM
 *					- Changed function name "PswdEepromStartAddSetup" to "EepromStartAddSetup"
 *					- Changed array name "PswdEepromAdd" to "EepromAdd"
 *
 * Dependencies:    Arduino.h
 *                  Uart_GSM.h
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

#ifndef _SecurityCmd_GSM_H_INCLUDED__
#define _SecurityCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
const char AT_CPIN[]     PROGMEM = "AT+CPIN=";          //  Command - Set PIN (Add via code pin and \r\n char)
const char AT_CLCK[]     PROGMEM = "AT+CLCK=";          //  Command - This command is used to lock/unlock ME or network facility. It's required PIN/PUK code
const char AT_CPWD[]     PROGMEM = "AT+CPWD=";          //  Command - This command is used to change password
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
const char ATQ_SPIC[]    PROGMEM = "AT+SPIC\r\n";       //  Query - Check times remained to input SIM PIN/PUK 
#endif
#ifdef FIBOCOM_G510
const char ATQ_TPIN[]    PROGMEM = "AT+TPIN?\r\n";      //  Query - Check times remained to input SIM PIN/PUK 
#endif
#ifdef QUECTEL_M95
const char ATQ_QTRPIN[]  PROGMEM = "AT+QTRPIN\r\n";     //  Query - Check times remained to input SIM PIN/PUK 
#endif
const char ATQ_CPIN[]    PROGMEM = "AT+CPIN?\r\n";      //  Query - Check if PIN is set
//======================================================================

//======================================================================
//  AT COMMAND ANSWER - STORED IN FLASH MEMORY
const char AT_ANSW_CPIN_READY[]     PROGMEM = "+CPIN: READY";           // Key word for answer at the query CPIN. SIM READY
const char AT_ANSW_CPIN_PIN[]       PROGMEM = "+CPIN: SIM PIN";         // Key word for answer at the query CPIN. PIN required
const char AT_ANSW_CPIN_PUK[]       PROGMEM = "+CPIN: SIM PUK";         // Key word for answer at the query CPIN. PUK required
const char AT_ANSW_CPIN_PH_PIN[]    PROGMEM = "+CPIN: PH_SIM PIN";      // Key word for answer at the query CPIN. PH PIN required
const char AT_ANSW_CPIN_PH_PUK[]    PROGMEM = "+CPIN: PH_SIM PUK";      // Key word for answer at the query CPIN. PH PUK required
const char AT_ANSW_CPIN_PIN2[]      PROGMEM = "+CPIN: SIM PIN2";        // Key word for answer at the query CPIN. PIN2 required
const char AT_ANSW_CPIN_PUK2[]      PROGMEM = "+CPIN: SIM PUK2";        // Key word for answer at the query CPIN. PUK2 required

const char AT_ANSW_CLCK[]           PROGMEM = "+CLCK:";                 // Key word for answer at command CLCK

#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
const char AT_ANSW_SPIC[]           PROGMEM = "+SPIC:";                 // Key word for answer at command SPIC
#endif
#ifdef FIBOCOM_G510
const char AT_ANSW_TPIN[]           PROGMEM = "+TPIN:";                 // Key word for answer at command TPIN
#endif
#ifdef QUECTEL_M95
const char AT_ANSW_QTRPIN[]         PROGMEM = "+QTRPIN:";               // Key word for answer at command QTRPIN
#endif
//======================================================================

//======================================================================
#define SIM_READY               0x00
#define SIM_PIN1_REQ            0x01    //  SIM PIN REQUIRED
#define SIM_PUK1_REQ            0x02    //  SIM PUK REQUIRED
#define SIM_PH_PIN_REQ          0x03    //  PH SIM PIN REQUIRED
#define SIM_PH_PUK_REQ          0x04    //  PH SIM PUK REQUIRED
#define SIM_PIN2_REQ            0x05    //  SIM PIN2 REQUIRED
#define SIM_PUK2_REQ            0x06    //  SIM PUK2 REQUIRED
#define LONG_PSWD_REQ           0x07    //  LONG PASSWORD
#define SHORT_PSWD_REQ          0x08    //  SHORT PASSWORD
//======================================================================

//======================================================================
//  CLCK/CPWD FAC Parameters
const char FAC_AO[]  PROGMEM = "\"AO\"";        //  (4-digit PSWD) - BAOC     (Barr All Outgoing Calls)
const char FAC_OI[]  PROGMEM = "\"OI\"";        //  (4-digit PSWD) - BOIC     (Barr Outgoing International Calls)
const char FAC_OX[]  PROGMEM = "\"OX\"";        //  (4-digit PSWD) - BOIC HC  (Barr Outgoing International Calls except to Home Country)
const char FAC_AI[]  PROGMEM = "\"AI\"";        //  (4-digit PSWD) - BAIC     (Barr All Incoming Calls)
const char FAC_IR[]  PROGMEM = "\"IR\"";        //  (4-digit PSWD) - BIC ROAM (Barr Incoming Calls when roaming outside the home country)
const char FAC_AB[]  PROGMEM = "\"AB\"";        //  (4-digit PSWD) - All Barring services
const char FAC_AG[]  PROGMEM = "\"AG\"";        //  (4-digit PSWD) - All Outgoing Barring services  
const char FAC_AC[]  PROGMEM = "\"AC\"";        //  (4-digit PSWD) - All Incoming Barring services  
const char FAC_FD[]  PROGMEM = "\"FD\"";        //  (4-digit PSWD) - SIM card or active application in the UICC fixed dialling memory feature   
const char FAC_SC[]  PROGMEM = "\"SC\"";        //  (4-digit PSWD) - SIM (lock SIM/UICC card). SIM/UICC asks password in MT power-up and when this lock command issued. Correspond to PIN1 code
const char FAC_PN[]  PROGMEM = "\"PN\"";        //  (8-digit PSWD) - Network Personalization, correspond to NCK code (8-digit password)
const char FAC_PU[]  PROGMEM = "\"PU\"";        //  (8-digit PSWD) - Network subset Personalization correspond to NSCK code (8-digit password)
const char FAC_PP[]  PROGMEM = "\"PP\"";        //  (8-digit PSWD) - Service Provider Personalization Correspond to SPCK code (8-digit password)
const char FAC_P2[]  PROGMEM = "\"P2\"";        //  (8-digit PSWD) - SIM PIN 2

#define CODE_FAC_AO     0x00
#define CODE_FAC_OI     0x01
#define CODE_FAC_OX     0x02
#define CODE_FAC_AI     0x03
#define CODE_FAC_IR     0x04
#define CODE_FAC_AB     0x05
#define CODE_FAC_AG     0x06
#define CODE_FAC_AC     0x07
#define CODE_FAC_FD     0x08
#define CODE_FAC_SC     0x09
#define CODE_FAC_PN     0x0A
#define CODE_FAC_PU     0x0B
#define CODE_FAC_PP     0x0C
#define CODE_FAC_P2     0x0D

#define CODE_FAC_UNLOCK 0x00
#define CODE_FAC_LOCK   0x01
#define CODE_FAC_QUERY  0x02
//======================================================================

//======================================================================
//  AT+CPIN OFFSET
#define CPIN_OFFSET         8
#define CPUK_OFFSET         19
//======================================================================

//======================================================================
//  AT+CLCK OFFSET
#define CLCK_FAC_OFFSET     8
#define CLCK_MODE_OFFSET    13
//======================================================================

//======================================================================
//  SEND CMD STATE STEP
#define CMD_SECURITY_IDLE           0x00

#define WAIT_ANSWER_CMD_ATQ_CPIN    0x01
#define WAIT_ANSWER_CMD_ATQ_SPIC    0x02    //  Answer at query - Check times remained to input SIM PIN/PUK (SIMCOM  - SIM900)
#define WAIT_ANSWER_CMD_ATQ_TPIN    0x03    //  Answer at query - Check times remained to input SIM PIN/PUK (FIBOCOM - G510)
#define WAIT_ANSWER_CMD_ATQ_QTRPIN  0x04    //  Answer at query - Check times remained to input SIM PIN/PUK (QUECTEL - M95)
#define WAIT_ANSWER_CMD_ATQ_CPINC   0x05    //  Answer at query - Check times remained to input SIM PIN/PUK (AI THINK - A9)

#define WAIT_ANSWER_CMD_CPIN        0x0A
#define WAIT_ANSWER_CMD_CLCK        0x0B
#define WAIT_ANSWER_CMD_CPWD        0x0C
//======================================================================

#define PIN_LEN   10

class SecurityCmd_GSM {
    
public:
    #if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
        uint8_t SetCmd_ATQ_SPIC(void);
    #endif
    #ifdef FIBOCOM_G510
        uint8_t SetCmd_ATQ_TPIN(void);
    #endif
    #ifdef QUECTEL_M95
        uint8_t SetCmd_ATQ_QRTPIN(void);
    #endif
    
    uint8_t SetCmd_ATQ_CPIN(void);
    
    uint8_t SetCmd_CPIN_PUK(uint8_t TypeOfCmd);
    uint8_t SetCmd_CLCK(uint8_t TypeOfFac, uint8_t Mode);
    uint8_t SetCmd_CPWD(uint8_t TypeOfFac);
    
    uint8_t NewPin[PIN_LEN];
    
    union SecurityFlag {
        uint32_t  SecurityDoubleWord;
        struct {
            uint8_t SIM_Status              :3;     //  "000": READY      -> MT is not pending for any password
                                                    //  "001": SIM PIN    -> MT is waiting for SIM PIN to be given
                                                    //  "010": SIM PUK    -> MT is waiting for SIM PUK to be given
                                                    //  "011": PH SIM PIN -> ME is waiting for phone to SIM card (antitheft)
                                                    //  "100": PH SIM PUK -> ME is waiting for phone to SIM PUK (antitheft)
                                                    //  "101": SIM PIN 2
                                                    //  "110": SIM PUK 2
            uint8_t IncorrectPSW            :1;     //  "1": Inserted incorrect password
            uint8_t PinRetry                :2;     //  Times remained to input PIN 1
            uint8_t PukRetry                :4;     //  Times remained to input PUK 1
            uint8_t Pin2Retry               :2;     //  Times remained to input PIN 2
            uint8_t Puk2Retry               :4;     //  Times remained to input PUK 2
            uint8_t ClckStatus              :1;     //  Facility Lock Status -> "0": Not active
                                                    //                          "1": Active
            uint8_t ClckClass               :3;     //  Facility Lock Class  -> "1": Voice (Telephony)
                                                    //                          "2": Data refers to all bearer services
                                                    //                          "4": Fax (Facsimile services)
                                                    //                          "7": All classes
            uint8_t Free                    :12;    //  Free
        } Bit;  
    } SecurityFlag;
    //-------------------------------------------------------------------------------------
    
    uint8_t * EepromAddPin;
    uint8_t * EepromAddPuk;
    
    void    GsmSecurityWaitAnswer(void);    
    void    GsmSecurityRetrySendCmd(void);
    void    EepromAddPinPuk(uint8_t TypeOfCmd);
    
private:
    uint8_t * FlashFacAdd;
    void      FlashAddFac(uint8_t TypeOfFac);
};

extern SecurityCmd_GSM Security;

#endif