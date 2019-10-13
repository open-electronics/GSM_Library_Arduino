/*********************************************************************
 *
 *       GSM Sketch - TDG133
 *
 *********************************************************************
 * FileName:        GSM_TDG133.ino
 *                  AT_CmdFunction.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  OutComingSmsVoc.ino
 *                  ProcessStringCmd.ino
 *                  SerialStringCmd.ino
 *                  TimersInt.ino
 *                  _SetupEeeprom.ino
 *                  
 * Revision:        1.0.0
 * Date:            31/01/2019
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  GprsCmd_GSM.h
 *                  TcpIpCmd_GSM.h
 *                  HttpCmd_GSM.h
 *
 * Arduino Board:   Arduino Mega 2560 or Fishino Mega 2560
 *                  See file Io_GSM.h (GSM Library) to define hardware and Arduino Board to use
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
 * ====================================================================
 * NOTES
 * ====================================================================
 * The code for this demo is splitted into nine files:
 *
 *  - GSM_TDG133       -> Main file project
 *  - AT_CmdFunction   -> Contains the code to manage generic commands AT and the structure to manage the GSM functions library
 *  - DigitalInput     -> Contains the code to manage the digital inputs and the respective functions to manage them
 *  - DigitalOutput    -> Contains the code to manage the digital outputs and the respective functions to manage them
 *  - OutComingSmsVoc  -> Contains the code to manage the SMS Outcoming and the Vocal Call Outcoming
 *  - ProcessStringCmd -> Contains the code to decode the string commands received from serial monitor or SMS
 *  - SerialStringCmd  -> Contains the code to manage string commands received from serial monitor
 *  - TimerInt         -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
 *                        With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
 *                        For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
 *  - _SetupEeprom     -> Contains the code to configure ATmega EEPROM with default parameters, string ecc
 
 **********************************************************************/

#include <avr/eeprom.h>
#include "Uart_GSM.h"
#include "Io_GSM.h"
#include "Isr_GSM.h"
#include "GenericCmd_GSM.h"
#include "SecurityCmd_GSM.h"
#include "PhoneBookCmd_GSM.h"
#include "SmsCmd_GSM.h"
#include "PhonicCallCmd_GSM.h"
#include "GprsCmd_GSM.h"
#include "TcpIpCmd_GSM.h"
#include "HttpCmd_GSM.h"

SecurityCmd_GSM   Security;
PhoneBookCmd_GSM  PhoneBook;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
Isr_GSM           Isr;
Io_GSM            Io;
GprsCmd_GSM       Gprs;
TcpIpCmd_GSM      TcpIp;
HttpCmd_GSM       Http;

#define TRUE  0
#define FALSE 1

//======================================================================
//  CONFIG DIRECTIVES
//#define WRITE_DEFAULT_DATA_EEPROM      //  If this directive is enabled then the sketch run only the code necessary to write the default data into EEPROM
//======================================================================

//======================================================================
#ifdef WRITE_DEFAULT_DATA_EEPROM
  const char STR_PIN1[]                     PROGMEM = " -> PIN1                    = ";
  const char STR_PUK1[]                     PROGMEM = " -> PUK1                    = ";
  const char STR_PIN2[]                     PROGMEM = " -> PIN2                    = ";
  const char STR_PUK2[]                     PROGMEM = " -> PUK2                    = ";
  const char STR_PH_PIN[]                   PROGMEM = " -> PH_PIN                  = ";
  const char STR_PH_PUK[]                   PROGMEM = " -> PH_PUK                  = ";
  const char STR_LONG_PSW[]                 PROGMEM = " -> LONG PASSWORD           = ";
  const char STR_SHORT_PSW[]                PROGMEM = " -> SHORT PASSWORD          = ";
  
  const char STR_SYSTEM_PWD_CODE[]          PROGMEM = " -> SYSTEM PASSWORD         = ";
  const char STR_ALARM_INPUT_1_HIGH[]       PROGMEM = " -> DEFAULT ALARM 1 HIGH    = ";
  const char STR_ALARM_INPUT_1_LOW[]        PROGMEM = " -> DEFAULT ALARM 1 LOW     = ";
  const char STR_ALARM_INPUT_2_HIGH[]       PROGMEM = " -> DEFAULT ALARM 2 HIGH    = ";
  const char STR_ALARM_INPUT_2_LOW[]        PROGMEM = " -> DEFAULT ALARM 2 LOW     = ";
  const char STR_SYSTEM_START_UP[]          PROGMEM = " -> DEFAULT START UP        = ";
  const char STR_ALARM_POWER_SUPPLY[]       PROGMEM = " -> DEFAULT POWER SUPPLY    = ";
  const char STR_RESTORED_POWER_SUPPLY[]    PROGMEM = " -> DEFAULT RESTORED SUPPLY = ";
      
  const char STR_ADDRESS[]                  PROGMEM = "Address: ";
    
  const char STR_ERASE_EEPROM_START[]       PROGMEM = "Starts erase Eeprom\n";
  const char STR_ERASE_EEPROM[]             PROGMEM = "Erase eeprom in progress: ";
  const char STR_ERASE_EEPROM_STOP[]        PROGMEM = "Erase eeprom finished\n";
  
  const char STR_WRITE_EEPROM[]             PROGMEM = "Eeprom initialization in progress. Wait please \n\n";
  const char STR_READ_EEPROM[]              PROGMEM = "Read data from Eeprom. Wait please \n\n";
#endif

const char STR_SEP[]   PROGMEM = " --------------------------------------------------------------------------------------------------------- \n";
const char STR_SEP2[]  PROGMEM = "|  ";
const char STR_SEP2B[] PROGMEM = "| ";
const char STR_SEP3[]  PROGMEM = "  |";
const char STR_SEP4[]  PROGMEM = " |";
const char STR_ADD[]   PROGMEM = "| Add  | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F ";
const char STR_SCII[]  PROGMEM = "| Ascii            |\n";
//======================================================================

#define TDG133_REV                    "1.0"

#define ARDUINO_MEGA_EEPROM_MAX       4096
#define ROW_MAX_ARDUINO_MEGA          ((ARDUINO_MEGA_EEPROM_MAX / 16) - 1)
#define COLUMN_MAX                    15

//======================================================================
//  EEPROM FACTORY PARAMETERS ADDRESS
#define SYS_PSWD_ADD                  0x0050
#define FLAGS_ADD                     0x005A
#define SMS_NOTIFICATION_ADD          0x005B
#define VOICE_NOTIFICATION_ADD        0x005C
#define INPUTS_LEVEL_THRESHOLD_ADD    0x005D
#define INPUT_1_INHIBITION_TIME_ADD   0x005E
#define INPUT_2_INHIBITION_TIME_ADD   0x005F
#define INPUT_1_OBSERVATION_TIME_ADD  0x0060
#define INPUT_2_OBSERVATION_TIME_ADD  0x0061
#define INPUT_1_MAX_NUMBER_SMS_ADD    0x0062
#define INPUT_2_MAX_NUMBER_SMS_ADD    0x0063
#define HH_REPORT_SMS_ADD             0x0064
#define MM_REPORT_SMS_ADD             0x0065
#define SS_REPORT_SMS_ADD             0x0066
#define ECHO_PHONE_NUMBER_POINTER_ADD 0x0067
#define GATE_TIME_ADD                 0x0068
#define OUTPUT_RELAY_STATE_ADD        0x006F
#define ALARM_INPUT_1_HIGH_ADD        0x0070
#define ALARM_INPUT_1_LOW_ADD         0x00D6
#define ALARM_INPUT_2_HIGH_ADD        0x013C
#define ALARM_INPUT_2_LOW_ADD         0x01A2
#define SYSTEM_START_UP_ADD           0x0208
#define POWER_SUPPLY_ALARM_ADD        0x026E
#define RESTORE_POWER_SUPPLY_ADD      0x02D4
#define EEPROM_FREE_SPACE_ADD         0x033A
//======================================================================

//======================================================================
//  EEPROM DEFAULT FACTORY PARAMETERS

//==========================================
#define DEFAULT_FLAGS                 0b00010100
//                                      ||||||||
//                                      |||||||+----> If "1" the Inhibition time for input 1 is ignored
//                                      ||||||+-----> If "1" the Inhibition time for input 2 is ignored
//                                      |||||+------> If "1" Enable input SMS report otherwise disable input SMS report
//                                      ||||+-------> If "1" input SMS binary format otherwise text mode
//                                      |||+--------> If "1" Enable recovery state realy after lack of power otherwise Disable
//                                      ||+---------> If "1" Enable start-up nothification otherwise Disable start-up nothification
//                                      |+----------> If "1" Enable lack of power notification otherwise disable
//                                      +-----------> If "1" Enable echo function otherwise Disable echo function
#define DEFAULT_SMS_NOTIFICATION      0xFF
#define DEFAULT_VOICE_NOTIFICATION    0xFF
#define DEFAULT_INHIBITION_TIME       5     //  5 Minutes
#define DEFAULT_OBSERVATION_TIME      1     //  1 Second
#define DEFAULT_MAX_NUMBER_SMS        99    //  99 Infinite SMS; 0 No send SMS
#define DEFAULT_HH_REPORT_SMS         0
#define DEFAULT_MM_REPORT_SMS         5
#define DEFAULT_SS_REPORT_SMS         0
#define DEFAULT_GATE_TIME             3
//==========================================

//==========================================
#define SYS_PSWD_MAX_LENGHT           5     //  String lenght
#define SMS_MESSAGE_MAX_LENGHT        100   //  Max String lenght
#define MAX_INHIBITION_TIME           99    //  99 Minutes
#define MAX_OBSERVATION_TIME          59    //  59 Second
#define MAX_NUMBER_SMS                99    //  Max value for number of SMs to send
//==========================================

//==========================================
//  GSM Library PIN, PUK etc
const char FLASH_PIN1_CODE[]              PROGMEM = "\"4629\"";
const char FLASH_PIN2_CODE[]              PROGMEM = "\"8888\"";
const char FLASH_PH_PIN_CODE[]            PROGMEM = "\"5555\"";
const char FLASH_PUK1_CODE[]              PROGMEM = "\"54094153\"";
const char FLASH_PUK2_CODE[]              PROGMEM = "\"99999999\"";
const char FLASH_PH_PUK_CODE[]            PROGMEM = "\"44444444\"";
const char FLASH_LONG_PSW_CODE[]          PROGMEM = "\"12345678\"";
const char FLASH_SHORT_PSW_CODE[]         PROGMEM = "\"1234\"";
//==========================================

//==========================================
//  SYSTEM PASSWORD; SMS TEXT FOR ALARM, START-UP etc
const char FLASH_SYSTEM_PWD_CODE[]        PROGMEM = "12345";
const char FLASH_ALARM_INPUT_1_HIGH[]     PROGMEM = "ALARM!! INPUT 1 HIGH";
const char FLASH_ALARM_INPUT_1_LOW[]      PROGMEM = "ALARM!! INPUT 1 LOW";
const char FLASH_ALARM_INPUT_2_HIGH[]     PROGMEM = "ALARM!! INPUT 2 HIGH";
const char FLASH_ALARM_INPUT_2_LOW[]      PROGMEM = "ALARM!! INPUT 2 LOW";
const char FLASH_SYSTEM_START_UP[]        PROGMEM = "SYSTEM START-UP";
const char FLASH_ALARM_POWER_SUPPLY[]     PROGMEM = "ALARM!! LACK OF POWER SUPPLY";
const char FLASH_RESTORED_POWER_SUPPLY[]  PROGMEM = "RESTORED POWER SUPPLY";
//==========================================

//======================================================================

//======================================================================
//  TDG133  HARDWARE DEFINE (INPUT AND OUTPUT)

//--------------------------------------------
//  Directives used to manage Relay
#define PIN_RELAY_1               28    //  Relay 1 mapped on the I/O 28 (Pin 69). The associated RED led is mapped on the I/O 37 (Pin 62) (See GSM Library)  
#define PIN_RELAY_2               29    //  Relay 2 mapped on the I/O 29 (Pin 70). The associated RED led is mapped on the I/O 36 (Pin 61) (See GSM Library)

#define RELAY_STATE_OFF           0
#define RELAY_STATE_ON            1
#define RELAY_STATE_TOGGLE        2
#define RELAY_STATE_TOGGLE_WAIT   3
#define RELAY_STATE_TOGGLE_ONLY   4
#define RELAY_STATE_NOTHING       255
//--------------------------------------------

//--------------------------------------------
#define PIN_INPUT_1       48    //  Input 1 mapped on the I/O 48 (Pin 49)
#define PIN_INPUT_2       49    //  Input 2 mapped on the I/O 49 (Pin 50)

#define LEVEL_HIGH_INPUT  0
#define LEVEL_LOW_INPUT   1
#define TOGGLE_INPUT      2

#define INPUT_STATE_IDLE  0          
#define INPUT_STATE_WAIT  1
//--------------------------------------------
//======================================================================

//======================================================================
//  COMMANDS STRING
const char CMD_PWD[]          PROGMEM = "PWD";      //  Command used to change the password:
                                                    //  "PWDxxxxx;pwd"         -> "xxxxx" is the new password; "pwd" is the current password
const char CMD_PRN_EE[]       PROGMEM = "PRNEE";    //  Command used to print eeprom data:
                                                    //  "PRNEE;pwd"            -> "pwd" is the current password
const char CMD_NUM[]          PROGMEM = "NUM";      //  Command used to store a phone number:
                                                    //  "NUMx+39nnnnnnnnnnn;text;pwd"  -> "x" position in the list; "nnnnnnnnnnn" is the phone number with country code (+39 for Italy); "text" is the description (Max lenght 14 chars); "pwd" is the current password
                                                    //  Command used to remove a phone number:
                                                    //  "NUMx;pwd"             -> "x" position in the list; "pwd" is the current password
const char CMD_QUERY_NUM[]    PROGMEM = "NUM?";     //  Command used to request the list of phone numbers currently stored in the device (Only first 8 locations):
                                                    //  "NUM?;pwd"             -> "pwd" is the current password
                                                    //  Command used to request a single phone numbers currently stored in the device:
                                                    //  "NUM?;x;pwd"           -> "x" position in the list; "pwd" is the current password
const char CMD_QUERY_ANUM[]   PROGMEM = "ANUM?";    //  Command used to request all phone numbers currently stored in the device (250 locations):
                                                    //  "ANUM?;pwd"            -> "x" position in the list; "pwd" is the current password
const char CMD_QUERY_FNUM[]   PROGMEM = "FNUM?";    //  Command used to find a number into a PhoneBook:
                                                    //  "FNUM?;text;pwd"       -> "text" text to find into PhoneBook (Max lenght 14 chars); "pwd" is the current password                             
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NOT IMPLEMENTED (Command MAC and command DAC)
const char CMD_MAC[]          PROGMEM = "MAC";      //  Command used to store the phone numbers that control the gate opener function only
                                                    //  "MAC+39xxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the phone number you want to store in the list (200 numbers max) with international code; "pwd" is the curren password
const char CMD_DAC[]          PROGMEM = "DAC";      //  Command used to delete a phone number from the gate control list
                                                    //  "DAC+39xxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the phone number you want to store in the list (200 numbers max) with international code; "pwd" is the curren password
                                                    //  "DAC;pwd"              -> Delete all numbers in the gate control list; "pwd" is the curren password
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char CMD_RES[]          PROGMEM = "RES";      //  Command used to reset the initial settings of the system and delete all stored phone numbers:
                                                    //  "RES;pwd"              -> "pwd" is the current password
const char CMD_DEL_SMS[]      PROGMEM = "DSMS";     //  Command used to delete SMS by SIM memory      
                                                    //  "DSMSx;pwd;            -> "x" is the memoery index, "pwd" is the current password
const char CMD_DEL_ALL_SMS[]  PROGMEM = "DASMS";    //  Command used to delete all SMS by SIM memory      
                                                    //  "DASMS;pwd;            -> "pwd" is the current password
const char CMD_SMS[]          PROGMEM = "SMS";      //  Command used to allow the number in the specified position to receive SMS on input status:
                                                    //  "SMSxxxxxxxx:ON;pwd"   -> "xxxxxxxx" position of the first 8 numbers; "ON" activates the function; "pwd" is the current password
                                                    //  "SMSxxxxxxxx:OFF;pwd"  -> "xxxxxxxx" position of the first 8 numbers; "OFF" deactivates the function; "pwd" is the current password
const char CMD_VOC[]          PROGMEM = "VOC";      //  Command used to allows the number on the specified position to receive the ringtone on the inputs status:
                                                    //  "VOCxxxxxxxx:ON;pwd"   -> "xxxxxxxx" position of the first 8 numbers; "ON" activates the function; "pwd" is the current password
                                                    //  "VOCxxxxxxxx:OFF;pwd"  -> "xxxxxxxx" position of the first 8 numbers; "OFF" deactivates the function; "pwd" is the current password
const char CMD_LIV[]          PROGMEM = "LIV";      //  Command used to set a HIGH, LOW or TOGGLE level as the alarm condition for inputs IN1 or IN2:
                                                    //  "LIVx:A;pwd"           -> "x" stands for input 1 or 2; "A" stands for HIGH level; "pwd" is the current password
                                                    //  "LIVx:B;pwd"           -> "x" stands for input 1 or 2; "B" stands for LOW level; "pwd" is the current password
                                                    //  "LIVx:V;pwd"           -> "x" stands for input 1 or 2; "V" stands for variation (LOW to HIGH or HIGH to LOW); "pwd" is the current password
const char CMD_QUERY_LIV[]    PROGMEM = "LIV?";     //  Command used to request the alarm activation level:
                                                    //  "LIV?"
const char CMD_INI[]          PROGMEM = "INI";      //  Command used to set the inhibition time on input 1:
                                                    //  "INIx:mm;pwd"          -> "x" is the input; "mm" is the time in prime minutes; "pwd" is the current password
const char CMD_QUERY_INI[]    PROGMEM = "INI?";     //  Command used to request the current inhibition time setting regarding the inputs:
                                                    //  "INI?"
const char CMD_TIZ1[]         PROGMEM = "TIZ1";     //  Command used to reset/disable the inhibition time if input 1 is idle:
                                                    //  "TIZ1x;pwd"            -> "x" is the setting parameter (if "0" no reset; if "1" reset); "pwd" is the current password
const char CMD_TIZ2[]         PROGMEM = "TIZ2";     //  Command used to reset/disable the inhibition time if input 2 is idle:
                                                    //  "TIZ2x;pwd"            -> "x" is the setting parameter (if "0" no reset; if "1" reset); "pwd" is the current password
const char CMD_OSS[]          PROGMEM = "OSS";      //  Command used to define the lenght of the observation time regarding input 1:
                                                    //  "OSSx:ss;pwd"          -> "x" is the input index; "ss" is the time in seconds; "pwd" is the current password
const char CMD_QUERY_OSS[]    PROGMEM = "OSS?";     //  Command used to request the current observation time setting regarding the inputs
                                                    //  "OSS?"
const char CMD_TIN1A[]        PROGMEM = "TIN1A";    //  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension presence:
                                                    //  "TIN1A:xxxx;pwd"       -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" is the current password
                                                    //                             The default message is "ALARM!! INPUT 1 HIGH"
const char CMD_TIN1B[]        PROGMEM = "TIN1B";    //  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension absence:
                                                    //  "TIN1B:xxxx;pwd"       -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" is the current password
                                                    //                             The default message is "ALARM!!"
const char CMD_TIN2A[]        PROGMEM = "TIN2A";    //  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension presence:
                                                    //  "TIN2A:xxxx;pwd"       -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" is the current password
                                                    //                             The default message is "ALARM!! INPUT 2 HIGH"
const char CMD_TIN2B[]        PROGMEM = "TIN2B";    //  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension absence:
                                                    //  "TIN2B:xxxx;pwd"       -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" is the current password
                                                    //                             The default message is "ALARM!!"
const char CMD_ALN[]          PROGMEM = "ALN";      //  Command used to define the number of SMS that the device must send when the alarm occurs:
                                                    //  "ALNy:xx;pwd"          -> "y" is the input index; "xx" the number of SMS to be sent. If "xx" = 0 disables the SMS sending, if "xx" = 99 if alarm detected the system sends an infinite number of SMS until the alarm disappear; "pwd" is the current password
const char CMD_QUERY_ALN[]    PROGMEM = "ALN?";     //  Command used to request the current setting for the number of sms thath must be send when an alarm occurs
                                                    //  "ALN?"
const char CMD_OUT[]          PROGMEM = "OUT";      //  Command used to activate the specified output relay:
                                                    //  "OUTx:ON;pwd"          -> Activate output relay 1; "pwd" is the current password
                                                    //  "OUTx:OFF;pwd"         -> Deactivate output relay 1; "pwd" is the current password
                                                    //  "OUTx:ss;pwd"          -> Inverts the condition of the specified relay for desired time; "ss" is a period between 1 and 59 seconds; "pwd" is the current password
const char CMD_QUERY_STA[]    PROGMEM = "STA?";     //  Command used to request the condition of the remote outputs
                                                    //  "STA?"
const char CMD_AUTOC[]        PROGMEM = "AUTOC";    //  Command used to set the time between report SMS sending
                                                    //  "AUTOC:hh:mm:ss;pwd"   -> "hh" hours; "mm" minutes; "ss" seconds; "pwd" is the current password
const char CMD_QUERY_AUTOC[]  PROGMEM = "AUTOC?";   //  Command used to request the settings of the report function
                                                    //  "AUTOC?"
const char CMD_FORS[]         PROGMEM = "FORS";     //  Command used to set the TEXT format of the SMS in the report mode
                                                    //  "FORS:x;pwd"           -> "x" is the SMS format; "1" TEXT format; "2" Binary format; "pwd" is the current password
const char CMD_RIP[]          PROGMEM = "RIP";      //  Command used to enable the Command to store the relay status in case of black-out and restores it when power is back on
                                                    //  "RIPx;pwd"             -> "x" is the setting parameter (if "0" disabled; if "1" enabled); "pwd" is the current password
const char CMD_QUERY_RIP[]    PROGMEM = "RIP?";     //  Command used to request the current setting for relay status recovery
                                                    //  "RIP?"
const char CMD_AVV[]          PROGMEM = "AVV";      //  Command used to enable the function to send a start-up SMS
                                                    //  "AVVx;pwd"             -> "x" is the setting parameter (if "0" disabled; if "1" enabled); "pwd" is the current password
const char CMD_TSU[]          PROGMEM = "TSU";      //  Command used to set the text of the message that the system sends during start-up
                                                    //  "TSU:xxxxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized
                                                    //                            The default message is "SYSTEM STARTUP"
const char CMD_TAC[]          PROGMEM = "TAC";      //  Command used to define how relay 1 (Related to the gate control) should be enabled when there is an incoming call from one of the 200 phone numbers or one of the 8 numbers in the list
                                                    //  "TAC:ss;pwd"           -> "ss" is the time in seconds during which the relay should remain excited. The default value is 3 seconds; "pwd" is the current password
                                                    //  "TAC:00;pwd"           -> Activates bistable mode; "pwd" is the current password
const char CMD_ECHO[]         PROGMEM = "ECHO";     //  Command used to set the forward function. This function is used to forward the generic SMs received at the selected number
                                                    //  "ECHO:x;pwd"           -> "x" position in the list. "0" disables ECHO function; "pwd" is the current password
const char CMD_QUERY_ECHO[]   PROGMEM = "ECHO?";    //  Command used to request the settings of the ECHO function
                                                    //  "ECHO?"
const char CMD_QUERY_QUAL[]   PROGMEM = "QUAL?";    //  Command used to request the quality of the radio signal
                                                    //  "QUAL?"
const char CMD_RISP[]         PROGMEM = "RISP";     //  Command used to disable all reply message. This command must be positioned at the beginning of a multiple message
                                                    //  "RISP,.........."
const char CMD_PWRF[]         PROGMEM = "PWRF";     //  Command used to Enable/Disable the notification of the lack of power supply
                                                    //  "PWRFx;pwd"            -> "x" Enable/Disable; "1" Enable; "0" Disable; "pwd" is the current password
const char CMD_PWRR[]         PROGMEM = "PWRR";     //  Command used to Enable/Disable the notification of the return of power supply
                                                    //  "PWRRx;pwd"            -> "x" Enable/Disable; "1" Enable; "0" Disable; "pwd" is the current password
const char CMD_TPWPF[]        PROGMEM = "TPWPF";    //  Command used to set the text of the message that the system sends during lack of power supply
                                                    //  "TPWPF:xxxxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" is the current password
const char CMD_TPWPB[]        PROGMEM = "TPWPB";    //  Command used to set the text of the message that the system sends during go back power supply
                                                    //  "TPWPB:xxxxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" is the current password                                                 
const char CMD_QUERY_OPER[]   PROGMEM = "OPER?";    //  Command used to request the SIM Operator Name
                                                    //  "OPER?"
const char CMD_QUERY_REV[]    PROGMEM = "REV?";     //  Command used to request the TDG133 Firmware Rev and GSM Rev
                                                    //  "REV?"

const char CMD_STATE_ON[]     PROGMEM = "ON";
const char CMD_STATE_OFF[]    PROGMEM = "OFF";
//======================================================================

//======================================================================
//  Define commands Code
#define MAX_CMD_CODE      255

#define CMD_PWD_CODE    	0   //  Code 00 -> Command used to change the password
#define CMD_PRNEE_CODE    1   //  Code 01 -> Command used to print eeprom data
#define CMD_QANUM_CODE    2   //  Code 02 -> Command used to request all phone numbers currently stored in the device (250 locations)
#define CMD_QFNUM_CODE    3   //  Code 03 -> Command used to find a number into a PhoneBook
#define CMD_QNUM_CODE     4   //  Code 04 -> Command used to request the list of phone numbers currently stored in the device (Only first 8 locations)
#define CMD_NUM_CODE    	5   //  Code 05 -> Command used to store a phone number
#define CMD_MAC_CODE      6   //  Code 06 -> Not implemented
#define CMD_DAC_CODE      7   //  Code 07 -> Not implemented 
#define CMD_RES_CODE    	8   //  Code 08 -> Command used to reset the initial settings of the system and delete all stored phone numbers
#define CMD_DSMS_CODE     9   //  Code 09 -> Command used to delete SMS by SIM memory  
#define CMD_DASMS_CODE    10  //  Code 10 -> Command used to delete all SMS by SIM memory
#define CMD_SMS_CODE    	11  //  Code 11 -> Command used to allow the number in the specified position to receive SMS on input status
#define CMD_VOC_CODE    	12  //  Code 12 -> Command used to allows the number on the specified position to receive the ringtone on the inputs status
#define CMD_QLIV_CODE     13  //  Code 13 -> Command used to request the alarm activation level
#define CMD_LIV_CODE    	14  //  Code 14 -> Command used to set a HIGH, LOW or TOGGLE level as the alarm condition for inputs IN1 or IN2
#define CMD_QIN_CODE      15  //  Code 15 -> Command used to request the current inhibition time setting regarding the inputs
#define CMD_INI_CODE   	  16  //  Code 16 -> Command used to set the inhibition time on input 1
#define CMD_TIZ1_CODE   	17  //  Code 17 -> Command used to reset/disable the inhibition time if input 1 is idle
#define CMD_TIZ2_CODE   	18  //  Code 18 -> Command used to reset/disable the inhibition time if input 2 is idle
#define CMD_QOSS_CODE     19  //  Code 19 -> Command used to request the current observation time setting regarding the inputs
#define CMD_OSS_CODE   	  20  //  Code 20 -> Command used to define the lenght of the observation time regarding input 1
#define CMD_TIN1A_CODE  	21  //  Code 21 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension presence
#define CMD_TIN1B_CODE  	22  //  Code 22 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension absence:
#define CMD_TIN2A_CODE  	23  //  Code 23 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension presence
#define CMD_TIN2B_CODE  	24  //  Code 24 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension absence:
#define CMD_QALN_CODE     25  //  Code 25 -> Command used to request the current setting for the number of sms thath must be send when an alarm occurs
#define CMD_ALN_CODE    	26  //  Code 26 -> Command used to define the number of SMS that the device must send when the alarm occurs
#define CMD_OUT_CODE    	27  //  Code 27 -> Command used to activate the specified output relay
#define CMD_QSTA_CODE    	28  //  Code 28 -> Command used to request the condition of the remote outputs
#define CMD_QAUTOC_CODE   29  //  Code 29 -> Command used to request the settings of the report function
#define CMD_AUTOC_CODE    30  //  Code 30 -> Command used to set the time between report SMS sending
#define CMD_FORS_CODE     31  //  Code 31 -> Command used to set the TEXT format of the SMS in the report mode
#define CMD_QRIP_CODE     32  //  Code 32 -> Command used to request the current setting for relay status recovery
#define CMD_RIP_CODE    	33  //  Code 33 -> Command used to enable the Command to store the relay 1 status in case of black-out and restores it when power is back on
#define CMD_AVV_CODE    	34  //  Code 34 -> Command used to enable the function to send a start-up SMS
#define CMD_TSU_CODE    	35  //  Code 35 -> Command used to set the text of the message that the system sends during start-up
#define CMD_TAC_CODE    	36  //  Code 36 -> Command used to define how relay 1 (Related to the gate control) should be enabled when there is an incoming call from one of the 200 phone numbers or one of the 8 numbers in the list
#define CMD_QECHO_CODE    37  //  Code 37 -> Command used to request the settings of the ECHO function
#define CMD_ECHO_CODE     38  //  Code 38 -> Command used to set the forward function. This function is used to forward the generic SMs received at the selected number
#define CMD_QUAL_CODE     39  //  Code 39 -> Command used to request the quality of the radio signal
#define CMD_RISP_CODE     40  //  Code 40 -> Command used to disable all reply message. This command must be positioned at the beginning of a multiple message
#define CMD_PWRF_CODE     41  //  Code 41 -> Command used to Enable/Disable the notification of the lack of power supply
#define CMD_PWRR_CODE     42  //  Code 42 -> Command used to Enable/Disable the notification of return of power supply
#define CMD_TPWPF_CODE    43  //  Code 43 -> Command used to set the text of the message that the system sends during lack of power supply
#define CMD_TPWPB_CODE    44  //  Code 44 -> Command used to set the text of the message that the system sends during go back power supply
#define CMD_QOPER_CODE    45  //  Code 45 -> Command used to request the SIM Operator Name
#define CMD_QREV_CODE     46  //  Code 46 -> Command used to request the TDG133 Firmware Rev and GSM Rev          
//======================================================================

//======================================================================
//  Write Eeprom Variables
char    StrTemp[128];
//======================================================================

//======================================================================
//  Serial Monitor Strings
const char ADMIN[]                    PROGMEM = "ADMIN";

const char CMD_RECEIVED[]             PROGMEM = "# Command received by user -> ";
const char CMD_ERROR[]                PROGMEM = "# Command syntax error. Retry";
const char CMD_INVALID_CHAR[]         PROGMEM = "# Command syntax error. Found invalid char inside the text command\n";

const char CMD_INVALID_PASSWORD[]     PROGMEM = "# Inserted invalid password";
const char CMD_ERASE_PB_INDEX[]       PROGMEM = "# The selected index (Phonebook SIM memory) was erased from phonebook SIM\n";
const char CMD_ERASE_PB_INDEX_2[]     PROGMEM = "# Erased Phonebook memory entry: ";
const char CMD_READ_PB_INDEX[]        PROGMEM = "# The selected index of SIM phonebook was read\n";
const char CMD_READ_PB_EMPTY[]        PROGMEM = " is empty\n";
const char CMD_READ_PB_NUMBER_1[]     PROGMEM = "# The phone number read at the location: ";
const char CMD_READ_PB_NUMBER_2[]     PROGMEM = " is -> ";
const char CMD_READ_PB_TEXT[]         PROGMEM = "# The text associated to the phone number is: ";

const char CMD_FOUND_PB_TEXT[]        PROGMEM = "# Found the phone number into the phonebook. The location is: ";
const char CMD_NOT_FOUND_PB_TEXT[]    PROGMEM = "# Not found the phone number into the phonebook\n";

const char QUERY_INPUT_LEVEL_TEXT[]   PROGMEM = "# Alarm activation level for input";
const char INPUT_INPUT_INDEX[]        PROGMEM = "# Inserted invalid input index\n";

const char QUERY_INPUT_INHIB_TIME[]   PROGMEM = "# Alarm inhibition time for input";
const char INVALID_INHIB_TIME[]       PROGMEM = "# Inserted invalid value. Max value is 99 minutes\n";

const char QUERY_INPUT_OBSER_TIME[]   PROGMEM = "# Alarm observation time for input";
const char INVALID_OBSER_TIME[]       PROGMEM = "# Inserted invalid value. Max value is 99 seconds\n";

const char QUERY_INPUT_MAX_SMS[]      PROGMEM = "# Max num. of SMS to send if alarm occur (99 infinite; 0 nothing). Input";
const char INVALID_MAX_SMS[]          PROGMEM = "# Inserted invalid value. Max value is 99\n";

const char QUERY_RELAY_STATUS[]       PROGMEM = "# Relay status for output";

const char QUERY_REPORT_SMS[]         PROGMEM = "# SMS report status: ";
const char QUERY_REPORT_SMS_TIMEOUT[] PROGMEM = "# If enabled, the SMS report of the Inputs/Outputs is sent every: ";
const char QUERY_REPORT_SMS_FORMAT[]  PROGMEM = "# If enabled, the SMS report format is: ";

const char QUERY_RECOVERY_RELAY[]     PROGMEM = "# Recovery relay status: ";

const char QUERY_QUALITY_SIGNAL[]     PROGMEM = "# GSM Quality signal (RSSI): ";
const char QUERY_CHANNEL_BIT_ERROR[]  PROGMEM = "# GSM Channel bit error (BER): ";
const char QUALITY_SIGNAL_NOT_KNOWN[] PROGMEM = "Not known or not detectable\n";

const char QUERY_ECHO[]               PROGMEM = "# Echo function: ";
const char QUERY_ECHO_PHONE_POINTER[] PROGMEM = "# The phone number used for echo function is: ";

const char QUERY_OPERATOR[]           PROGMEM = "# The SIM operator is: ";

const char CMD_NOT_IMPLEMENTED[]      PROGMEM = "# This command is not implemented yet\n";

const char QUERY_TDG133_REV[]         PROGMEM = "# The TDG133 Rev: ";
const char QUERY_GMI[]                PROGMEM = "# Manufacturer: ";
const char QUERY_GMM[]                PROGMEM = "# TA Model: ";
const char QUERY_GMR[]                PROGMEM = "# TA ";
const char QUERY_GSN[]                PROGMEM = "# TA (IMEI): ";

const char INPUT_1_TEXT[]             PROGMEM = " 1: ";
const char INPUT_2_TEXT[]             PROGMEM = " 2: ";

const char INPUT_ACTIVATED[]          PROGMEM = "Activated Input";

const char SMS_FORMAT_BYNARY[]        PROGMEM = "BINARY\n";
const char SMS_FORMAT_TEXT[]          PROGMEM = "TEXT\n";
const char OUTPUT_RELAY_ON[]          PROGMEM = "ON\n";
const char OUTPUT_RELAY_OFF[]         PROGMEM = "OFF\n";
const char INPUT_LEVEL_HIGH[]         PROGMEM = "HIGH\n";
const char INPUT_LEVEL_LOW[]          PROGMEM = "LOW\n";
const char INPUT_LEVEL_TOGGLE[]       PROGMEM = "TOGGLE\n";

const char INPUT_STATE[]              PROGMEM = "Input ";
const char OUTPUT_STATE[]             PROGMEM = "Output ";

const char MAX_SMS_TO_SEND[]          PROGMEM = "# Max number of sms to send if the input alarm occur: ";
const char NUMBER_OF_SMS_SENT[]       PROGMEM = "# The number of SMS sent for this alarm is: "; 
const char SEND_SMS_IN_PROGRESS[]     PROGMEM = "# Sends a finite number of SMS alarm referred to the input: ";
const char SEND_SMS_IN_PROGRESS_2[]   PROGMEM = "# Sends a infinite numbers of SMS alarm referred to the Input: ";
const char SEND_SMS_IN_PROGRESS_3[]   PROGMEM = "# System not enabled to send a alarm SMS referred to the input: ";
const char START_VOICE[]              PROGMEM = "# Starts a alarm voice call referred to the input: ";
const char START_VOICE_2[]            PROGMEM = "# Starts a alarm voice call (Infinite loop) referred to the input: ";
const char START_VOICE_3[]            PROGMEM = "# System not enabled to start a alarm voice call referred to the input: ";

const char CMD_PROCESSED_CMD[]        PROGMEM = "# Command ";
const char CMD_PROCESSED_SUCCES[]     PROGMEM = "processed successfully\n";
//======================================================================

//======================================================================
typedef void State;
typedef State (*Pstate)();

Pstate IncomingSms_Management;      //  States machine used to manage the incoming SMS
Pstate IncomingCall_Management;     //  States machine used to manage the incoming Call
Pstate OutcomingEchoSms;            //  States machine used to manage the outcoming echo SMS
Pstate TestSimPhoneBook;            //  States machine used to test first index of PhoneBook
Pstate OutcomingAnswerSms;          //  States machine used to manage the outcoming answer SMS
//======================================================================

//======================================================================
//  State Machine for user AT command
#define RD_N_PHB_MEM        0       //  Reads n phone book memory
#define RD_PHB_MEM          1       //  Reads phone book memory
#define FN_PHB_MEM          2       //  Find phone book memory
#define WR_PHB_MEM          3       //  Writes phone book memory
#define ER_N_PHB_MEM        4       //  Erase all phone book memory
#define ER_PHB_MEM          5       //  Erases phone book memory
#define ER_SMS_MEM          6       //  Erases SMS from SIM memory
#define ER_ALL_SMS_MEM      7       //  Erases all SMS from SIM memory
#define NOTHING             255
//======================================================================

//======================================================================
//  State Machine for system AT command
#define ATQ_CREG_CMD        0
#define ATQ_CSQ_CMD         1
#define ATQ_CPAS_CMD        2
#define ATQ_COPS_CMD        3
#define ATQ_CPMS_CMD        4
#define ATQ_GMI_CMD         5
#define ATQ_GMM_CMD         6
#define ATQ_GMR_CMD         7
#define ATQ_GSN_CMD         8
//======================================================================

//======================================================================
//  State Machine for Outgoing Sms
#define READ_PHONEBOOK      0
#define SEND_SMS            1
#define START_VOC           2
#define END_VOC             3
//======================================================================

//======================================================================
#define MAX_INPUT_NUM       2
#define MAX_OUTPUT_NUM      2

uint8_t SmsReportState;                     //  State of the State Machine used to send Report sms
uint8_t StartUpSmsState;                    //  State of the State Machine used to send Start-Up sms
uint8_t PowerUpSmsState;                    //  State of the State Machine used to send Power-Up sms
uint8_t StateSystemAtCmd;                   //  State of the System AT command state machine
uint8_t StateUserAtCommand;                 //  State of the User AT command state machine
uint8_t SmsCallOutState[MAX_INPUT_NUM];     //  State of the outgoing Sms and Voice Call State machine

uint8_t MaxSmsOutCounterIn[MAX_INPUT_NUM];  //  Counter of outgoing SMS for alarm input 1 and input 2
uint8_t SmsCallOutShifter[MAX_INPUT_NUM];   //  Used to mask      
uint8_t SmsIndex;                           //  SMS SIM memory index
uint8_t PhoneIndex;                         //  PhoneBook memory index
uint8_t MaxPhoneIndex;                      //  Max PhoneBook memory index

uint8_t StateInput[MAX_INPUT_NUM];          //  State of the State Machine used to manage the alarm inputs
uint8_t InputPin[MAX_INPUT_NUM];            //  Array of input pins used for TDG133  (Alarm inputs)
uint8_t InputLedPin[MAX_OUTPUT_NUM];        //  Array of output pin used for TDG133  (Inputs Leds)
uint8_t OutputPin[MAX_OUTPUT_NUM];          //  Array of output pins used for TDG133 (Relay)
uint8_t OutputLedPin[MAX_OUTPUT_NUM];       //  Array of output pin used for TDG133  (Relay Leds)
uint8_t StateRelay[MAX_OUTPUT_NUM];         //  State of the Relay 1 and Relay 2
uint8_t LastStateRelay[MAX_OUTPUT_NUM];     //  Last state of the Relay 1 and Relay 2
//======================================================================

//======================================================================
String  StrSerialInput = "";
String  StrProcessCmd  = "";
String  StrSysPassword = "";
String  VeryLongSmsOut = "";

uint8_t PhoneNumberCopy[MAX_LENGHT_PN];

uint8_t Input_InhibTime[MAX_INPUT_NUM];       //  Inhibition Time for Input 1 and Input 2 (Value in minutes)
uint8_t Input_ObserTime[MAX_INPUT_NUM];       //  Observation Time for Input 1 and Input 2 (Value in seconds)
uint8_t Input_MaxSmsNumber[MAX_INPUT_NUM];    //  Max number of sms that the system is enabled to send
uint8_t EchoPhoneNumberPointer;               //  Pointer to Phone number for Echo function
uint8_t Output_1_GateTimeOut;                 //  Output 1 Timeout (Gate function). If "0" bistable mode

union  TDG133_Flags {
  uint32_t  DoubleWord;
  struct {
    uint8_t Byte_0;   // LSB
    uint8_t Byte_1;
    uint8_t Byte_2;
    uint8_t Byte_3;   //  MSB
  } Byte;
  struct {
    uint8_t  DisableInhibTimeInput1       :1; //  If "1" the Inhibition time for input 1 is ignored
    uint8_t  DisableInhibTimeInput2       :1; //  If "1" the Inhibition time for input 2 is ignored
    uint8_t  Sms_InOut_Report             :1; //  If "1" Enable input SMS report otherwise disable input SMS report
    uint8_t  Sms_InOut_Report_Binary      :1; //  If "1" input SMS binary format otherwise text mode
    uint8_t  RecoveryStateRelay           :1; //  If "1" Enable recovery state realy after lack of power otherwise Disable
    uint8_t  StartUpSms                   :1; //  If "1" Enable start-up nothification otherwise Disable start-up nothification
    uint8_t  LackOfPowerNotification      :1; //  If "1" Enable lack of power notification otherwise disable
    uint8_t  EchoFunction                 :1; //  If "1" Enable echo function otherwise Disable echo function
    uint8_t  NotificationSmsNumber_1      :1; //  "1" SMS  Notification for number 1 on
    uint8_t  NotificationSmsNumber_2      :1; //  "1" SMS  Notification for number 2 on
    uint8_t  NotificationSmsNumber_3      :1; //  "1" SMS  Notification for number 3 on
    uint8_t  NotificationSmsNumber_4      :1; //  "1" SMS  Notification for number 4 on
    uint8_t  NotificationSmsNumber_5      :1; //  "1" SMS  Notification for number 5 on
    uint8_t  NotificationSmsNumber_6      :1; //  "1" SMS  Notification for number 6 on
    uint8_t  NotificationSmsNumber_7      :1; //  "1" SMS  Notification for number 7 on
    uint8_t  NotificationSmsNumber_8      :1; //  "1" SMS  Notification for number 8 on
    uint8_t  NotificationCallNumber_1     :1; //  "1" CALL Notification for number 1 on
    uint8_t  NotificationCallNumber_2     :1; //  "1" CALL Notification for number 2 on
    uint8_t  NotificationCallNumber_3     :1; //  "1" CALL Notification for number 3 on
    uint8_t  NotificationCallNumber_4     :1; //  "1" CALL Notification for number 4 on
    uint8_t  NotificationCallNumber_5     :1; //  "1" CALL Notification for number 5 on
    uint8_t  NotificationCallNumber_6     :1; //  "1" CALL Notification for number 6 on
    uint8_t  NotificationCallNumber_7     :1; //  "1" CALL Notification for number 7 on
    uint8_t  NotificationCallNumber_8     :1; //  "1" CALL Notification for number 8 on
    uint8_t  SetLevelInput_1              :2; //  Input 1 level; High, Low or Toggle
    uint8_t  SetLevelInput_2              :2; //  Input 2 level; High, Low or Toggle
    uint8_t  PowerUpSms                   :1; //  If "1" Enable start-up nothification power-up otherwise Disable start-up nothification power-up
    uint8_t  Free                         :2; //  Free
    uint8_t  ReplyMessage                 :1; //  If "1" Reply Message Enable; "0" Reply Message Disable (Not send answer SMS)
  } Bit;
} TDG133_Flags;

union System_Flags {
  uint32_t DoubleWord;
  struct {
    uint8_t Byte_0;   // LSB
    uint8_t Byte_1;
    uint8_t Byte_2;
    uint8_t Byte_3;   //  MSB
  } Byte;  
  struct {
    uint8_t SetupAtCmdInProgress   :1;  //  "1" Process AT command in progress
    uint8_t SinglePbRead           :1;  //  "1" Reads single phonebook memory location
    uint8_t StartFindEntry         :1;  //  "1" Starts to find phonebook entry
    uint8_t TestFirstSimPbIndex    :1;  //  "1" The system is testing the first location of the phonebook. If the system founds a valid number then jumps the process used to wait a first phonic call necessary to store the first number into PhoneBook
    uint8_t IncomingCallInProgress :1;  //  "1" Incoming call in progress
    uint8_t IncomingSmsInProgress  :1;  //  "1" Incoming SMS in progress
    uint8_t SmsReportInProgress    :1;  //  "1" System busy to send SMS Input Report 
    uint8_t OutGoingEchoSms        :1;  //  "1" Outgoing echo SMS in progress
    uint8_t PhoneBookIndexFound    :1;  //  "1" -> Found phone number into SIM PhoneBook; "0" -> Not found phone number into SIM PhoneBook
    uint8_t PhoneBookIndexHigher   :1;  //  "1" -> PhoneBook index major of index 8;      "0" -> PhoneBook index minor of index 9
    uint8_t SendAnswerSms          :1;  //  "1" Start Process to send answer SMS
    uint8_t OutGoingAnswerSms      :1;  //  "1" Outgoing answer SMS in progress
    uint8_t ForceNoSmsAnswer       :1;  //  "1" No answer SMS. This flag is used when string command ANUM? was sent by SMS
    uint8_t Free_2                 :3;  //  Free
    uint8_t Free_3                 :8;  //  Free
    uint8_t Free_4                 :8;  //  Free
  } Bit;
} System_Flags;

union AddInputAlarmString {
  struct {
    uint16_t SmsMessageInput_High;
    uint16_t SmsMessageInput_Low;
  } EepromAdd[MAX_INPUT_NUM];  
} AddInputAlarmString;

union  InputAlarm {
  uint8_t Byte[MAX_INPUT_NUM];
  struct {
    uint8_t  SendSmsInput_High     :1;  //  "1" Send Notification Sms for Input level High
    uint8_t  SendSmsInput_Low      :1;  //  "1" Send Notification Sms for Input level Low
    uint8_t  DisableInhibTimeInput :1;  //  If "1" the Inhibition time for Input 1 or Input 2 is ignored
    uint8_t  SetLevelInput         :2;  //  Input 1 or Input 2 level; High, Low or Toggle
    uint8_t  Free                  :2;
    uint8_t  SmsOutProcessBusy     :1;  //  "1" System to process outgoing sms busy
  } Input[MAX_INPUT_NUM];
} InputAlarm;
//======================================================================

//======================================================================
//  Timer 5 variables
uint8_t   BlinkWaitPhonicCallLed;             //  TimeOut Led blinking when the system is in waiting for a first incoming call to set the first index of the PhoneBook
uint8_t   TimeOutSendAtCmd;                   //  TimeOut to send AT CMD

uint16_t  TimeOutWaitPhonicCall;              //  TimeOut to wait a first phonic call
uint16_t  TimeOutVoiceCall;                   //  TimeOut Voice Call
uint16_t  TimeOutGateFunction;                //  TimeOut gate function
uint16_t  ToggleTimeOutRelay[MAX_OUTPUT_NUM]; //  Used to toggle Relay 1 or Relay 2 output. Max time 59 Seconds
uint16_t  DebouncingInput[MAX_INPUT_NUM];     //  Debouncing TimeOut for digital Input 1 and Input 2 (TDG133)

uint32_t  InhibTimeOut_Input[MAX_INPUT_NUM];  //  Inhibition time for input 1 and input 2
uint32_t  TimeOutSendSmsReport;               //  Used to send a SMS report of the inputs state if this function is enabled
//======================================================================

//======================================================================
//  GSM Board I/O Input pin define (Not used in this sketch)
uint8_t P3_Button = 9;         //  Digital input. Button P3. See schematic
uint8_t P4_Button = 10;        //  Digital input. Button P4. See schematic
//======================================================================

//======================================================================
//  Variables to manage the input pin (Debouncing)
union  TDG133_Input {
  struct {
    uint8_t  InputStatus     :1;
    uint8_t  LastInputStatus :1;
    uint8_t  InputReaded     :1;
    uint8_t  InputVar        :1;
    uint8_t  Free            :4;
  } In;  
} TDG133_Input[MAX_INPUT_NUM];
//======================================================================

#ifdef WRITE_DEFAULT_DATA_EEPROM
  //======================================================================
  //  Sketch used to programm the Eeprom memory with default parameters
  //  the main loop function is empty. No functions required
  void setup() {
    Gsm.PswdEepromStartAddSetup();                      //  Reads start address for password string (PIN, PUK....)
    Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
   
    Clear_Eeprom();                                     //  Clear Eeprom. If data saved in Eeprom is not equal to 0x00 then the function forces it to 0x00
    Initialize_PIN_PUK_Eeprom();                        //  Saves PIN, PUK ecc in Eeprom memory
    Initialize_PWD_Eeprom();                            //  Saves system Password in Eeprom memory
    Initialize_Flags_Eeprom();                          //  Saves Default Flags
    Initialize_System_Alarm_Eeprom();                   //  Saves string data 
    delay(2000);                                        //  Waits 2 seconds
    Verify_Eeprom();                                    //  Verifies the EEPROM memory. Prints on the "Arduino Serial monitor" the content of the EEPROM memory
  }
  void loop() { }
  //======================================================================
#else
  //======================================================================
  //  TDG133 Sketch
  void setup() {
    Gsm.PswdEepromStartAddSetup();    //  Loads PIN,PUK ecc Eeeprom address
    SetupTimer5();                    //  Initialize timer 5
    SetGSMBoardInputPin();            //  Initialize digital input of shield board (P3 and P4)
    SetGsmBoardOutputPin();           //  Initialize digital output
    TestGsmBoardLed_Trigger();        //  Test digital output
  
    Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
    Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
    Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1
      
    Gsm.InitPowerON_GSM();                              //  Start GSM initialization     
    Gsm.UartFlag.Bit.EnableUartSM = 0;                  //
    Gsm.ExecuteUartState();                             //  Initialization Software Uart State Machine
    Gsm.UartFlag.Bit.EnableUartSM = 1;                  //

    //----------------------------------------------- 
    //  Configs GSM library variables
    Sms.SmsReceivedIndex  = 0;
    Sms.SmsWriteMemoryAdd = 0;
    //----------------------------------------------- 
    
    System_Flags.Bit.TestFirstSimPbIndex = 1;
   
    //-----------------------------------------------
    //  Reads EEPROM data
    ReadSystemDataFromEeprom();
    //-----------------------------------------------

    //----------------------------------------------- 
    //  Sets TDG133 digital inputs/outputs
    SetTDG133_Relay();
    RestoreStateIntoEeprom();   //  Start-Up State Relay
    SetTDG133_Input();
    SetStateInput();            //  Start-Up State Input
    SetLedAlarmInputs();
    //-----------------------------------------------  

    MaxSmsOutCounterIn[0] = 0;
    MaxSmsOutCounterIn[1] = 0;
    SmsCallOutShifter[0]  = 0;
    SmsCallOutShifter[1]  = 0;
    
    AddInputAlarmString.EepromAdd[0].SmsMessageInput_High = ALARM_INPUT_1_HIGH_ADD;
    AddInputAlarmString.EepromAdd[0].SmsMessageInput_Low  = ALARM_INPUT_1_LOW_ADD;
    AddInputAlarmString.EepromAdd[1].SmsMessageInput_High = ALARM_INPUT_2_HIGH_ADD;
    AddInputAlarmString.EepromAdd[1].SmsMessageInput_Low  = ALARM_INPUT_2_LOW_ADD;
        
    //----------------------------------------------- 
    //  Sets State Machine
    IncomingSms_Management  = ReadNewSmsIdle;        //  Initializes states machine used to manage incoming SMS
    IncomingCall_Management = IncomingCall_Idle;     //  Initializes states machine used to manage incoming Call
    OutcomingEchoSms        = OutgoingEchoSms_Idle;  //  Initializes states machine used to manage outgoing echo SMS
    TestSimPhoneBook        = TestPhoneBook_Step1;   //  Initializes states machine used to test first index of the PhoneBook
    OutcomingAnswerSms      = SendAnswerSms_Idle;    //  Initializes states machine used to manage outgoing answer SMS
    //-----------------------------------------------

    //-----------------------------------------------
    //  Start-Up State Sms outgoing and voice call State
    SmsCallOutState[0] = READ_PHONEBOOK;
    SmsCallOutState[1] = READ_PHONEBOOK;

    StartUpSmsState = READ_PHONEBOOK;
    PowerUpSmsState = READ_PHONEBOOK;
    SmsReportState  = READ_PHONEBOOK;    
    //-----------------------------------------------
    
    StateSystemAtCmd   = ATQ_CREG_CMD;
    StateUserAtCommand = NOTHING;
  }
  //======================================================================
  
  //======================================================================
  //  Main Loop TDG133
  void loop() {
    DebouncingTDG133_Input();     //  Debouncing inputs (The observation time is used to debounce the inputs)
    ManageTDG133_Input(0);        //  Manage TDG133 Input 1
    ManageTDG133_Input(1);        //  Manage TDG133 Input 2
    ManageTDG133_Relay(0);        //  Manage TDG133 Output Relay 1       
    ManageTDG133_Relay(1);        //  Manage TDG133 Output Relay 2
    
    ProcessStateMachineGsm();     //  Function used to process GSM library state machine
  }
  //======================================================================
#endif
