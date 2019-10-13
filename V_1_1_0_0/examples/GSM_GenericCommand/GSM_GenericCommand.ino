/*********************************************************************
 *
 *       GSM Sketch - Test Generic Command
 *
 *********************************************************************
 * FileName:        GSM_Test.ino
 * Revision:        1.0.0
 * Date:            08/08/2018
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

#define ATQ_CPIN_CMD  0
#define ATQ_CREG_CMD  1
#define ATQ_CSQ_CMD   2
#define ATQ_CPAS_CMD  3
#define ATQ_COPS_CMD  4
#define ATQ_GMI_CMD   5
#define ATQ_GMM_CMD   6
#define ATQ_GMR_CMD   7
#define ATQ_GSN_CMD   8
#define ATQ_CCLK_CMD  9

#define PRINT_DATA_CPIN_CMD  1
#define PRINT_DATA_CREG_CMD  2
#define PRINT_DATA_CSQ_CMD   3
#define PRINT_DATA_CPAS_CMD  4
#define PRINT_DATA_COPS_CMD  5
#define PRINT_DATA_GMI_CMD   6
#define PRINT_DATA_GMM_CMD   7
#define PRINT_DATA_GMR_CMD   8
#define PRINT_DATA_GSN_CMD   9
#define PRINT_DATA_CCLK_CMD  0

//======================================================================
const char STR_END[]            PROGMEM = "==============================";
const char STR_CPIN_START[]     PROGMEM = "============ CPIN ============";
const char STR_CREG_START[]     PROGMEM = "============ CREG ============";
const char STR_CSQ_START[]      PROGMEM = "============ CSQ  ============";
const char STR_CPAS_START[]     PROGMEM = "============ CPAS ============";
const char STR_COPS_START[]     PROGMEM = "============ COPS ============";
const char STR_GMI_START[]      PROGMEM = "============ GMI  ============";
const char STR_GMM_START[]      PROGMEM = "============ GMM  ============";
const char STR_GMR_START[]      PROGMEM = "============ GMR  ============";
const char STR_GSN_START[]      PROGMEM = "============ GSN  ============";
const char STR_CCLK_START[]     PROGMEM = "============ CCLK ============";

const char STR_SIM_READY[]      PROGMEM = "SIM OK";
const char STR_SIM_PIN[]        PROGMEM = "PIN CODE required";
const char STR_SIM_PUK[]        PROGMEM = "PUK CODE required";

const char STR_CREG_STAT_0[]    PROGMEM = "Not registered, ME is not currently searching a new operator to register to";
const char STR_CREG_STAT_1[]    PROGMEM = "Registered, home network";
const char STR_CREG_STAT_2[]    PROGMEM = "Not registered, but ME is currently searching a new operator to register to";
const char STR_CREG_STAT_3[]    PROGMEM = "Registration denied";
const char STR_CREG_STAT_4[]    PROGMEM = "Unknown";
const char STR_CREG_STAT_5[]    PROGMEM = "Registered, roaming";

const char STR_CREG_LAC[]       PROGMEM = "Location area: ";
const char STR_CREG_CI[]        PROGMEM = "Cell ID: ";

const char STR_CSQ_RSSI[]       PROGMEM = "Rssi: ";
const char STR_CSQ_BER[]        PROGMEM = "Ber: ";

const char STR_CPAS_0[]         PROGMEM = "Ready (MT allows commands from TA/TE)";
const char STR_CPAS_2[]         PROGMEM = "Unknown (MT is not guaranteed to respond to instructions)";
const char STR_CPAS_3[]         PROGMEM = "Ringing (MT is ready for commands from TA/TE, but the ringing is active)";
const char STR_CPAS_4[]         PROGMEM = "Call in progress (MT is ready for commands from TA/TE, but a calla is in progress)";

const char STR_COPS_MODE_0[]    PROGMEM = "Automatic mode; <oper> field is ignored";
const char STR_COPS_MODE_1[]    PROGMEM = "Manual operator selection; <oper> field shall be present";
const char STR_COPS_MODE_2[]    PROGMEM = "Manual deregister from network";
const char STR_COPS_MODE_3[]    PROGMEM = "Set only <format> (for read Command +COPS?) – not shown in Read Command response";
const char STR_COPS_MODE_4[]    PROGMEM = "Manual/automatic selected; if manual selection fails, automatic mode (<mode>=0) is entered";

const char STR_COPS_FORMAT_0[]  PROGMEM = "Long format alphanumeric <oper>; can be up to 16 characters longd";
const char STR_COPS_FORMAT_1[]  PROGMEM = "Short format alphanumeric <oper>";
const char STR_COPS_FORMAT_2[]  PROGMEM = "Numeric <oper>; GSM Location Area Identification number";

const char STR_COPS_OPERATOR[]  PROGMEM = "Operator name: ";

const char STR_GMI[]            PROGMEM = "Manufacturer Identification: ";
const char STR_GMM[]            PROGMEM = "TA Model Identification: ";
const char STR_GMR[]            PROGMEM = "TA Revision Identification of Software Release: ";
const char STR_GSN[]            PROGMEM = "TA Serial Number Identification (IMEI): ";

const char STR_CCLK_DATE[]      PROGMEM = "Date: ";
const char STR_CCLK_TIME[]      PROGMEM = "Time: ";
const char STR_CCLK_GMT[]       PROGMEM = "GMT: ";

//======================================================================

//======================================================================
//  Variables
uint8_t Counter;
uint8_t StateSendCmd;
bool    PrintData;
char    Temp[20];
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.PswdEepromStartAddSetup();
  
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

  delay(500);
  Isr.TimeOutWait = T_15SEC;
 
  Sms.SmsReceivedIndex  = 0;
  Sms.SmsWriteMemoryAdd = 0;
  StateSendCmd          = 0;
  PrintData = FALSE;
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
  PrintDataReceived();
}
//======================================================================

//======================================================================
void Send_AT_Cmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;

      switch (StateSendCmd)
      {
        case ATQ_CPIN_CMD:
          Security.SetCmd_ATQ_CPIN();
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_CREG_CMD:
          Gsm.SetCmd_ATQ_CREG();
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_CSQ_CMD:
          Gsm.SetCmd_ATQ_CSQ();
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_CPAS_CMD:
          Gsm.SetCmd_ATQ_CPAS();
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_COPS_CMD:
          Gsm.SetCmd_ATQ_COPS();
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_GMI_CMD:
          Gsm.SetCmd_ATQ_GMI(GMI_V25_FORMAT);
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_GMM_CMD:
          Gsm.SetCmd_ATQ_GMM(GMM_V25_FORMAT);
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_GMR_CMD:
          Gsm.SetCmd_ATQ_GMR(GMR_V25_FORMAT);
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_GSN_CMD:
          Gsm.SetCmd_ATQ_GSN(GSN_V25_FORMAT);
          PrintData = TRUE;
          StateSendCmd++;
          break;
        case ATQ_CCLK_CMD:
          Gsm.SetCmd_AT_CCLK(true);
          PrintData = TRUE;
          StateSendCmd = 0;
          break; 
        default:
          break;
      }
    }
  }  
}
//======================================================================

//======================================================================
void PrintDataReceived(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
      switch (StateSendCmd)
      {
        case PRINT_DATA_CPIN_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_CPIN_START, strlen(STR_CPIN_START), TRUE, FALSE);
            if (Security.SecurityFlag.Bit.SIM_Status == 0) {
              LocalReadStringFLASH((uint8_t *)STR_SIM_READY, strlen(STR_SIM_READY), TRUE, FALSE);
            }
            if (Security.SecurityFlag.Bit.SIM_Status == 1) {
              LocalReadStringFLASH((uint8_t *)STR_SIM_PIN, strlen(STR_SIM_PIN), TRUE, FALSE);
            }
            if (Security.SecurityFlag.Bit.SIM_Status == 2) {
              LocalReadStringFLASH((uint8_t *)STR_SIM_PUK, strlen(STR_SIM_PUK), TRUE, FALSE);
            }
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE);          
          }
          break;
        case PRINT_DATA_CREG_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_CREG_START, strlen(STR_CREG_START), TRUE, FALSE);
            if (Gsm.CREG_Info.Flag.Bit.Stat == 0) {
              LocalReadStringFLASH((uint8_t *)STR_CREG_STAT_0, strlen(STR_CREG_STAT_0), TRUE, FALSE);  
            }
            if (Gsm.CREG_Info.Flag.Bit.Stat == 1) {
              LocalReadStringFLASH((uint8_t *)STR_CREG_STAT_1, strlen(STR_CREG_STAT_1), TRUE, FALSE);  
            }            
            if (Gsm.CREG_Info.Flag.Bit.Stat == 2) {
              LocalReadStringFLASH((uint8_t *)STR_CREG_STAT_2, strlen(STR_CREG_STAT_2), TRUE, FALSE);  
            }            
            if (Gsm.CREG_Info.Flag.Bit.Stat == 3) {
              LocalReadStringFLASH((uint8_t *)STR_CREG_STAT_3, strlen(STR_CREG_STAT_3), TRUE, FALSE);  
            }
            if (Gsm.CREG_Info.Flag.Bit.Stat == 4) {
              LocalReadStringFLASH((uint8_t *)STR_CREG_STAT_4, strlen(STR_CREG_STAT_4), TRUE, FALSE);  
            }
            if (Gsm.CREG_Info.Flag.Bit.Stat == 5) {
              LocalReadStringFLASH((uint8_t *)STR_CREG_STAT_5, strlen(STR_CREG_STAT_5), TRUE, FALSE);  
            }
            LocalReadStringFLASH((uint8_t *)STR_CREG_LAC, strlen(STR_CREG_LAC), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.CREG_Info.LAC_Info[0], strlen(Gsm.CREG_Info.LAC_Info), TRUE); 
            LocalReadStringFLASH((uint8_t *)STR_CREG_CI, strlen(STR_CREG_CI), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.CREG_Info.CI_Info[0], strlen(Gsm.CREG_Info.CI_Info), TRUE);
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_CSQ_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_CSQ_START, strlen(STR_CSQ_START), TRUE, FALSE);
            LocalReadStringFLASH((uint8_t *)STR_CSQ_RSSI, strlen(STR_CSQ_RSSI), FALSE, FALSE);
            Serial.println(Gsm.CSQ_Info.Rssi);
            LocalReadStringFLASH((uint8_t *)STR_CSQ_BER, strlen(STR_CSQ_BER), FALSE, FALSE);
            Serial.println(Gsm.CSQ_Info.Ber);
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_CPAS_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_CPAS_START, strlen(STR_CPAS_START), TRUE, FALSE);
            if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
              LocalReadStringFLASH((uint8_t *)STR_CPAS_0, strlen(STR_CPAS_0), TRUE, FALSE);              
            }
            if (Gsm.SimFlag.Bit.PhoneActivityStatus == 2) {
              LocalReadStringFLASH((uint8_t *)STR_CPAS_2, strlen(STR_CPAS_2), TRUE, FALSE);
            }
            if (Gsm.SimFlag.Bit.PhoneActivityStatus == 3) {
              LocalReadStringFLASH((uint8_t *)STR_CPAS_3, strlen(STR_CPAS_3), TRUE, FALSE);
            }
            if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
              LocalReadStringFLASH((uint8_t *)STR_CPAS_4, strlen(STR_CPAS_4), TRUE, FALSE);
            }
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE);          
          }
          break;
        case PRINT_DATA_COPS_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_COPS_START, strlen(STR_COPS_START), TRUE, FALSE);
            if (Gsm.SimFlag.Bit.OperatorSelMode == 0) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_MODE_0, strlen(STR_COPS_MODE_0), TRUE, FALSE); 
            }
            if (Gsm.SimFlag.Bit.OperatorSelMode == 1) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_MODE_1, strlen(STR_COPS_MODE_1), TRUE, FALSE); 
            }
            if (Gsm.SimFlag.Bit.OperatorSelMode == 2) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_MODE_2, strlen(STR_COPS_MODE_2), TRUE, FALSE); 
            }
            if (Gsm.SimFlag.Bit.OperatorSelMode == 3) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_MODE_3, strlen(STR_COPS_MODE_3), TRUE, FALSE); 
            }
            if (Gsm.SimFlag.Bit.OperatorSelMode == 4) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_MODE_4, strlen(STR_COPS_MODE_4), TRUE, FALSE); 
            }

            if (Gsm.SimFlag.Bit.OperatorSelFormat == 0) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_FORMAT_0, strlen(STR_COPS_FORMAT_0), TRUE, FALSE);  
            }
            if (Gsm.SimFlag.Bit.OperatorSelFormat == 1) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_FORMAT_1, strlen(STR_COPS_FORMAT_1), TRUE, FALSE);  
            }
            if (Gsm.SimFlag.Bit.OperatorSelFormat == 2) {
              LocalReadStringFLASH((uint8_t *)STR_COPS_FORMAT_2, strlen(STR_COPS_FORMAT_2), TRUE, FALSE); 
            }
            LocalReadStringFLASH((uint8_t *)STR_COPS_OPERATOR, strlen(STR_COPS_OPERATOR), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.OperatorName[0], strlen(Gsm.OperatorName), TRUE);                       
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_GMI_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_GMI_START, strlen(STR_GMI_START), TRUE, FALSE);
            LocalReadStringFLASH((uint8_t *)STR_GMI, strlen(STR_GMI), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.ME_Info.GMI_Info[0], strlen(Gsm.ME_Info.GMI_Info), TRUE);                       
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_GMM_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_GMM_START, strlen(STR_GMM_START), TRUE, FALSE);
            LocalReadStringFLASH((uint8_t *)STR_GMM, strlen(STR_GMM), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.ME_Info.GMM_Info[0], strlen(Gsm.ME_Info.GMM_Info), TRUE);                       
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_GMR_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_GMR_START, strlen(STR_GMR_START), TRUE, FALSE);
            LocalReadStringFLASH((uint8_t *)STR_GMR, strlen(STR_GMR), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.ME_Info.GMR_Info[0], strlen(Gsm.ME_Info.GMR_Info), TRUE);                       
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_GSN_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_GSN_START, strlen(STR_GSN_START), TRUE, FALSE);
            LocalReadStringFLASH((uint8_t *)STR_GSN, strlen(STR_GSN), FALSE, FALSE);
            LocalPrintString((uint8_t *)&Gsm.ME_Info.IMEI_Info[0], strlen(Gsm.ME_Info.IMEI_Info), TRUE);                       
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break;
        case PRINT_DATA_CCLK_CMD:
          if (PrintData == TRUE) {
            PrintData = FALSE;
            LocalReadStringFLASH((uint8_t *)STR_CCLK_START, strlen(STR_CCLK_START), TRUE, FALSE);
            LocalReadStringFLASH((uint8_t *)STR_CCLK_DATE, strlen(STR_CCLK_DATE), FALSE, FALSE);
            sprintf(Temp, "%02d/%02d/%02d", Gsm.Clock_Info.Clock.Bit.Day, Gsm.Clock_Info.Clock.Bit.Month, Gsm.Clock_Info.Clock.Bit.Year);
            Serial.println(Temp);
            LocalReadStringFLASH((uint8_t *)STR_CCLK_TIME, strlen(STR_CCLK_TIME), FALSE, FALSE);
            sprintf(Temp, "%02d:%02d:%02d", Gsm.Clock_Info.Clock.Bit.Hours, Gsm.Clock_Info.Clock.Bit.Minutes, Gsm.Clock_Info.Clock.Bit.Seconds);
            Serial.println(Temp);
            LocalReadStringFLASH((uint8_t *)STR_CCLK_GMT, strlen(STR_CCLK_GMT), FALSE, FALSE);                      
            Serial.println(Gsm.Clock_Info.Clock.Bit.GMT);
            LocalReadStringFLASH((uint8_t *)STR_END, strlen(STR_END), TRUE, FALSE); 
          }
          break; 
        default:
          break;
      }  
  }
}
//======================================================================



//======================================================================
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
void LocalPrintString(uint8_t *SramPointer, uint8_t Lenght, boolean PrintCR) {
  uint8_t k;
  
  for (k = 0; k < Lenght; k++) {
    Serial.print(char(*(uint8_t *)SramPointer++));
  }
  if (PrintCR == TRUE) {
    Serial.print("\n");
  }
}
//======================================================================
