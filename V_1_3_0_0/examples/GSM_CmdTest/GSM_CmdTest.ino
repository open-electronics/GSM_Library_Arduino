/*********************************************************************
 *
 *       GSM Sketch - Test Cmd
 *
 *********************************************************************
 * FileName:        GSM_Test.ino
 * Revision:        1.0.0
 * Date:            01/12/2018
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 * Arduino Board:   Arduino Uno R3, Fishino Uno R2, Arduino Mega 2560 or Fishino Mega 2560
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

#define TRUE  0
#define FALSE 1

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.EepromStartAddSetup();
  Io.SetOutputTrigger();                              //  Sets I/O triggers
  Io.SetOutputLed();                                  //  Sets I/O leds
  Io.CheckOutputTrigger();                            //  Checks I/O output
  Io.CheckOutputLed();                                //  Checks I/O output (LEDS)

  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1
  Gsm.InitPowerON_GSM();                              //  Start GSM initialization
  
  Gsm.UartFlag.Bit.EnableUartSM = 0;                  // |  
  Gsm.ExecuteUartState();                             // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                  // |

//  PhoneBook.PhoneNumber[0]  = '\"';
//  PhoneBook.PhoneNumber[1]  = '+';
//  PhoneBook.PhoneNumber[2]  = '3';
//  PhoneBook.PhoneNumber[3]  = '9';
//  PhoneBook.PhoneNumber[4]  = '3';
//  PhoneBook.PhoneNumber[5]  = '4';
//  PhoneBook.PhoneNumber[6]  = '9';
//  PhoneBook.PhoneNumber[7]  = '1';
//  PhoneBook.PhoneNumber[8]  = '5';
//  PhoneBook.PhoneNumber[9]  = '4';
//  PhoneBook.PhoneNumber[10] = '4';  
//  PhoneBook.PhoneNumber[11] = '8';
//  PhoneBook.PhoneNumber[12] = '0';
//  PhoneBook.PhoneNumber[13] = '8';
//  PhoneBook.PhoneNumber[14] = '\"';

//  PhoneBook.PhoneNumber[0]  = '+';
//  PhoneBook.PhoneNumber[1]  = '3';
//  PhoneBook.PhoneNumber[2]  = '9';
//  PhoneBook.PhoneNumber[3]  = '3';
//  PhoneBook.PhoneNumber[4]  = '4';
//  PhoneBook.PhoneNumber[5]  = '9';
//  PhoneBook.PhoneNumber[6]  = '1';
//  PhoneBook.PhoneNumber[7]  = '5';
//  PhoneBook.PhoneNumber[8]  = '4';
//  PhoneBook.PhoneNumber[9] = '4';  
//  PhoneBook.PhoneNumber[10] = '8';
//  PhoneBook.PhoneNumber[11] = '0';
//  PhoneBook.PhoneNumber[12] = '8';
  
//  PhoneBook.PhoneText[0] = '\"';
//  PhoneBook.PhoneText[1] = 'C';
//  PhoneBook.PhoneText[2] = 'i';
//  PhoneBook.PhoneText[3] = 'a';
//  PhoneBook.PhoneText[4] = 'o';
//  PhoneBook.PhoneText[5] = '\"';

//  PhoneBook.PhoneNumberType = 129;
  
  delay(500);
  Isr.TimeOutWait = T_15SEC;
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
    #ifdef ARDUINO_UNO_REV3
      Io.LedBlink(TRIGGER_2, 25, T_2SEC);
    #endif
    #ifdef ARDUINO_MEGA2560_REV3
      Io.LedBlink(PIN_LED9, 25, T_2SEC);
    #endif
  }

  Send_AT_Cmd();
}
//======================================================================

//======================================================================
void Send_AT_Cmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_2SEC;

      //Gsm.SetCmd_AT_CMIC(0, 2);
      //Gsm.SetCmd_AT_CLVL(80);
      //Gsm.SetCmd_AT_CRSL(80);
      //Gsm.SetCmd_AT_CALM(0,0);

      //PhoneBook.SetCmd_AT_CPBR(1,false);
      //PhoneBook.SetCmd_AT_CPBW(6,CPBW_WRITE_ENTRY);

      //PhonicCall.SetCmd_ATD();
      //PhonicCall.SetCmd_ATD_PhoneNumberMemory(6);

      //Sms.SetCmd_AT_CMGD(4,0);
      //Sms.SetCmd_AT_CMGR(1,STATE_NOT_CHANGE);
    }
  }  
}
//======================================================================
