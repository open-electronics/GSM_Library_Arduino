/*********************************************************************
 *
 *       GSM Sketch - Management of Security command and PhoneBook command
 *
 *********************************************************************
 * FileName:        GSM_Security_PhoneBook.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  TimersInt.ino
 * Revision:        1.0.0
 * Date:            11/08/2018
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 * Arduino Board:   Arduino Mega 2560 or Fishino Mega 2560
 *                  See file Io_GSM.h to define hardware and Arduino Board to use
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
 * The code for this demo is splitted into four files:
 *
 *  - GSM_Security_PhoneBook  -> Main file project
 *  - DigitalInput            -> Contains code to manage digital inputs and the respective functions to manage it
 *  - DigitalOutput           -> Contains code to manage digital output and the respective functions to manage it
 *  - TimerInt                -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
 *                               With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
 *                               For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
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

SecurityCmd_GSM   Security;
PhoneBookCmd_GSM  PhoneBook;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
Isr_GSM           Isr;
Io_GSM            Io;

//======================================================================
//  Insert below a valid phone number
const char CALL_PHONE_NUMBER[]      PROGMEM = "+393491544808";
//======================================================================

//======================================================================
const char CMD_RECEIVED[]           PROGMEM = "# Command received by user -> ";
const char CMD_ERROR[]              PROGMEM = "# Command syntax error. Retry";

const char CMD_CALL[]               PROGMEM = "CmdCall";                  //  Text Command to execute a generic phonic call. See below for the parameters

const char CMD_FAC_LOCK[]           PROGMEM = "CmdFacLock";               //  Text Command to execute a facility lock.   See below for the parameters
const char CMD_FAC_UNLOCK[]         PROGMEM = "CmdFacUnLock";             //  Text Command to execute a facility unlock. See below for the parameters
const char CMD_FAC_CHECK[]          PROGMEM = "CmdFacCheck";              //  Text Command to execute a facility query.  See below for the parameters

const char CMD_FAC_SET_PWD[]        PROGMEM = "CmdFacSetPwd";             //  Text Command to set or change password.    See below for the parameters

const char CMD_SEL_PHB_MEM[]        PROGMEM = "CmdSelPhonebookMem";       //  Text Command to select a type of phone book memory. See below for the parameters
const char CMD_QRY_PHB_MEM[]        PROGMEM = "CmdQueryPhonebookMem";     //  Text Command to check which phone book type of memory was selected. See below for the parameters
const char CMD_RD_PHB_MEM[]         PROGMEM = "CmdReadPhonebookMem";      //  Text Command to read a phone number saved into phone book. See below for the parameters
const char CMD_WR_PHB_MEM[]         PROGMEM = "CmdWritePhonebookMem";     //  Text Command to write a phone number into phone book. See below for the parameters
const char CMD_ER_PHB_MEM[]         PROGMEM = "CmdErasePhonebookMem";     //  Text Command to erase a phone number from phone book. See below for the parameters
//======================================================================

#define TRUE  0
#define FALSE 1

//======================================================================
//  Below the list of codes used for the state machine used to manage the commands sent by serial monitor
#define START_CALL          0
#define START_GENERIC_CALL  1
#define STOP_CALL           2
#define FAC_LOCK            3
#define FAC_UNLOCK          4
#define FAC_CHECK           5
#define FAC_SET_PWD         6
#define SEL_PHB_MEM         7
#define QRY_PHB_MEM         8
#define RD_PHB_MEM          9
#define WR_PHB_MEM          10
#define ER_PHB_MEM          11
#define NOTHING             255
//======================================================================

//======================================================================
//  Below the list of codes used for the state machine used to manage the generic AT commands
#define CMD_ATQ_CPAS        0
#define CMD_ATQ_CSQ         1
#define CMD_ATQ_CREG        2
#define CMD_ATQ_CPIN        3
#define CMD_ATQ_COPS        4
//======================================================================

//======================================================================
//  State Machine defines
typedef void State;
typedef State (*Pstate)();

Pstate Input_Management;    //  States machine used to manage the digital inputs (P3 and P4)
//======================================================================

//======================================================================
//  Variables
uint8_t * EepromAdd;

uint8_t Counter;
uint8_t StateSendCmd;

uint8_t ActionSelected;

String  SerialInput   = "";
String  TempString    = "";
uint8_t StringIndex   = 0;

uint8_t FacCode;
uint8_t MemCode;
uint8_t MemAdd;

union SketchFlag {
  uint8_t  SketchWord;
  struct {
    uint8_t PhoneBookNumber   :1;   //  "1": Phonenumber has been read from Phone Book
    uint8_t Free              :7;
  } Bit;  
} SketchFlag;
//======================================================================

//======================================================================
//  Timer 0 variables
uint8_t  DebouncingTimeOut;    //  Debouncing TimeOut for digital inputs
uint16_t TimeOutWait;          //  TimeOut to manage pulse pressing
uint16_t TimeOutP3;            //  TimeOut to manage P3 button
uint16_t TimeOutP4;            //  TimeOut to manage P4 button

//======================================================================

//======================================================================
//  I/O Input pin define
uint8_t P3_Button = 9;         //  Digital input. Button P3. See schematic
uint8_t P4_Button = 10;        //  Digital input. Button P4. See schematic
//======================================================================

//======================================================================
//  Variables to manage the input pin (Debouncing)
union  DigInputStatus {
  uint8_t  Input;
  struct {
    uint8_t  P3_Button  : 1; //  Bit 0
    uint8_t  P4_Button  : 1; //  Bit 1
    uint8_t  Free       : 6;
  } In;
} DigInputStatus;

union  DigInputReaded {
  uint8_t  Input;
  struct {
    uint8_t  P3_Button  : 1; //  Bit 0
    uint8_t  P4_Button  : 1; //  Bit 1
    uint8_t  Free       : 6;
  } In;
} DigInputReaded;

uint8_t DigInputVar;
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Input_Management = Input_Idle;                      //  Initializes states machine used to manage the digital inputs (P3 and P4)

  Gsm.EepromStartAddSetup();
  SetupTimer5();                                      //  Initialize timer 5
  SetInputPin();                                      //  Initialize digital input of shield board (P3 and P4)
  SetOutputPin();                                     //  Initialize digital output
  TestLed_Trigger();                                  //  Test digital output

  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt

  Gsm.SetBaudRateUart1(true, BAUD_19200);              //  Enables and configures the Hardware/Software UART 1
  Gsm.ClearBuffer();                                  //  Clear library UART Buffer
    
  Gsm.InitPowerON_GSM();                              //  Start GSM initialization     
  Gsm.UartFlag.Bit.EnableUartSM = 0;                  // |  
  Gsm.ExecuteUartState();                             // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                  // |
 
  delay(500);
  Isr.TimeOutWait = T_15SEC;  

  StateSendCmd   = 0;
  ActionSelected = NOTHING;
}
//======================================================================

//======================================================================
void loop() {
  DebouncingInput();          //  Debouncing digital inputs (P3 and P4)
  Input_Management();         //  State Machine to manage Digital input
  
  ProcessStateMachineGsm();

  ProcessSerialCmd();
  ProcessUserATcmd();
  ProcessGenericATcmd();
}
//======================================================================


//======================================================================
void ProcessStateMachineGsm(void) {
  Gsm.ExecuteUartState();
  if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
    Gsm.InitGsmSendCmd();
    Gsm.InitGsmWaitAnswer();
  } else {  
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();

    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }  
}
//======================================================================

//======================================================================
//  Functions to process the commands sent from serial port
//
//  ----------------------------------------------------------
//  How to send a command to execute a phonic call:
//
//  CmdCall: +39349*******
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to execute a lock, unlock or interrogate a ME or a network facility <fac>:
//
//  CmdFacLock: <fac code>
//  CmdFacUnLock: <fac code>
//  CmdFacCheck: <fac code>
//
//  <fac code> -> "AO"  ->  0     (4-digit PSWD) - BAOC     (Barr All Outgoing Calls)
//                "OI"  ->  1     (4-digit PSWD) - BOIC     (Barr Outgoing International Calls)
//                "OX"  ->  2     (4-digit PSWD) - BOIC HC  (Barr Outgoing International Calls except to Home Country)
//                "AI"  ->  3     (4-digit PSWD) - BAIC     (Barr All Incoming Calls)
//                "IR"  ->  4     (4-digit PSWD) - BIC ROAM (Barr Incoming Calls when roaming outside the home country)
//                "FD"  ->  8     (4-digit PSWD) - SIM card or active application in the UICC fixed dialling memory feature  
//                "SC"  ->  9     (4-digit PSWD) - SIM (lock SIM/UICC card). SIM/UICC asks password in MT power-up and when this lock command issued. Correspond to PIN1 code
//                "PN"  ->  10    (8-digit PSWD) - Network Personalization, correspond to NCK code (8-digit password)
//                "PU"  ->  11    (8-digit PSWD) - Network subset Personalization correspond to NSCK code (8-digit password)
//                "PP"  ->  12    (8-digit PSWD) - Service Provider Personalization Correspond to SPCK code (8-digit password)
//
//  The password, related with previously codes, was saved on the EEPROM memory with the sketch "GSM_SetEeprom.ino".
//  Automatically the library code will read the password from the EEPROM and use it with the related AT command
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to change ME or a network facility <fac> password:
//
//  CmdFacSetPwd: <fac code>, <new password>
//
//  The parameter "new password" must be included with quotation marks 
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to select phonebook memory or write it:
//
//  CmdSelPhonebookMem: <MEM code>
//  CmdQueryPhonebookMem:
//  CmdReadPhonebookMem: <MEM add>
//  CmdWritePhonebookMem: <MEM add>, <PhoneNumber>, <PhoneNumberType>, <Text>
//  CmdErasePhonebookMem: <MEM add>
//
//  <MEM code> -> "DC"  ->  0    ME dialed calls list (+CPBW may not be applicable for this storage)
//                "EN"  ->  1    SIM (or MT) emergency number (+CPBW is not be applicable for this storage)
//                "FD"  ->  2    SIM fix dialing phonebook. If a SIM card is present or if a UICC with an active GSM application is present, the information in EFFDN under DFTelecom is selected
//                "MC"  ->  3    MT missed (unaswered received) calls list (+CPBW may not be applicable for this storage)
//                "ON"  ->  4    SIM (or MT) own numbers (MSISDNs) list (Reading of this storage may be available through +CNUM also). When storing information in the SIM/UICC, if a SIM card is present or if a UICC with an active GSM application is present, the information in EFMSISDN under DFTelecom is selected
//                "RC"  ->  5    MT received calls list (+CPBW may not be applicable for this storage)
//                "SM"  ->  6    SIM/UICC phonebook. If a SIM card is present or if a UICC whit an active GSM application is present, the EFADN under DFTelecom is selected 
//                "LA"  ->  7    Last number all list (LND/LNM/LNR)
//                "ME"  ->  8    ME phonebook
//                "BN"  ->  9    SIM barred dialed number
//                "SD"  ->  10   SIM service dial number
//                "VM"  ->  11   SIM voice mailbox
//                "LD"  ->  12   SIM last dialing phonebook
//
//  <PhoneNumber>     -> Phone number to save into phonebook. The number must be sent between quotation marks
//  <PhoneNumberType> -> Phone number type:
//                       129 -> National Number
//                       145 -> International number
//  <Text>            -> Number desciption
//  ----------------------------------------------------------
void ProcessSerialCmd(void) {
  if (Serial.available() > 0) {
    ReadSerialCmd();
    StringIndex = CheckChar(':');
    if (StringIndex > 0) {
      TempString = SerialInput.substring(0, StringIndex);
      if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_CALL, strlen(CMD_CALL), TRUE, TRUE)) == 0) {
        //  Received command "CmdCall: +39349*******"
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        SerialInput.toCharArray(PhoneBook.PhoneNumber, sizeof PhoneBook.PhoneNumber);
        ActionSelected = START_GENERIC_CALL;        
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_FAC_LOCK, strlen(CMD_FAC_LOCK), TRUE, TRUE)) == 0) {
        //  Received command "CmdFacLock: <fac code>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();  
        FacCode = SerialInput.toInt();
        ActionSelected = FAC_LOCK;
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_FAC_UNLOCK, strlen(CMD_FAC_UNLOCK), TRUE, TRUE)) == 0) {
        //  Received command "CmdFacUnLock: <fac code>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        FacCode = SerialInput.toInt();
        ActionSelected = FAC_UNLOCK;
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_FAC_CHECK, strlen(CMD_FAC_CHECK), TRUE, TRUE)) == 0) {
        //  Received command "CmdFacCheck: <fac code>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        FacCode = SerialInput.toInt();
        ActionSelected = FAC_CHECK;
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_FAC_SET_PWD, strlen(CMD_FAC_SET_PWD), TRUE, TRUE)) == 0) {
        //  Received command "CmdFacSetPwd: <fac code>, <new password>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        StringIndex = CheckChar(',');
        if (StringIndex > 0) {
          TempString = SerialInput.substring(0, StringIndex);
          TempString.trim();
          FacCode = TempString.toInt();     
          SerialInput.remove(0, StringIndex + 1);
          SerialInput.trim();
          SerialInput.toCharArray(Security.NewPin, sizeof Security.NewPin);
          ActionSelected = FAC_SET_PWD;
        } else {
          LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
          return;        
        }
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_SEL_PHB_MEM, strlen(CMD_SEL_PHB_MEM), TRUE, TRUE)) == 0) {
        //  Received command "CmdSelPhonebookMem: <MEM code>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        MemCode = SerialInput.toInt();
        ActionSelected = SEL_PHB_MEM;
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_QRY_PHB_MEM, strlen(CMD_QRY_PHB_MEM), TRUE, TRUE)) == 0) {
        //  Received command "CmdQueryPhonebookMem:
        ActionSelected = QRY_PHB_MEM;
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_RD_PHB_MEM, strlen(CMD_RD_PHB_MEM), TRUE, TRUE)) == 0) {
        //  Received command "CmdReadPhonebookMem:
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        MemAdd = SerialInput.toInt();
        ActionSelected = RD_PHB_MEM;
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_WR_PHB_MEM, strlen(CMD_WR_PHB_MEM), TRUE, TRUE)) == 0) {
        //  Received command "CmdWritePhonebookMem: <MEM add>, <PhoneNumber>, <PhoneNumberType>, <Text>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        StringIndex = CheckChar(',');
        if (StringIndex > 0) {
          TempString = SerialInput.substring(0, StringIndex);
          TempString.trim();
          MemAdd = TempString.toInt();
          SerialInput.remove(0, StringIndex + 1);
          SerialInput.trim();
          StringIndex = CheckChar(',');
          if (StringIndex > 0) {
            TempString = SerialInput.substring(0, StringIndex);
            TempString.trim();
            TempString.toCharArray(PhoneBook.PhoneNumber, sizeof PhoneBook.PhoneNumber);
            SerialInput.remove(0, StringIndex + 1);
            SerialInput.trim();
            StringIndex = CheckChar(',');
            if (StringIndex > 0) {
              TempString = SerialInput.substring(0, StringIndex);
              TempString.trim();              
              PhoneBook.PhoneNumberType = TempString.toInt();
              SerialInput.remove(0, StringIndex + 1);
              SerialInput.trim();
              SerialInput.toCharArray(PhoneBook.PhoneText, sizeof PhoneBook.PhoneText);
              ActionSelected = WR_PHB_MEM;
            } else {
              LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
              return;        
            } 
          } else {
            LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
            return;        
          } 
        } else {
          LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
          return;        
        }
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_ER_PHB_MEM, strlen(CMD_ER_PHB_MEM), TRUE, TRUE)) == 0) {
        //  Received command "CmdErasePhonebookMem: <MEM add>
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        MemAdd = SerialInput.toInt();
        ActionSelected = ER_PHB_MEM;
      } else { 
        LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
        return;
      }
    }
  }
}

void ReadSerialCmd(void) {
  SerialInput = "";
  SerialInput = Serial.readString();
  SerialInput.trim();
  LocalReadStringFLASH((uint8_t *)CMD_RECEIVED, strlen(CMD_RECEIVED), FALSE, FALSE);
  Serial.println(SerialInput);
}

uint8_t CheckChar(char CharToSearch) {
  uint8_t TempIndex = 0;
  TempIndex = SerialInput.indexOf(CharToSearch, 0);
  if (TempIndex == 255) {
    return(0);
  }
  return(TempIndex);
}

String LocalReadStringFLASH(uint8_t *FlashPointer, uint8_t Lenght, boolean PrintCR, boolean NoPrint) {
  uint8_t k;
  char    myChar;
  String  TempString;
  
  for (k = 0; k < Lenght; k++) {
    myChar = pgm_read_byte_near(FlashPointer + k);
    if (NoPrint == FALSE) { 
      Serial.print(myChar);
    }
    TempString += myChar;
  }
  if (NoPrint == FALSE) { 
    if (PrintCR == TRUE) {
      Serial.print("\n");
    }
  } else {
    return(TempString); 
  }
  return("");
}
//======================================================================

//======================================================================
//  Function used to send AT command for outgoing phonic call or to send a SMS 
void ProcessUserATcmd(void) {
  uint8_t Count;
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    switch (ActionSelected)
    {
      case START_CALL:            //  Start phonic call
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
          if (SketchFlag.Bit.PhoneBookNumber == 1) {
            SketchFlag.Bit.PhoneBookNumber = 0;
            if (PhoneBook.PhoneNumber[Count] == ASCII_QUOTATION_MARKS) {
              Count = 0;
              do {
                PhoneBook.PhoneNumber[Count] = PhoneBook.PhoneNumber[Count + 1];
                if (PhoneBook.PhoneNumber[Count] == ASCII_QUOTATION_MARKS) {
                  PhoneBook.PhoneNumber[Count] = 0;
                }
              } while (Count++ < (sizeof(PhoneBook.PhoneNumber) - 1));                
            }
          } else {
            Gsm.ReadStringFLASH((uint8_t *)CALL_PHONE_NUMBER, (uint8_t *)PhoneBook.PhoneNumber, strlen(CALL_PHONE_NUMBER));  
          }
          PhonicCall.SetCmd_ATD();          
        } else if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
          Serial.println("Command aborted. Phonic call yet in progress");
          Serial.println("");
        }
        ActionSelected = NOTHING;
        break;
      case START_GENERIC_CALL:    //  Start phonic call to a Generic Phone Number
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
          PhonicCall.SetCmd_ATD();  
        } else if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
          Serial.println("Command aborted. Phonic call yet in progress");
          Serial.println("");
        }
        ActionSelected = NOTHING;
        break;
      case STOP_CALL:           //  Stop phonic call 
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
          PhonicCall.SetCmd_ATH(0);  
        }
        ActionSelected = NOTHING;
        break;
      case FAC_LOCK:
        Security.SetCmd_CLCK(FacCode, 1);
        ActionSelected = NOTHING;
        break;
      case FAC_UNLOCK:
        Security.SetCmd_CLCK(FacCode, 0);
        ActionSelected = NOTHING;
        break;
      case FAC_CHECK:
        Security.SecurityFlag.Bit.ClckStatus = 1;
        Security.SetCmd_CLCK(FacCode, 2);
        ActionSelected = NOTHING;
        break;
      case FAC_SET_PWD:
        Security.SetCmd_CPWD(FacCode);
        ActionSelected = NOTHING;
        break;
      case SEL_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBS(MemCode, false);
        ActionSelected = NOTHING;
        break;
      case QRY_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBS(MemCode, true);
        ActionSelected = NOTHING;
        break;
      case RD_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBR(MemAdd, false);
        SketchFlag.Bit.PhoneBookNumber = 1;
        ActionSelected = NOTHING;
        break;
      case WR_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBW(MemAdd, 1);
        ActionSelected = NOTHING;
        break;
      case ER_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBW(MemAdd, 0);
        ActionSelected = NOTHING;
        break;          
      case NOTHING:
        break;
      default:
        break;
    }    
  }  
} 
//======================================================================

//======================================================================
//  Function to send generic command in the loop()
void ProcessGenericATcmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;
      
      switch (StateSendCmd)
      {
        case CMD_ATQ_CPAS:
          Gsm.SetCmd_ATQ_CPAS();
          ++StateSendCmd;
          break;
        case CMD_ATQ_CSQ:
          Gsm.SetCmd_ATQ_CSQ();
          ++StateSendCmd;
          break;
        case CMD_ATQ_CREG:
          Gsm.SetCmd_ATQ_CREG();
          ++StateSendCmd;
          break;
        case CMD_ATQ_CPIN:
          Security.SetCmd_ATQ_CPIN();
          ++StateSendCmd;
          break;
        case CMD_ATQ_COPS:
          Gsm.SetCmd_ATQ_COPS();
          StateSendCmd = 0;
          break;
        default:
          break;
      }
    }
  }  
}
//======================================================================
