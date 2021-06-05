/*********************************************************************
 *
 *       GSM Sketch - Hardware test
 *
 *********************************************************************
 * FileName:        GSM_HW_Test.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 * Revision:        1.0.0
 * Date:            27/01/2019
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  Gprs_Gsm.h
 *                  Http_gsm.h
 * Arduino Board:   Arduino Uno, Arduino Mega 2560 or Fishino Uno Fishino Mega 2560
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
 *  - GSM_HW_Test   -> Main file project
 *  - DigitalInput  -> Contains code to manage digital inputs and the respective functions to manage it
 *  - DigitalOutput -> Contains code to manage digital output and the respective functions to manage it
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

const char STR_P3_PRESSED[]     PROGMEM = "P3 push-button pressed";
const char STR_P3_RELEASED[]    PROGMEM = "P3 push-button released";

const char STR_P4_PRESSED[]     PROGMEM = "P4 push-button pressed";
const char STR_P4_RELEASED[]    PROGMEM = "P4 push-button released";

#define TRUE  0
#define FALSE 1

//======================================================================
//  State Machine defines
typedef void State;
typedef State (*Pstate)();

Pstate Input_Management;    //  States machine used to manage the digital inputs (P3 and P4)
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
//  Timer 0 variables
uint8_t  DebouncingTimeOut;    //  Debouncing TimeOut for digital inputs
uint16_t TimeOutWait;          //  TimeOut to manage pulse pressing
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Input_Management = Input_Idle;                          //  Initializes states machine used to manage the digital inputs (P3 and P4)
  
  Gsm.EepromStartAddSetup();
  Gprs.EepromStartAddSetup();
  TcpIp.EepromStartAddSetup();
  Http.EepromStartAddSetup();

  SetupTimer5();
  SetInputPin();                                      //  Initialize digital input of shield board (P3 and P4)
  SetOutputPin();                                     //  Initialize digital output
  TestLed_Trigger();                                  //  Test digital output

  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1
}
//======================================================================

//======================================================================
void loop() {
  DebouncingInput();          //  Debouncing digital inputs (P3 and P4)
  Input_Management();         //  State Machine to manage Digital input
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
