/*********************************************************************
 *
 *       Phonebook command
 *
 *********************************************************************
 * FileName:        Phonebookcmd_GSM.h
 * Revision:        1.0.0
 * Date:            10/09/2016
 *
 * Revision:        1.1.0
 *                  17/02/2019
 *                  - Added the AT command AT+CPBF
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

#ifndef _PhoneBookCmd_GSM_H_INCLUDED__
#define _PhoneBookCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
const char AT_CPBS[]     PROGMEM = "AT+CPBS=";              //  Command - Select PhoneBook Memory Storage. See datasheet for details
const char AT_CPBR[]     PROGMEM = "AT+CPBR=";              //  Command - Read current PhoneBook Entries. See datasheet for details
const char AT_CPBF[]     PROGMEM = "AT+CPBF=";              //  Command - Find PhoneBook entries
const char AT_CPBW[]     PROGMEM = "AT+CPBW=";              //  Command - Write PhoneBook Entry.

const char ATQ_CPBS[]    PROGMEM = "AT+CPBS?\r\n";          //  Query - Check selected PhoneBook Memory
const char ATQ_CPBF[]    PROGMEM = "AT+CPBF?\r\n";          //  Query - Check maximum lenght of the fields
const char ATQ_CPBR[]    PROGMEM = "AT+CPBR=?\r\n";         //  Query - Check location range supported by the current storage

//======================================================================

//======================================================================
//  AT COMMAND ANSWER - STORED IN FLASH MEMORY
const char AT_ANSW_CPBS[]           PROGMEM = "+CPBS:";
const char AT_ANSW_CPBR[]           PROGMEM = "+CPBR:";
const char AT_ANSW_CPBF[]           PROGMEM = "+CPBF:";
//======================================================================

//======================================================================
//  PHONE BOOK Memory storage available
const char PBM_DC[]  PROGMEM = "\"DC\"";    //  ME dialed calls list (+CPBW may not be applicable for this storage)
const char PBM_EN[]  PROGMEM = "\"EN\"";    //  SIM (or MT) emergency number (+CPBW is not be applicable for this storage)
const char PBM_FD[]  PROGMEM = "\"FD\"";    //  SIM fix dialing phonebook. If a SIM card is present or if a UICC with an active GSM application is present, the information in EFFDN under DFTelecom is selected
const char PBM_MC[]  PROGMEM = "\"MC\"";    //  MT missed (unaswered received) calls list (+CPBW may not be applicable for this storage)
const char PBM_ON[]  PROGMEM = "\"ON\"";    //  SIM (or MT) own numbers (MSISDNs) list (Reading of this storage may be available through +CNUM also). When storing information in the SIM/UICC, if a SIM card is present or if a UICC with an active GSM application is present, the information in EFMSISDN under DFTelecom is selected
const char PBM_RC[]  PROGMEM = "\"RC\"";    //  MT received calls list (+CPBW may not be applicable for this storage)
const char PBM_SM[]  PROGMEM = "\"SM\"";    //  SIM/UICC phonebook. If a SIM card is present or if a UICC whit an active GSM application is present, the EFADN under DFTelecom is selected 
const char PBM_LA[]  PROGMEM = "\"LA\"";    //  Last number all list (LND/LNM/LNR)  
const char PBM_ME[]  PROGMEM = "\"ME\"";    //  ME phonebook
const char PBM_BN[]  PROGMEM = "\"BN\"";    //  SIM barred dialed number
const char PBM_SD[]  PROGMEM = "\"SD\"";    //  SIM service dial number
const char PBM_VM[]  PROGMEM = "\"VM\"";    //  SIM voice mailbox
const char PBM_LD[]  PROGMEM = "\"LD\"";    //  SIM last dialing phonebook

#define CODE_PBM_DC     0x00
#define CODE_PBM_EN     0x01
#define CODE_PBM_FD     0x02
#define CODE_PBM_MC     0x03
#define CODE_PBM_ON     0x04
#define CODE_PBM_RC     0x05
#define CODE_PBM_SM     0x06
#define CODE_PBM_LA     0x07
#define CODE_PBM_ME     0x08
#define CODE_PBM_BN     0x09
#define CODE_PBM_SD     0x0A
#define CODE_PBM_VM     0x0B
#define CODE_PBM_LD     0x0C
//======================================================================

//======================================================================
//  AT+CPBS OFFSET
#define CPBS_PBM_OFFSET     8
//======================================================================

//======================================================================
//  AT+CPBR OFFSET
#define CPBR_INDEX_OFFSET   8
//======================================================================

//======================================================================
//  AT+CPBF OFFSET
#define CPBF_INDEX_OFFSET   8
//======================================================================

//======================================================================
//  AT+CPBW OFFSET
#define CPBW_INDEX_OFFSET   8

#define CPBW_ERASE_ENTRY                0x00
#define CPBW_WRITE_ENTRY                0x01
#define CPBW_WRITE_FIRST_EMPTY_ENTRY    0x02
//======================================================================

//======================================================================
//  SEND CMD STATE STEP
#define CMD_PHONEBOOK_IDLE          0x00

#define WAIT_ANSWER_CMD_AT_CPBS     0x01    //  Answer at command - Select Phonebook Memory Storage
#define WAIT_ANSWER_CMD_AT_CPBR     0x02    //  Answer at command - Read Phonebook Entry
#define WAIT_ANSWER_CMD_AT_CPBF     0x03    //  Answer at command - Find Phonebook Entry
#define WAIT_ANSWER_CMD_AT_CPBW     0x04    //  Answer at command - Write Phonebook Entry
//======================================================================

#define MAX_PN_LEN	       40      //  Max lenght Phone Number Array
#define MAX_PT_LEN	       20      //  Max lenght Phone Text Array

class PhoneBookCmd_GSM {
    
public: 
    uint8_t SetCmd_AT_CPBS(uint8_t TypeOfCmd, bool Query);
    uint8_t SetCmd_AT_CPBR(uint8_t PhoneBookIndex, bool Query);
    uint8_t SetCmd_AT_CPBF(void);
    uint8_t SetCmd_AT_CPBW(uint8_t Index, uint8_t CmdType);
    
    union PhoneBookFlag {
        uint32_t  PhoneBookDoubleWord;
        struct {
            uint8_t StorageCode             :4;     // "0x00": "DC"
                                                    // "0x01": "EN"
                                                    // "0x02": "FD"
                                                    // "0x03": "MC"
                                                    // "0x04": "ON"
                                                    // "0x05": "RC"
                                                    // "0x06": "SM"
                                                    // "0x07": "LA"
                                                    // "0x08": "ME"
                                                    // "0x09": "BN"
                                                    // "0x0A": "SD"
                                                    // "0x0B": "VM"
                                                    // "0x0C": "LD"
            uint8_t EmptyMemoryLocation     :1;     // "1": Empty memory location
            uint8_t InvalidInputParameter   :1;     // "1": Invalid Input Parameter
            uint8_t InvalidInputFormat      :1;     // "1": Invalid Input Format
            uint8_t Free                    :1;     // Free
            uint8_t UsedLocations           :8;     // Byte type value indicating the total number of used locations in selected memory
            uint8_t TotalLocations          :8;     // Byte type value indicating the total number of locations in selected memory
            uint8_t MemoryFailure           :1;     // "1": Memory failure
            uint8_t MemoryFull              :1;     // "1": Memory full
            uint8_t InvalidIndex            :1;     // "1": Memory invalid index
            uint8_t NotFound                :1;     // "1": Memory not found
            uint8_t TextStrTooLong          :1;     // "1": Memory Text too long
            uint8_t DialStrTooLong          :1;     // "1": Memory Dial too long
            uint8_t InvalidTextChar         :1;     // "1": Memory Invalid char
            uint8_t InvalidDialChar         :1;     // "1": Memory Invalid char
        } Bit;  
    } PhoneBookFlag;
    
    uint8_t IndexLocation;              //  PhoneBook Location
    uint8_t MaxPhoneNumberLen;       	//  Phone number MAX length
    uint8_t MaxPhoneNumberTextLen;   	//  Text MAX length
    uint8_t PhoneText[MAX_PT_LEN];   	//  Text associated at phone number 
    uint8_t PhoneNumber[MAX_PN_LEN]; 	//  Phone Number
    uint8_t PhoneNumberType;            //  129 National number
                                        //  161 National number
                                        //  145 International number
                                        //  177 Network specific

    void    GsmPhoneBookWaitAnswer(void);   
    void    GsmPhoneBookRetrySendCmd(void);
    
private:
    uint8_t * FlashPbmAdd;

    void FlashAddPhoneBook(uint8_t TypeOfFac);
    void PhoneNumberNotFound(void);
};

extern PhoneBookCmd_GSM PhoneBook;

#endif