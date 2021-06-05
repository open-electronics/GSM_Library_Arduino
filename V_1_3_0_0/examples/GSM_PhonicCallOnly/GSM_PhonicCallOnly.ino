/*********************************************************************
 *
 *       GSM Sketch - Management of Phonic calls Only
 *
 *********************************************************************
 * FileName:        GSM_Sms_PhonicCall.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  TimersInt.ino
 * Revision:        1.0.0
 * Date:            06/11/2018
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
const char CALL_PHONE_NUMBER[]      PROGMEM = "+393490000808";
//======================================================================

#define TRUE  0
#define FALSE 1

//======================================================================
//  State Machine defines
typedef void State;
typedef State (*Pstate)();

Pstate Input_Management;    //  States machine used to manage the digital inputs (P3 and P4)
//======================================================================

boolean ExecuteCall;
boolean StopCall;

//======================================================================
//  Timer 5 variables
uint8_t  DebouncingTimeOut;    //  Debouncing TimeOut for digital inputs
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
  Input_Management = Input_Idle;       //  Initializes states machine used to manage the digital inputs (P3 and P4)
  
  Gsm.EepromStartAddSetup();
  SetupTimer5();                                      //  Initialize timer 5
  SetInputPin();                                      //  Initialize digital input of shield board (P3 and P4)
  Io.SetOutputLed();                                  //  Sets I/O leds
  Io.SetOutputTrigger();                              //  Sets I/O triggers
  Io.CheckOutputLed();                                //  Checks I/O output (LEDS)
  Io.CheckOutputTrigger();                            //  Checks I/O output  

  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1
    
  Gsm.InitPowerON_GSM();                              //  Start GSM initialization     
  Gsm.UartFlag.Bit.EnableUartSM = 0;                  // |  
  Gsm.ExecuteUartState();                             // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                  // |
 
  delay(500);
  Isr.TimeOutWait = T_15SEC;  

  ExecuteCall = TRUE;
  StopCall    = FALSE;
}
//======================================================================

//======================================================================
void loop() {
  Gsm.ExecuteUartState();
  if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
    Gsm.InitGsmSendCmd();
    Gsm.InitGsmWaitAnswer();
  } else {  
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();

    if (ExecuteCall == TRUE) {
      ExecuteCall = FALSE;
      Gsm.ReadStringFLASH((uint8_t *)CALL_PHONE_NUMBER, (uint8_t *)PhoneBook.PhoneNumber, strlen(CALL_PHONE_NUMBER));
      PhonicCall.SetCmd_ATD();      
    }
    if (StopCall == TRUE) {
      StopCall = FALSE;
      PhonicCall.SetCmd_ATH(0);
    }
    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  } 
  
  DebouncingInput();          //  Debouncing digital inputs (P3 and P4)
  Input_Management();         //  State Machine to manage Digital input
}
//======================================================================
