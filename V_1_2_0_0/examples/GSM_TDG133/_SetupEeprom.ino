/*
 ______________________________________________________________________________________________________________________________
|                 |                                                                                                            |
| EEPROM ADD      | DESCRIPTION                                                                                                |
|_________________|____________________________________________________________________________________________________________|
| 0x0000 - 0x004F | GSM Library data (PIN1, PIN2, PUK1, PUK2 ecc)                                                              |
| 0x0050 - 0x0059 | Password (6 Byte) - Default value "12345"                                                                  |
| 0x005A - 0x006F | Flags 22 Byte                                                                                              |
|     0x005A      | Generic Flags: xxxxxxxxb                                                                                   |
|                 |                ||||||||                                                                                    |
|                 |                |||||||+----> If "1" the Inhibition time for input 1 is ignored                             |
|                 |                ||||||+-----> If "1" the Inhibition time for input 2 is ignored                             |
|                 |                |||||+------> If "1" Enable input SMS report otherwise disable input SMS report             |
|                 |                ||||+-------> If "1" input SMS binary format otherwise text mode                            |
|                 |                |||+--------> If "1" Enable recovery state realy after lack of power otherwise Disable      |
|                 |                ||+---------> If "1" Enable start-up nothification otherwise Disable start-up nothification |
|                 |                |+----------> If "1" Enable lack of power notification otherwise disable                    |
|                 |                +-----------> If "1" Enable echo function otherwise Disable echo function                   |
|     0x005B      | Notification SMS Enable/Disable: xxxxxxxxb                                                                 |
|                 |                                  ||||||||                                                                  |
|                 |                                  |||||||+---> Phone number 1: "0" Disable; "1" Enable                      |
|                 |                                  ||||||+----> Phone number 2: "0" Disable; "1" Enable                      |
|                 |                                  |||||+-----> Phone number 3: "0" Disable; "1" Enable                      |
|                 |                                  ||||+------> Phone number 4: "0" Disable; "1" Enable                      |
|                 |                                  |||+-------> Phone number 5: "0" Disable; "1" Enable                      |
|                 |                                  ||+--------> Phone number 6: "0" Disable; "1" Enable                      |
|                 |                                  |+---------> Phone number 7: "0" Disable; "1" Enable                      |
|                 |                                  +----------> Phone number 8: "0" Disable; "1" Enable                      |
|     0x005C      | Notification Phonic Call Enable/Disable: xxxxxxxxb                                                         |
|                 |                                          ||||||||                                                          |
|                 |                                          |||||||+---> Phone number 1: "0" Disable; "1" Enable              |
|                 |                                          ||||||+----> Phone number 2: "0" Disable; "1" Enable              |
|                 |                                          |||||+-----> Phone number 3: "0" Disable; "1" Enable              |
|                 |                                          ||||+------> Phone number 4: "0" Disable; "1" Enable              |
|                 |                                          |||+-------> Phone number 5: "0" Disable; "1" Enable              |
|                 |                                          ||+--------> Phone number 6: "0" Disable; "1" Enable              |
|                 |                                          |+---------> Phone number 7: "0" Disable; "1" Enable              |
|                 |                                          +----------> Phone number 8: "0" Disable; "1" Enable              |
|     0x005D      | Sets input 1 or 2 level High/Low: xxxxxxxxb                                                                |
|                 |                                   ||||||||                                                                 |
|                 |                                   ||||||++---> Input 1 Level: "00" Level HIGH; "01" Level LOW; "10" TOGGLE |
|                 |                                   ||||++-----> Input 2 Level: "00" Level HIGH; "01" Level LOW; "10" TOGGLE |
|                 |                                   ++++-------> Free                                                        |
|     0x005E      | Inhibition time Input 1. Default value is 5 minutes                                                        |
|     0x005F      | Inhibition time Input 2. Default value is 5 minutes                                                        |
|     0x0060      | Observation time for the Input 1. Default value is 1 second                                                |
|     0x0061      | Observation time for the Input 2. Default value is 1 second                                                |
|     0x0062      | Max number of SMS that the system can send for the Input 1                                                 |
|     0x0063      | Max number of SMS that the system can send for the Input 2                                                 |
|     0x0064      | hh (Hours - TimeOut for sending SMS input report)                                                          |
|     0x0065      | mm (Minutes - TimeOut for sending SMS input report)                                                        |
|     0x0066      | ss (Seconds - TimeOut for sending SMS input report)                                                        |
|     0x0067      | Echo function Phone Number Pointer                                                                         |
|     0x0068      | Gate function time associated to the Relay 1. Default value 3 seconds                                      |
| 0x0069 - 0x006E | Free                                                                                                       |
|     0x006F      | Relay Output State High/Low: xxxxxxxxb                                                                     |
|                 |                              ||||||||                                                                      |
|                 |                              |||||||+----> Output Relay 1: "0" Relay OFF; "1" Relay ON                     |
|                 |                              ||||||+-----> Output Relay 2: "0" Relay OFF; "1" Relay ON                     |
|                 |                              ++++++------> Free                                                            |
| 0x0070 - 0x00D5 | Text for input 1 active     - Default value "ALLARM!! INPUT 1 HIGH"                                        |
| 0x00D6 - 0x013B | Text for input 1 non active - Default value "ALLARM!! INPUT 1 LOW"                                         |
| 0x013C - 0x01A1 | Text for input 2 active     - Default value "ALLARM!! INPUT 2 HIGH"                                        |
| 0x01A2 - 0x0207 | Text for input 2 non active - Default value "ALLARM!! INPUT 2 LOw"                                         |
| 0x0208 - 0x026D | Text for system start up    - Default value "SYSTEM START-UP"                                              |
| 0x026E - 0x02D3 | Text for lack of Power      - Default value "ALLARM!! LACK OF POWER SUPPLY"                                |
| 0x02D4 - 0x0339 | Text for lack of Power      - Default value "RESTORED POWER SUPPLY"                                        |
| 0x033A - 0x0FFF | Free space memory                                                                                          |
|_________________|____________________________________________________________________________________________________________|
*/

#ifdef WRITE_DEFAULT_DATA_EEPROM
//======================================================================
void Clear_Eeprom(void) {
  uint16_t EeAdd = 0;
  
  PrintFlashData_InitEeprom((uint8_t *)STR_ERASE_EEPROM_START, strlen(STR_ERASE_EEPROM_START), FALSE); 
  PrintFlashData_InitEeprom((uint8_t *)STR_ERASE_EEPROM, strlen(STR_ERASE_EEPROM), FALSE); 
  do {
    Serial.print(".");
    if ((EeAdd > 10) && ((EeAdd % 32) == 0)) {
      Serial.println();
    }
    if (eeprom_read_byte(EeAdd) != 0x00) {
      eeprom_write_byte(EeAdd, 0x00);
    }
  } while (EeAdd++ < ARDUINO_MEGA_EEPROM_MAX);

  Serial.println();
  PrintFlashData_InitEeprom((uint8_t *)STR_ERASE_EEPROM_STOP, strlen(STR_ERASE_EEPROM_STOP), FALSE); 
}
//======================================================================

//======================================================================
void Initialize_PIN_PUK_Eeprom(void) {
  PrintFlashData_InitEeprom((uint8_t *)STR_WRITE_EEPROM, strlen(STR_WRITE_EEPROM), FALSE);
  
  PrintFlashData_InitEeprom((uint8_t *)FLASH_PIN1_CODE, strlen(FLASH_PIN1_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPin1Code, &StrTemp[0], (sizeof(StrTemp) - 1));
  
  PrintFlashData_InitEeprom((uint8_t *)FLASH_PIN2_CODE, strlen(FLASH_PIN2_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPin2Code, &StrTemp[0], (sizeof(StrTemp) - 1));
  
  PrintFlashData_InitEeprom((uint8_t *)FLASH_PH_PIN_CODE, strlen(FLASH_PH_PIN_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPhPinCode, &StrTemp[0], (sizeof(StrTemp) - 1));
  
  PrintFlashData_InitEeprom((uint8_t *)FLASH_PUK1_CODE, strlen(FLASH_PUK1_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPuk1Code, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_PUK2_CODE, strlen(FLASH_PUK2_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPuk2Code, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_PH_PUK_CODE, strlen(FLASH_PH_PUK_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddPhPukCode, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_LONG_PSW_CODE, strlen(FLASH_LONG_PSW_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddLongPswdCode, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_SHORT_PSW_CODE, strlen(FLASH_SHORT_PSW_CODE), TRUE); 
  WriteEeprom(Gsm.PswdEepromAdd.StartAddShortPswdCode, &StrTemp[0], (sizeof(StrTemp) - 1));
}
//======================================================================

//======================================================================
void Initialize_PWD_Eeprom(void) {
  PrintFlashData_InitEeprom((uint8_t *)FLASH_SYSTEM_PWD_CODE, strlen(FLASH_SYSTEM_PWD_CODE), TRUE); 
  WriteEeprom(SYS_PSWD_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));
}
//======================================================================

//======================================================================
void Initialize_Flags_Eeprom(void) {
  eeprom_write_byte(FLAGS_ADD,                     DEFAULT_FLAGS);
  eeprom_write_byte(SMS_NOTIFICATION_ADD,          DEFAULT_SMS_NOTIFICATION);
  eeprom_write_byte(VOICE_NOTIFICATION_ADD,        DEFAULT_VOICE_NOTIFICATION);
  eeprom_write_byte(INPUTS_LEVEL_THRESHOLD_ADD,    0x00); 
  eeprom_write_byte(INPUT_1_INHIBITION_TIME_ADD,   DEFAULT_INHIBITION_TIME);
  eeprom_write_byte(INPUT_2_INHIBITION_TIME_ADD,   DEFAULT_INHIBITION_TIME);
  eeprom_write_byte(INPUT_1_OBSERVATION_TIME_ADD,  DEFAULT_OBSERVATION_TIME);
  eeprom_write_byte(INPUT_2_OBSERVATION_TIME_ADD,  DEFAULT_OBSERVATION_TIME);
  eeprom_write_byte(INPUT_1_MAX_NUMBER_SMS_ADD,    DEFAULT_MAX_NUMBER_SMS);
  eeprom_write_byte(INPUT_2_MAX_NUMBER_SMS_ADD,    DEFAULT_MAX_NUMBER_SMS);
  eeprom_write_byte(HH_REPORT_SMS_ADD,             DEFAULT_HH_REPORT_SMS);
  eeprom_write_byte(MM_REPORT_SMS_ADD,             DEFAULT_MM_REPORT_SMS);
  eeprom_write_byte(SS_REPORT_SMS_ADD,             DEFAULT_SS_REPORT_SMS);
  eeprom_write_byte(ECHO_PHONE_NUMBER_POINTER_ADD, 0x00);
  eeprom_write_byte(GATE_TIME_ADD,                 DEFAULT_GATE_TIME);
}
//======================================================================

//======================================================================
void Initialize_System_Alarm_Eeprom(void) {
  PrintFlashData_InitEeprom((uint8_t *)FLASH_ALARM_INPUT_1_HIGH, strlen(FLASH_ALARM_INPUT_1_HIGH), TRUE); 
  WriteEeprom(ALARM_INPUT_1_HIGH_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_ALARM_INPUT_1_LOW, strlen(FLASH_ALARM_INPUT_1_LOW), TRUE); 
  WriteEeprom(ALARM_INPUT_1_LOW_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));
  
  PrintFlashData_InitEeprom((uint8_t *)FLASH_ALARM_INPUT_2_HIGH, strlen(FLASH_ALARM_INPUT_2_HIGH), TRUE); 
  WriteEeprom(ALARM_INPUT_2_HIGH_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_ALARM_INPUT_2_LOW, strlen(FLASH_ALARM_INPUT_2_LOW), TRUE); 
  WriteEeprom(ALARM_INPUT_2_LOW_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_SYSTEM_START_UP, strlen(FLASH_SYSTEM_START_UP), TRUE); 
  WriteEeprom(SYSTEM_START_UP_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_ALARM_POWER_SUPPLY, strlen(FLASH_ALARM_POWER_SUPPLY), TRUE); 
  WriteEeprom(POWER_SUPPLY_ALARM_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));

  PrintFlashData_InitEeprom((uint8_t *)FLASH_RESTORED_POWER_SUPPLY, strlen(FLASH_RESTORED_POWER_SUPPLY), TRUE); 
  WriteEeprom(RESTORE_POWER_SUPPLY_ADD, &StrTemp[0], (sizeof(StrTemp) - 1));
}
//======================================================================

//======================================================================
void Verify_Eeprom(void) {
  PrintFlashData_InitEeprom((uint8_t *)STR_READ_EEPROM, strlen(STR_READ_EEPROM), FALSE);

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPin1Code);
  PrintFlashData_InitEeprom((uint8_t *)STR_PIN1, strlen(STR_PIN1), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddPin1Code);
  
  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPin2Code);
  PrintFlashData_InitEeprom((uint8_t *)STR_PIN2, strlen(STR_PIN2), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddPin2Code);

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPhPinCode);
  PrintFlashData_InitEeprom((uint8_t *)STR_PH_PIN, strlen(STR_PH_PIN), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddPhPinCode);

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddShortPswdCode);
  PrintFlashData_InitEeprom((uint8_t *)STR_SHORT_PSW, strlen(STR_SHORT_PSW), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddShortPswdCode);

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPuk1Code);
  PrintFlashData_InitEeprom((uint8_t *)STR_PUK1, strlen(STR_PUK1), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddPuk1Code);

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPuk2Code);
  PrintFlashData_InitEeprom((uint8_t *)STR_PUK2, strlen(STR_PUK2), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddPuk2Code);  

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddPhPukCode);
  PrintFlashData_InitEeprom((uint8_t *)STR_PH_PUK, strlen(STR_PH_PUK), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddPhPukCode);

  PrintEepromDataAddress(Gsm.PswdEepromAdd.StartAddLongPswdCode);
  PrintFlashData_InitEeprom((uint8_t *)STR_LONG_PSW, strlen(STR_LONG_PSW), FALSE);
  PrintEepromData(Gsm.PswdEepromAdd.StartAddLongPswdCode);

  PrintEepromDataAddress(SYS_PSWD_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_SYSTEM_PWD_CODE, strlen(STR_SYSTEM_PWD_CODE), FALSE);
  PrintEepromData(SYS_PSWD_ADD);

  PrintEepromDataAddress(ALARM_INPUT_1_HIGH_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_ALARM_INPUT_1_HIGH, strlen(STR_ALARM_INPUT_1_HIGH), FALSE);
  PrintEepromData(ALARM_INPUT_1_HIGH_ADD);

  PrintEepromDataAddress(ALARM_INPUT_1_LOW_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_ALARM_INPUT_1_LOW, strlen(STR_ALARM_INPUT_1_LOW), FALSE);
  PrintEepromData(ALARM_INPUT_1_LOW_ADD);

  PrintEepromDataAddress(ALARM_INPUT_2_HIGH_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_ALARM_INPUT_2_HIGH, strlen(STR_ALARM_INPUT_2_HIGH), FALSE);
  PrintEepromData(ALARM_INPUT_2_HIGH_ADD);

  PrintEepromDataAddress(ALARM_INPUT_2_LOW_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_ALARM_INPUT_2_LOW, strlen(STR_ALARM_INPUT_2_LOW), FALSE);
  PrintEepromData(ALARM_INPUT_2_LOW_ADD);

  PrintEepromDataAddress(SYSTEM_START_UP_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_SYSTEM_START_UP, strlen(STR_SYSTEM_START_UP), FALSE);
  PrintEepromData(SYSTEM_START_UP_ADD);

  PrintEepromDataAddress(POWER_SUPPLY_ALARM_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_ALARM_POWER_SUPPLY, strlen(STR_ALARM_POWER_SUPPLY), FALSE);
  PrintEepromData(POWER_SUPPLY_ALARM_ADD);

  PrintEepromDataAddress(RESTORE_POWER_SUPPLY_ADD);
  PrintFlashData_InitEeprom((uint8_t *)STR_RESTORED_POWER_SUPPLY, strlen(STR_RESTORED_POWER_SUPPLY), FALSE);
  PrintEepromData(RESTORE_POWER_SUPPLY_ADD);
  
  Serial.print("\n"); 
  ReadEepromRawData();
}
//======================================================================

//======================================================================
void PrintEepromDataAddress(uint8_t *EeAdd) {
  char AddressBuffer[8];
  
  PrintFlashData_InitEeprom((uint8_t *)STR_ADDRESS, strlen(STR_ADDRESS), FALSE);
  sprintf(AddressBuffer, "0x%04X", EeAdd);
  Serial.print(AddressBuffer);  
}
//======================================================================

//======================================================================
void PrintEepromData(uint8_t *EeAdd) {
  do {
    Serial.print(char(eeprom_read_byte((uint8_t *)EeAdd)));
  } while (eeprom_read_byte((uint8_t *)EeAdd++) != 0); 
  Serial.print("\n");
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
#endif

//======================================================================
void ReadSystemDataFromEeprom(void) {
  StrSysPassword           = ReadEepromString(SYS_PSWD_ADD, SYS_PSWD_MAX_LENGHT);
  TDG133_Flags.Byte.Byte_0 = eeprom_read_byte(FLAGS_ADD);
  TDG133_Flags.Byte.Byte_1 = eeprom_read_byte(SMS_NOTIFICATION_ADD);
  TDG133_Flags.Byte.Byte_2 = eeprom_read_byte(VOICE_NOTIFICATION_ADD);
  TDG133_Flags.Byte.Byte_3 = eeprom_read_byte(INPUTS_LEVEL_THRESHOLD_ADD);
  Input_InhibTime[0]       = eeprom_read_byte(INPUT_1_INHIBITION_TIME_ADD);
  Input_InhibTime[1]       = eeprom_read_byte(INPUT_2_INHIBITION_TIME_ADD);
  Input_ObserTime[0]       = eeprom_read_byte(INPUT_1_OBSERVATION_TIME_ADD);
  Input_ObserTime[1]       = eeprom_read_byte(INPUT_2_OBSERVATION_TIME_ADD);
  Input_MaxSmsNumber[0]    = eeprom_read_byte(INPUT_1_MAX_NUMBER_SMS_ADD);
  Input_MaxSmsNumber[1]    = eeprom_read_byte(INPUT_2_MAX_NUMBER_SMS_ADD);
  EchoPhoneNumberPointer   = eeprom_read_byte(ECHO_PHONE_NUMBER_POINTER_ADD);
  Output_1_GateTimeOut     = eeprom_read_byte(GATE_TIME_ADD);
  
  InputAlarm.Input[0].DisableInhibTimeInput = TDG133_Flags.Bit.DisableInhibTimeInput1;
  InputAlarm.Input[1].DisableInhibTimeInput = TDG133_Flags.Bit.DisableInhibTimeInput2;
  InputAlarm.Input[0].SetLevelInput = TDG133_Flags.Bit.SetLevelInput_1;
  InputAlarm.Input[1].SetLevelInput = TDG133_Flags.Bit.SetLevelInput_2;

  Convert_HH_MM_SS_ToTimeOut(eeprom_read_byte(HH_REPORT_SMS_ADD), eeprom_read_byte(MM_REPORT_SMS_ADD), eeprom_read_byte(SS_REPORT_SMS_ADD));  
}
//======================================================================

//======================================================================
void ReadEepromRawData(void) {
  char     Data[2];
  uint16_t EeAdd = 0;
  uint16_t Row;
  uint8_t  Column;
  
  PrintFlashData_InitEeprom((uint8_t *)STR_SEP,  strlen(STR_SEP),  FALSE);
  PrintFlashData_InitEeprom((uint8_t *)STR_ADD,  strlen(STR_ADD),  FALSE);
  PrintFlashData_InitEeprom((uint8_t *)STR_SCII, strlen(STR_SCII), FALSE);
  PrintFlashData_InitEeprom((uint8_t *)STR_SEP,  strlen(STR_SEP),  FALSE);
  
  Row = 0;
  do {  
    if (EeAdd < 256) {
      PrintFlashData_InitEeprom((uint8_t *)STR_SEP2, strlen(STR_SEP2), FALSE);
    } else {
      PrintFlashData_InitEeprom((uint8_t *)STR_SEP2B, strlen(STR_SEP2B), FALSE);  
    }
    sprintf(Data, "%02X", EeAdd);
    Serial.print(Data);
    PrintFlashData_InitEeprom((uint8_t *)STR_SEP3, strlen(STR_SEP3), FALSE);
    Column = 0;
    do {
      sprintf(Data, "%02X", (uint8_t)char(eeprom_read_byte(EeAdd++)));
      Serial.print(" ");
      Serial.print(Data);
      PrintFlashData_InitEeprom((uint8_t *)STR_SEP4, strlen(STR_SEP4), FALSE);
    } while (Column++ < COLUMN_MAX);

    Serial.print(" ");
    EeAdd -= 16;
    Column = 0;
    do {  
      if (((uint8_t)eeprom_read_byte(EeAdd) < ASCII_SPACE) || ((uint8_t)eeprom_read_byte(EeAdd) > ASCII_z)) {
        Serial.print(".");
        EeAdd++;   
      } else {
        Serial.print(char((uint8_t)eeprom_read_byte(EeAdd++)));   
      }
    } while (Column++ < COLUMN_MAX);
    PrintFlashData_InitEeprom((uint8_t *)STR_SEP4, strlen(STR_SEP4), FALSE);
    Serial.println();   
  } while (Row++ < ROW_MAX_ARDUINO_MEGA);

  PrintFlashData_InitEeprom((uint8_t *)STR_SEP, strlen(STR_SEP), FALSE);
  Serial.println();
}
//======================================================================

//======================================================================
void PrintFlashData_InitEeprom(uint8_t *FlashPointer, uint8_t Lenght, boolean NoPrint) {
  uint8_t k;
  
  Gsm.ClearBuffer(&StrTemp[0], strlen(StrTemp));
  for (k = 0; k < Lenght; k++) {
    StrTemp[k] = pgm_read_byte_near(FlashPointer + k);
    if (NoPrint == FALSE) { 
      Serial.print(StrTemp[k]);
    }
  }
}
//======================================================================
