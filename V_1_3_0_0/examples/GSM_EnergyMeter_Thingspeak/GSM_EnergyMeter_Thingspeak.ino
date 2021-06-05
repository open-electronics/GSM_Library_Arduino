/*********************************************************************
 *
 *       GSM Sketch - Energy Meter and Thingspeak
 *
 *********************************************************************
 * FileName:        GSM_Http_Thingspeak.ino
 *                  DigitalInput.ino
 *                  DigitalOutput.ino
 *                  Measures.ino
 *                  TimersInt.ino
 *                  
 * Revision:        1.0.0
 * Date:            23/11/2019
 * 
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  GenericCmd_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  Gprs_Gsm.h
 *                  Http_gsm.h
 *                  Cmd_MCP39F511.h
 *                  Io_MCP39F511.h
 *                  Isr_MCP39F511.h
 *                  Uart_MCP39F511.h
 *                  
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
 *  - GSM_EnergyMeter_Thingspeak  -> Main file project
 *  - DigitalInput                -> Contains code to manage digital inputs and the respective functions to manage it
 *  - DigitalOutput               -> Contains code to manage digital output and the respective functions to manage it
 *  - Measures                    -> Contains code to manage MCP39F511 device for electric measures
 *  - TimerInt                    -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
 *                                   With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
 *                                   For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
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

#include "Cmd_MCP39F511.h"
#include "Io_MCP39F511.h"
#include "Isr_MCP39F511.h"
#include "Uart_MCP39F511.h"

Io_GSM            Io;
Isr_GSM           Isr;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
PhoneBookCmd_GSM  PhoneBook;
SecurityCmd_GSM   Security;
GprsCmd_GSM       Gprs;
TcpIpCmd_GSM      TcpIp;
HttpCmd_GSM       Http;

Cmd_MCP39F511     Cmd_39F511;

#define TRUE  0
#define FALSE 1

#define SKETCH_THING_SPEAK_REV        "1.0 \n"

#define TIMEOUT_THINGSPEAK_SEND_DATA  30  //  Values accepted are 20 sec, 30 sec or 60 sec

//**************************************************************************************************************************************************
//  Energy Meter settings (MCP39F511)
//
//======================================================================
//  MCP39F511  HARDWARE DEFINE (INPUT AND OUTPUT)

//--------------------------------------------
//  Directives used to manage Output
#define PIN_RELAY_1               22      //  Relay 1      mapped on the I/O 22 (Pin 75)
#define PIN_RELAY_2               23      //  Relay 2      mapped on the I/O 23 (Pin 76)
#define PIN_BUZZER                44      //  Buzzer       mapped on the I/O 44 (Pin 53)
#define PIN_LED_1                 24      //  Red Led 1    mapped on the I/O 24 (Pin 73)
#define PIN_LED_2                 25      //  Led Yellow 2 mapped on the I/O 25 (Pin 74)
#define PIN_LED_3                 26      //  Led Green 3  mapped on the I/O 26 (Pin 71)
#define PIN_LED_4                 27      //  Led Green 4  mapped on the I/O 27 (Pin 72)

#define OUTPUT_STATE_OFF           0
#define OUTPUT_STATE_ON            1
#define OUTPUT_STATE_TOGGLE        2
#define OUTPUT_STATE_TOGGLE_WAIT   3
#define OUTPUT_STATE_TOGGLE_ONLY   4
#define OUTPUT_STATE_NOTHING       255

#define SELECT_RELAY_1             0
#define SELECT_RELAY_2             1
#define SELECT_BUZZER              2
#define SELECT_LED_1               3
#define SELECT_LED_2               4
#define SELECT_LED_3               5
#define SELECT_LED_4               6

#define OUTPUT_OFF                 0
#define OUTPUT_ON                  1
//--------------------------------------------

//--------------------------------------------
#define IN_P1                     42    //  P1 Input mapped on the I/O 42 (Pin 55)
#define IN_P2                     43    //  P2 Input mapped on the I/O 43 (Pin 56)

#define LEVEL_HIGH_INPUT          0
#define LEVEL_LOW_INPUT           1
#define TOGGLE_INPUT              2

#define INPUT_STATE_IDLE          0          
#define INPUT_STATE_WAIT          1
//--------------------------------------------
//======================================================================

//======================================================================
//  CONST STRING
const char STR_SKETCH_REV[]               PROGMEM = "GSM Energy Meter ThingSpeak Sketch Rev: ";
const char STR_REV[]                      PROGMEM = "MCP39F511 Device Rev: ";
const char STR_NEGATIVE_ACTIVE_POWER[]    PROGMEM = "Active power is negative (export) and is in quadrants 2,3\n";
const char STR_POSITIVE_ACTIVE_POWER[]    PROGMEM = "Active power is positive (import) and is in quadrants 1,4\n";
const char STR_NEGATIVE_REACTIVE_POWER[]  PROGMEM = "Reactive power is negative (capacitive load) and is in quadrants 3,4\n";
const char STR_POSITIVE_REACTIVE_POWER[]  PROGMEM = "Reactive power is positive and is in quadrants 1,2\n";
const char STR_ZCD[]                      PROGMEM = "Zero Cross Detected!\n";
const char STR_ZCD_ERROR[]                PROGMEM = "Zero Cross Failure!\n";
const char STR_VOLTAGE[]                  PROGMEM = "Instant Value Vrms: ";
const char STR_CURRENT[]                  PROGMEM = "Instant Value Irms: ";
const char STR_FREQUENCY[]                PROGMEM = "Instant Value Freq: ";
const char STR_POWER_FACTOR[]             PROGMEM = "Instant Value PF:   ";
const char STR_APPARENT_POWER[]           PROGMEM = "Instant Value S:    ";
const char STR_ACTIVE_POWER[]             PROGMEM = "Instant Value P:    ";
const char STR_REACTIVE_POWER[]           PROGMEM = "Instant Value Q:    ";

#ifdef ENABLE_AVERAGE_CALC 
  const char STR_AVERAGE_VOLTAGE[]          PROGMEM = "Average Value Vrms: ";
  const char STR_AVERAGE_CURRENT[]          PROGMEM = "Average Value Irms: ";
  const char STR_AVERAGE_FREQUENCY[]        PROGMEM = "Average Value Freq: ";
  const char STR_AVERAGE_POWER_FACTOR[]     PROGMEM = "Average Value PF:   ";
  const char STR_AVERAGE_APPARENT_POWER[]   PROGMEM = "Average Value S:    ";
  const char STR_AVERAGE_ACTIVE_POWER[]     PROGMEM = "Average Value P:    ";
  const char STR_AVERAGE_REACTIVE_POWER[]   PROGMEM = "Average Value Q:    ";
#endif
const char STR_THRESHOLD[]                PROGMEM = "Power Threshold selected: ";
//======================================================================

//======================================================================
typedef void State;
typedef State (*Pstate)();

Pstate ReadDataMCP39F511;       //  State Machine used to read data from MCP39F511
//======================================================================

//======================================================================
//
uint8_t OutputPin[7];
uint8_t LastStateOutputPin[7];
uint8_t InputPin[2];

union DigitalInput {
  struct {
    uint8_t  InputStatus     :1;
    uint8_t  InputReaded     :1;
    uint8_t  InputVar        :1;
    uint8_t  LastInputStatus :1;
    uint8_t  Free            :4;
  } Bit;
} DigitalInput[2];
//======================================================================
//**************************************************************************************************************************************************


//**************************************************************************************************************************************************
//  GSM Settings
const char STR_COMPOSE_STRING_DATA[]    PROGMEM = "Compose string data to send";

const char HTTP_THINGSPEAK_FIELD1[]     PROGMEM = "field1=";    //  Voltage
const char HTTP_THINGSPEAK_FIELD2[]     PROGMEM = "field2=";    //  Current
const char HTTP_THINGSPEAK_FIELD3[]     PROGMEM = "field3=";    //  Power Factor
const char HTTP_THINGSPEAK_FIELD4[]     PROGMEM = "field4=";    //  Active Power
const char HTTP_THINGSPEAK_FIELD5[]     PROGMEM = "field5=";    //  Reactive Power
const char HTTP_THINGSPEAK_FIELD6[]     PROGMEM = "field6=";    //  Apparent Power
const char HTTP_THINGSPEAK_FIELD7[]     PROGMEM = "field7=";    //  Free. Not Used
const char HTTP_THINGSPEAK_FIELD8[]     PROGMEM = "field8=";    //  Free. Not Used

//======================================================================
//  Below the list of codes used for the state machine used to manage the generic AT commands
#define CMD_AT_HTTP_INIT                0
#define CMD_ATQ_HTTP_STATUS_1           1
#define CMD_AT_HTTP_PARA_STEP_1         2
#define CMD_AT_HTTP_PARA_STEP_2         3
#define CMD_AT_HTTP_ACTION              4
#define CMD_ATQ_HTTP_STATUS_2           5
#define CMD_AT_HTTP_TERM                6

#define CMD_AT_NOTHING                  255
//======================================================================

//======================================================================
//  I/O Input pin define
uint8_t P3_Button = 9;         //  Digital input. Button P3. See schematic
uint8_t P4_Button = 10;        //  Digital input. Button P4. See schematic
//======================================================================

//======================================================================
//  Variables
uint8_t LocalStateSendCmd;
//======================================================================
//**************************************************************************************************************************************************

//======================================================================
//  Timer 5 variables (GSM Energy Meter Shield)
uint16_t  TimeOutReadData;
uint16_t  TimeOutPrintData;
uint16_t  TimeOutHttpAction;
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.EepromStartAddSetup();
  Gprs.EepromStartAddSetup();
  TcpIp.EepromStartAddSetup();
  Http.EepromStartAddSetup();
  
  SetupTimer5();                                      //  Initialize timer 5
  SetInputGsmPin();                                   //  Initialize digital input of the GSM shield board
  SetOutputGsmPin();                                  //  Initialize digital output of the GSM shield board
  TestGsmLed_Trigger();                               //  Test digital output of the GSM shield board
  
  SetInputEnergyMeterPin();                           //  Initialize digital input of the Energy Meter board
  SetOutputEnergyMeterPin();                          //  Initialize digital output of the Energy Meter board
  TestEnergyMeter_BuzzerLed();                        //  Test digital output of the Energy Meter shield board

  Isr.Ext_Timer1_Vector(Isr_39F511.Timer1_Interrupt); //  Sets system to manage Timer1 ISR for 39F511 library (ISR Timer1 vector is alr4eady used by GSM library)

  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);             //  Enables and configures the Hardware/Software UART 1
  
  Isr.EnableLibInterrupt();                           //  Enables GSM Library Interrupt
  
  Uart_39F511.SetBaudRateUart(true, BAUD_115200);     // Enables and configure MCP39F511 UART
  Isr_39F511.EnableLibInterrupt();                    // Enables Energy Meter Library Interrupt
  Io_39F511.MCP39F511_SetControlLine();
  Io_39F511.MCP39F511_MclrLine(1);
  delay(500);
  Io_39F511.MCP39F511_ResetLine(1);

  Gprs.GprsFlag.Bit.EnableGprsFunctions = 1;          //  Enable GPRS functions
  Http.HttpFlag.Bit.EnableHttpFunctions = 1;          //  Enable HTTP functions
  Gsm.InitPowerON_GSM();                              //  Start GSM/GPRS initialization

  Gsm.UartFlag.Bit.EnableUartSM = 0;                  // |  
  Gsm.ExecuteUartState();                             // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1;                  // |
 
  delay(500);
  Isr.TimeOutWait  = T_120SEC;  
  TimeOutReadData  = T_2SEC;
  TimeOutPrintData = T_10SEC;

  DigitalInput[0].Bit.InputStatus = 1;
  DigitalInput[1].Bit.InputStatus = 1;  

  //----------------------------------------------- 
  //  Sets State Machine
  ReadDataMCP39F511 = ReadDataIdle;

  LocalStateSendCmd = CMD_AT_HTTP_INIT;
  //----------------------------------------------- 
  
  //-----------------------------------------------
  //  Prints sketch revision
  LocalReadString_FLASH((uint8_t *)STR_SKETCH_REV, strlen(STR_SKETCH_REV), FALSE, FALSE);
  Serial.println(SKETCH_THING_SPEAK_REV);
  //-----------------------------------------------    
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

    EnergyMeterReadData();   
    EnergyMeterPrintData();
      
    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }
}
//======================================================================

//======================================================================
void EnergyMeterReadData(void) {
  ReadDataMCP39F511();            //  State machine used to read measures from MCP39F511
  
  if ((TimeOutReadData == 0) && (Cmd_39F511.GenericFlags.ReadDataInProgress == 0)) {
    TimeOutReadData = T_2SEC;
    ReadDataMCP39F511 = ReadStatus;   //  Set state machine to start to read data from MCP39F511
  }
}
void EnergyMeterPrintData(void) {
  if ((TimeOutPrintData == 0) && (Cmd_39F511.GenericFlags.ReadDataInProgress == 0)) {
    TimeOutPrintData = T_5SEC;
    PrintDataOnSerialMonitor();
  }  
}
//======================================================================

//======================================================================
//  Function to send HTTP command
void ProcessHttpCmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && \ 
      (Gsm.GsmFlag.Bit.GsmInitInProgress    == 0) && (Gsm.GsmFlag.Bit.GprsInitInProgress == 0) && (Gsm.GsmFlag.Bit.HttpInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;

      switch (LocalStateSendCmd)
      {
        case CMD_AT_HTTP_INIT:
          Http.SetCmd_AT_HTTP_INIT();
          LocalStateSendCmd = CMD_ATQ_HTTP_STATUS_1;
          break;
        case CMD_ATQ_HTTP_STATUS_1:
          Http.SetCmd_AT_HTTP_STATUS();
          LocalStateSendCmd = CMD_AT_HTTP_PARA_STEP_1;
          break;
        case CMD_AT_HTTP_PARA_STEP_1:
          if (Http.HttpFlag.Bit.HttpStatus > 0) {
            Http.SetCmd_AT_HTTP_STATUS();
            break;
          }
          Io.LedOn(PIN_LED6);
          Http.SetCmd_AT_HTTP_PARA(HTTP_PARAM_CID_CODE);
          LocalStateSendCmd = CMD_AT_HTTP_PARA_STEP_2;
          break;
        case CMD_AT_HTTP_PARA_STEP_2:
          Http.HttpFlag.Bit.AddHttpParameters = 1;
          SetDataToHttpThingSpeak();
          Http.SetCmd_AT_HTTP_PARA(HTTP_PARAM_URL_CODE);
          LocalStateSendCmd = CMD_AT_HTTP_ACTION;
          break;
        case CMD_AT_HTTP_ACTION:
          Http.SetCmd_AT_HTTP_ACTION(HTTP_ACTION_METHOD_GET);
          LocalStateSendCmd = CMD_ATQ_HTTP_STATUS_2;
          break;
        case CMD_ATQ_HTTP_STATUS_2:
          TimeOutHttpAction = T_5SEC;
          Http.SetCmd_AT_HTTP_STATUS();
          LocalStateSendCmd = CMD_AT_HTTP_TERM;
          break;
        case CMD_AT_HTTP_TERM:
          if (Http.HttpFlag.Bit.HttpStatus > 0) {
            Http.SetCmd_AT_HTTP_STATUS();
            break;
          }
          if (Http.HttpFlag.Bit.HttpStatusCode != 200) {
            if (TimeOutHttpAction > 0) {
              break;  
            }
          }
          Http.SetCmd_AT_HTTP_TERM();
          LocalStateSendCmd = CMD_AT_HTTP_INIT;
#if TIMEOUT_THINGSPEAK_SEND_DATA == 20
          Isr.TimeOutWait = T_12SEC;    //  Select this if you want send data every 20 sec (12 Sec timeout + 8 Sec time execution AT cmd)    [1576800 samples/year]
#elif TIMEOUT_THINGSPEAK_SEND_DATA == 30
          Isr.TimeOutWait = T_22SEC;    //  Select this if you want send data every 30 sec (22 Sec timeout + 8 Sec time execution AT cmd)    [1051200 samples/year]
#elif TIMEOUT_THINGSPEAK_SEND_DATA == 60
          Isr.TimeOutWait = T_52SEC;    //  Select this if you want send data every 60 sec (52 Sec timeout + 8 Sec time execution AT cmd)    [525600  samples/year]
#else
          //  default value
          Isr.TimeOutWait = T_22SEC;    //  Select this if you want send data every 30 sec (22 Sec timeout + 8 Sec time execution AT cmd)    [1051200 samples/year]
#endif     
          Io.LedOff(PIN_LED6);     
          break;
        case CMD_AT_NOTHING:
          Io.LedOff(PIN_LED6);
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
void SetDataToHttpThingSpeak(void) {
  uint8_t Index    = 0;

  LocalReadString_FLASH((uint8_t *)STR_COMPOSE_STRING_DATA, strlen(STR_COMPOSE_STRING_DATA), TRUE, FALSE);
  
  Gsm.ClearBuffer(&Http.UrlParameters[0], sizeof(Http.UrlParameters));

  //-------------------------------------------------------------
  //  Voltage
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD1, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD1));
  Index = strlen(Http.UrlParameters);
#ifdef ENABLE_AVERAGE_CALC   
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_RmsVoltageRawAverage())/(pow(10, VOLTAGE_DEC)), (uint8_t *)(Http.UrlParameters + Index), 3, VOLTAGE_DEC);
#else
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ReadRmsVoltageRaw())/(pow(10, VOLTAGE_DEC)), (uint8_t *)(Http.UrlParameters + Index), 3, VOLTAGE_DEC);
#endif
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  //  Current
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD2, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD2));
  Index = strlen(Http.UrlParameters);
#ifdef ENABLE_AVERAGE_CALC  
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_RmsCurrentRawAverage())/(pow(10, CURRENT_DEC)), (uint8_t *)(Http.UrlParameters + Index), 2, CURRENT_DEC);  
#else
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ReadRmsCurrentRaw())/(pow(10, CURRENT_DEC)), (uint8_t *)(Http.UrlParameters + Index), 2, CURRENT_DEC);  
#endif
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  //  Power Factor
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD3, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD3));
  Index = strlen(Http.UrlParameters);
#ifdef ENABLE_AVERAGE_CALC   
  Cmd_39F511.CalcAveragePowerFactor();
  if (Cmd_39F511.GenericFlags.PowerFactorAverageSign == 1) {
    Http.UrlParameters[Index++] = ASCII_MINUS;
  }  
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.CalcAveragePowerFactor())/(pow(10, POWER_FACTOR_DEC)), (uint8_t *)(Http.UrlParameters + Index), 1, POWER_FACTOR_DEC);  
#else
  Cmd_39F511.MCP39F511_ReadPowerFactorRaw();
  Cmd_39F511.CalcPowerFactor();
  if (Cmd_39F511.GenericFlags.PowerFactorSign == 1) {
    Http.UrlParameters[Index++] = ASCII_MINUS;
  }  
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.CalcPowerFactor())/(pow(10, POWER_FACTOR_DEC)), (uint8_t *)(Http.UrlParameters + Index), 1, POWER_FACTOR_DEC); 
#endif  
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  //  Active Power
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD4, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD4));
  Index = strlen(Http.UrlParameters);
  if (Cmd_39F511.SystemStatus.Sign_PA == 0) {
    Http.UrlParameters[Index++] = ASCII_MINUS;
  }
#ifdef ENABLE_AVERAGE_CALC     
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ActivePowerRawAverage())/(pow(10, ACTIVE_POWER_DEC)), (uint8_t *)(Http.UrlParameters + Index), 5, ACTIVE_POWER_DEC);  
#else
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ReadActivePowerRaw())/(pow(10, ACTIVE_POWER_DEC)), (uint8_t *)(Http.UrlParameters + Index), 5, ACTIVE_POWER_DEC);  
#endif
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  //  Reactive Power
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD5, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD5));
  Index = strlen(Http.UrlParameters);
  if (Cmd_39F511.SystemStatus.Sign_PR == 0) {
    Http.UrlParameters[Index++] = ASCII_MINUS;
  }
#ifdef ENABLE_AVERAGE_CALC   
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ReactivePowerRawAverage())/(pow(10, REACTIVE_POWER_DEC)), (uint8_t *)(Http.UrlParameters + Index), 5, REACTIVE_POWER_DEC);  
#else
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ReadReactivePowerRaw())/(pow(10, REACTIVE_POWER_DEC)), (uint8_t *)(Http.UrlParameters + Index), 5, REACTIVE_POWER_DEC);  
#endif
  //-------------------------------------------------------------

  //-------------------------------------------------------------
  //  Apparent Power
  Http.UrlParameters[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)HTTP_THINGSPEAK_FIELD6, ((uint8_t *)Http.UrlParameters + Index), strlen(HTTP_THINGSPEAK_FIELD6));
  Index = strlen(Http.UrlParameters);
#ifdef ENABLE_AVERAGE_CALC   
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ApparentPowerRawAverage())/(pow(10, APPARENT_POWER_DEC)), (uint8_t *)(Http.UrlParameters + Index), 5, APPARENT_POWER_DEC);  
#else
  Index += Gsm.ConvertFloatToString((float)(Cmd_39F511.MCP39F511_ReadApparentPowerRaw())/(pow(10, APPARENT_POWER_DEC)), (uint8_t *)(Http.UrlParameters + Index), 5, APPARENT_POWER_DEC);  
#endif
  //-------------------------------------------------------------

}
//======================================================================
