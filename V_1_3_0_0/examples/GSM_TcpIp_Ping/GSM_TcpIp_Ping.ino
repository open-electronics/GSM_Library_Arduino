/*********************************************************************
 *
 *       GSM Sketch - TCP/IP Ping
 *
 *********************************************************************
 * FileName:        GSM_TcpIp_Ping.ino
 * Revision:        1.0.0
 * Date:            03/01/2021
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
 * Arduino Board:   Arduino Uno R3, Fishino Uno R2, Arduino Mega 2560 or Fishino Mega 2560
 *                  See file Io_GSM.h to define hardware and Arduino Board to use
 *
 * Description:     This sketch show how to use Ping command
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

#include <avr/eeprom.h>

#include "Uart_GSM.h"
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
GprsCmd_GSM       Gprs;
TcpIpCmd_GSM      TcpIp;
HttpCmd_GSM       Http;

#define TCP_IP_PING_REV     "1.0"

#define TRUE  0
#define FALSE 1
                        
//======================================================================
//  Below the list of codes used for the state machine used to manage the generic AT commands
#define CMD_ATQ_GMI                         0
#define CMD_ATQ_GMM                         1
#define CMD_ATQ_GMR                         2
#define CMD_AT_CIPPING                      3
#define CMD_AT_VIEW_ANSW                    4
//======================================================================

//======================================================================
const char STR_PING_TIME[]            PROGMEM = "The time expended to wait for the response for the ping request. Unit in ms: ";
const char STR_PING_TTL[]             PROGMEM = "The value of time to live of the response packet for the ping request: ";
  
#ifdef QUECTEL_M95
  const char STR_PING_OK[]              PROGMEM = "Ping OK";
  const char STR_PING_FINISHED[]        PROGMEM = "Ping Finished Normally";
  const char STR_PING_TIMEOUT[]         PROGMEM = "Ping failed -> Timeout for the ping request";
  const char STR_PING_STACK_BUSY[]      PROGMEM = "Ping failed -> Stack TCP/IP Busy";
  const char STR_PING_NOT_FOUND[]       PROGMEM = "Ping failed -> Host not found";
  const char STR_PING_CONTEXT_FAILED[]  PROGMEM = "Ping failed -> Failed to activate PDP Context";
  
  const char STR_PING_SENT[]            PROGMEM = "Total number of sending the ping request: ";
  const char STR_PING_RCVD[]            PROGMEM = "Total number of the ping requests that received the response: ";
  const char STR_PING_LOST[]            PROGMEM = "Total number of the ping requests that were timeout: ";
  const char STR_PING_MIN_T[]           PROGMEM = "The minimum response time. Unit in ms: ";
  const char STR_PING_MAX_T[]           PROGMEM = "The maximum response time. Unit in ms: ";
  const char STR_PING_AVG_T[]           PROGMEM = "The average response time. Unit in ms: ";
#endif
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
  const char STR_PING_OK[]              PROGMEM = "Ping OK. Ping Number: ";
  const char STR_PING_TIMEOUT[]         PROGMEM = "Ping failed -> Timeout for the ping request. Ping number: ";
#endif
//======================================================================


//======================================================================
typedef void State;
typedef State (*Pstate)();

//======================================================================

//======================================================================
//  Variables
uint8_t StateSendCmd;       //  Present State
uint8_t FutureStateCmd;     //  Future State
//======================================================================

//======================================================================
//  Timer 5 variables
uint16_t TimeOutWait;          //  TimeOut to manage pulse pressing
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.EepromStartAddSetup();
  Gprs.EepromStartAddSetup();
  TcpIp.EepromStartAddSetup();
  Http.EepromStartAddSetup();
  
  SetupTimer5();                                            //  Initialize timer 5
  SetOutputsPin();                                          //  Initialize digital outputs
  TestLed_Trigger();                                        //  Test digital outputs
  
  Isr.EnableLibInterrupt();                                 //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);          //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);                   //  Enables and configures the Hardware/Software UART 1

  Gprs.GprsFlag.Bit.EnableGprsFunctions = 1;                //  Enable GPRS functions
  TcpIp.TcpIpFlag.Bit.MultiIp           = SINGLE_IP_CONN;   //  Enable Single IP
  Gsm.InitPowerON_GSM();                                    //  Start GSM/GPRS initialization
  
  Gsm.UartFlag.Bit.EnableUartSM = 0;                        // |  
  Gsm.ExecuteUartState();                                   // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                        // |
            
  SetStartStateMachine();
            
  delay(500);
}
//======================================================================

//======================================================================
void loop() {
  ProcessStateMachineGsm();
  ProcessGprsTcpIpCmd();
}
//======================================================================

//======================================================================
void ProcessStateMachineGsm(void) {
  Gsm.ExecuteUartState();
  if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
    SetStartStateMachine();
    Gsm.InitGsmSendCmd();
    Gsm.InitGsmWaitAnswer();
  } else if (Gsm.GsmFlag.Bit.GprsInitInProgress == 1) {
    Gprs.InitGprsSendCmd();
    Gprs.InitGprsWaitAnswer(); 
  } else {
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();
      
    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }
}
//======================================================================

//======================================================================
void SetStartStateMachine(void) {
  StateSendCmd   = 0xFF;
  FutureStateCmd = CMD_ATQ_GMI;  
}
//======================================================================

//======================================================================
String LocalReadString_FLASH(uint8_t *FlashPointer, uint8_t Lenght, boolean PrintCR, boolean NoPrint) {
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
  }
  return(TempString); 
}
//======================================================================
