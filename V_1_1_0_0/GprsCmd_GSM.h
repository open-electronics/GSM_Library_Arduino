/*********************************************************************
 *
 *       Gprs command (Supported SIMCOM SIMCOM_SIM800C, SIMCOM_SIM900, SIMCOM_SIM928A and QUECTEL M95)
 *                               FIBOCOM G510 not supported
 *
 *********************************************************************
 * FileName:        GprsCmd_GSM.h
 * Revision:        1.0.0 (First issue)
 * Date:            15/10/2018
 * Dependencies:    Arduino.h
 *                  Uart_Gsm.h
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

#ifndef _GprsCmd_GSM_H_INCLUDED__
#define _GprsCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  APN - STORED IN EEPROM
//  When user compiles this code the data is inserted in the Eeprom memory space, and a file .eep is created
//  The path where to find .eep file (for example) -> C:\Documents and Settings\YourUserName\LocalSettings\Temp\arduino_build_xxxxxx
//  The Arduino IDE not writes Eeprom memory space automatically. Then the user must write manually this data in 
//  the eeprom space memory using a ISP connection or by code into "void setup()" routine

const uint8_t  EEMEM APN[32]           = "\"ibox.tim.it\"";         //  YOUR APN - Allocated 32 Byte - Inserts your APN to save it into EEPROM memory. Remember to include always the double quotation marks 
                                                                    //  Other operator are (For example): "web.omnitel.it"
                                                                    //                                    "internet.wind"
                                                                    //                                    "web.coopvoce.it"
const uint8_t  EEMEM APN_USER_NAME[32] = "\"YourUserName\"";        //  A string parameter which indicates the GPRS user name. The max length is 32 bytes
const uint8_t  EEMEM APN_PASSWORD[32]  = "\"YourPassword\"";        //  A string parameter which indicates the GPRS password. The max length is 32 bytes
const uint8_t  EEMEM PDP_TYPE[6]       = "\"IP\"";                  //  PDP Type (Packet Data Protocol type) a string parameter which specifies the type of packet data protocol
                                                                    //  "IP" Internet Protocol or "PPP" Point to Point Protocol
//======================================================================

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined (QUECTEL_M95)
    const char AT_CGATT[]     PROGMEM = "AT+CGATT=";                //  Command - Attach or Detach from GPRS Service
    const char AT_CGREG[]     PROGMEM = "AT+CGREG=";                //  Command - Set Network Registration (if <n> = 2 -> Enable network registration unsolicited result code with location information)
    const char AT_CGEREP[]    PROGMEM = "AT+CGEREP=1\r\n";          //  Command - Control Unsolicited GPRS Event Reporting
    const char AT_CGDCONT[]   PROGMEM = "AT+CGDCONT=1,\"IP\",";     //  Command - Define PDP Context
    const char AT_CGACT[]     PROGMEM = "AT+CGACT=1,1\r\n";         //  Command - PDP Context Activate or Deactivate
    const char AT_CGPADDR[]   PROGMEM = "AT+CGPADDR=1\r\n";         //  Command - Show PDP Address

    const char ATQ_CGATT[]    PROGMEM = "AT+CGATT?\r\n";            //  Query - Signal quantity report
    const char ATQ_CGREG[]    PROGMEM = "AT+CGREG?\r\n";            //  Query - Network registration
#endif
//======================================================================

//======================================================================
//  AT COMMAND ANSWER - STORED IN FLASH MEMORY
const char AT_ANSW_CGREG[]          PROGMEM = "+CGREG:";            // Key word for answer at command CGREG
const char AT_ANSW_CGATT[]          PROGMEM = "+CGATT:";            // Key word for answer at command CGATT
const char AT_ANSW_CGATT_DETACH[]   PROGMEM = "+CGEV: ME DETACH";   // Key word for answer at command CGATT Detach
const char AT_ANSW_CGDCONT[]        PROGMEM = "+CGDCONT:";          // Key word for answer at command CGDCONT
//======================================================================

//======================================================================
//  AT+CGATT OFFSET
#define CGATT_STATE_OFFSET  9

#define CGATT_DETACH        0
#define CGATT_ATTACH        1
//======================================================================

//======================================================================
//  AT+CGREG OFFSET
#define CGREG_PAR_OFFSET    9

#define CGREG_DISABLE       0
#define CGREG_ENABLE        1
//======================================================================

//======================================================================
//  AT+CGDCONT OFFSET
#define CGDCONT_OFFSET      18
//======================================================================

//======================================================================
//  SEND CMD STATE STEP
#define CMD_GPRS_IDLE               0x00

#define WAIT_ANSWER_CMD_AT_CGATT    0x01
#define WAIT_ANSWER_CMD_ATQ_CGATT   0x02
#define WAIT_ANSWER_CMD_AT_CGREG    0x03
#define WAIT_ANSWER_CMD_ATQ_CGREG   0x04
//======================================================================

//======================================================================
//  INIT GPRS STATE STEP
//#define   INIT_IDLE                       0x00    //  See GenericCmd_GSM.h
#define INIT_CMD_QUERY_CGATT                0x01
#define INIT_CMD_CGATT                      0x02
#define INIT_CMD_CIPMUX                     0x03
#define INIT_CMD_CGEREP                     0x04
#define INIT_CMD_CSTT                       0x05
#define INIT_CMD_CIICR                      0x06
#define INIT_CMD_CIFSR                      0x07
#define INIT_CMD_CGDCONT                    0x08
#define INIT_CMD_CGACT                      0x09
#define INIT_CMD_CGPADDR                    0x0A
#define INIT_CMD_CIPHEAD                    0x0B
#define INIT_CMD_CIPSRIP                    0x0C

//#define INIT_WAIT_IDLE                    0x00    //  See GenericCmd_GSM.h
#define INIT_WAIT_ANSWER_CMD_QUERY_CGATT    0x01
#define INIT_WAIT_ANSWER_CMD_CGATT          0x02
#define INIT_WAIT_ANSWER_CMD_CIPMUX         0x03
#define INIT_WAIT_ANSWER_CMD_CGEREP         0x04
#define INIT_WAIT_ANSWER_CMD_CSTT           0x05
#define INIT_WAIT_ANSWER_CMD_CIICR          0x06
#define INIT_WAIT_ANSWER_CMD_CIFSR          0x07
#define INIT_WAIT_ANSWER_CMD_CGDCONT        0x08
#define INIT_WAIT_ANSWER_CMD_CGACT          0x09
#define INIT_WAIT_ANSWER_CMD_CGPADDR        0x0A
#define INIT_WAIT_ANSWER_CMD_CIPHEAD        0x0B
#define INIT_WAIT_ANSWER_CMD_CIPSRIP        0x0C
//======================================================================

class GprsCmd_GSM {
    
public:
    //-------------------------------------------------------------------------------------
    struct {
        uint16_t StartAddApn;               //  EEPROM start address for APN
        uint16_t StartAddApnUserName;       //  EEPROM start address for User Name
        uint16_t StartAddApnPassword;       //  EEPROM start address for Password
        uint16_t StartAddPdpType;           //  EEPROM start address for PDP Type
    } GprsEepromAdd;
    void GprsEepromStartAddSetup(void);
    //-------------------------------------------------------------------------------------
    
    uint8_t SetCmd_AT_CGREG(bool Query, uint8_t n); //  Returned data saved into "CREG_Info" structure
                            
    //-------------------------------------------------------------------------------------
    uint8_t SetCmd_AT_CGATT(bool Query, uint8_t State);
    union GprsFlag {
        uint8_t  GprsByte;
        struct {
            uint8_t EnableGprsFunctions     :1;     //  "1": Enable GPRS functions. Sets this flag in the Setup() function if you want to use GPRS functions
            uint8_t GprsService             :1;     //  "0" GPRS Detached; "1" GPRS Attached
            uint8_t Cid                     :2;     //  A numeric parameter which specifies a particular PDP context definition (see +CGDCONT Command)
            uint8_t GprsError               :1;     //  "1" Unspecified GPRS Error
            uint8_t GprsSuspended           :1;     //  "1" GPRS is suspended
            uint8_t PdpFailure              :1;     //  "1" PDP Authentication failure
            uint8_t Free                    :1;     //  Free bits
        } Bit;  
    } GprsFlag;
    //-------------------------------------------------------------------------------------

    uint8_t PDP_IP_Address[18];             // String type, IP address. Format: "<n>.<n>.<n>.<n>" where <n>=0..255
    
    void    InitGprsSendCmd(void);          //  Send init command (Init mode only)
    void    InitGprsWaitAnswer(void);       //  Wait init answer (Init mode only)
    void    GprsWaitAnswer(void);   
    void    GprsRetrySendCmd(void);
    
private:

};

extern GprsCmd_GSM Gprs;

#endif