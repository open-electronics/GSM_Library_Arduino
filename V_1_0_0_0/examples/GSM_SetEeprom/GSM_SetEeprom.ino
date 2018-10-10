/*********************************************************************
 *
 *       GSM Sketch - Set EEPROM location with PIN, PUK code
 *
 *********************************************************************
 * FileName:        GSM_SetEeprom.ino
 * Revision:        1.0.0
 * Date:            05/08/2018
 * Dependencies:    Uart_GSM.h
 *                  Isr_GSM.h
 *                  SecurityCmd_GSM.h
 *                  PhoneBookCmd_GSM.h
 *                  SmsCmd_GSM.h
 *                  PhonicCallCmd_GSM.h
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

SecurityCmd_GSM   Security;
PhoneBookCmd_GSM  PhoneBook;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
Isr_GSM           Isr;
Io_GSM            Io;

#define TRUE  0
#define FALSE 1

//======================================================================
//  Variables
uint8_t * Add;
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Gsm.EepromStartAddSetup();
  Initialize_PIN_PUK_Eeprom();
  delay(500);

  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 
  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  
  delay(500);
  Isr.TimeOutWait = T_15SEC;  
}
//======================================================================

//======================================================================
void loop() {
  Verify_Eeprom();
}
//======================================================================

//======================================================================
void Initialize_PIN_PUK_Eeprom(void) {
  Add = Gsm.EepronAdd.StartAddPin1Code;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '6');
  eeprom_write_byte((uint8_t *)Add++, '2');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddPin2Code;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '8');
  eeprom_write_byte((uint8_t *)Add++, '8');
  eeprom_write_byte((uint8_t *)Add++, '8');
  eeprom_write_byte((uint8_t *)Add++, '8');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddPhPinCode;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddPuk1Code;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '0');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '1');
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, '3');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddPuk2Code;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, '9');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddPhPukCode;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddLongPswdCode;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '1');
  eeprom_write_byte((uint8_t *)Add++, '2');
  eeprom_write_byte((uint8_t *)Add++, '3');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, '5');
  eeprom_write_byte((uint8_t *)Add++, '6');
  eeprom_write_byte((uint8_t *)Add++, '7');
  eeprom_write_byte((uint8_t *)Add++, '8');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);

  Add = Gsm.EepronAdd.StartAddShortPswdCode;
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add++, '1');
  eeprom_write_byte((uint8_t *)Add++, '2');
  eeprom_write_byte((uint8_t *)Add++, '3');
  eeprom_write_byte((uint8_t *)Add++, '4');
  eeprom_write_byte((uint8_t *)Add++, 0x22);
  eeprom_write_byte((uint8_t *)Add, 0x00);
}
//======================================================================

//======================================================================
void Verify_Eeprom(void) {
  char AddressBuffer[8];
  if (Isr.TimeOutWait == 0) {
    Isr.TimeOutWait = T_15SEC;
 
    Add = Gsm.EepronAdd.StartAddPin1Code;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> PIN1 = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n");
    
    Add = Gsm.EepronAdd.StartAddPuk1Code;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> PUK1 = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n");

    Add = Gsm.EepronAdd.StartAddPin2Code;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> PIN2 = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n");
    
    Add = Gsm.EepronAdd.StartAddPuk2Code;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> PUK2 = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n");   

    Add = Gsm.EepronAdd.StartAddPhPinCode;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> PH_PIN = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n");
    
    Add = Gsm.EepronAdd.StartAddPhPukCode;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> PH_PUK = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n"); 

    Add = Gsm.EepronAdd.StartAddLongPswdCode;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> LONG PASSWORD = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n");
    
    Add = Gsm.EepronAdd.StartAddShortPswdCode;
    sprintf(AddressBuffer, "0x%04X", Add);
    Serial.print("Address: ");
    Serial.print(AddressBuffer);
    Serial.print(" -> SHORT PASSWORD = ");
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print(char(eeprom_read_byte((uint8_t *)Add++)));
    Serial.print("\n"); 
    Serial.print("\n"); 
    Serial.print("\n"); 
  }  
}
//======================================================================
