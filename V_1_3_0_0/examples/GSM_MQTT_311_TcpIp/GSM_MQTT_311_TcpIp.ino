/*********************************************************************
 *
 *       GSM Sketch - Mqtt 3.1.1 used with TCP/IP protocol
 *
 *********************************************************************
 * FileName:        GSM_TcpIp_Mqtt.ino
 * Revision:        1.0.0
 * Date:            06/12/2020
 * Dependencies:    MQTT_GSM.h
 *                  Uart_GSM.h
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
 * Description:     This sketch show how to use Mqtt library with ThingSpeak Server
 *                  ThingSpeak Server support PUBLISH and SUBSCRIBE command with QoS 0 only
                    Note 1: When test Mqtt PING command no PUBLISH o SUBSCRIBE command are supported 
 *                  Note 2: When test Mqtt SUBSCRIBE command no PUBLISH command are supported. The SUBSCRIBE command
 *                          requires a different password to connect client at ThingSpeak Mqtt Server
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

#include "MQTT_GSM.h"
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
Mqtt_GSM          Mqtt;
GprsCmd_GSM       Gprs;
TcpIpCmd_GSM      TcpIp;
HttpCmd_GSM       Http;

#define TCP_IP_MQTT_REV     "1.0"

#define TRUE  0
#define FALSE 1

//#define TEST_MQTT_PING_CMD    // Activate Mqtt PING command. If this row is not commented the sketch sends Mqtt PING
                              // command to the server. To disable this function comment this line
//#define TEST_SUBSCRIBE_QOS_0  // Activate Mqtt SUBSCRIBE command. If this row is not commented the sketch sends Mqtt SUBSCRIBE
                              // command to the server (QoS = 0). To disable this function comment this line
                              
//======================================================================
//  Below the list of the state machine codes used to manage the generic AT commands and MQTT packets
#define CMD_ATQ_GMI                         0
#define CMD_ATQ_GMM                         1
#define CMD_ATQ_GMR                         2
#define CMD_AT_QIDNSIP                      3
#define CMD_ATQ_CIPSTATUS                   4
#define CMD_ATQ_CIPSTATUS_ANSW              5
#define CMD_AT_CIPSTART                     6
#define CMD_AT_CIPCLOSE                     7

#define CMD_AT_CIPSEND_MQTT_CONN_STP_1      8
#define CMD_AT_CIPSEND_MQTT_CONN_STP_2      9
#define CMD_AT_GET_ANSW_MQTT_CONN           10
#define DECODE_ANSW_MQTT_CONN               11
#define DECODE_ANSW_MQTT_CONN_ERR           12

#define CMD_AT_CIPSEND_MQTT_PUB_STP_1       13
#define CMD_AT_CIPSEND_MQTT_PUB_STP_2       14
#define CMD_AT_GET_ANSW_MQTT_PUB            15
#define CMD_AT_GET_ANSW_MQTT_PUB_QOS_ERR    16

#define DECODE_ANSW_MQTT_PUB_QOS_1          17
#define DECODE_ANSW_MQTT_PUB_QOS_2          18
#define DECODE_ANSW_MQTT_PUB_QOS_1_ERR      19
#define DECODE_ANSW_MQTT_PUB_QOS_2_ERR      20

#define CMD_AT_CIPSEND_MQTT_PUBREL_STP_1    21
#define CMD_AT_CIPSEND_MQTT_PUBREL_STP_2    22
#define CMD_AT_GET_ANSW_MQTT_PUBREL         23
#define DECODE_ANSW_MQTT_PUBREL             24
#define DECODE_ANSW_MQTT_PUBREL_ERR         25

#define CMD_AT_CIPSEND_MQTT_PING_1          26
#define CMD_AT_CIPSEND_MQTT_PING_2          27
#define CMD_AT_GET_ANSW_MQTT_PING           28
#define DECODE_ANSW_MQTT_PING               29
#define DECODE_ANSW_MQTT_PING_ERR           30

#define CMD_AT_CIPSEND_MQTT_SUBSCRIBE_STP_1 31
#define CMD_AT_CIPSEND_MQTT_SUBSCRIBE_STP_2 32
#define CMD_AT_GET_ANSW_MQTT_SUBSCRIBE      33
#define DECODE_ANSW_MQTT_SUBSCRIBE          34
#define DECODE_ANSW_MQTT_SUBSCRIBE_ERR      35

#define DECODE_MQTT_PUBLISH_RECEIVED        36

#define CMD_AT_CIPSEND_MQTT_DISCONN_STP_1   37
#define CMD_AT_CIPSEND_MQTT_DISCONN_STP_2   38
//======================================================================

//======================================================================
#define PACKET_IDENTIFIER                   16    // Packet identifier used
#define SUBSCRIBE_TOPIC_FILTER_USED         3     // Select which "Topic Filter" you want to use in the sketch with Mqtt SUBSCRIBE/UNSUBSCRIBE command
                                                  // Value accepted are: 0; 1; 2; 3
                                                  // To select the correct value fiirst check data saved into EEPROM memory for this parameter. For this sketch
                                                  // and ThingSpeak Mqtt Server the Topic Filter implemented are:
                                                  // - 0 - Subscribe "Field 1"      -> "channels/998668/subscribe/fields/field1/PODDI37ZU2FS1W5D"
                                                  // - 1 - Subscribe "Field 2"      -> "channels/998668/subscribe/fields/field2/PODDI37ZU2FS1W5D"
                                                  // - 2 - Subscribe "All Fields"   -> "channels/998668/subscribe/fields/+/PODDI37ZU2FS1W5D"
                                                  // - 3 - Subscribe "Json Format"  -> "channels/998668/subscribe/json/PODDI37ZU2FS1W5D"
//======================================================================

//=======================================================================================================
const char STR_TCP_UDP_CONNECTED[]         PROGMEM = "Tcp/Udp connected";
const char STR_TCP_UDP_NOT_CONNECTED[]     PROGMEM = "Tcp/Udp not connected yet";
const char STR_TCP_UDP_CONNECTING[]        PROGMEM = "Tcp/Udp connecting in progress";

const char STR_TCP_SERVER_CONNECTED[]      PROGMEM = "TCP/IP connection to the server: \"mqtt.thingspeak.com\"; Port: 1883";
const char STR_MQTT_CONN_OK[]              PROGMEM = "Mqtt CONNECT success. Connection accepted";
const char STR_MQTT_CONN_KO_1[]            PROGMEM = "The Server does not support the level of the MQTT protocol requested by the Client";
const char STR_MQTT_CONN_KO_2[]            PROGMEM = "The Client identifier is correct UTF-8 but not allowed by the Server";
const char STR_MQTT_CONN_KO_3[]            PROGMEM = "The Network Connection has been made but the MQTT service is unavailable";
const char STR_MQTT_CONN_KO_4[]            PROGMEM = "The data in the user name or password is malformed";
const char STR_MQTT_CONN_KO_5[]            PROGMEM = "The Client is not authorized to connect";

const char STR_MQTT_PUB_SENT[]             PROGMEM = "Mqtt PUBLISH sent (QoS = 0)";
const char STR_MQTT_PUBACK_OK[]            PROGMEM = "Mqtt PUBLISH success (QoS = 1). Received PUBACK from Server";
const char STR_MQTT_PUBACK_KO[]            PROGMEM = "Mqtt PUBLISH failed. No PUBACK received or other error occurred (QoS = 1)";
const char STR_MQTT_PUBREC_OK[]            PROGMEM = "Mqtt PUBLISH success (QoS = 2). Received PUBREC from Server";
const char STR_MQTT_PUBREC_KO[]            PROGMEM = "Mqtt PUBLISH failed. No PUBREC received or other error occurred (QoS = 2)";
const char STR_MQTT_PUBCOMP_OK[]           PROGMEM = "Mqtt PUBREL (PUB Sequence) success (QoS = 2). Received PUBCOMP from Server";
const char STR_MQTT_PUBCOMP_KO[]           PROGMEM = "Mqtt PUBREL (PUB Sequence) failed. No PUBCOMP received or other error occurred (QoS = 2)";

const char STR_MQTT_SUBACK_OK[]            PROGMEM = "Mqtt SUBSCRIBE success (QoS = 0). Received SUBACK from Server";
const char STR_MQTT_SUBACK_KO[]            PROGMEM = "Mqtt SUBSCRIBE failed. No SUBACK received or other error occurred (QoS = 0)";

const char STR_MQTT_QOS_ERR[]              PROGMEM = "Sets wrong value for QoS parameter";

const char STR_MQTT_PING_OK[]              PROGMEM = "Mqtt PING success";
const char STR_MQTT_PING_KO[]              PROGMEM = "Mqtt PING failure";

const char STR_MQTT_PUB_RECEIVED[]         PROGMEM = "Mqtt PUBLISH received (QoS = 0)";

const char STR_MQTT_FIELD_1[]              PROGMEM = "field1=";
const char STR_MQTT_FIELD_2[]              PROGMEM = "field2=";
const char STR_MQTT_FIELD_3[]              PROGMEM = "field3=";
const char STR_MQTT_FIELD_4[]              PROGMEM = "field4=";
const char STR_MQTT_FIELD_5[]              PROGMEM = "field5=";
const char STR_MQTT_FIELD_6[]              PROGMEM = "field6=";
const char STR_MQTT_FIELD_7[]              PROGMEM = "field7=";
const char STR_MQTT_FIELD_8[]              PROGMEM = "field8=";

const char STR_STATUS_MQTT_PUB[]           PROGMEM = "status=MQTTPUBLISH";
//=======================================================================================================

//======================================================================
//  Variables
uint8_t  PastStateSendCmd;         //  Past    State
uint8_t  PresentStateSendCmd;      //  Present State
uint8_t  FutureStateSendCmd;       //  Future  State

uint8_t  BckPastStateSendCmd;      //  Backup Past    State
uint8_t  BckPresentStateSendCmd;   //  Backup Present State
uint8_t  BckFutureStateSendCmd;    //  Backup Future  State

uint8_t  FieldValue;               //  Field value set, this is the value of the field that the system send at
                                   //  the ThingSpeak Server. Incremental value
uint16_t PingTimeOut;              //  TimeOut for sending the MQTT PING PACKET 
uint16_t BckTimeOut;               //  Backup TimeOut

uint8_t  LastStateTCP_UDP;
uint8_t  LastStateConn;
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.EepromStartAddSetup();
  Gprs.EepromStartAddSetup();
  TcpIp.EepromStartAddSetup();
  Http.EepromStartAddSetup();
  Mqtt.EepromStartAddSetup();

  Mqtt.InitData();
  Mqtt.ReadEepromData();
  
  SetupTimer5();                                            //  Initialize timer 5
  SetOutputsPin();                                          //  Initialize digital outputs
  TestLed_Trigger();                                        //  Test digital outputs
  
  Isr.EnableLibInterrupt();                                 //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);          //  Enables and configure uart for debug 
  Gsm.SetBaudRateUart1(true, BAUD_19200);                   //  Enables and configures the Hardware/Software UART 1

  Gprs.GprsFlag.Bit.EnableGprsFunctions = 1;                //  Enable GPRS functions
  TcpIp.TcpIpFlag.Bit.MultiIp = SINGLE_IP_CONN;             //  Enable Single IP
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
  ProcessSM_Gsm();
  ProcessSM_MqttPacket_AT_Cmd();
}
//======================================================================

//======================================================================
void ProcessSM_Gsm(void) {
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

#ifdef TEST_SUBSCRIBE_QOS_0
    CheckPublishDataFromServer();
#endif
    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }

  PrintTcpUdpState();
}
//======================================================================

//======================================================================
void SetStartStateMachine(void) {
  FutureStateSendCmd = CMD_ATQ_GMI;  
}
//======================================================================

//======================================================================
void CheckPublishDataFromServer(void) {
  if (TcpIp.TcpIpHandleReceivedDataFlag.Bit.DataReadyToRead == DATA_READY_TO_READ) {
    if (FutureStateSendCmd == CMD_ATQ_CIPSTATUS) {
      if (Isr.TimeOutWait > 0) {
        TcpIp.TcpIpHandleReceivedDataFlag.Bit.DataReadyToRead = 0;
        BckTimeOut             = Isr.TimeOutWait;
        BckPastStateSendCmd    = PastStateSendCmd;
        BckPresentStateSendCmd = PresentStateSendCmd;
        BckFutureStateSendCmd  = FutureStateSendCmd;
        
        SetCmdToGetAnswer();
        FutureStateSendCmd = DECODE_MQTT_PUBLISH_RECEIVED;
        Isr.TimeOutWait = T_1SEC;
      }
    }    
  }
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
