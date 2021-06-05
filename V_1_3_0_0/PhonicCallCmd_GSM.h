/*********************************************************************
 *
 *       PhonicCall command
 *
 *********************************************************************
 * FileName:        PhonicCallCmd_GSM.h
 * Revision:        1.0.0
 * Date:            01/10/2016
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

#ifndef _PhonicCallCmd_GSM_H_INCLUDED__
#define _PhonicCallCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
const char AT_ATA[]                         PROGMEM = "ATA\r\n";            //  Command - Answer incoming call
const char AT_ATD[]                         PROGMEM = "ATD";                //  Command - Mobile Originated Call to Dial A number
const char AT_ATD_PHONE_NUMBER_MEMORY[]     PROGMEM = "ATD>";               //  Command - Originate Call to phone number in current memory
const char AT_ATDL[]                        PROGMEM = "ATDL";               //  Command - Redial Last Telephone Number Used

const char AT_ATH[]                         PROGMEM = "ATH";                //  Command - Hang-Up incoming call

//#if defined(SIMCOM_SIM800C) || defined(FIBOCOM_G510)
//const char AT_ATH[]                       PROGMEM = "ATH\r\n";            //  Command - Hang-Up incoming call
//#endif
//#if defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95)
//const char AT_ATH[]                       PROGMEM = "ATH0\r\n";           //  Command - Hang-Up incoming call
//#endif

//======================================================================

//======================================================================
//  ATD OFFSET
#define ATD_OFFSET  3
//======================================================================

//======================================================================
//  ATD2 OFFSET
#define ATD_PHONE_NUM_MEM_OFFSET    4
//======================================================================

//======================================================================
//  ATH OFFSET
#define ATH_OFFSET  3
//======================================================================

//======================================================================
//  SEND CMD STATE STEP
#define CMD_PHONIC_CALL_IDLE    0x00

#define WAIT_ANSWER_CMD_ATA                     0x01
#define WAIT_ANSWER_CMD_ATH                     0x02
#define WAIT_ANSWER_CMD_ATD                     0x03
#define WAIT_ANSWER_CMD_ATD_PHONE_NUMBER_MEMORY 0x04
#define WAIT_ANSWER_CMD_ATDL                    0x05
//======================================================================

class PhonicCallCmd_GSM {
    
public: 
    uint8_t SetCmd_ATA(void);
    uint8_t SetCmd_ATD(void);
    uint8_t SetCmd_ATD_PhoneNumberMemory(uint8_t n);
    uint8_t SetCmd_ATDL(void);
    uint8_t SetCmd_ATH(uint8_t n);

    union PhonicCallFlag {
        uint8_t  PhonicCallByte;
        struct {
            uint8_t IncomingCallInProgress  :1;     //  "1": Incoming Call in progress
            uint8_t OutgoingCallInProgress  :1;     //  "1": Outgoing Call in progress
            uint8_t NoDialtone              :1;     //  "1": No dial tone detected
            uint8_t Busy                    :1;     //  "1": Engaged (busy) signal detected
            uint8_t NoCarrier               :1;     //  "1": The connection has been terminated or the attempt to establish a connection failed
            uint8_t NoAnswer                :1;     //  "1": If the remote station does not answer
            uint8_t CallSetupInProgress     :1;     //  "1": CME error. Call Setup in progress
            uint8_t Free                    :1;     //  Free
        } Bit;  
    } PhonicCallFlag;
    
    void    GsmPhonicCallWaitAnswer(void);  
    void    GsmPhonicCallRetrySendCmd(void);
    
private:

};

extern PhonicCallCmd_GSM PhonicCall;

#endif