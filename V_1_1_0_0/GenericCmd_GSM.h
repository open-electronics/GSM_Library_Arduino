/*********************************************************************
 *
 *       Generic command for GSM
 *
 *********************************************************************
 * FileName:        GenericCmd_GSM.h
 * Revision:        1.0.0 (First issue)
 * Date:            01/10/2016
 *
 * Revision:        1.1.0
 *                  15/10/2018
 *                  - The name of function "void EepromStartAddSetup(void)" was changed in "void PswdEepromStartAddSetup(void)"
 *                  - The name of structure "EepromAdd" was changed in "PswdEepromAdd"
 *                  - Added a section to parametrize the audio interface (Speaker volume and microphone gain). One for each GSM engine
 *                  - Added new error code: Unspecified GPRS error (0x0094), PDP authentication failure (0x0095) and GPRS is suspended (0x0212)
 *                  - Removed the AT command ECHO CANC in the GSM engine initialization
 *                  - Added the AT command AT+CPBS in the GSM engine initialization
 *                  - Added the AT command AT+CALM in the GSM engine initialization
 *                  - Added the AT command AT+CRSL in the GSM engine initialization
 *                  - Added the AT command AT+CALM to the available commands list
 *                  - Added the AT command AT+CRSL to the available commands list
 *                  - Added the AT command AT+CALS to the available commands list
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
 *
 * Dependencies:    Arduino.h
 *                  Io_Gsm.h
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

#ifndef _GenericCmd_GSM_H_INCLUDED__
#define _GenericCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Io_GSM.h"

//======================================================================
//  PIN/PUK CODE - STORED IN EEPROM
//  The PIN1 code and PUK1 code are distributed by your thelephony Operator
//  These code are intended like factory values
//  When user compiles this code the data is inserted in the Eeprom memory space, and a file .eep is created
//  The path where to find .eep file (for example) -> C:\Documents and Settings\YourUserName\LocalSettings\Temp\arduino_build_xxxxxx
//  The Arduino IDE not writes Eeprom memory space automatically. Then the user must write manually this data in 
//  the eeprom space memory using a ISP connection or by code into "void setup()" routine

const uint8_t  EEMEM LONG_GEN_PSWD[11]  = "\"12345678\"";
const uint8_t  EEMEM PH_PUK_CODE[11]    = "\"44444444\"";       
const uint8_t  EEMEM PUK2_CODE[11]      = "\"99999999\"";
const uint8_t  EEMEM PUK1_CODE[11]      = "\"54094153\"";       //  PUK Code SIM Futura (Original code)
const uint8_t  EEMEM SHORT_GEN_PSWD[7]  = "\"1234\"";
const uint8_t  EEMEM PH_PIN_CODE[7]     = "\"5555\"";
const uint8_t  EEMEM PIN2_CODE[7]       = "\"8888\"";
const uint8_t  EEMEM PIN1_CODE[7]       = "\"4629\"";           //  PIN Code SIM Futura (Original code)
//======================================================================

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
const char AT[]          PROGMEM = "AT\r\n";
const char ATF0[]        PROGMEM = "AT&F0\r\n";                     //  Command - Set all TA parameters to manufacturer defaults
const char ATW0[]        PROGMEM = "AT&W0\r\n";                     //  Command - Store the current configuration in profile 0
const char ATZ0[]        PROGMEM = "ATZ0\r\n";                      //  Command - Restore profile 0
const char ATV1[]        PROGMEM = "ATV1\r\n";                      //  Command - Set TA Response Format (if <n> = 1 -> Long Result code format)
const char ATQ0[]        PROGMEM = "ATQ0\r\n";                      //  Command - Set Result Code Presentation Mode (if <n> = 0 -> TA transmits result code)
const char ATE0[]        PROGMEM = "ATE0\r\n";                      //  Command - Set Command Echo Mode (if <n> = 0 -> Echo mode off)
const char AT_CMEE[]     PROGMEM = "AT+CMEE=1\r\n";                 //  Command - Set Report Mobile Equiment Error (if <n> = 1 -> Enable +CME ERROR - Use numeric error)
const char AT_IPR[]      PROGMEM = "AT+IPR=0\r\n";                  //  Command - Set TE-TA Autobaud mode
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95) || defined(AI_THINKER_A9)
    const char AT_ICF[]      PROGMEM = "AT+ICF=3,0\r\n";            //  Command - Set TE-TA Control Character Framing (8 Data, 0 Parity, 1 Stop) - (Parity Odd)
#endif
#ifdef FIBOCOM_G510
    const char AT_ICF[]      PROGMEM = "AT+ICF=3,4\r\n";            //  Command - Set TE-TA Control Character Framing (8 Data, 0 Parity, 1 Stop) - (Parity Odd)
#endif
    const char AT_IFC[]      PROGMEM = "AT+IFC=0,0\r\n";            //  Command - Set TE-TA Local Data Flow Control (No Flow control)
    const char AT_CALS[]     PROGMEM = "AT+CALS=0\r\n";             //  Command - Select Alert Sound (0 - 19)
#ifdef EARPHONES_JACK
    #ifdef SIMCOM_SIM800C
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=60\r\n";         //  Command - Set Ringer Sound Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=60\r\n";         //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+CMIC=0,6\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8
    #endif  
    #ifdef SIMCOM_SIM900
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=4\r\n";          //  Command - Set Ringer Sound Level (Integer value 0-4) - 0 LEVEL OFF; 1 LEVEL LOW, 2 LEVEL MEDIUM, 3 LEVEL HIGH, 4 LEVEL CRESCENDO
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=60\r\n";         //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)       
        const char AT_CMIC[]    PROGMEM = "AT+CMIC=0,6\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8
    #endif  
    #ifdef SIMCOM_SIM928A
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=4\r\n";          //  Command - Set Ringer Sound Level (Integer value 0-4) - 0 LEVEL OFF; 1 LEVEL LOW, 2 LEVEL MEDIUM, 3 LEVEL HIGH, 4 LEVEL CRESCENDO
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=60\r\n";         //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)       
        const char AT_CMIC[]    PROGMEM = "AT+CMIC=0,6\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8
    #endif  
    #ifdef QUECTEL_M95
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=60\r\n";         //  Command - Set Ringer Sound Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=40\r\n";         //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+QMIC=0,2\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8        
    #endif
    #ifdef FIBOCOM_G510
        const char AT_CALM[]    PROGMEM = "AT+CALM=0,0\r\n";        //  Command - Set Alert Sound Mode: 0 Ring; 1 Silent mode (First parameter "Call Mode", second "SMS Mode")
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=7\r\n";          //  Command - Set Ringer Sound Level (0-7 Value) - 0 is lowest volume (not mute)
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=4\r\n";          //  Command - Set Loud Speaker Volume Level (0-6 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+MMICG=4\r\n";         //  Command - Set MIC Gain: value of gain 8
    #endif
    #ifdef AI_THINKER_A9
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=4\r\n";          //  Command - Set Loud Speaker Volume Level (0-6 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+VGR=3\r\n";           //  Command - Set MIC Gain: value of gain 6
    #endif
#else 
    #ifdef SIMCOM_SIM800C
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=60\r\n";         //  Command - Set Ringer Sound Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=100\r\n";        //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+CMIC=0,4\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8        
    #endif  
    #ifdef SIMCOM_SIM900
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=4\r\n";          //  Command - Set Ringer Sound Level (Integer value 0-4) - 0 LEVEL OFF; 1 LEVEL LOW, 2 LEVEL MEDIUM, 3 LEVEL HIGH, 4 LEVEL CRESCENDO
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=100\r\n";        //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)   
        const char AT_CMIC[]    PROGMEM = "AT+CMIC=0,4\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8
    #endif      
    #ifdef SIMCOM_SIM928A
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=4\r\n";          //  Command - Set Ringer Sound Level (Integer value 0-4) - 0 LEVEL OFF; 1 LEVEL LOW, 2 LEVEL MEDIUM, 3 LEVEL HIGH, 4 LEVEL CRESCENDO
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=100\r\n";        //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)       
        const char AT_CMIC[]    PROGMEM = "AT+CMIC=0,4\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8
    #endif  
    #ifdef QUECTEL_M95
        const char AT_CALM[]    PROGMEM = "AT+CALM=0\r\n";          //  Command - Set Alert Sound Mode: 0 Normal mode; 1 Silent mode
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=60\r\n";         //  Command - Set Ringer Sound Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=70\r\n";         //  Command - Set Loud Speaker Volume Level (0-100 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+QMIC=0,2\r\n";        //  Command - Set MIC Gain: <Channel> 0, value of gain 8        
    #endif
    #ifdef FIBOCOM_G510
        const char AT_CALM[]    PROGMEM = "AT+CALM=0,0\r\n";        //  Command - Set Alert Sound Mode: 0 Ring; 1 Silent mode (First parameter "Call Mode", second "SMS Mode")
        const char AT_CRSL[]    PROGMEM = "AT+CRSL=7\r\n";          //  Command - Set Ringer Sound Level (0-7 Value) - 0 is lowest volume (not mute)
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=6\r\n";          //  Command - Set Loud Speaker Volume Level (0-6 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+MMICG=4\r\n";         //  Command - Set MIC Gain: value of gain 8
    #endif
    #ifdef AI_THINKER_A9
        const char AT_CLVL[]    PROGMEM = "AT+CLVL=6\r\n";          //  Command - Set Loud Speaker Volume Level (0-6 Value) - 0 is lowest volume (not mute)
        const char AT_CMIC[]    PROGMEM = "AT+VGR=4\r\n";           //  Command - Set MIC Gain: value of gain 6
    #endif
#endif

const char AT_CRC[]     PROGMEM = "AT+CRC=1\r\n";                   //  Command - Set Cellular Result Code for Incoming Call Indication (if <n> = 1 -> Enable Extended Format)
const char AT_CLIP[]    PROGMEM = "AT+CLIP=1\r\n";                  //  Command - Set Calling Line Identification Presentation (if <n> = 1 -> Enable CLIP Notification)
const char AT_CMGF[]    PROGMEM = "AT+CMGF=1\r\n";                  //  Command - Set SMS Message Format (if <n> = 1 -> Text Mode; 0 -> PDU Mode)
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined (QUECTEL_M95) || defined(FIBOCOM_G510)
    const char AT_CSCS[]    PROGMEM = "AT+CSCS=\"IRA\"\r\n";        //  Command - Set TE Character Set ("IRA" -> International Reference Alphabet)
#endif
#ifdef AI_THINKER_A9
    const char AT_CSCS[]    PROGMEM = "AT+CSCS=\"GSM\"\r\n";            //  Command - Set TE Character Set ("GSM" -> GSM 7bit)
#endif
const char AT_CNMI[]     PROGMEM = "AT+CNMI=1,1,0,0,0\r\n";             //  Command - Set New SMS Message Indications
const char AT_CPMS_DEF[] PROGMEM = "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n";  //  Command - Set Preferrend SMS Message Storage (Default)
const char AT_CSMP[]     PROGMEM = "AT+CSMP=17,167,0,0\r\n";            //  Command - Set SMS Text Mode Parameters
const char AT_CPBS_DEF[] PROGMEM = "AT+CPBS=\"SM\"\r\n";                //  Command - Select Phonebook Memory Storage
const char AT_CREG[]     PROGMEM = "AT+CREG=2\r\n";                     //  Command - Set Network Registration (if <n> = 2 -> Enable network registration unsolicited result code with location information)
const char AT_CFUN[]     PROGMEM = "AT+CFUN=x\r\n";                     //  Command - Set Phone Functionality (if <x> = 0 -> Minimum functionality; 1 -> Full functionality; 4 -> Disable phone both transmit and receive RF circuits)
const char AT_CCLK[]     PROGMEM = "AT+CCLK=\"";                        //  Command - Write clock
const char AT_CPAS[]     PROGMEM = "AT+CPAS\r\n";                       //  Command - Phone Activity Status

const char ATV[]         PROGMEM = "AT&V\r\n";                          //  Query   - Display current configuration for sesult code (See ATV command)
const char ATQ_CSQ[]     PROGMEM = "AT+CSQ\r\n";                        //  Query   - Signal quality report
const char ATQ_CREG[]    PROGMEM = "AT+CREG?\r\n";                      //  Query   - Network registration
const char ATQ_CCLK[]    PROGMEM = "AT+CCLK?\r\n";                      //  Query   - Read clock
const char ATQ_COPS[]    PROGMEM = "AT+COPS?\r\n";                      //  Query   - Operator Selection
const char AT_GMI[]      PROGMEM = "AT+GMI\r\n";                        //  Query   - Request Manufacturer Identification (AT Commands According to V.25TER)
const char AT_CGMI[]     PROGMEM = "AT+CGMI\r\n";                       //  Query   - Request Manufacturer Identification (AT Commands According to 3GPP TS 27.007)
const char AT_GMM[]      PROGMEM = "AT+GMM\r\n";                        //  Query   - Request TA Model Identification (AT Commands According to V.25TER)
const char AT_CGMM[]     PROGMEM = "AT+CGMM\r\n";                       //  Query   - Request TA Model Identification (AT Commands According to 3GPP TS 27.007)
const char AT_GMR[]      PROGMEM = "AT+GMR\r\n";                        //  Query   - Request TA Revision Identification of Software Release (AT Commands According to V.25TER)
const char AT_CGMR[]     PROGMEM = "AT+CGMR\r\n";                       //  Query   - Request TA Revision Identification of Software Release (AT Commands According to 3GPP TS 27.007)
const char AT_GSN[]      PROGMEM = "AT+GSN\r\n";                        //  Query   - Request TA Serial Number Identification (IMEI) (AT Commands According to V.25TER)
const char AT_CGSN[]     PROGMEM = "AT+CGSN\r\n";                       //  Query   - Request TA Serial Number Identification (IMEI) (AT Commands According to 3GPP TS 27.007)

#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    const char AT_PW_DOWN[]  PROGMEM = "AT+CPOWD=1\r\n";                //  Command - SIM900 software Power Down (Normal Power Down)
#endif
#ifdef QUECTEL_M95
    const char AT_PW_DOWN[]  PROGMEM = "AT+QPOWD=1\r\n";                //  Command - G510 software Power Down (Normal Power Down)
#endif
#ifdef FIBOCOM_G510
    const char AT_PW_DOWN[]  PROGMEM = "AT+MRST\r\n";                   //  Command - M95 software Power Down
#endif
#ifdef AI_THINKER_A9
    const char AT_PW_DOWN[]  PROGMEM = "AT+CPOF\r\n";                   //  Command - M95 software Power Down
#endif
//======================================================================

//======================================================================
//  AT COMMAND ANSWER - STORED IN FLASH MEMORY
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    const char AT_ANSW_PWR_ON[]         PROGMEM = "RDY";                // Answer HW Power On at fixed baudrate
    const char AT_ANSW_PWR_OFF[]        PROGMEM = "NORMAL POWER DOWN";  // Answer HW Power Off
#endif
#ifdef QUECTEL_M95
    const char AT_ANSW_PWR_ON[]         PROGMEM = "RDY";                // Answer HW Power On at fixed baudrate
    const char AT_ANSW_PWR_OFF[]        PROGMEM = "NORMAL POWER DOWN";  // Answer HW Power Off
#endif
#ifdef FIBOCOM_G510
    const char AT_ANSW_PWR_ON[]         PROGMEM = "AT command ready";   // Answer HW Power On at fixed baud rate
    const char AT_ANSW_PWR_OFF[]        PROGMEM = "";                   // Answer HW Power Off
#endif
#ifdef AI_THINKER_A9
    const char AT_ANSW_PWR_ON[]         PROGMEM = "AT command ready";   // Answer HW Power On at fixed baud rate
    const char AT_ANSW_PWR_OFF[]        PROGMEM = "MS OFF";             // Answer HW Power Off
#endif
const char AT_ANSW_OK[]             PROGMEM = "OK";                     // Answer at commands executed with succes
const char AT_ANSW_CME_ERROR[]      PROGMEM = "+CME ERROR:";            // Key word answer for general Errors 
const char AT_ANSW_CMS_ERROR[]      PROGMEM = "+CMS ERROR:";            // Key word answer for CMS Errors
const char AT_ANSW_ERROR[]          PROGMEM = "ERROR:";                 // Key word Error (No code)
const char AT_ANSW_CSQ[]            PROGMEM = "+CSQ:";                  // Key word for answer at the query AT+CSQ
const char AT_ANSW_CREG[]           PROGMEM = "+CREG:";                 // Key word for answer at the query AT+CREG?
const char AT_ANSW_COPS[]           PROGMEM = "+COPS:";                 // Key word for answer at the command AT+COPS
const char AT_ANSW_CPAS[]           PROGMEM = "+CPAS:";                 // Key word for answer at the command AT+CPAS
const char AT_ANSW_ATD_NOANSWER[]   PROGMEM = "NO ANSWER";              // Key word for answer at the command ATD
const char AT_ANSW_ATD_NOCARRIER[]  PROGMEM = "NO CARRIER";             // Key word for answer at the command ATD
const char AT_ANSW_ATD_NODIALTONE[] PROGMEM = "NO DIALTONE";            // Key word for answer at the command ATD
const char AT_ANSW_ATD_BUSY[]       PROGMEM = "BUSY";                   // Key word for answer at the command ATD

const char AT_ANSW_CRING_VOICE[]    PROGMEM = "+CRING: VOICE";          // Key Word to identify phonic call
const char AT_ANSW_CLIP[]           PROGMEM = "+CLIP:";                 // Key Word to identify phone number into phonic call
const char AT_ANSW_CMTI[]           PROGMEM = "+CMTI:";                 // Key Word to identify SMS received
const char AT_ANSW_CCLK[]           PROGMEM = "+CCLK:";

const char AT_MAJOR[]               PROGMEM = ">";
//======================================================================

//======================================================================
//  AT POWER DOWN CODE
#define URGENT_POWEROFF         0
#define NORMAL_POWEROFF         1
//======================================================================

//======================================================================
//  AT+CFUN CODE
#define MINIMUM_FUNCTIONALITY   0
#define FULL_FUNCTIONALITY      1
#define DISABLE_FUNCTIONALITY   4
//======================================================================

//======================================================================
//  "+CME ERROR" / "+CMS ERROR" OFFSET
#define ERROR_CODE_OFFSET       12
//======================================================================

//======================================================================
//  AT+CCLK OFFSET
#define CCLK_OFFSET             9
//======================================================================

//======================================================================
//  AT+CSQ MAX Parameters
#define MAX_RSSI                99
#define MAX_BER                 99
//======================================================================

//======================================================================
//  AT+COPS OFFSET
#define COPS_OPERATOR_OFFSET    11
//======================================================================

//======================================================================
//  CPAS ASPECTED VALUES
#define CPAS_READY              0   // Ready (MT allows commands from TA/TE)
#define CPAS_UNKNOWN            2   // Unknown (MT is not guaranteed to respond to instructions)
#define CPAS_RINGING            3   // Ringing (MT is ready for commands from TA/TE, but the ringer is active)
#define CPAS_CALL_IN_PROGRESS   4   // Call in progress (MT is ready for commands from TA/TE, but a call is in progress)
//======================================================================

//======================================================================
//  GMI/CGMI
#define GMI_V25_FORMAT          0
#define CGMI_3GPP_FORMAT        1
//======================================================================

//======================================================================
//  GMM/CGMM
#define GMM_V25_FORMAT          0
#define CGMM_3GPP_FORMAT        1
//======================================================================

//======================================================================
//  GMR/CGMR
#define GMR_V25_FORMAT          0
#define CGMR_3GPP_FORMAT        1
//======================================================================

//======================================================================
//  GSN/CGSN
#define GSN_V25_FORMAT          0
#define CGSN_3GPP_FORMAT        1
//======================================================================

//======================================================================
//  AT+CALS OFFSET
#define CALS_OFFSET             8
#define MAX_SOUND_TYPE          19      //  Max value for Alert Sound Type. SIMCOM Only

#ifdef SIMCOM_SIM800C
    #define START_PLAYING_RING_TONE     1
    #define STOP_PLAYING_RING_TONE      0
#endif
#ifdef SIMCOM_SIM900
    #define START_PLAYING_RING_TONE     0
    #define STOP_PLAYING_RING_TONE      1
#endif
//======================================================================

//======================================================================
//  AT+CALM OFFSET
#define CALM_OFFSET             8

#define RING_MODE               0
#define SILENT_MODE             1
//======================================================================

//======================================================================
//  AT+CRSL OFFSET
#define CRSL_LEVEL_OFFSET       8
#if defined(SIMCOM_SIM800C) || defined(QUECTEL_M95)
    #define MAX_SOUND_VALUE 100
#endif
#if defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define MAX_SOUND_VALUE     4
#endif
#ifdef FIBOCOM_G510
    #define MAX_SOUND_VALUE     7
#endif
//======================================================================

//======================================================================
//  AT+CLVL OFFSET
#define CLVL_LEVEL_OFFSET       8
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A) || defined(QUECTEL_M95)
    #define MAX_VOLUME_VALUE    100
#endif
#ifdef FIBOCOM_G510
    #define MAX_VOLUME_VALUE    6
#endif
#ifdef AI_THINKER_A9
    #define MAX_VOLUME_VALUE    8
#endif
//======================================================================

//======================================================================
//  AT+CMIC OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CMIC_CHANNEL_OFFSET     8
    #define MAX_CHANNEL_VALUE       3
    #define MAX_MIC_GAIN_VALUE      15
#endif
#ifdef QUECTEL_M95
    #define CMIC_CHANNEL_OFFSET     8
    #define MAX_CHANNEL_VALUE       2
    #define MAX_MIC_GAIN_VALUE      15
#endif
#ifdef FIBOCOM_G510
    #define CMIC_CHANNEL_OFFSET     9
    #define MAX_CHANNEL_VALUE       0
    #define MAX_MIC_GAIN_VALUE      15
#endif
#ifdef AI_THINKER_A9
    #define CMIC_CHANNEL_OFFSET     7
    #define MAX_CHANNEL_VALUE       0
    #define MAX_MIC_GAIN_VALUE      8
#endif
//======================================================================

//======================================================================
//  ASCII HEX CODE
#define ASCII_NULL                      0x00    //  Dec -> 000 (Null)
    
#define ASCII_TAB                       0x09    //  Dec -> 009          <TAB>
#define ASCII_LINE_FEED                 0x0A    //  Dec -> 010 ('\n')   <LF>
#define ASCII_VERTICAL_TAB              0x0B    //  Dec -> 011          <VT>
#define ASCII_FORM_FEED                 0x0C    //  Dec -> 012          <FF>
#define ASCII_CARRIAGE_RET              0x0D    //  Dec -> 013 ('\r')   <CR>
    
#define ASCII_SUB                       0x1A    //  DEC -> 026          <SUB>
#define ASCII_ESC                       0x1B    //  DEC -> 027          <ESC>
    
#define ASCII_SPACE                     0x20    //  DEC -> 032          <SPACE>
    
#define ASCII_ESCLAMATION_MARKS         0x21    //  DEC -> 033 ('!')    
#define ASCII_QUOTATION_MARKS           0x22    //  DEC -> 034 ('"')
#define ASCII_HASH                      0x23    //  DEC -> 035 ('#')
#define ASCII_DOLLAR                    0x24    //  DEC -> 036 ('$')
#define ASCII_PERCENTAGE                0x25    //  DEC -> 037 ('%')
#define ASCII_AMPERSAND                 0x26    //  DEC -> 038 ('&')
#define ASCII_QUOTE                     0x27    //  DEC -> 039 (''')
#define ASCII_OPEN_ROUND_BRACKET        0x28    //  Dec -> 040 ('(')
#define ASCII_CLOSE_ROUND_BRACKET       0x29    //  Dec -> 041 (')')
#define ASCII_CLOSE_ASTERISK            0x2A    //  Dec -> 042 ('*')    
#define ASCII_PLUS                      0x2B    //  Dec -> 043 ('+')
#define ASCII_COMMA                     0x2C    //  Dec -> 044 (',')
#define ASCII_MINUS                     0x2D    //  Dec -> 045 ('-')
#define ASCII_DOT                       0x2E    //  Dec -> 046 ('.')    
#define ASCII_SLASH                     0x2F    //  Dec -> 047 ('/')
    
#define ASCII_0                         0x30    //  Dec -> 048 ('0')
#define ASCII_1                         0x31    //  Dec -> 049 ('1')
#define ASCII_2                         0x32    //  Dec -> 050 ('2')
#define ASCII_3                         0x33    //  Dec -> 051 ('3')
#define ASCII_4                         0x34    //  Dec -> 052 ('4')
#define ASCII_5                         0x35    //  Dec -> 053 ('5')
#define ASCII_6                         0x36    //  Dec -> 054 ('6')
#define ASCII_7                         0x37    //  Dec -> 055 ('7')
#define ASCII_8                         0x38    //  Dec -> 056 ('8')
#define ASCII_9                         0x39    //  Dec -> 057 ('9')
#define ASCII_COLON                     0x3A    //  Dec -> 058 (':')
#define ASCII_SEMI_COLON                0x3B    //  Dec -> 059 (';')
#define ASCII_LOWER                     0x3C    //  Dec -> 060 ('<')
#define ASCII_EQUAL                     0x3D    //  Dec -> 061 ('=')
#define ASCII_GREATER                   0x3E    //  Dec -> 062 ('>')
#define ASCII_QUESTION_MARK             0x3F    //  Dec -> 063 ('?')
#define ASCII_AT                        0x40    //  Dec -> 064 ('@')

#define ASCII_A                         0x41    //  Dec -> 065 ('A')
#define ASCII_B                         0x42    //  Dec -> 066 ('B')
#define ASCII_C                         0x43    //  Dec -> 067 ('C')
#define ASCII_D                         0x44    //  Dec -> 068 ('D')
#define ASCII_E                         0x45    //  Dec -> 069 ('E')
#define ASCII_F                         0x46    //  Dec -> 070 ('F')
#define ASCII_G                         0x47    //  Dec -> 071 ('G')
#define ASCII_H                         0x48    //  Dec -> 072 ('H')
#define ASCII_I                         0x49    //  Dec -> 073 ('I')
#define ASCII_J                         0x4A    //  Dec -> 074 ('J')
#define ASCII_K                         0x4B    //  Dec -> 075 ('K')
#define ASCII_L                         0x4C    //  Dec -> 076 ('L')
#define ASCII_M                         0x4D    //  Dec -> 077 ('M')
#define ASCII_N                         0x4E    //  Dec -> 078 ('N')
#define ASCII_O                         0x4F    //  Dec -> 079 ('O')
#define ASCII_P                         0x50    //  Dec -> 080 ('P')
#define ASCII_Q                         0x51    //  Dec -> 081 ('Q')
#define ASCII_R                         0x52    //  Dec -> 082 ('R')
#define ASCII_S                         0x53    //  Dec -> 083 ('S')
#define ASCII_T                         0x54    //  Dec -> 084 ('T')
#define ASCII_U                         0x55    //  Dec -> 085 ('U')
#define ASCII_V                         0x56    //  Dec -> 086 ('V')
#define ASCII_W                         0x57    //  Dec -> 087 ('W')
#define ASCII_X                         0x58    //  Dec -> 088 ('X')
#define ASCII_Y                         0x59    //  Dec -> 089 ('Y')
#define ASCII_Z                         0x5A    //  Dec -> 090 ('Z')
    
#define ASCII_a                         0x61    //  Dec -> 097 ('a')
#define ASCII_b                         0x62    //  Dec -> 098 ('b')
#define ASCII_c                         0x63    //  Dec -> 099 ('c')
#define ASCII_d                         0x64    //  Dec -> 100 ('d')
#define ASCII_e                         0x65    //  Dec -> 101 ('e')
#define ASCII_f                         0x66    //  Dec -> 102 ('f')
#define ASCII_g                         0x67    //  Dec -> 103 ('g')
#define ASCII_h                         0x68    //  Dec -> 104 ('h')
#define ASCII_i                         0x69    //  Dec -> 105 ('i')
#define ASCII_j                         0x6A    //  Dec -> 106 ('j')
#define ASCII_k                         0x6B    //  Dec -> 107 ('k')
#define ASCII_l                         0x6C    //  Dec -> 108 ('l')
#define ASCII_m                         0x6D    //  Dec -> 109 ('m')
#define ASCII_n                         0x6E    //  Dec -> 110 ('n')
#define ASCII_o                         0x6F    //  Dec -> 111 ('o')
#define ASCII_p                         0x70    //  Dec -> 112 ('p')
#define ASCII_q                         0x71    //  Dec -> 113 ('q')
#define ASCII_r                         0x72    //  Dec -> 114 ('r')
#define ASCII_s                         0x73    //  Dec -> 115 ('s')
#define ASCII_t                         0x74    //  Dec -> 116 ('t')
#define ASCII_u                         0x75    //  Dec -> 117 ('u')
#define ASCII_v                         0x76    //  Dec -> 118 ('v')
#define ASCII_w                         0x77    //  Dec -> 119 ('w')
#define ASCII_x                         0x78    //  Dec -> 120 ('x')
#define ASCII_y                         0x79    //  Dec -> 121 ('y')
#define ASCII_z                         0x7A    //  Dec -> 122 ('z')
    
#define ASCII_DEL                       0x7F    //  Dec -> 127          <DEL>
//======================================================================

//======================================================================
//  INIT STATE STEP
#define INIT_IDLE                       0x00
#define INIT_POWER_ON                   0x01
#define INIT_POWER_OFF                  0x02
#define INIT_RESET                      0x03
#define INIT_CMD_AT                     0x04
#define INIT_CMD_ATF0                   0x05
#define INIT_CMD_ATE0                   0x06
#define INIT_CMD_ATQ0                   0x07
#define INIT_CMD_ATV1                   0x08
#define INIT_CMD_CMEE                   0x09
#define INIT_CMD_IPR                    0x0A
#define INIT_CMD_ICF                    0x0B
#define INIT_CMD_IFC                    0x0C
#define INIT_CMD_CPINQ                  0x0D
#define INIT_CMD_SPIC                   0x0E
#define INIT_CMD_CPIN                   0x0F
#define INIT_CMD_CALM                   0x10
#define INIT_CMD_CRSL                   0x11
#define INIT_CMD_CLVL                   0x12
#define INIT_CMD_CMIC                   0x13
#define INIT_CMD_CRC                    0x14
#define INIT_CMD_CLIP                   0x15
#define INIT_CMD_CMGF                   0x16
#define INIT_CMD_CSCS                   0x17
#define INIT_CMD_CNMI                   0x18
#define INIT_CMD_CPMS                   0x19
#define INIT_CMD_CSMP                   0x1A
#define INIT_CMD_CPBS                   0x1B
#define INIT_CMD_CREG                   0x1C
#define INIT_CMD_ATW0                   0x1D

#define INIT_WAIT_IDLE                  0x00
#define INIT_WAIT_ANSWER_POWER_ON       0x01
#define INIT_WAIT_ANSWER_POWER_OFF      0x02
#define INIT_WAIT_ANSWER_RESET          0x03
#define INIT_WAIT_ANSWER_CMD_AT         0x04
#define INIT_WAIT_ANSWER_CMD_ATF0       0x05
#define INIT_WAIT_ANSWER_CMD_ATE0       0x06
#define INIT_WAIT_ANSWER_CMD_ATQ0       0x07
#define INIT_WAIT_ANSWER_CMD_ATV1       0x08
#define INIT_WAIT_ANSWER_CMD_CMEE       0x09
#define INIT_WAIT_ANSWER_CMD_IPR        0x0A
#define INIT_WAIT_ANSWER_CMD_ICF        0x0B
#define INIT_WAIT_ANSWER_CMD_IFC        0x0C
#define INIT_WAIT_ANSWER_CMD_CPINQ      0x0D
#define INIT_WAIT_ANSWER_CMD_SPIC       0x0E
#define INIT_WAIT_ANSWER_CMD_CPIN       0x0F
#define INIT_WAIT_ANSWER_CMD_CALM       0x10
#define INIT_WAIT_ANSWER_CMD_CRSL       0x11
#define INIT_WAIT_ANSWER_CMD_CLVL       0x12
#define INIT_WAIT_ANSWER_CMD_CMIC       0x13
#define INIT_WAIT_ANSWER_CMD_CRC        0x14
#define INIT_WAIT_ANSWER_CMD_CLIP       0x15
#define INIT_WAIT_ANSWER_CMD_CMGF       0x16
#define INIT_WAIT_ANSWER_CMD_CSCS       0x17
#define INIT_WAIT_ANSWER_CMD_CNMI       0x18
#define INIT_WAIT_ANSWER_CMD_CPMS       0x19
#define INIT_WAIT_ANSWER_CMD_CSMP       0x1A
#define INIT_WAIT_ANSWER_CMD_CPBS       0x1B
#define INIT_WAIT_ANSWER_CMD_CREG       0x1C
#define INIT_WAIT_ANSWER_CMD_ATW0       0x1D
//======================================================================

//======================================================================
//  SEND CMD STATE STEP
#define CMD_IDLE                        0x00

#define CMD_WAIT_IDLE                   0x00
#define WAIT_ANSWER_CMD_GENERIC         0x01
#define WAIT_ANSWER_CMD_AT_CFUN         0x02
#define WAIT_ANSWER_CMD_ATQ_COPS        0x03
#define WAIT_ANSWER_CMD_ATQ_CPAS        0x04
#define WAIT_ANSWER_CMD_ATQ_GMI         0x05
#define WAIT_ANSWER_CMD_ATQ_CGMI        0x06
#define WAIT_ANSWER_CMD_ATQ_GMM         0x07
#define WAIT_ANSWER_CMD_ATQ_CGMM        0x08
#define WAIT_ANSWER_CMD_ATQ_GMR         0x09
#define WAIT_ANSWER_CMD_ATQ_CGMR        0x0A
#define WAIT_ANSWER_CMD_ATQ_GSN         0x0B
#define WAIT_ANSWER_CMD_ATQ_CGSN        0x0C
#define WAIT_ANSWER_CMD_ATQ_CSQ         0x0D
#define WAIT_ANSWER_CMD_ATQ_CREG        0x0E
#define WAIT_ANSWER_CMD_PW_DOWN         0x0F
#define WAIT_ANSWER_CMD_AT_CALS         0x10
#define WAIT_ANSWER_CMD_AT_CALM         0x11
#define WAIT_ANSWER_CMD_AT_CRSL         0x12
#define WAIT_ANSWER_CMD_AT_CLVL         0x13
#define WAIT_ANSWER_CMD_AT_CCLK         0x14
#define WAIT_ANSWER_CMD_AT_CMIC         0x15
//======================================================================

//======================================================================
//  Summary of some CME ERROR CODE
#define CME_PHONE_FAILURE               0x0000  // 000 -> Phone Failure
#define CME_NO_CONN_PHONE               0x0001  // 001 -> No conncetion to Phone
#define CME_PHONE_LINK_RESERVED         0x0002  // 002 -> Phone-Adaptor link reserved
#define CME_NOT_ALLOWED                 0x0003  // 003 -> Operation not allowed
#define CME_NOT_SUPPORTED               0x0004  // 004 -> Operation not supported
#define CME_PH_SIM_PIN_REQ              0x0005  // 005 -> PH-SIM PIN required
#define CME_PH_FSIM_PIN_REQ             0x0006  // 006 -> PH-FSIM PIN required
#define CME_PH_FSIM_PUK_REQ             0x0007  // 007 -> PH-FSIM PUK required
    
#define CME_SIM_NOT_INSERTED            0x000A  // 010 -> SIM not inserted
#define CME_SIM_PIN_REQ                 0x000B  // 011 -> SIM PIN required  
#define CME_SIM_PUK_REQ                 0x000C  // 012 -> SIM PUK required
#define CME_SIM_FAILURE                 0x000D  // 013 -> SIM failure
#define CME_SIM_BUSY                    0x000E  // 014 -> SIM busy
#define CME_SIM_WRONG                   0x000F  // 015 -> SIM wrong
#define CME_INCORRECT_PSW               0x0010  // 016 -> SIM Incorrect PIN/PASSWORD
#define CME_SIM_PIN2_REQ                0x0011  // 017 -> SIM PIN2 required
#define CME_SIM_PUK2_REQ                0x0012  // 018 -> SIM PUK2 required
    
#define CME_MEMORY_FULL                 0x0014  // 020 -> Memory full
#define CME_INVALID_INDEX               0x0015  // 021 -> Invalid index
#define CME_NOT_FOUND                   0x0016  // 022 -> Not found
#define CME_MEMORY_FAILURE              0x0017  // 023 -> Memory failure
#define CME_TEXT_STR_TOO_LONG           0x0018  // 024 -> Text string too long
#define CME_INVALID_TEXT_CHAR           0x0019  // 025 -> Invalid characters in text string
#define CME_DIAL_STR_TOO_LONG           0x001A  // 026 -> Dial string too long
#define CME_INVALID_DIAL_CHAR           0x001B  // 027 -> Invalid characters in dial string
    
#define CME_NO_NETWORK_SERVICE          0x001E  // 030 -> No network service
#define CME_NETWORK_TIMOUT              0x001F  // 031 -> Network timeout
#define CME_NETWORK_NOT_ALLOWED         0x0020  // 032 -> Network not allowed - emergency call only
    
#define CME_INVALID_PARAMETERS          0x0035  // 053 -> Parameters are invalid
    
#define CME_UNKNOWN                     0x0064  // 100 -> Unknown
    
#define CME_ILLEGAL_MS                  0x0067  // 103 -> Illegal MS
    
#define CME_ILLEGAL_ME                  0x006A  // 106 -> Illegal ME
#define CME_GPRS_NOT_ALLOWED            0x006B  // 107 -> GPRS services not allowed
    
#define CME_UNSPEC_GPRS_ERROR           0x0094  // 148 -> Unspecified GPRS error
#define CME_PDP_FAILUR                  0x0095  // 149 -> PDP authentication failure

#define CME_INVALID_SIM_CMD             0x02F2  // 754 -> Invalid SIM command
    
#define CME_INVALID_INPUT_VALUE_        0x02FD  // 765 -> Invalid input value
#define CME_UNSUPPORTED_MODE            0x02FE  // 766 -> Unsupported mode
#define CME_OPERATION_FAILED            0x02FF  // 767 -> Operatoion failed
    
#define CME_SIM_NETWORK_REJECTED        0x0302  // 770 -> SIM network rejected
#define CME_CALL_SETUP_INPROGRESS       0x0303  // 771 -> Call Setup Inprogress

#define CME_SIM_POWERED_DOWN            0x0304  // 772 -> SIM powered down
#define CME_SIM_FILE_NOT_PRESENT        0x0305  // 773 -> SIM powered down

#define CME_INVALID_INPUT_VALUE         0x0EB5  // 3765 -> Invalid input value
//======================================================================

//======================================================================
//  Summary of some CMS ERROR CODE
#define CMS_BEARER_CAPABILITY           0x003A  // 058 -> Bearer capability not presently available
    
#define CMS_ME_FAILURE                  0x012C  // 300 -> ME Failure
#define CMS_SMS_RESERVED                0x012D  // 301 -> SMS Reserved ME
#define CMS_NOT_ALLOWED                 0x012E  // 302 -> Operation not allowed
#define CMS_NOT_SUPPORTED               0x012F  // 303 -> Operation not supported
#define CMS_INVALID_PDU_MODE            0x0130  // 304 -> Invalid PDU mode
#define CMS_INVALID_TEXT_MODE           0x0131  // 305 -> Invalid TEXT mode
    
#define CMS_SIM_NOT_INSERTED            0x0136  // 310 -> SIM not inserted
#define CMS_SIM_PIN_NECESSARY           0x0137  // 311 -> SIM PIN necessary
#define CMS_PH_SIM_PIN_NECESSARY        0x0138  // 312 -> PH-SIM PIN necessary
#define CMS_SIM_FAILURE                 0x0139  // 313 -> SIM failure
#define CMS_SIM_BUSY                    0x013A  // 314 -> SIM busy
#define CMS_SIM_WRONG                   0x013B  // 315 -> SIM wrong
#define CMS_SIM_PUK_REQ                 0x013C  // 316 -> SIM PUK required
#define CMS_SIM_PIN2_REQ                0x013D  // 317 -> SIM PIN2 required
#define CMS_SIM_PUK2_REQ                0x013E  // 318 -> SIM PUK2 required
    
#define CMS_MEMORY_FAILURE              0x0140  // 320 -> Memory failure
#define CMS_INVALID_INDEX               0x0141  // 321 -> Invalid memory index
#define CMS_MEMORY_FULL                 0x0142  // 322 -> Memory full
#define CMS_INVALID_INPUT_PARAMETER     0x0143  // 323 -> Invalid input parameter
#define CMS_INVALID_INPUT_FORMAT        0x0144  // 324 -> Invalid input format
#define CMS_INVALID_INPUT_VALUE         0x0145  // 325 -> Invalid input value
    
#define CMS_SMSC_ADD_UNKNOWN            0x014A  // 330 -> SMSC address unknown
#define CMS_NO_NETWORK                  0x014B  // 331 -> No network
#define CMS_NETWORK_TIMEOUT             0x014C  // 332 -> Network timeout
    
#define CMS_NO_CNMA_ACK                 0x0154  // 340 -> No CNMA ack
    
#define CMS_UNKNOWN                     0x01F4  // 500 -> Unknown
    
#define CMS_SIM_NOT_READY               0x0200  // 512 -> SIM not ready
#define CMS_UNREAD_RECORDS_ON_SIM       0x0201  // 513 -> Unread records on SIM
    
#define CMS_GPRS_SUSPENDED              0x0212  // 530 -> GPRS is suspended
    
#define XXX_ERR_2069                    0x0815  // 2069
#define XXX_ERR_2076                    0x081C  // 2076
#define XXX_ERR_2086                    0x0826  // 2086 -> Network Out Of Order
#define XXX_ERR_2098                    0x0832  // 2098 -> Out of phone credit
#define XXX_ERR_49155                   0xC003  // 49155
//======================================================================

class GenericCmd_GSM {
    
public:
    //-------------------------------------------------------------------------------------
    struct {
        uint16_t StartAddPin1Code;
        uint16_t StartAddPin2Code;
        uint16_t StartAddPhPinCode;
        uint16_t StartAddPuk1Code;
        uint16_t StartAddPuk2Code;
        uint16_t StartAddPhPukCode;
        uint16_t StartAddShortPswdCode;
        uint16_t StartAddLongPswdCode;
    } PswdEepromAdd;
    void PswdEepromStartAddSetup(void);
    //-------------------------------------------------------------------------------------
    
    void    InitPowerON_GSM(void);
    void    InitReset_GSM(void);
    uint8_t SetCmd_SwPowerDown(uint8_t Type);
    uint8_t SetCmd_HwPowerDown(void);
    uint8_t SetCmd(uint8_t Lenght);
    uint8_t SetCmd_AT_CFUN(uint8_t FUN_Code);
    uint8_t SetCmd_AT_CALS(uint8_t n, uint8_t TestAlertSound);
    uint8_t SetCmd_AT_CALM(uint8_t Mode, uint8_t SmsMode);
    uint8_t SetCmd_AT_CRSL(uint8_t SoundLevel);
    uint8_t SetCmd_AT_CLVL(uint8_t VolumeLevel);
    uint8_t SetCmd_AT_CMIC(uint8_t Channel, uint8_t GainLevel);
    
    uint8_t SetCmd_ATQ_CPAS(void);
    uint8_t SetCmd_ATQ_COPS(void);
    //------------------------------------------------------------------------------------- 
    uint8_t SetCmd_ATQ_CSQ(void);
    struct {
        uint8_t Rssi;
        uint8_t Ber;
    } CSQ_Info; 
    //-------------------------------------------------------------------------------------
        
    //-------------------------------------------------------------------------------------
    uint8_t SetCmd_ATQ_CREG(void);  
    struct {
        union Flag {
            uint8_t  FlagByte;
            struct {
                uint8_t n       :2;     //  "0": Disable network registration unsollecited result code
                                        //  "1": Enable network registration unsollecited result code +CREG:<stat>
                                        //  "2": Enable network registration unsollecited result code with location information +CREG:<stat>[,<lac>,<ci>]
                uint8_t Stat    :3;     //  "0": Not registered, MT is not currently searching a new operator to register to
                                        //  "1": Registered, home network
                                        //  "2": Not registered, but MT is currently searching a nesw operator to register to
                                        //  "3": Registration denied
                                        //  "4": Unknown
                                        //  "5": Registered, roaming
                uint8_t Free    :3;     //  Free
            } Bit;  
        } Flag;         
        uint8_t LAC_Info[7];
        uint8_t CI_Info[7];
    } CREG_Info;                            
    //-------------------------------------------------------------------------------------
    
    //-------------------------------------------------------------------------------------
    uint8_t SetCmd_ATQ_GMI(uint8_t FormatAt);
    uint8_t SetCmd_ATQ_GMM(uint8_t FormatAt);
    uint8_t SetCmd_ATQ_GMR(uint8_t FormatAt);
    uint8_t SetCmd_ATQ_GSN(uint8_t FormatAt);
    struct {
        uint8_t GMI_Info[16];
        uint8_t GMM_Info[16];
        uint8_t GMR_Info[48];
        uint8_t IMEI_Info[16];
    } ME_Info;
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------
    uint8_t SetCmd_AT_CCLK(bool Query);
    struct {
        union Flag {
            uint64_t  ClockLong;
            struct {
                uint8_t Year    :7;     //  Year:        00  -  99
                uint8_t Month   :4;     //  Month:       01  -  12
                uint8_t Day     :5;     //  Day:         01  -  31
                uint8_t Hours   :5;     //  Hour:        00  -  23
                uint8_t Minutes :6;     //  Minutes:     00  -  59
                uint8_t Seconds :6;     //  Minutes:     00  -  59
                int8_t  GMT     :7;     //  GMT:        -47 .. +48
                uint8_t Free    :24;
            } Bit;
        } Clock;
    } Clock_Info;
    //-------------------------------------------------------------------------------------
    
    //------------------------------------------------------------------------------------- 
    uint16_t GsmErrorCode;
    uint8_t  AlignBaudRateSelector;
    uint8_t  RetryCounter;
    uint8_t  StateInitSendCmd;
    uint8_t  StateSendCmd;
    uint8_t  StateInitWaitAnswerCmd;
    uint8_t  StateWaitAnswerCmd;
    uint8_t  TempStringCompare[32];
    uint8_t  CharPointers[16];
    uint8_t  OperatorName[32];
    uint8_t  BckCmdData[4];                 //  Freeze parameter used in cmd functions
    
    union GsmFlag {
        uint32_t  GsmWord;
        struct {
            uint8_t GsmInitInProgress       :1;     //  "1": GSM Init in progress
            uint8_t GprsInitInProgress      :1;     //  "1": GPRS Init in progress
            uint8_t HttpInitInProgress      :1;     //  "1": HTTP Init in progress
            uint8_t GsmSendCmdInProgress    :1;     //  "1": GSM Send command in progress
            uint8_t OperationNotAllowed     :1;     //  "1": Executed invalid operation
            uint8_t OperationNotSupported   :1;     //  "1": Operation not supported
            uint8_t UnknownError            :1;     //  "1": Unknown error 
            uint8_t ErrorWithoutCode        :1;     //  "1": Error without code
            uint8_t AT_CmdAutoBaudDetect    :1;     //  "1": Sends AT command to autobauding
            uint8_t AlignBaudRateProcess    :1;     //  "1": Uart BaudRate alignement in progress
            uint8_t GsmPowerOn              :1;     //  "1": Performed Hardware Power ON
            uint8_t GsmPowerOff             :1;     //  "1": Performed Hardware Power OFF
            uint8_t GsmReset                :1;     //  "1": Performed Hardware Reset
            uint8_t GsmModuleOff            :1;     //  "1": GSM Module OFF
            uint8_t CringOccurred           :1;     //  "1": Cring Occurred
            uint8_t SmsReceived             :1;     //  "1": Received new SMS
            uint8_t IncomingCall            :1;     //  "1": Incoming Call
            uint8_t GsmNoAnswer             :1;     //  "1": GSM no answer
            uint8_t Free_1                  :6;     //  Free Bits
            uint8_t NoAutoResetGsmError     :1;     //  "1": Disable Auto Reset GSM Error
            uint8_t Free_2                  :4;     //  Free Bits
            uint8_t UartTimeOutSelect       :3;     //  "000" -> 250mSec TimeOut
                                                    //  "001" -> 1   Sec TimeOut
                                                    //  "010" -> 2,5 Sec TimeOut
                                                    //  "011" -> 5   Sec TimeOut
                                                    //  "100" -> 10  Sec TimeOut
                                                    //  "101" -> 15  Sec TimeOut
                                                    //  "110" -> 20  Sec TimeOut
                                                    //  "111" -> 25  Sec TimeOut
        } Bit;  
    } GsmFlag;

    union SimFlag {
        uint16_t  SimWord;
        struct {
            uint8_t SIM_NotInserted         :1;     //  "1": SIM not inserted
            uint8_t SIM_Wrong               :1;     //  "1": SIM wrong
            uint8_t SIM_Failure             :1;     //  "1": SIM failure
            uint8_t SIM_Busy                :1;     //  "1": SIM busy
            uint8_t SIM_NotReady            :1;     //  "1": SIM not ready
            uint8_t OperatorSelMode         :3;     //  "0": Automatic Mode;<oper> field ignored
                                                    //  "1": Manual Mode;<oper> field shall be present, and <AcT> optional
                                                    //  "4": Manual/Automatic (<Oper> field shall be present); if manual selection fails automatic mode is entered
            uint8_t OperatorSelFormat       :2;     //  "0": Long format alphanumeric <oper>
                                                    //  "1": Short format alphanumeric <oper>
                                                    //  "2": Numeric <oper>; GSM Location Area Identification number
            uint8_t PhoneActivityStatus     :3;     //  "0": Ready (MT allows commands from TA/TE)
                                                    //  "2": Unknown (MT is not guaranteed to respond to instructions)
                                                    //  "3": Ringing (MT is ready for commands from TA/TE, but the ringing is active)
                                                    //  "4": Call in progress (MT is ready for commands from TA/TE, but a calla is in progress)
            uint8_t SIM_OutOfPhoneCredit    :1;     //  "1": Out of phone credit
            uint8_t Free                    :2;     //  Free
        } Bit;  
    } SimFlag;
    //-------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------- 
    uint8_t  TestAT_Cmd_Answer(uint8_t *FlashPointer, uint8_t *BufferPointer, uint8_t BufferSize);
    void     ReadStringFLASH(uint8_t *FlashPointer, uint8_t *BufferPointer, uint8_t BufferSize);
    uint8_t  FindColonCommaCarriageRet(void);
    uint8_t  FindBracketMinus(void);
    uint16_t MyPow(uint16_t x, uint8_t y);
    uint16_t ExtractParameterByte(uint8_t Counter, uint8_t Mul);
    void     ExtractCharArray(uint8_t *ArrayPointerOut, uint8_t *ArrayPointerIn, uint8_t ArrayOutLenght, uint8_t StopChar);
    uint8_t  CountDigit(uint8_t Number);    
    uint8_t  ConvertNumberToChar(uint16_t Number, uint8_t *ArrayStartIndex, uint8_t HowManyDigit);
    //-------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------- 
    void     InitGsmSendCmd(void);              //  Send init command (Init mode only)
    void     InitGsmWaitAnswer(void);           //  Wait init answer (Init mode only)
    void     GsmSendCmd(void);                  //  Send generic command (Normal mode only)
    void     GsmGenericWaitAnswer(void);        //  Wait generic answer (Normal mode only)
    void     ProcessGsmError(void);             //  GSM error management
    void     ProcessUnsolicitedCode(void);      //  Process Unsolicited result code
    void     GsmAnswerStateProcess(void);
    uint8_t  CommandsClassState;                //  If "0" process generic command at answer
                                                //  If "1" process security command at answer
                                                //  If "2" process phonebook command at answer
                                                //  If "3" process sms command at answer
                                                //  If "4" process phonic call command at answer
    
    void     StartSendData(uint8_t IdleState, uint8_t WaitState, uint8_t ProcessStateClass);    //  Start process to send AT CMD
    void     GsmRetrySendCmd(void);                                                             //  Process to retry send AT CMD
    void     ResetAllFlags(void);                   //  Reset All Flags
    void     ResetGsmFlags(void);                   //  Reset Gsm Flags Only
    void     ResetSecurityFlags(void);              //  Reset Security Flags Only
    void     ResetPhoneBookFlags(void);             //  Reset PhoneBook Flags Only
    void     ResetSmsFlags(void);                   //  Reset Sms Flags Only
    void     ResetPhonicCallFlags(void);            //  Reset Phonic Call Flags Only
    void     ResetGprsFlags(void);                  //  Reset Gprs Flags Only
    void     ResetHttpFlags(void);                  //  Reset Http Flags Only
    void     ResetTcpIpFlags(uint8_t IndexConn);    //  Reset TcpIp Flags Only
    //-------------------------------------------------------------------------------------

private:
    void On_GSM(void);
    void Off_GSM(void);
    void Reset_GSM(void);
    
    void InitAligneBaudRate_GSM(void);
    void CallAutoBaudeFunctions(void);
    
    void ErrorCodeStringConverter(uint8_t Pointer);
};

#endif