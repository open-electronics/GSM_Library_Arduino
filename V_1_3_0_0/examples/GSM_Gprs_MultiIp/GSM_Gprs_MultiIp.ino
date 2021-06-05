  /*********************************************************************
 *
 *       GSM Sketch - Management of GPRS communication
 *
 *********************************************************************
 * FileName:        GSM_Gprs.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  TimersInt.ino
 * Revision:        1.0.0
 * Date:            24/12/2018
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  Gprs_Gsm.h
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
 *  - GSM_Gprs_MultiIp  -> Main file project
 *  - DigitalInput      -> Contains code to manage digital inputs and the respective functions to manage it
 *  - DigitalOutput     -> Contains code to manage digital output and the respective functions to manage it
 *  - TimerInt          -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
 *                         With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
 *                         For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
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

Io_GSM            Io;
Isr_GSM           Isr;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
PhoneBookCmd_GSM  PhoneBook;
SecurityCmd_GSM   Security;
GprsCmd_GSM       Gprs;
TcpIpCmd_GSM      TcpIp;
HttpCmd_GSM       Http;

const char STR_TCP_UDP_NOT_CONNECTED[]     PROGMEM = "Tcp/Udp not connected yet";
const char STR_COMPOSE_STRING_DATA[]       PROGMEM = "Compose string data to send";

const char STR_PHP_FILE[]                  PROGMEM = "Test_1.php?";
const char STR_PHP_PAR_1[]                 PROGMEM = "Name=";
const char STR_PHP_PAR_2[]                 PROGMEM = "Age=";

const char STR_NAME[]                      PROGMEM = "Matteo";
const char STR_AGE[]                       PROGMEM = "42";

const char STR_PYTHON_TEST[]               PROGMEM = "Hello RaspberryPi";

#define TRUE  0
#define FALSE 1

//======================================================================
//  Below the list of codes used for the state machine used to manage the generic AT commands
#define CMD_ATQ_CIPSTATUS_CONN_0        0
#define CMD_ATQ_CIPSTATUS_CONN_1        1
#define CMD_AT_CIPSTART                 2
#define CMD_AT_CIPSEND_CONN_0_STEP_1    3
#define CMD_AT_CIPSEND_CONN_0_STEP_2    4
#define CMD_AT_CIPSEND_CONN_1_STEP_1    5
#define CMD_AT_CIPSEND_CONN_1_STEP_2    6
#define CMD_AT_CIPCLOSE                 7

#define CMD_AT_NOTHING                  255
//======================================================================

//======================================================================
//  State Machine defines
typedef void State;
typedef State (*Pstate)();

Pstate Input_Management;    //  States machine used to manage the digital inputs (P3 and P4)
//======================================================================

//======================================================================
//  Variables
uint8_t StateSendCmd;

uint8_t LastStateTCP_UDP_0;
uint8_t LastStateConn_0;
uint8_t LastStateTCP_UDP_1;
uint8_t LastStateConn_1;
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
  Input_Management = Input_Idle;                          //  Initializes states machine used to manage the digital inputs (P3 and P4)

  Gsm.EepromStartAddSetup();
  Gprs.EepromStartAddSetup();
  TcpIp.EepromStartAddSetup();
  
  SetupTimer5();                                            //  Initialize timer 5
  SetInputPin();                                            //  Initialize digital input of shield board (P3 and P4)
  SetOutputPin();                                           //  Initialize digital output
  TestLed_Trigger();                                        //  Test digital output

  Isr.EnableLibInterrupt();                                 //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);          //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);                   //  Enables and configures the Hardware/Software UART 1

  Gprs.GprsFlag.Bit.EnableGprsFunctions = 1;                //  Enable GPRS functions
  TcpIp.TcpIpFlag.Bit.MultiIp           = MULTI_IP_CONN;    //  Enable Multi IP
  Gsm.InitPowerON_GSM();                                    //  Start GSM/GPRS initialization

  Gsm.UartFlag.Bit.EnableUartSM = 0;                        // |  
  Gsm.ExecuteUartState();                                   // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                        // |
 
  delay(500);
  Isr.TimeOutWait = T_15SEC;  

  StateSendCmd = CMD_ATQ_CIPSTATUS_CONN_0;
}
//======================================================================

//======================================================================
void loop() {
  DebouncingInput();          //  Debouncing digital inputs (P3 and P4)
  Input_Management();         //  State Machine to manage Digital input
  
  ProcessStateMachineGsm();
  ProcessGprsTcpIpCmd();
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
  } else {  
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();
    
    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }
  
  PrintTcpUdpState();
}
//======================================================================

//======================================================================
void PrintTcpUdpState(void) {
  if (TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD != LastStateTCP_UDP_0) {
    LastStateTCP_UDP_0 = TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD;
    Serial.print("TCP/UDP - 0: ");
    Serial.println(TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD);
  }
  if (TcpIp.TcpIpStateFlag.BitState[0].ClientState != LastStateConn_0) {
    LastStateConn_0 = TcpIp.TcpIpStateFlag.BitState[0].ClientState;
    Serial.print("Client - 0: ");
    Serial.println(TcpIp.TcpIpStateFlag.BitState[0].ClientState);
  } 

  if (TcpIp.TcpIpStateFlag.BitState[1].State_TCP_UPD != LastStateTCP_UDP_1) {
    LastStateTCP_UDP_1 = TcpIp.TcpIpStateFlag.BitState[1].State_TCP_UPD;
    Serial.print("TCP/UDP - 1: ");
    Serial.println(TcpIp.TcpIpStateFlag.BitState[1].State_TCP_UPD);
  }
  if (TcpIp.TcpIpStateFlag.BitState[1].ClientState != LastStateConn_1) {
    LastStateConn_1 = TcpIp.TcpIpStateFlag.BitState[1].ClientState;
    Serial.print("Client - 1: ");
    Serial.println(TcpIp.TcpIpStateFlag.BitState[1].ClientState);
  }  
}
//======================================================================

//======================================================================
//  Function to send generic command in the loop()
void ProcessGprsTcpIpCmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && \ 
      (Gsm.GsmFlag.Bit.GsmInitInProgress    == 0) && (Gsm.GsmFlag.Bit.GprsInitInProgress == 0) && (Gsm.GsmFlag.Bit.HttpInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_3SEC;

      switch (StateSendCmd)
      {
        case CMD_ATQ_CIPSTATUS_CONN_0:
          TcpIp.SetCmd_AT_CIPSTATUS(0);
          StateSendCmd = CMD_AT_CIPSEND_CONN_0_STEP_1;
          break;
        case CMD_ATQ_CIPSTATUS_CONN_1:
          TcpIp.SetCmd_AT_CIPSTATUS(1);
          StateSendCmd = CMD_AT_CIPSEND_CONN_1_STEP_1;
          break;
        case CMD_AT_CIPSTART:
          TcpIp.SetCmd_AT_CIPSTART(TcpIp.TcpIpFlag.Bit.IndexConnection, 0, 0);
          StateSendCmd = CMD_ATQ_CIPSTATUS_CONN_0;
          break;
        case CMD_AT_CIPSEND_CONN_0_STEP_1:
          if ((TcpIp.TcpIpStateFlag.BitState[0].ClientState == CLIENT_CLOSED_CODE)  || \
              (TcpIp.TcpIpStateFlag.BitState[0].ClientState == CLIENT_INITIAL_CODE) || \
              (TcpIp.TcpIpStateFlag.BitState[0].ClientState == CLIENT_CONNECTING)) {
            LocalReadStringFLASH((uint8_t *)STR_TCP_UDP_NOT_CONNECTED, strlen(STR_TCP_UDP_NOT_CONNECTED), TRUE, FALSE);
            StateSendCmd = CMD_ATQ_CIPSTATUS_CONN_1;
            break;
          }
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          StateSendCmd = CMD_AT_CIPSEND_CONN_0_STEP_2;
          break;
        case CMD_AT_CIPSEND_CONN_0_STEP_2:
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetDataToSendConn_0());

            TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
            StateSendCmd = CMD_AT_CIPCLOSE;
            Isr.TimeOutWait = T_5SEC;
          }
          break;
        case CMD_AT_CIPSEND_CONN_1_STEP_1:
          if ((TcpIp.TcpIpStateFlag.BitState[1].ClientState == CLIENT_CLOSED_CODE)  || \
              (TcpIp.TcpIpStateFlag.BitState[1].ClientState == CLIENT_INITIAL_CODE) || \
              (TcpIp.TcpIpStateFlag.BitState[1].ClientState == CLIENT_CONNECTING)) {
            LocalReadStringFLASH((uint8_t *)STR_TCP_UDP_NOT_CONNECTED, strlen(STR_TCP_UDP_NOT_CONNECTED), TRUE, FALSE);
            StateSendCmd = CMD_ATQ_CIPSTATUS_CONN_0;
            break;
          }
          TcpIp.SetCmd_AT_CIPSEND(false, 1);
          StateSendCmd = CMD_AT_CIPSEND_CONN_1_STEP_2;          
          break;
        case CMD_AT_CIPSEND_CONN_1_STEP_2:
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetDataToSendConn_1());

            TcpIp.TcpIpFlag.Bit.IndexConnection = 1;
            StateSendCmd = CMD_AT_CIPCLOSE;
            Isr.TimeOutWait = T_5SEC;
          }
          break;              
        case CMD_AT_CIPCLOSE:
          if ((TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState == CLIENT_CLOSED_CODE) || \
              (TcpIp.TcpIpStateFlag.BitState[TcpIp.TcpIpFlag.Bit.IndexConnection].ClientState == CLIENT_INITIAL_CODE)) {
            LocalReadStringFLASH((uint8_t *)STR_TCP_UDP_NOT_CONNECTED, strlen(STR_TCP_UDP_NOT_CONNECTED), TRUE, FALSE);
            StateSendCmd = CMD_ATQ_CIPSTATUS_CONN_0;
            break;
          }
          TcpIp.SetCmd_AT_CIPCLOSE(0, TcpIp.TcpIpFlag.Bit.IndexConnection);
          StateSendCmd = CMD_ATQ_CIPSTATUS_CONN_0;
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
//  This function generate the string used for GET PHP request. See below.
//  Most of the strings used to compose this long string were saved in to
//  FLASH memory to save SRAM memory space
//
//  ------------------------------------------------------------
//  GET /Test_1.php?Name=Matteo&Age=42 HTTP/1.1<CR><LF>
//  Host:79.35.125.98:1080<CR><LF>
//  Connection:keep-alive<CR><LF>
//  <CR><LF><SUB>
//  ------------------------------------------------------------
uint8_t SetDataToSendConn_0(void) {
  uint8_t Index;
  
  LocalReadStringFLASH((uint8_t *)STR_COMPOSE_STRING_DATA, strlen(STR_COMPOSE_STRING_DATA), TRUE, FALSE);
  
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
  Gsm.ReadStringFLASH((uint8_t *)GET_STR, (uint8_t *)Gsm.GSM_Data_Array, strlen(GET_STR));
  Index = strlen(Gsm.GSM_Data_Array);            
  Gsm.ReadStringFLASH((uint8_t *)STR_PHP_FILE, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(STR_PHP_FILE));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.ReadStringFLASH((uint8_t *)STR_PHP_PAR_1, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(STR_PHP_PAR_1));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.ReadStringFLASH((uint8_t *)STR_NAME, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(STR_NAME));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.GSM_Data_Array[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)STR_PHP_PAR_2, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(STR_PHP_PAR_2));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.ReadStringFLASH((uint8_t *)STR_AGE, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(STR_AGE));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.GSM_Data_Array[Index++] = ASCII_SPACE;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_STR, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HTTP_STR));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.ReadStringFLASH((uint8_t *)HOST_STR, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(HOST_STR));
  Index = strlen(Gsm.GSM_Data_Array);
  Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteServerIp[0], sizeof(REMOTE_SERVER_IP_0));
  Gsm.GSM_Data_Array[Index++] = ASCII_COLON;
  Index += Gsm.LoadDataFromEepromIntoArray(&Gsm.GSM_Data_Array[Index], TcpIp.EepromAdd.StartAddRemoteServerPort[0], sizeof(REMOTE_SERVER_PORT_0));
  
  Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
  Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;       
  Gsm.ReadStringFLASH((uint8_t *)KEEP_ALIVE_STR, ((uint8_t *)Gsm.GSM_Data_Array + Index), strlen(KEEP_ALIVE_STR));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;

  return(Index);
}
//======================================================================

//======================================================================
uint8_t SetDataToSendConn_1(void) {
  uint8_t Index;
  
  LocalReadStringFLASH((uint8_t *)STR_COMPOSE_STRING_DATA, strlen(STR_COMPOSE_STRING_DATA), TRUE, FALSE);
  
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], sizeof(Gsm.GSM_Data_Array));
  Gsm.ReadStringFLASH((uint8_t *)STR_PYTHON_TEST, (uint8_t *)Gsm.GSM_Data_Array, strlen(STR_PYTHON_TEST));
  Index = strlen(Gsm.GSM_Data_Array);
  Gsm.GSM_Data_Array[Index++] = ASCII_CARRIAGE_RET;
  Gsm.GSM_Data_Array[Index++] = ASCII_LINE_FEED;  
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
    
  return(Index);
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
