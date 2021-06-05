/*********************************************************************
 *
 *       GSM Sketch - Management of HTTP communication
 *
 *********************************************************************
 * FileName:        GSM_Http_Thingspeak.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  TimersInt.ino
 * Revision:        1.0.0
 * Date:            25/01/2019
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  Gprs_Gsm.h
 *                  Http_gsm.h
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
 *  - GSM_Http_Thingspeak  -> Main file project
 *  - DigitalInput         -> Contains code to manage digital inputs and the respective functions to manage it
 *  - DigitalOutput        -> Contains code to manage digital output and the respective functions to manage it
 *  - TimerInt             -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
 *                            With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
 *                            For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
 **********************************************************************/

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

Io_GSM            Io;
Isr_GSM           Isr;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
PhoneBookCmd_GSM  PhoneBook;
SecurityCmd_GSM   Security;
GprsCmd_GSM       Gprs;
TcpIpCmd_GSM      TcpIp;
HttpCmd_GSM       Http;

#define TRUE  0
#define FALSE 1

const char STR_COMPOSE_STRING_DATA[]    PROGMEM = "Compose string data to send";

const char HTTP_THINGSPEAK_FIELD1[]     PROGMEM = "field1=";
const char HTTP_THINGSPEAK_FIELD2[]     PROGMEM = "field2=";

//======================================================================
//  Below the list of codes used for the state machine used to manage the generic AT commands
#define CMD_AT_HTTP_INIT                0
#define CMD_ATQ_HTTP_STATUS             1
#define CMD_AT_HTTP_PARA_STEP_1         2
#define CMD_AT_HTTP_PARA_STEP_2         3
#define CMD_AT_HTTP_ACTION              4
#define CMD_AT_HTTP_TERM                5

#define CMD_AT_NOTHING                  255
//======================================================================

//======================================================================
//  Variables
uint8_t StateSendCmd;

uint8_t DataField1;
uint8_t DataField2;
//======================================================================

//======================================================================
//  Timer 5 variables
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
  Gsm.EepromStartAddSetup();
  Gprs.EepromStartAddSetup();
  TcpIp.EepromStartAddSetup();
  Http.EepromStartAddSetup();
  
  SetupTimer5();                                      //  Initialize timer 5
  SetInputPin();                                      //  Initialize digital input of shield board (P3 and P4)
  SetOutputPin();                                     //  Initialize digital output
  TestLed_Trigger();                                  //  Test digital output

  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1

  Gprs.GprsFlag.Bit.EnableGprsFunctions = 1;          //  Enable GPRS functions
  Http.HttpFlag.Bit.EnableHttpFunctions = 1;          //  Enable HTTP functions
  Gsm.InitPowerON_GSM();                              //  Start GSM/GPRS initialization

  Gsm.UartFlag.Bit.EnableUartSM = 0;                  // |  
  Gsm.ExecuteUartState();                             // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                  // |
 
  delay(500);
  Isr.TimeOutWait = T_15SEC;  

  StateSendCmd = CMD_AT_HTTP_INIT;
  DataField1 = 0;
  DataField2 = 255;
}
//======================================================================

//======================================================================
void loop() {
  ProcessStateMachineGsm();
  ProcessHttpCmd();
}
//======================================================================

//======================================================================
void ProcessStateMachineGsm(void) {
  Gsm.ExecuteUartState();
  if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
    Gsm.InitGsmSendCmd();
    Gsm.InitGsmWaitAnswer();
  } else if (Gsm.GsmFlag.Bit.GprsInitInProgress == 1) {
    Gprs.InitGprsSendCmd();
    Gprs.InitGprsWaitAnswer();
  } else if (Gsm.GsmFlag.Bit.HttpInitInProgress == 1) {
    Http.InitHttpSendCmd();
    Http.InitHttpWaitAnswer();  
  } else {  
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();
    
    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }
}
//======================================================================

//======================================================================
//  Function to send HTTP command
void ProcessHttpCmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && \ 
      (Gsm.GsmFlag.Bit.GsmInitInProgress    == 0) && (Gsm.GsmFlag.Bit.GprsInitInProgress == 0) && (Gsm.GsmFlag.Bit.HttpInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_3SEC;

      switch (StateSendCmd)
      {
        case CMD_AT_HTTP_INIT:
          Http.SetCmd_AT_HTTP_INIT();
          StateSendCmd = CMD_ATQ_HTTP_STATUS;
          break;
        case CMD_ATQ_HTTP_STATUS:
          Http.SetCmd_AT_HTTP_STATUS();
          StateSendCmd = CMD_AT_HTTP_PARA_STEP_1;
          break;
        case CMD_AT_HTTP_PARA_STEP_1:
          Http.SetCmd_AT_HTTP_PARA(HTTP_PARAM_CID_CODE);
          StateSendCmd = CMD_AT_HTTP_PARA_STEP_2;
          break;
        case CMD_AT_HTTP_PARA_STEP_2:
          Http.HttpFlag.Bit.AddHttpParameters = 1;
          SetDataToHttpThingSpeak();
          Http.SetCmd_AT_HTTP_PARA(HTTP_PARAM_URL_CODE);
          StateSendCmd = CMD_AT_HTTP_ACTION;
          break;
        case CMD_AT_HTTP_ACTION:
          Http.SetCmd_AT_HTTP_ACTION(HTTP_ACTION_METHOD_GET);
          StateSendCmd = CMD_ATQ_HTTP_STATUS;
          Isr.TimeOutWait = T_30SEC;
          break;
        case CMD_AT_HTTP_TERM:
          Http.SetCmd_AT_HTTP_TERM();
          StateSendCmd = CMD_ATQ_HTTP_STATUS;
          break;
        case CMD_AT_NOTHING:
          break;
        default:
          break;
      }
    }
  }  
}
//======================================================================

//======================================================================
//  This function generate the string parameters used for GET HTTP request
//  to ThingSpeak web site. Most of the strings used to compose this long
//  string were saved in to FLASH memory to save SRAM memory space
//
//  ------------------------------------------------------------
//  &field1=xxx&Field2=xxx"
//  ------------------------------------------------------------
void SetDataToHttpThingSpeak(void) {
  uint8_t Index    = 0;
  uint8_t Count    = 0;

  LocalReadStringFLASH((uint8_t *)STR_COMPOSE_STRING_DATA, strlen(STR_COMPOSE_STRING_DATA), TRUE, FALSE);
  
  Gsm.ClearBuffer(&Http.UrlParameters[0], sizeof(Http.UrlParameters));
  
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD1, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD1));
  Index = strlen(Http.UrlParameters);
  Index += Gsm.ConvertNumberToString(DataField1, ((uint8_t *)(Http.UrlParameters) + Index), 3);
  DataField1 += 5;
 
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD2, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD2));
  Index = strlen(Http.UrlParameters);
  Index += Gsm.ConvertNumberToString(DataField2, ((uint8_t *)(Http.UrlParameters) + Index), 3);
  DataField2 -= 5;  
}
//======================================================================

//======================================================================
//  This function is used to print text on Arduino IDE Serial Monitor.
//  Useful during the debug of C code written
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
