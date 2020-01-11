/*********************************************************************
 *
 *       GSM Sketch - Management of SMS messages and Phonic calls
 *
 *********************************************************************
 * FileName:        GSM_Sms_PhonicCall.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  TimersInt.ino
 * Revision:        1.0.0
 * Date:            10/08/2018
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
 *  - GSM_Sms_PhonicCall  -> Main file project
 *  - DigitalInput        -> Contains code to manage digital inputs and the respective functions to manage it
 *  - DigitalOutput       -> Contains code to manage digital output and the respective functions to manage it
 *  - TimerInt            -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
 *                           With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
 *                           For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
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

//======================================================================
const char SMS_PHONE_NUMBER[]       PROGMEM = "\"+393491544808\"";
const char SMS_TEXT[]               PROGMEM = "FuturaGroup GSM Library R.1.0. Sending of the SMS message in process. Hello how are you?";

const char CALL_PHONE_NUMBER[]      PROGMEM = "+393491544808";
//======================================================================

//======================================================================
const char CMD_RECEIVED[]           PROGMEM = "# Command received by user -> ";
const char CMD_ERROR[]              PROGMEM = "# Command syntax error. Retry";

const char SMS_RECEIVED[]           PROGMEM = "Start process SMS received";

const char CMD_CALL[]               PROGMEM = "CmdCall";
const char CMD_SMS[]                PROGMEM = "CmdSms";
const char CMD_ERASE_SMS[]          PROGMEM = "CmdEraseSms";

const char LED7_ON[]                PROGMEM = "Led7On";
const char LED7_OFF[]               PROGMEM = "Led7Off";
const char LED7_BLINK[]             PROGMEM = "Led7Blink";
//======================================================================

#define TRUE  0
#define FALSE 1

#define SEND_SMS            0
#define SEND_GENERIC_SMS    1
#define START_CALL          2
#define START_GENERIC_CALL  3
#define STOP_CALL           4
#define NOTHING             255

#define CMD_ATQ_CPAS        0
#define CMD_ATQ_CSQ         1
#define CMD_ATQ_CREG        2
#define CMD_ATQ_CPIN        3
#define CMD_ATQ_COPS        4
#define CMD_AT_CMGR         5
#define CMD_AT_CMGD         6
#define ERASE_SMS_RECEIVED  7

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

String  SerialInput = "";
String  TempString  = "";
String  SmsReceived = "";
uint8_t StringIndex = 0;
//======================================================================

//======================================================================
//  Timer 5 variables
uint16_t TimeOutBlinkLed;      //  TimeOut blink led 7
uint8_t  DebouncingTimeOut;    //  Debouncing TimeOut for digital inputs
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
  
  Gsm.PswdEepromStartAddSetup();
  SetupTimer5();                                      //  Initialize timer 5
  SetInputPin();                                      //  Initialize digital input of shield board (P3 and P4)
  SetOutputPin();                                     //  Initialize digital output
  TestLed_Trigger();                                  //  Test digital output

  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1
    
  Gsm.InitPowerON_GSM();                              //  Start GSM initialization     
  Gsm.UartFlag.Bit.EnableUartSM = 0;                  // |  
  Gsm.ExecuteUartState();                             // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                  // |
 
  delay(500);
  Isr.TimeOutWait = T_15SEC;  
  Sms.SmsReceivedIndex  = 0;
  Sms.SmsWriteMemoryAdd = 0;
  
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
  ProcessUserAtCmd();
  ProcessGenericAtCmd();

  ProcessSmsReceived();
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
    if (TimeOutBlinkLed > 0) {
      Io.LedBlink(PIN_LED7, 50, T_250MSEC);
    }
  }  
}
//======================================================================

//======================================================================
//  Functions to process the commands sent from serial port
//
//  ----------------------------------------------------------
//  How to send a command to execute a phonic call:
//
//  CmdCall: +39349xxxxxxx
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to send a SMS:
//
//  CmdSms: "+39349xxxxxxx"#Sms Text, max 160 chars
//
//  Note: In the command to send an SMS it is necessary add a double quotation marks. First and after telephone number
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to erase SMS received
//
//  CmdEraseSms:
//  ----------------------------------------------------------

void ProcessSerialCmd(void) {
  if (Serial.available() > 0) {
    ReadSerialCmd();
    StringIndex = CheckChar(':');
    if (StringIndex > 0) {
      TempString = SerialInput.substring(0, StringIndex);
      if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_CALL, strlen(CMD_CALL), TRUE, TRUE)) == 0) {
        //  Received command "CmdCall: +39349xxxxxxx"
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        SerialInput.toCharArray(PhoneBook.PhoneNumber, sizeof PhoneBook.PhoneNumber);
        ActionSelected = START_GENERIC_CALL;        
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_SMS, strlen(CMD_SMS), TRUE, TRUE)) == 0) {
        //  Received command "CmdSms: "+39349xxxxxxx"#Message Text"
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        StringIndex = CheckChar('#');
        if (StringIndex > 0) {
          TempString = SerialInput.substring(0, StringIndex);
          TempString.trim();
          TempString.toCharArray(PhoneBook.PhoneNumber, sizeof PhoneBook.PhoneNumber);
          SerialInput.remove(0, StringIndex + 1);
          SerialInput.trim();
          SerialInput.toCharArray(Sms.SmsText, sizeof Sms.SmsText);
          ActionSelected = SEND_GENERIC_SMS;  
        } else {
          LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
          return;        
        }
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_ERASE_SMS, strlen(CMD_ERASE_SMS), TRUE, TRUE)) == 0) {
        StateSendCmd = ERASE_SMS_RECEIVED;
      } else {
        LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
        return;        
      }     
    } else {
      LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
      return;
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
void ProcessUserAtCmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    switch (ActionSelected)
    {
      case SEND_SMS:              //  Sends SMS
        Gsm.ReadStringFLASH((uint8_t *)SMS_PHONE_NUMBER, (uint8_t *)PhoneBook.PhoneNumber, strlen(SMS_PHONE_NUMBER));
        Gsm.ReadStringFLASH((uint8_t *)SMS_TEXT, (uint8_t *)Sms.SmsText, strlen(SMS_TEXT));
        Sms.SetCmd_AT_CMGS();
        ActionSelected = NOTHING;
        break;
      case SEND_GENERIC_SMS:      //  Sends a Generic SMS to a Generic Phone number
        Sms.SetCmd_AT_CMGS();
        ActionSelected = NOTHING;
        break;
      case START_CALL:            //  Start phonic call
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
          Gsm.ReadStringFLASH((uint8_t *)CALL_PHONE_NUMBER, (uint8_t *)PhoneBook.PhoneNumber, strlen(CALL_PHONE_NUMBER));
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
void ProcessGenericAtCmd(void) {
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
          ++StateSendCmd;
          break;
        case CMD_AT_CMGR:
          if (Sms.SmsReceivedIndex > 0) {
            Sms.SetCmd_AT_CMGR(Sms.SmsReceivedIndex, STATE_CHANGE);
            ++StateSendCmd;
            break;
          }
          StateSendCmd = 0;
          break;
        case CMD_AT_CMGD:
            Sms.SetCmd_AT_CMGD(Sms.SmsReceivedIndex, DEF_FLAG_NORMAL);
            Sms.SmsReceivedIndex = 0;
            StateSendCmd = 0;
          break;
        case ERASE_SMS_RECEIVED:
          if (Sms.SmsWriteMemoryAdd++ < 30) {
            Sms.SetCmd_AT_CMGD(Sms.SmsWriteMemoryAdd, DEF_FLAG_NORMAL);
          } else {
            Sms.SmsWriteMemoryAdd = 0;
            StateSendCmd = 0;
          }
          break;
        default:
          break;
      }
    }
  }  
}
//======================================================================

//======================================================================
//  Function to process SMS received
//
//  Command Led7On    -> Led 7 on the board ON
//  Command Led7Off   -> Led 7 on the board OFF
//  Command Led7Blink -> Led 7 on the board BLINK
//
void ProcessSmsReceived(void) {
  uint8_t Counter;
  
  if (Sms.SmsFlag.Bit.SmsReadOk == 1) {
    Sms.SmsFlag.Bit.SmsReadOk = 0;
    if (Sms.SmsFlag.Bit.SmsReadStat == SMS_REC_UNREAD) {
        SmsReceived = "";
        Counter = 0;
        do {
          SmsReceived += char(Sms.SmsText[Counter]);
        } while (Counter++ < sizeof Sms.SmsText);

        LocalReadStringFLASH((uint8_t *)SMS_RECEIVED, strlen(SMS_RECEIVED), TRUE, FALSE);
        Serial.println(SmsReceived);
        
        if (SmsReceived.compareTo(LocalReadStringFLASH((uint8_t *)LED7_ON, strlen(LED7_ON), TRUE, TRUE)) == 0) {
          Io.LedOn(PIN_LED7);
          return;  
        }
        if (SmsReceived.compareTo(LocalReadStringFLASH((uint8_t *)LED7_OFF, strlen(LED7_OFF), TRUE, TRUE)) == 0) {
          Io.LedOff(PIN_LED7);
          return;
        }
        if (SmsReceived.compareTo(LocalReadStringFLASH((uint8_t *)LED7_BLINK, strlen(LED7_BLINK), TRUE, TRUE)) == 0) {
          TimeOutBlinkLed = T_3SEC;       
          return;
        }      
    }
  }
}
//======================================================================
