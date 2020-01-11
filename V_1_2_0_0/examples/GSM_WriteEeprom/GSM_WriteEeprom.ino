/*********************************************************************
 *
 *       GSM Sketch - Set EEPROM location with PIN, PUK code
 *
 *********************************************************************
 * FileName:        GSM_SetEeprom.ino
 * Revision:        1.0.0
 * Date:            05/08/2018
 * 
 * Revision:        1.1.0
 * Date:            21/10/2018
 *                  - The strings used to print on the serial monitor now are memorized into FLASH memory. This metod deliver a more SRAM memory
 *                    to develop the user pplication
 *                  - Added function to read data strings from the FLASH memory
 *                  - Added function to clear Eeprom memory (1024 byte for Arduino Uno and 4096 for Arduino Mega)
 *                  - Added function to write data in sequentially mode into Eeprom memory. The new function used is "void WriteEeprom(uint8_t *EeAdd, uint8_t *SrAdd, uint8_t Lenght)"
 *                  - Added function to read data from the Eeprom (First 256 Bytes only) and visualize it in the table format. The new function used is "void ReadEepromRawData(void)"
 *                  - Improved "Initialize_PIN_PUK_Eeprom" function
 *                  - Improved "void Verify_Eeprom(void)" function
 *                  - Added GPRS data string into Eeprom
 *                  - Added TCP/IP data string into Eeprom
 *                  - Added HTTP data string into Eeprom
 * 
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
 *                  GprsCmd_GSM.h
 *                  TcpIpCmd_GSM.h
 *                  HttpCmd_GSM.h
 * Arduino Board:   Arduino Uno R3, Arduino Mega 2560, Fishino Uno R3 or Fishino Mega 2560       
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
//  Enable/Disable parameters EEPROM SECTION
#define WRITE_PWD_CODE      //  Enable function to write EEPROM with PWD code
#define WRITE_GPRS_CODE     //  Enable function to write EEPROM with GPRS code 
#define WRITE_TCP_IP_CODE   //  Enable function to write EEPROM with TCP/IP code 
#define WRITE_HTTP_CODE     //  Enable function to write EEPROM with HTTP code

#define READ_PWD_CODE       //  Enable function to read EEPROM with PWD code
#define READ_GPRS_CODE      //  Enable function to read EEPROM with GPRS code 
#define READ_TCP_IP_CODE    //  Enable function to read EEPROM with TCP/IP code 
#define READ_HTTP_CODE      //  Enable function to read EEPROM with HTTP code
//======================================================================

#define TRUE  0
#define FALSE 1

#define ARDUINO_UNO_EEPROM_MAX   1024
#define ARDUINO_MEGA_EEPROM_MAX  4096

#define COLUMN_MAX 15
#define ROW_MAX_ARDUINO_UNO   ((ARDUINO_UNO_EEPROM_MAX  / 16) - 1)
#define ROW_MAX_ARDUINO_MEGA  ((ARDUINO_MEGA_EEPROM_MAX / 16) - 1)

//======================================================================
#ifdef WRITE_PWD_CODE
  const char FLASH_PIN1_CODE[]              PROGMEM = "\"4629\"";
  const char FLASH_PIN2_CODE[]              PROGMEM = "\"8888\"";
  const char FLASH_PH_PIN_CODE[]            PROGMEM = "\"5555\"";
  const char FLASH_PUK1_CODE[]              PROGMEM = "\"75392978\"";
  const char FLASH_PUK2_CODE[]              PROGMEM = "\"99999999\"";
  const char FLASH_PH_PUK_CODE[]            PROGMEM = "\"44444444\"";
  const char FLASH_LONG_PSW_CODE[]          PROGMEM = "\"12345678\"";
  const char FLASH_SHORT_PSW_CODE[]         PROGMEM = "\"1234\"";
#endif
#ifdef WRITE_GPRS_CODE
  const char FLASH_APN[]                    PROGMEM = "\"iliad\"";
  const char FLASH_APN_USER_NAME[]          PROGMEM = "\"YourUserName\"";
  const char FLASH_APN_PASSWORD[]           PROGMEM = "\"YourPassword\"";
  const char FLASH_PDP[]                    PROGMEM = "\"IP\"";
#endif
#ifdef WRITE_TCP_IP_CODE
  const char FLASH_REMOTE_SERVER_IP_0[]     PROGMEM = "\"79.35.125.98\"";
  const char FLASH_REMOTE_SERVER_IP_1[]     PROGMEM = "\"79.35.125.98\"";
  const char FLASH_REMOTE_SERVER_IP_2[]     PROGMEM = "\"100.100.100.100\"";
  const char FLASH_REMOTE_DOMAIN_SERVER_0[] PROGMEM = "\"YourDomainServer.org\"";
  const char FLASH_REMOTE_DOMAIN_SERVER_1[] PROGMEM = "\"YourDomainServer.org\"";
  const char FLASH_REMOTE_DOMAIN_SERVER_2[] PROGMEM = "\"YourDomainServer.org\"";
  const char FLASH_REMOTE_SERVER_PORT_0[]   PROGMEM = "\"1080\"";
  const char FLASH_REMOTE_SERVER_PORT_1[]   PROGMEM = "\"11000\"";
  const char FLASH_REMOTE_SERVER_PORT_2[]   PROGMEM = "\"65535\"";
#endif
#ifdef WRITE_HTTP_CODE
  const char FLASH_HTTP_URL[]               PROGMEM = "\"http://api.thingspeak.com/update.json?api_key=89QC6FHH0AU9636Z\"";
  const char FLASH_HTTP_PROXY_IP[]          PROGMEM = "\"100.100.100.100\"";
  const char FLASH_HTTP_PROXY_PORT[]        PROGMEM = "\"22000\"";
  const char FLASH_HTTP_UA[]                PROGMEM = "\"SIMCom_MODULE\"";
#endif
//======================================================================

//======================================================================
#ifdef WRITE_PWD_CODE
  const char STR_PIN1[]                              PROGMEM = " -> PIN1                           = ";
  const char STR_PUK1[]                              PROGMEM = " -> PUK1                           = ";
  const char STR_PIN2[]                              PROGMEM = " -> PIN2                           = ";
  const char STR_PUK2[]                              PROGMEM = " -> PUK2                           = ";
  const char STR_PH_PIN[]                            PROGMEM = " -> PH_PIN                         = ";
  const char STR_PH_PUK[]                            PROGMEM = " -> PH_PUK                         = ";
  const char STR_LONG_PSW[]                          PROGMEM = " -> LONG PASSWORD                  = ";
  const char STR_SHORT_PSW[]                         PROGMEM = " -> SHORT PASSWORD                 = ";
#endif
#ifdef WRITE_GPRS_CODE
  const char STR_APN[]                               PROGMEM = " -> APN                            = ";
  const char STR_APN_USER_NAME[]                     PROGMEM = " -> APN USER NAME                  = ";
  const char STR_APN_PASSWORD[]                      PROGMEM = " -> APN PASSWORD                   = ";
  const char STR_PDP_TYPE[]                          PROGMEM = " -> PDP TYPE                       = ";
#endif
#ifdef WRITE_TCP_IP_CODE
  const char STR_TCP_IP_REMOTE_SERVER_IP_ADD_0[]     PROGMEM = " -> TCP/IP REMOTE SERVER ADDRESS 1 = ";
  const char STR_TCP_IP_REMOTE_SERVER_IP_ADD_1[]     PROGMEM = " -> TCP/IP REMOTE SERVER ADDRESS 2 = ";
  const char STR_TCP_IP_REMOTE_SERVER_IP_ADD_2[]     PROGMEM = " -> TCP/IP REMOTE SERVER ADDRESS 3 = ";
  const char STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_0[] PROGMEM = " -> REMOTE DOMAIN SERVER ADDRESS 1 = ";
  const char STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_1[] PROGMEM = " -> REMOTE DOMAIN SERVER ADDRESS 2 = ";
  const char STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_2[] PROGMEM = " -> REMOTE DOMAIN SERVER ADDRESS 3 = ";
  const char STR_TCP_IP_REMOTE_SERVER_PORT_ADD_0[]   PROGMEM = " -> TCP/IP REMOTE PORT           1 = ";
  const char STR_TCP_IP_REMOTE_SERVER_PORT_ADD_1[]   PROGMEM = " -> TCP/IP REMOTE PORT           2 = ";
  const char STR_TCP_IP_REMOTE_SERVER_PORT_ADD_2[]   PROGMEM = " -> TCP/IP REMOTE PORT           3 = ";
#endif
#ifdef WRITE_HTTP_CODE
  const char STR_HTTP_URL_LINK_ADD[]                 PROGMEM = " -> HTTP Url Link                  = ";
  const char STR_HTTP_PROXY_IP_ADD[]                 PROGMEM = " -> Proxy IP                       = ";
  const char STR_HTTP_PROXY_PORT_ADD[]               PROGMEM = " -> Proxy Port                     = ";
  const char STR_HTTP_UA_ADD[]                       PROGMEM = " -> User Agent                     = ";
#endif
const char STR_ADDRESS[]                             PROGMEM = "Address: ";
//======================================================================

const char STR_SEP[]   PROGMEM = " --------------------------------------------------------------------------------------------------------- \n";
const char STR_SEP2[]  PROGMEM = "|  ";
const char STR_SEP2B[] PROGMEM = "| ";
const char STR_SEP3[]  PROGMEM = "  |";
const char STR_SEP4[]  PROGMEM = " |";
const char STR_ADD[]   PROGMEM = "| Add  | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F ";
const char STR_SCII[]  PROGMEM = "| Ascii            |\n";

const char STR_ERASE_EEPROM_START[]  PROGMEM = "Starts erase Eeprom\n";
const char STR_ERASE_EEPROM[]        PROGMEM = "Erase eeprom in progress: ";
const char STR_ERASE_EEPROM_STOP[]   PROGMEM = "Erase eeprom finished\n";

const char STR_WRITE_EEPROM[]        PROGMEM = "Eeprom initialization in progress. Wait please \n\n";
const char STR_READ_EEPROM[]         PROGMEM = "Read data from Eeprom. Wait please \n\n"; 

//======================================================================
//  Variables
char    TempString[128];
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.PswdEepromStartAddSetup();    //  Reads start address for password string (PIN, PUK....)
  Gprs.GprsEepromStartAddSetup();   //  Reads start address for GPRS string
  TcpIp.TcpIpEepromStartAddSetup(); //  Reads start address for TCP/IP string  
  Http.HttpEepromStartAddSetup();   //  Reads start address for HTTP string  
  
  Io.SetOutputLed();                //  Sets I/O leds
  Io.SetOutputTrigger();            //  Sets I/O triggers
  Io.CheckOutputLed();              //  Checks I/O output (LEDS)
  Io.CheckOutputTrigger();          //  Checks I/O output

  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  delay(500);
  
  Clear_Eeprom();                   //  Clear Eeprom. If data saved in Eeprom is not equal to 0x00 then the function forces it to 0x00
  Initialize_PIN_PUK_Eeprom();      //  Saves PIN, PUK ecc in Eeprom memory
  Initialize_GPRS_Eeprom();         //  Saves GPRS data in Eeprom memory (APN, PDP ecc)
  Initialize_TCP_IP_Eeprom();       //  Saves TCP/IP data in Eeprom
  Initialize_HTTP_Eeprom();         //  Saves HTTP data in Eeprom
  delay(500);
  
  Isr.TimeOutWait = T_10SEC;
}
//======================================================================

//======================================================================
void loop() {
  Verify_Eeprom();
}
//======================================================================

//======================================================================
void Clear_Eeprom(void) {
  uint16_t EeAdd = 0;
  
  LocalReadStringFLASH((uint8_t *)STR_ERASE_EEPROM_START, strlen(STR_ERASE_EEPROM_START), FALSE); 
  LocalReadStringFLASH((uint8_t *)STR_ERASE_EEPROM, strlen(STR_ERASE_EEPROM), FALSE); 
  do {
    Serial.print(".");
    if ((EeAdd > 10) && ((EeAdd % 32) == 0)) {
      Serial.println();
    }
    if (eeprom_read_byte(EeAdd) != 0x00) {
      eeprom_write_byte(EeAdd, 0x00);
    }
#ifdef ARDUINO_UNO_REV3  
  } while (EeAdd++ < ARDUINO_UNO_EEPROM_MAX);
#endif
#ifdef ARDUINO_MEGA2560_REV3    
  } while (EeAdd++ < ARDUINO_MEGA_EEPROM_MAX);
#endif
  Serial.println();
  LocalReadStringFLASH((uint8_t *)STR_ERASE_EEPROM_STOP, strlen(STR_ERASE_EEPROM_STOP), FALSE); 
}
//======================================================================

//======================================================================
void Initialize_PIN_PUK_Eeprom(void) {
#ifdef WRITE_PWD_CODE
  LocalReadStringFLASH((uint8_t *)STR_WRITE_EEPROM, strlen(STR_WRITE_EEPROM), FALSE);
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PIN1_CODE, strlen(FLASH_PIN1_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPin1Code, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PIN2_CODE, strlen(FLASH_PIN2_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPin2Code, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PH_PIN_CODE, strlen(FLASH_PH_PIN_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPhPinCode, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PUK1_CODE, strlen(FLASH_PUK1_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPuk1Code, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PUK2_CODE, strlen(FLASH_PUK2_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPuk2Code, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PH_PUK_CODE, strlen(FLASH_PH_PUK_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPhPukCode, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_LONG_PSW_CODE, strlen(FLASH_LONG_PSW_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddLongPswdCode, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_SHORT_PSW_CODE, strlen(FLASH_SHORT_PSW_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddShortPswdCode, &TempString[0], (sizeof(TempString) - 1));
#endif
}
//======================================================================

//======================================================================
void Initialize_GPRS_Eeprom(void) {
#ifdef WRITE_GPRS_CODE
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_APN, strlen(FLASH_APN), TRUE); 
  WriteEeprom(Gprs.GprsEepromAdd.StartAddApn, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_APN_USER_NAME, strlen(FLASH_APN_USER_NAME), TRUE); 
  WriteEeprom(Gprs.GprsEepromAdd.StartAddApnUserName, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_APN_PASSWORD, strlen(FLASH_APN_PASSWORD), TRUE); 
  WriteEeprom(Gprs.GprsEepromAdd.StartAddApnPassword, &TempString[0], (sizeof(TempString) - 1));
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_PDP, strlen(FLASH_PDP), TRUE); 
  WriteEeprom(Gprs.GprsEepromAdd.StartAddPdpType, &TempString[0], (sizeof(TempString) - 1));
#endif
}
//======================================================================

//======================================================================
void Initialize_TCP_IP_Eeprom(void) {
#ifdef WRITE_TCP_IP_CODE
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_SERVER_IP_0, strlen(FLASH_REMOTE_SERVER_IP_0), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[0], &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_DOMAIN_SERVER_0, strlen(FLASH_REMOTE_DOMAIN_SERVER_0), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[0], &TempString[0], (sizeof(TempString) - 1)); 
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_SERVER_PORT_0, strlen(FLASH_REMOTE_SERVER_PORT_0), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[0], &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_SERVER_IP_1, strlen(FLASH_REMOTE_SERVER_IP_1), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[1], &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_DOMAIN_SERVER_1, strlen(FLASH_REMOTE_DOMAIN_SERVER_1), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[1], &TempString[0], (sizeof(TempString) - 1)); 
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_SERVER_PORT_1, strlen(FLASH_REMOTE_SERVER_PORT_1), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[1], &TempString[0], (sizeof(TempString) - 1));  
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_SERVER_IP_2, strlen(FLASH_REMOTE_SERVER_IP_2), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[2], &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_DOMAIN_SERVER_2, strlen(FLASH_REMOTE_DOMAIN_SERVER_2), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[2], &TempString[0], (sizeof(TempString) - 1)); 
  
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_REMOTE_SERVER_PORT_2, strlen(FLASH_REMOTE_SERVER_PORT_2), TRUE); 
  WriteEeprom(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[2], &TempString[0], (sizeof(TempString) - 1));
#endif
}
//======================================================================

//======================================================================
void Initialize_HTTP_Eeprom(void) {
#ifdef WRITE_HTTP_CODE
  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_HTTP_URL, strlen(FLASH_HTTP_URL), TRUE); 
  WriteEeprom(Http.HttpEepromAdd.StartAddHttpUrlLink, &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_HTTP_PROXY_IP, strlen(FLASH_HTTP_PROXY_IP), TRUE); 
  WriteEeprom(Http.HttpEepromAdd.StartAddHttpProxyIp, &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_HTTP_PROXY_PORT, strlen(FLASH_HTTP_PROXY_PORT), TRUE); 
  WriteEeprom(Http.HttpEepromAdd.StartAddHttpProxyPort, &TempString[0], (sizeof(TempString) - 1));  

  ClearTempString();
  LocalReadStringFLASH((uint8_t *)FLASH_HTTP_UA, strlen(FLASH_HTTP_UA), TRUE); 
  WriteEeprom(Http.HttpEepromAdd.StartAddHttpUA, &TempString[0], (sizeof(TempString) - 1));
#endif  
}
//======================================================================
  
//======================================================================
void ClearTempString(void) {
  uint8_t Index = 0;
  do {
    TempString[Index] = 0x00;
  } while (Index++ < (sizeof(TempString) - 1));
}
//======================================================================

//======================================================================
void WriteEeprom(uint8_t *EeAdd, uint8_t *SrAdd, uint8_t Lenght) {
  uint8_t Index = 0;
  do {
    if (eeprom_read_byte((uint8_t *)EeAdd) != *(SrAdd + Index)) {
      eeprom_write_byte((uint8_t *)EeAdd++, *(SrAdd + Index));
    }
  } while (Index++ < Lenght);  
}
//======================================================================
  
//======================================================================
void Verify_Eeprom(void) {
  if (Isr.TimeOutWait == 0) {
    Isr.TimeOutWait = T_10SEC;
    
    LocalReadStringFLASH((uint8_t *)STR_READ_EEPROM, strlen(STR_READ_EEPROM), FALSE);
#ifdef READ_PWD_CODE  
    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPin1Code);
    LocalReadStringFLASH((uint8_t *)STR_PIN1, strlen(STR_PIN1), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddPin1Code);
    
    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPin2Code);
    LocalReadStringFLASH((uint8_t *)STR_PIN2, strlen(STR_PIN2), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddPin2Code);

    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPhPinCode);
    LocalReadStringFLASH((uint8_t *)STR_PH_PIN, strlen(STR_PH_PIN), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddPhPinCode);

    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddShortPswdCode);
    LocalReadStringFLASH((uint8_t *)STR_SHORT_PSW, strlen(STR_SHORT_PSW), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddShortPswdCode);

    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPuk1Code);
    LocalReadStringFLASH((uint8_t *)STR_PUK1, strlen(STR_PUK1), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddPuk1Code);

    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPuk2Code);
    LocalReadStringFLASH((uint8_t *)STR_PUK2, strlen(STR_PUK2), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddPuk2Code);  

    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPhPukCode);
    LocalReadStringFLASH((uint8_t *)STR_PH_PUK, strlen(STR_PH_PUK), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddPhPukCode);

    PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddLongPswdCode);
    LocalReadStringFLASH((uint8_t *)STR_LONG_PSW, strlen(STR_LONG_PSW), FALSE);
    PrintEepromData(Gsm.PswdEepromAdd.StartAddLongPswdCode);
#endif
#ifdef READ_GPRS_CODE
    PrintEepromDataAddress(Gprs.GprsEepromAdd.StartAddPdpType);
    LocalReadStringFLASH((uint8_t *)STR_PDP_TYPE, strlen(STR_PDP_TYPE), FALSE);
    PrintEepromData(Gprs.GprsEepromAdd.StartAddPdpType);
    
    PrintEepromDataAddress(Gprs.GprsEepromAdd.StartAddApnPassword);
    LocalReadStringFLASH((uint8_t *)STR_APN_PASSWORD, strlen(STR_APN_PASSWORD), FALSE);
    PrintEepromData(Gprs.GprsEepromAdd.StartAddApnPassword);

    PrintEepromDataAddress(Gprs.GprsEepromAdd.StartAddApnUserName);
    LocalReadStringFLASH((uint8_t *)STR_APN_USER_NAME, strlen(STR_APN_USER_NAME), FALSE);
    PrintEepromData(Gprs.GprsEepromAdd.StartAddApnUserName);        
    
    PrintEepromDataAddress(Gprs.GprsEepromAdd.StartAddApn);
    LocalReadStringFLASH((uint8_t *)STR_APN, strlen(STR_APN), FALSE);
    PrintEepromData(Gprs.GprsEepromAdd.StartAddApn);
#endif
#ifdef READ_TCP_IP_CODE
    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[2]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_SERVER_PORT_ADD_2, strlen(STR_TCP_IP_REMOTE_SERVER_PORT_ADD_2), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[2]);   

    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[1]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_SERVER_PORT_ADD_1, strlen(STR_TCP_IP_REMOTE_SERVER_PORT_ADD_1), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[1]); 
    
    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[0]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_SERVER_PORT_ADD_0, strlen(STR_TCP_IP_REMOTE_SERVER_PORT_ADD_0), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteServerPort[0]);   

    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[2]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_2, strlen(STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_2), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[2]);  
    
    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[1]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_1, strlen(STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_1), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[1]);  

    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[0]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_0, strlen(STR_TCP_IP_REMOTE_DOMAIN_SERVER_ADD_0), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteDomainServer[0]);  

    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[2]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_SERVER_IP_ADD_2, strlen(STR_TCP_IP_REMOTE_SERVER_IP_ADD_2), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[2]);    
    
    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[1]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_SERVER_IP_ADD_1, strlen(STR_TCP_IP_REMOTE_SERVER_IP_ADD_1), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[1]);    
    
    PrintEepromDataAddress(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[0]);
    LocalReadStringFLASH((uint8_t *)STR_TCP_IP_REMOTE_SERVER_IP_ADD_0, strlen(STR_TCP_IP_REMOTE_SERVER_IP_ADD_0), FALSE);
    PrintEepromData(TcpIp.TcpIpEepromAdd.StartAddRemoteServerIp[0]);
#endif
#ifdef READ_HTTP_CODE
    PrintEepromDataAddress(Http.HttpEepromAdd.StartAddHttpUrlLink);
    LocalReadStringFLASH((uint8_t *)STR_HTTP_URL_LINK_ADD, strlen(STR_HTTP_URL_LINK_ADD), FALSE);
    PrintEepromData(Http.HttpEepromAdd.StartAddHttpUrlLink);    
    
    PrintEepromDataAddress(Http.HttpEepromAdd.StartAddHttpProxyIp);
    LocalReadStringFLASH((uint8_t *)STR_HTTP_PROXY_IP_ADD, strlen(STR_HTTP_PROXY_IP_ADD), FALSE);
    PrintEepromData(Http.HttpEepromAdd.StartAddHttpProxyIp);    
    
    PrintEepromDataAddress(Http.HttpEepromAdd.StartAddHttpProxyPort);
    LocalReadStringFLASH((uint8_t *)STR_HTTP_PROXY_PORT_ADD, strlen(STR_HTTP_PROXY_PORT_ADD), FALSE);
    PrintEepromData(Http.HttpEepromAdd.StartAddHttpProxyPort);
    
    PrintEepromDataAddress(Http.HttpEepromAdd.StartAddHttpUA);
    LocalReadStringFLASH((uint8_t *)STR_HTTP_UA_ADD, strlen(STR_HTTP_UA_ADD), FALSE);
    PrintEepromData(Http.HttpEepromAdd.StartAddHttpUA);
#endif    
    Serial.print("\n"); 
    ReadEepromRawData();
  }  
}

void PrintEepromDataAddress(uint8_t *EeAdd) {
  char AddressBuffer[8];
  
  LocalReadStringFLASH((uint8_t *)STR_ADDRESS, strlen(STR_ADDRESS), FALSE);
  sprintf(AddressBuffer, "0x%04X", EeAdd);
  Serial.print(AddressBuffer);  
}

void PrintEepromData(uint8_t *EeAdd) {
  do {
    Serial.print(char(eeprom_read_byte((uint8_t *)EeAdd)));
  } while (eeprom_read_byte((uint8_t *)EeAdd++) != 0); 
  Serial.print("\n");
}
//======================================================================

//======================================================================
void ReadEepromRawData(void) {
  char    Data[2];
  uint16_t EeAdd = 0;
  uint16_t Row;
  uint8_t  Column;
  
  LocalReadStringFLASH((uint8_t *)STR_SEP,  strlen(STR_SEP),  FALSE);
  LocalReadStringFLASH((uint8_t *)STR_ADD,  strlen(STR_ADD),  FALSE);
  LocalReadStringFLASH((uint8_t *)STR_SCII, strlen(STR_SCII), FALSE);
  LocalReadStringFLASH((uint8_t *)STR_SEP,  strlen(STR_SEP),  FALSE);
  
  Row = 0;
  do {  
    if (EeAdd < 256) {
      LocalReadStringFLASH((uint8_t *)STR_SEP2, strlen(STR_SEP2), FALSE);
    } else {
      LocalReadStringFLASH((uint8_t *)STR_SEP2B, strlen(STR_SEP2B), FALSE);  
    }
    sprintf(Data, "%02X", EeAdd);
    Serial.print(Data);
    LocalReadStringFLASH((uint8_t *)STR_SEP3, strlen(STR_SEP3), FALSE);
    Column = 0;
    do {
      sprintf(Data, "%02X", char(eeprom_read_byte((uint8_t *)EeAdd++)));
      Serial.print(" ");
      Serial.print(Data);
      LocalReadStringFLASH((uint8_t *)STR_SEP4, strlen(STR_SEP4), FALSE);
    } while (Column++ < COLUMN_MAX);

    Serial.print(" ");
    EeAdd -= 16;
    Column = 0;
    do {  
      if (eeprom_read_byte((uint8_t *)EeAdd) < 0x20) {
        Serial.print(".");
        EeAdd++;   
      } else {
        Serial.print(char(eeprom_read_byte((uint8_t *)EeAdd++)));  
      }
    } while (Column++ < COLUMN_MAX);
    LocalReadStringFLASH((uint8_t *)STR_SEP4, strlen(STR_SEP4), FALSE);
    Serial.println();
#ifdef ARDUINO_UNO_REV3  
  } while (Row++ < ROW_MAX_ARDUINO_UNO);
#endif
#ifdef ARDUINO_MEGA2560_REV3    
  } while (Row++ < ROW_MAX_ARDUINO_MEGA);
#endif

  LocalReadStringFLASH((uint8_t *)STR_SEP, strlen(STR_SEP), FALSE);
  Serial.println();
}
//======================================================================

//======================================================================
void LocalReadStringFLASH(uint8_t *FlashPointer, uint8_t Lenght, boolean NoPrint) {
  uint8_t k;
  
  for (k = 0; k < Lenght; k++) {
    TempString[k] = pgm_read_byte_near(FlashPointer + k);
    if (NoPrint == FALSE) { 
      Serial.print(TempString[k]);
    }
  }
}
//======================================================================
