/*********************************************************************
 *
 *       I/O Board configuration
 *
 *********************************************************************
 * FileName:        IO_GSM.h
 * Revision:        1.0.0
 * Date:            08/05/2016
 *
 * Revision:        1.1.0
 *                  02/11/2018
 *                  - Added new directive "EARPHONES_JACK" used during GSM engine initialization. This directive is
 *                    necessary to discern which audio connector to use. If directive is active then the user can
 *                    connect your headset to the JK1 connector, otherwise the user can connect a microphone capsule
 *                    and a speaker to the connector CN4. This connection is in differential mode
 *                  - Added new directive "HARDWARE_13" that identify the hardware R.1.3. The new hardware R.1.3 is
 *                    similar but not equal to hardware R.1.2. The differences regard the audio interface
 *                  - Added new directive "ENABLE_ANSWER_TCP_IP_AT_CMD_STATE"
 *
 * Revision:        1.2.0
 *                  18/11/2019
 *                  - Change labels "HARDWARE_10", "HARDWARE_12"  and "HARDWARE_13"  in "HARDWARE_GSM_10", "HARDWARE_GSM_12" and "HARDWARE_GSM_13" 
 *                  - Change labels "SOFTWARE_UART1" and "HARDWARE_UART1" in "SOFTWARE_UART1_GSM" and "HARDWARE_UART1_GSM"
 * 
 * Revision:        1.3.0
 *                  18/01/2020
 *                  - Removed AI Thinker a9 support
 *                  - Added Quectel BC68 or BC95-G IOT support
 *
 * Dependencies:    Arduino.h
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

#ifndef _IO_GSM_H_INCLUDED__
#define _IO_GSM_H_INCLUDED__

#include "Arduino.h"

//  ======================================================================================================================================================
// |                    Function                                  |  JP1  |  JP2  |  JP3  |  JP4  |  JP5  |  JP6  |  JP7  |  JP8  |  JP9  |  JP10 |  JP11 |
//  ======================================================================================================================================================
// |                                                              |                                                                                       |       
// |               Arduino jumpers setting                        |                                                                                       |
// |                                                              |                                                                                       |
//  ======================================================================================================================================================
// | GSM -> RXD1 and TXD1 Software (PC spy NO)                    |   X   |   X   |   X   |  2-3  |  2-3  |  1-2  |  1-2  |   X   |   X   |   X   |   X   |
//  ======================================================================================================================================================
// | GSM -> RXD1 and TXD1 Software (PC spy YES)                   |  1-2  |  1-2  |  1-2  |  1-2  |  1-2  |  1-2  |  1-2  |   X   |   X   |   X   |   X   |
//  ======================================================================================================================================================
// | GSM -> RXD1 and TXD1 Hardware (PC spy NO)                    |   X   |   X   |   X   |  2-3  |  2-3  |  2-3  |  2-3  |   X   |   X   |   X   |   X   |
//  ======================================================================================================================================================
// | GSM -> RXD1 and TXD1 Hardware (PC spy YES)                   |  1-2  |  1-2  |  1-2  |  1-2  |  1-2  |  2-3  |  2-3  |   X   |   X   |   X   |   X   |
//  ======================================================================================================================================================
//                                                                                                                                                        |
//  ======================================================================================================================================================
// | GPS -> RXD2 and TXD2 Software (PC spy NO)                    |   X   |   X   |   X   |  2-3  |  2-3  |   X   |   X   |  1-2  |  1-2  |   X   |   X   |
//  ======================================================================================================================================================
// | GPS -> RXD2 and TXD2 Software (PC spy YES)                   |  2-3  |  2-3  |  2-3  |  1-2  |  1-2  |   X   |   X   |  1-2  |  1-2  |   X   |   X   |
//  ======================================================================================================================================================
// | GPS -> RXD2 and TXD2 Hardware (PC spy NO)                    |   X   |   X   |   X   |  2-3  |  2-3  |   X   |   X   |  2-3  |  2-3  |   X   |   X   |
//  ======================================================================================================================================================
// | GPS -> RXD2 and TXD2 Hardware (PC spy YES)                   |  2-3  |  2-3  |  2-3  |  1-2  |  1-2  |   X   |   X   |  2-3  |  2-3  |   X   |   X   |
//  ======================================================================================================================================================
// |                                                              |                                                                                       |       
// |           RaspberryPi jumpers setting                        |                                                                                       |
// |                                                              |                                                                                       |
//  ======================================================================================================================================================
// | GSM -> RX and TX (PC spy NO)                                 |   X   |   X   |   X   |  2-3  |  2-3  |   X   |   X   |   X   |   X   |  1-2  |  1-2  |
//  ======================================================================================================================================================
// | GSM -> RX and TX (PC spy YES)                                |  1-2  |  1-2  |  1-2  |  1-2  |  1-2  |   X   |   X   |   X   |   X   |  1-2  |  1-2  |
//  ======================================================================================================================================================
//                                                                                                                                                        |
//  ======================================================================================================================================================
// | GPS -> RXD and TXD (PC spy NO)                               |   X   |   X   |   X   |  2-3  |  2-3  |   X   |   X   |   X   |   X   |  2-3  |  2-3  |
//  ======================================================================================================================================================
// | GPS -> RXD and TXD (PC spy YES)                              |  2-3  |  2-3  |  2-3  |  1-2  |  1-2  |   X   |   X   |   X   |   X   |  2-3  |  2-3  |
//  ======================================================================================================================================================
// |                                                              |                                                                                       |       
// |         AT commands from PC - jumpers setting                |                                                                                       |
// |                                                              |                                                                                       |
//  ======================================================================================================================================================
// | GSM                                                          |   X   |  1-2  |  1-2  |  2-3  |  2-3  |   X   |   X   |   X   |   X   |   X   |   X   |
//  ======================================================================================================================================================
// | GPS                                                          |   X   |  2-3  |  2-3  |  2-3  |  2-3  |   X   |   X   |   X   |   X   |   X   |   X   |
//  ======================================================================================================================================================

//======================================================================
//  Selects which GSM engine you want to use in your application
//#define SIMCOM_SIM800C          //  Use this for SIMCOM  SIM800C GSM module
//#define SIMCOM_SIM900         //  Use this for SIMCOM  SIM900  GSM module
//#define SIMCOM_SIM928A        //  Use this for SIMCOM  SIM928A GSM module
//#define FIBOCOM_G510          //  Use this for FIBOCOM G510    GSM module
#define QUECTEL_M95           //  Use this for QUECTEL M95     GSM module
//#define QUECTEL_BC68_BC95G    //  Use this for QUECTEL BC68 or BC95-G IOT module
//======================================================================

//======================================================================
//  Selects Arduino Board
//#define ARDUINO_UNO_REV3
#define ARDUINO_MEGA2560_REV3
//======================================================================

//======================================================================
//  Selects the shield HARDWARE Revision
//#define HARDWARE_GSM_10       //  Board Hardware R.1.0    // OBSOLETE. NOT USE IT
//#define HARDWARE_GSM_12       //  Board Hardware R.1.2
#define HARDWARE_GSM_13     //  Board Hardware R.1.3
//======================================================================

//======================================================================
//  Selects the Hardware or Software serial port
#define SOFTWARE_UART1_GSM
//#define HARDWARE_UART1_GSM
//======================================================================

//======================================================================
//  Selects audio interface. Available with hardware R.1.3 only
#ifdef HARDWARE_GSM_13
//  #define EARPHONES_JACK      //  Set this if you want to use your earphone connected to JK1 jack. Otherwise is intended to use differential connection for microphone and speaker (Connector CN4)
#endif
//======================================================================

//========================================================
//  No longer used. Hardware 1.0 is obsolete
#ifdef HARDWARE_GSM_10
    #define GSM_ON_OFF  3           //  Signal used to power ON or Power OFF the GSM module
    #define GSM_RESET   4           //  Signal used to reset the GSM module

    #ifdef ARDUINO_UNO_REV3
        #define PIN_TX1_SW  A3      //  UART1 SOFTWARE TX   (GSM  AT CMD)
        #define PIN_RX1_SW  A1      //  UART1 SOFTWARE RX   (GSM  AT CMD)
        #define PIN_TX2_SW  A2      //  UART2 SOFTWARE TX   (GPS  AT CMD)
        #define PIN_RX2_SW  A0      //  UART2 SOFTWARE RX   (GPS  AT CMD)
    #endif

    #ifdef ARDUINO_MEGA2560_REV3
        #define PIN_TX1_SW  A3      //  UART1 SOFTWARE TX   (GSM  AT CMD)   // NOTE: ERROR, This Pin isn't a "PORT CHANGE PIN". With the hardware revison 1.0 is not possible to use the UART software library with ArduinoMega 2560
        #define PIN_RX1_SW  A1      //  UART1 SOFTWARE RX   (GSM  AT CMD)   // NOTE: ERROR, This Pin isn't a "PORT CHANGE PIN". With the hardware revison 1.0 is not possible to use the UART software library with ArduinoMega 2560
        #define PIN_TX2_SW  A2      //  UART2 SOFTWARE TX   (GPS  AT CMD)   // NOTE: ERROR, This Pin isn't a "PORT CHANGE PIN". With the hardware revison 1.0 is not possible to use the UART software library with ArduinoMega 2560
        #define PIN_RX2_SW  A0      //  UART2 SOFTWARE RX   (GPS  AT CMD)   // NOTE: ERROR, This Pin isn't a "PORT CHANGE PIN". With the hardware revison 1.0 is not possible to use the UART software library with ArduinoMega 2560
        #define PIN_TX1_HW  18      //  UART1 HARDWARE TX   (GSM  AT CMD)
        #define PIN_RX1_HW  19      //  UART1 HARDWARE RX   (GSM  AT CMD)
        #define PIN_TX2_HW  16      //  UART2 HARDWARE TX   (GPS  AT CMD)
        #define PIN_RX2_HW  17      //  UART2 HARDWARE RX   (GPS  AT CMD)   
    #endif
        
    #define TRIGGER_1   10          //  Trigger Used during debug
    #define TRIGGER_2   11          //  Trigger Used during debug
    #define TRIGGER_3   12          //  Trigger Used during debug
#endif
//======================================================================

//======================================================================
//  Hardware R.1.2 or hardware R.1.3
#if defined(HARDWARE_GSM_12) || defined(HARDWARE_GSM_13)
    #define GSM_ON_OFF  5           //  Signal used to power ON or Power OFF the GSM module
    #define GSM_RESET   6           //  Signal used to reset the GSM module

    #ifdef ARDUINO_UNO_REV3         //  Arduino Uno Board or Fishino Uno board
        #define PIN_TX1_SW  A3      //  UART1 SOFTWARE TX
        #define PIN_RX1_SW  A1      //  UART1 SOFTWARE RX
        #define PIN_TX2_SW  A2      //  UART2 SOFTWARE TX
        #define PIN_RX2_SW  A0      //  UART2 SOFTWARE RX   
    #endif
    
    #ifdef ARDUINO_MEGA2560_REV3    //  Arduino Mega 2560 Board or Fishino Mega 2560 board
        #define PIN_TX1_SW  A11     //  UART1 SOFTWARE TX   (GSM  AT CMD)
        #define PIN_RX1_SW  A9      //  UART1 SOFTWARE RX   (GSM  AT CMD)
        #define PIN_TX2_SW  A10     //  UART2 SOFTWARE TX   (GPS  AT CMD)
        #define PIN_RX2_SW  A8      //  UART2 SOFTWARE RX   (GPS  AT CMD)
        #define PIN_TX1_HW  18      //  UART1 HARDWARE TX   (GSM  AT CMD)
        #define PIN_RX1_HW  19      //  UART1 HARDWARE RX   (GSM  AT CMD)
        #define PIN_TX2_HW  16      //  UART2 HARDWARE TX   (GPS  AT CMD)
        #define PIN_RX2_HW  17      //  UART2 HARDWARE RX   (GPS  AT CMD)
        
        #define PIN_LED4    37      //
        #define PIN_LED5    36      //
        #define PIN_LED6    35      //
        #define PIN_LED7    34      //
        #define PIN_LED8    33      //
        #define PIN_LED9    32      //
        
        #define PIN_DTR     31      //  Data Terminal Ready
        #define PIN_RTS     30      //  Request To Send
        #define PIN_CTS     A14     //  Clear To Send
        #define PIN_DCD     A15     //  Data Carrier Detect     
        
        #define PIN_GSM_LED A13     //  GSM Led PIN (Network Status Indicator). This line is active low.
                                    //   ===============================================
                                    //  | SIM900 GSM Module
                                    //   -----------------------------------------------
                                    //  | State                 | Module Function
                                    //   -----------------------------------------------
                                    //  | On                    | SIM900 is not running
                                    //   -----------------------------------------------
                                    //  | 64ms Off / 800ms On   | SIM900 not registered the network
                                    //   -----------------------------------------------
                                    //  | 64ms Off / 3000ms On  | SIM900 registered to the network
                                    //   ----------------------------------------------- 
                                    //  | 64ms Off / 300ms On   | SIM 900 GPRS communication is established
                                    //   ===============================================
                                    //
                                    //   ===============================================
                                    //  | M95 GSM Module
                                    //   -----------------------------------------------
                                    //  | State                 | Module Function
                                    //   -----------------------------------------------
                                    //  | On                    | M95 is not running
                                    //   -----------------------------------------------
                                    //  | 64ms Off / 800ms On   | M95 not registered the network
                                    //   -----------------------------------------------
                                    //  | 64ms Off / 2000ms On  | M95 registered to the network
                                    //   ----------------------------------------------- 
                                    //  | 64ms Off / 600ms On   | M95 GPRS communication is established
                                    //   ===============================================
                                    //
                                    //   ===============================================
                                    //  | G510 GSM Module
                                    //   -----------------------------------------------
                                    //  | State                 | Module Function
                                    //   -----------------------------------------------
                                    //  | On                    | G510 Off or Sleep mode
                                    //   -----------------------------------------------
                                    //  | 600ms On  / 600ms Off | G510 No Sim Card, SIM Pin, Register Network, Register Network Failure 
                                    //   -----------------------------------------------
                                    //  | 3000ms On / 75ms Off  | G510 Idle mode
                                    //   ----------------------------------------------- 
                                    //  | 75ms On   / 75ms Off  | G510 One or more GPRS context activated
                                    //   ----------------------------------------------- 
                                    //  | Off                   | Voice Call
                                    //   ===============================================                                    
    #endif
        
    #define TRIGGER_1   11          //  Trigger Used during debug
    #define TRIGGER_2   12          //  Trigger Used during debug
    #define TRIGGER_3   13          //  Trigger Used during debug
    
    #define PULSE_P3    9           //  P3 button, available only in board hardware R.1.1
    #define PULSE_P4    10          //  P4 button, available only in board hardware R.1.1
#endif
//======================================================================

//======================================================================
//  Enable State Machine Support
#define ENABLE_ANSWER_GENERIC_AT_CMD_STATE
#define ENABLE_ANSWER_SECURITY_AT_CMD_STATE
#define ENABLE_ANSWER_PHONEBOOK_AT_CMD_STATE
#define ENABLE_ANSWER_SMS_AT_CMD_STATE
#define ENABLE_ANSWER_PHONIC_CALL_AT_CMD_STATE
#define ENABLE_ANSWER_GPRS_AT_CMD_STATE
#define ENABLE_ANSWER_TCP_IP_AT_CMD_STATE
//#define ENABLE_ANSWER_HTTP_AT_CMD_STATE
//======================================================================

//======================================================================
//  Define STATE CODE FOR PROCESS AT COMMAND ANSWER (Each library has a univocal code)
#define ANSWER_GENERIC_AT_CMD_STATE         0   //  State used to process the AT generic commands answer
#define ANSWER_SECURITY_AT_CMD_STATE        1   //  State used to process the AT security commands answer
#define ANSWER_PHONEBOOK_AT_CMD_STATE       2   //  State used to process the AT phonebook commands answer
#define ANSWER_SMS_AT_CMD_STATE             3   //  State used to process the AT SMS commands answer
#define ANSWER_PHONIC_CALL_AT_CMD_STATE     4   //  State used to process the AT Phonic Call commands answer
#define ANSWER_GPRS_AT_CMD_STATE            5   //  State used to process the AT Gprs commands answer
#define ANSWER_TCP_IP_AT_CMD_STATE          6   //  State used to process the AT TCP/IP commands answer
//#define ANSWER_HTTP_AT_CMD_STATE            7   //  State used to process the AT Http commands answer

#define INIT_CMD_AT_NOTHING                 255
//======================================================================

//======================================================================
//  Debug mode directives. Comments the directives below to disable debug mode sections

//#define DEBUG_MODE        //  Master Debug Directive. If present activates debug mode
#ifdef DEBUG_MODE
    //#define STRING_COMPARE_DEBUG
    //#define UART_DEBUG
    //#define UART_FIND_BAUDRATE_DEBUG
    //#define GSM_AT_CMD_DEBUG
    //#define GSM_ON_OFF_CMD_DEBUG
    //#define GSM_SECURITY_DEBUG
    //#define GSM_ERROR_CODE
#endif

//  For debug codes are required abaout 2118 Byte. See above the "UART_DEBUG", "GSM_AT_CMD_DEBUG" etc. define.
//  If debug mode is not required disable the #define statement above. To do this comments the #define statement

//  Strings for Debug mode (MAX 32 Byte Lenght)
#ifdef UART_DEBUG
    const char STR_UART_IDLE_DEBUG[]            PROGMEM = "Uart SM: Idle\n\n\r";            //  SM -> "State Machine"
    const char STR_UART_SEND_DATA_DEBUG[]       PROGMEM = "Uart SM: Send Data\n\n\r";       //  SM -> "State Machine"
    const char STR_UART_WAIT_DATA_DEBUG[]       PROGMEM = "Uart SM: Wait Data\n\n\r";       //  SM -> "State Machine"
    const char STR_UART_BUFFER_OVERFLOW_DEBUG[] PROGMEM = "Software Uart Overflow\n\n\r";   //  If overflow occurred increase the "_SS_MAX_RX_BUFF" buffer lenght in the "SoftwareSerial.h" file    
#endif

#ifdef UART_FIND_BAUDRATE_DEBUG
    const char STR_UART_AT_AUTOBAUD_DEBUG[]     PROGMEM = "AT AutoBaud Command\n\r"; 
    const char STR_UART_FIND_BAUDRATE_DEBUG[]   PROGMEM = "Finding UART BaudRate: "; 
    const char STR_UART_BAUDRATE_2400_DEBUG[]   PROGMEM = "BaudRate -> 2400\n\r";
    const char STR_UART_BAUDRATE_4800_DEBUG[]   PROGMEM = "BaudRate -> 4800\n\r";
    const char STR_UART_BAUDRATE_9600_DEBUG[]   PROGMEM = "BaudRate -> 9600\n\r";
    const char STR_UART_BAUDRATE_19200_DEBUG[]  PROGMEM = "BaudRate -> 19200\n\r";
    const char STR_UART_BAUDRATE_38400_DEBUG[]  PROGMEM = "BaudRate -> 38400\n\r";
    const char STR_UART_BAUDRATE_57600_DEBUG[]  PROGMEM = "BaudRate -> 57600\n\r";
    const char STR_UART_BAUDRATE_115200_DEBUG[] PROGMEM = "BaudRate -> 115200\n\r";
#endif

#ifdef GSM_AT_CMD_DEBUG
    const char STR_GSM_SEP1[]                   PROGMEM = "**>>>>>>>>>>>>>>>>**\n\r";
    const char STR_GSM_SEP2[]                   PROGMEM = "**<<<<<<<<<<<<<<<<**\n\r";
    const char STR_GSM_SENT_DATA_DEBUG[]        PROGMEM = "GSM CMD: Sent data       -> ";
    const char STR_GSM_RECEIVED_DATA_DEBUG[]    PROGMEM = "GSM CMD: Received data   -> ";
    const char STR_GSM_RECEIVED_CHAR_DEBUG[]    PROGMEM = "Number of chars received -> ";
    const char STR_GSM_UNSOLICITED_DEBUG[]      PROGMEM = "Unsolicited Result Code  -> ";
#endif

#ifdef GSM_ON_OFF_CMD_DEBUG
    const char STR_PERFORM_POWER_ON_DEBUG[]     PROGMEM = "Performed HW Power ON\n\n\r";
    const char STR_PERFORM_POWER_OFF_DEBUG[]    PROGMEM = "Performed HW Power OFF\n\n\r";
    const char STR_PERFORM_RESET_DEBUG[]        PROGMEM = "Performed HW Reset\n\n\r";
#endif

#ifdef GSM_SECURITY_DEBUG   
    const char STR_SIM_READY_DEBUG[]            PROGMEM = "PIN READY\n\n\r";
    const char STR_SIM_PIN_DEBUG[]              PROGMEM = "SIM PIN REQ.\n\n\r";
    const char STR_SIM_PUK_DEBUG[]              PROGMEM = "PUK PIN REQ.\n\n\r";
    const char STR_PH_SIM_PIN_DEBUG[]           PROGMEM = "PH SIM PIN REQ.\n\n\r";
    const char STR_PH_SIM_PUK_DEBUG[]           PROGMEM = "PH SIM PUK REQ.\n\n\r";
    const char STR_SIM_PIN2_DEBUG[]             PROGMEM = "SIM PIN REQ.\n\n\r";
    const char STR_SIM_PUK2_DEBUG[]             PROGMEM = "PUK PIN REQ.\n\n\r";
#endif

#ifdef GSM_ERROR_CODE
    const char STR_ERROR_CODE_DEBUG[]           PROGMEM = "CMD AT ERROR: ";
#endif
//=======================================================================

class Io_GSM {
    
    public:
        void SetOutputLed(void);                    //  Sets Output Leds    Only HW R.1.2
        void SetOutputTrigger(void);                //  Sets Output Trigger

        void CheckOutputLed(void);                  //  Checks Output Leds  Only HW R.1.2
        void CheckOutputTrigger(void);              //  Checks Output Trigger
        
        void LedOn(uint8_t LedSelected);            //  Led ON              Only HW R.1.2
        void LedOff(uint8_t LedSelected);           //  Led OFF             Only HW R.1.2
        void TriggerOn(uint8_t TriggerSelected);
        void TriggerOff(uint8_t TriggerSelected);
        
        void LedBlink(uint8_t LedSelected, uint8_t DutyCycle, uint16_t TimeOutConstant);    //  Led Blink - Only HW R.1.2
        
    private:
        uint16_t LedBlinkAction(uint8_t LedSelected, uint16_t t_ON, uint16_t Timer, uint16_t TimeOutConstant);
};

extern Io_GSM Io;

#endif