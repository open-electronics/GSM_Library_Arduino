//======================================================================
//  This function converts Hours (hh), Minutes (mm) and Seconds (ss) in a
//  32bit timeout variable
void Convert_HH_MM_SS_ToTimeOut(uint8_t hh, uint8_t mm, uint8_t ss) {
  TimeOutSendSmsReport  = ss;
  TimeOutSendSmsReport += (uint16_t)mm * 60;
  TimeOutSendSmsReport += (uint32_t)hh * 3600;
  TimeOutSendSmsReport *= T_1SEC;
}
//======================================================================

//======================================================================
void TestTimeOutSmsInputReport(void) {
  String  SmsTextTemp;
  uint8_t n = 0;

  if (TimeOutSendSmsReport == 0) {
    if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
      if (TDG133_Flags.Bit.Sms_InOut_Report == 1) {
        //  If enabled send SMS Input Report
        System_Flags.Bit.SmsReportInProgress = 1;
        switch (SmsReportState)
        {
          case READ_PHONEBOOK:
            PhoneBook.SetCmd_AT_CPBR(1, false);
            SmsReportState++;
            break;  
          case SEND_SMS:
            if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
              //  PhoneBook index location empty
              PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation = 0;
              System_Flags.Bit.SmsReportInProgress = 0;
              SmsReportState = READ_PHONEBOOK;   //  Reset State
              Convert_HH_MM_SS_ToTimeOut(eeprom_read_byte(HH_REPORT_SMS_ADD), eeprom_read_byte(MM_REPORT_SMS_ADD), eeprom_read_byte(SS_REPORT_SMS_ADD));
              break;
            }
            if (TDG133_Flags.Bit.Sms_InOut_Report_Binary == 1) {
              //  Binary Mode
              SmsTextTemp = "O";
              do {
                if (LastStateRelay[n] == 1) {
                  SmsTextTemp.concat("1");
                } else {
                  SmsTextTemp.concat("0");
                }
              } while (++n < MAX_INPUT_NUM);
              SmsTextTemp.concat("I");
              n = 0;
              do {
                if (TDG133_Input[n].In.InputStatus == 1) {
                  SmsTextTemp.concat("1");
                } else {
                  SmsTextTemp.concat("0");
                }
              } while (++n < MAX_INPUT_NUM);
            } else {
              //  Text Mode
              do {
                SmsTextTemp.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_STATE, strlen(OUTPUT_STATE), FALSE, TRUE));
                SmsTextTemp.concat(n + 1);
                SmsTextTemp.concat(": ");
                if (LastStateRelay[n] == 1) {
                  SmsTextTemp.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_ON, strlen(OUTPUT_RELAY_ON), FALSE, TRUE));  
                } else {
                  SmsTextTemp.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_OFF, strlen(OUTPUT_RELAY_OFF), FALSE, TRUE)); 
                }
              } while (++n < MAX_OUTPUT_NUM);
              n = 0;
              do {
                SmsTextTemp.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_STATE, strlen(INPUT_STATE), FALSE, TRUE));
                SmsTextTemp.concat(n + 1);
                SmsTextTemp.concat(": ");
                if (TDG133_Input[n].In.InputStatus == 1) {
                  SmsTextTemp.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_HIGH, strlen(INPUT_LEVEL_HIGH), FALSE, TRUE));  
                } else {
                  SmsTextTemp.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_LOW, strlen(INPUT_LEVEL_LOW), FALSE, TRUE)); 
                }
              } while (++n < MAX_INPUT_NUM);
            }
            Gsm.ClearBuffer(&Sms.SmsText[0], sizeof(Sms.SmsText));
            SmsTextTemp.toCharArray(Sms.SmsText, MAX_SMS_LENGHT);
            Sms.SetCmd_AT_CMGS();
            System_Flags.Bit.SmsReportInProgress = 0;
            SmsReportState = READ_PHONEBOOK;   //  Reset State
            Convert_HH_MM_SS_ToTimeOut(eeprom_read_byte(HH_REPORT_SMS_ADD), eeprom_read_byte(MM_REPORT_SMS_ADD), eeprom_read_byte(SS_REPORT_SMS_ADD));
            break;
          default:
            System_Flags.Bit.SmsReportInProgress = 0;
            SmsReportState = READ_PHONEBOOK;   //  Reset State
            Convert_HH_MM_SS_ToTimeOut(eeprom_read_byte(HH_REPORT_SMS_ADD), eeprom_read_byte(MM_REPORT_SMS_ADD), eeprom_read_byte(SS_REPORT_SMS_ADD));
            break;
        }      
      } else {
        Convert_HH_MM_SS_ToTimeOut(eeprom_read_byte(HH_REPORT_SMS_ADD), eeprom_read_byte(MM_REPORT_SMS_ADD), eeprom_read_byte(SS_REPORT_SMS_ADD));
      }
    }
  }
}
//======================================================================

//======================================================================
void SendSmsPowerUp(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (TDG133_Flags.Bit.PowerUpSms == 1) {
      //  Enabled to send a Power-Up SMS. This SMS is send only at the first number saved in the phoneBook SIM
      switch (PowerUpSmsState)
      {
        case READ_PHONEBOOK:
          PhoneBook.SetCmd_AT_CPBR(1, false);
          PowerUpSmsState++;
          break;
        case SEND_SMS:
          if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
            //  PhoneBook index location empty
            PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation = 0;

            PowerUpSmsState = READ_PHONEBOOK;   //  Reset State
            TDG133_Flags.Bit.PowerUpSms = 0;
            break;
          }
          Gsm.ClearBuffer(&Sms.SmsText[0], sizeof(Sms.SmsText));
          ReadEepromString(RESTORE_POWER_SUPPLY_ADD, SMS_MESSAGE_MAX_LENGHT).toCharArray(Sms.SmsText, MAX_SMS_LENGHT);
          Sms.SetCmd_AT_CMGS();
          PowerUpSmsState = READ_PHONEBOOK;   //  Reset State
          TDG133_Flags.Bit.PowerUpSms = 0;
          break;
        default:
          PowerUpSmsState = READ_PHONEBOOK;   //  Reset State
          TDG133_Flags.Bit.PowerUpSms = 0;
          break;
      }
    }    
  }
}
//======================================================================

//======================================================================
void SendSmsStartUp(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (TDG133_Flags.Bit.StartUpSms == 1) {
      //  Enabled to send a Start-Up SMS. This SMS is send only at the first number saved in the phoneBook SIM
      switch (StartUpSmsState)
      {
        case READ_PHONEBOOK:
          PhoneBook.SetCmd_AT_CPBR(1, false);
          StartUpSmsState++;
          break;
        case SEND_SMS:
          if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
            //  PhoneBook index location empty
            PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation = 0;

            StartUpSmsState = READ_PHONEBOOK;   //  Reset State
            TDG133_Flags.Bit.StartUpSms = 0;
            break;
          }
          Gsm.ClearBuffer(&Sms.SmsText[0], sizeof(Sms.SmsText));
          ReadEepromString(SYSTEM_START_UP_ADD, SMS_MESSAGE_MAX_LENGHT).toCharArray(Sms.SmsText, MAX_SMS_LENGHT);
          Sms.SetCmd_AT_CMGS();
          StartUpSmsState = READ_PHONEBOOK;   //  Reset State
          TDG133_Flags.Bit.StartUpSms = 0;
          break;
        default:
          StartUpSmsState = READ_PHONEBOOK;   //  Reset State
          TDG133_Flags.Bit.StartUpSms = 0;
          break;
      }
    }    
  }
}
//======================================================================

//======================================================================
void SendSmsInputState(uint8_t n) {
  String StrTempLocal;
  
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if ((InputAlarm.Byte[n] & 0x03) != 0) {
      //  Start to send a SMS
      if (((TDG133_Flags.Byte.Byte_1 & (0x01 << SmsCallOutShifter[n])) != 0) || ((TDG133_Flags.Byte.Byte_2 & (0x01 << SmsCallOutShifter[n])) != 0)) {
        InputAlarm.Input[n].SmsOutProcessBusy = 1;
        switch (SmsCallOutState[n])
        {
          case READ_PHONEBOOK:
            PhoneBook.SetCmd_AT_CPBR((SmsCallOutShifter[n] + 1), false);
            if ((TDG133_Flags.Byte.Byte_1 & (0x01 << SmsCallOutShifter[n])) != 0) {
              SmsCallOutState[n]++;                  //  Set the next state. Sends Allarm SMS
            } else {
              SmsCallOutState[n] = START_VOC;        //  Set the next state. Starts a Voice Call
            }
            break;
          case SEND_SMS:
            if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
              //  PhoneBook index location empty
              PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation = 0;
              UpdateShifter(n);
              SmsCallOutState[n] = READ_PHONEBOOK;   //  Reset State
              InputAlarm.Input[n].SmsOutProcessBusy = 0;
              break;
            }
            Io.LedOn(PIN_LED8);
            if (Input_MaxSmsNumber[n] > 0) {
              //  OK. Send SMS. If "0" Not send SMS
              PrintNumberOfSmsSent(n);   //  Prints to the Serial Monitor
              if (MaxSmsOutCounterIn[n] < Input_MaxSmsNumber[n]) {
                if ((InputAlarm.Input[n].SendSmsInput_High == 1) || (InputAlarm.Input[n].SendSmsInput_Low == 1)) {
                  PrintSendingSms(n);   //  Prints to the Serial Monitor
                  ReadTextSmsMessage_Input(n);
                  Sms.SetCmd_AT_CMGS();                  
                }                  
              } else {
                if (Input_MaxSmsNumber[n] == MAX_NUMBER_SMS) {
                  if ((InputAlarm.Input[n].SendSmsInput_High == 1) || (InputAlarm.Input[n].SendSmsInput_Low == 1)) {
                    PrintSendingInfiniteSms(n);   //  Prints to the Serial Monitor
                    ReadTextSmsMessage_Input(n);
                    Sms.SetCmd_AT_CMGS(); 
                  }
                }
              }
            } else {
              PrintNoSendSms(n);   //  Prints to the Serial Monitor  
            }
            if ((TDG133_Flags.Byte.Byte_2 & (0x01 << SmsCallOutShifter[n])) != 0) {
              SmsCallOutState[n]++;                  //  Set the next state. Starts a Voice Call
            } else {
              //  Reset State
              UpdateShifter(n); 
              SmsCallOutState[n] = READ_PHONEBOOK;
              InputAlarm.Input[n].SmsOutProcessBusy = 0;
            }
            Io.LedOff(PIN_LED8);
            break;
          case START_VOC:
            if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
              //  PhoneBook index location empty
              PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation = 0;
              UpdateShifter(n);
              SmsCallOutState[n] = READ_PHONEBOOK;   //  Reset State
              InputAlarm.Input[n].SmsOutProcessBusy = 0;
              break;
            }
            StrTempLocal = PhoneBook.PhoneNumber;
            StrTempLocal.remove(StrTempLocal.indexOf(ASCII_QUOTATION_MARKS), 1);  
            StrTempLocal.remove(StrTempLocal.indexOf(ASCII_QUOTATION_MARKS), 1);
            StrTempLocal.toCharArray(PhoneBook.PhoneNumber, MAX_LENGHT_PN);  
            if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
              if (Input_MaxSmsNumber[n] > 0) {
                //  OK. Starts Voice Call. If "0" not Start Voice Call
                if (MaxSmsOutCounterIn[n] < Input_MaxSmsNumber[0]) {
                  if ((InputAlarm.Input[n].SendSmsInput_High == 1) || (InputAlarm.Input[n].SendSmsInput_Low == 1)) {
                    PrintVoiceCall(n);   //  Prints to the Serial Monitor 
                    PhonicCall.SetCmd_ATD();  
                  }                    
                } else {
                  if (Input_MaxSmsNumber[n] == MAX_NUMBER_SMS) {
                    if ((InputAlarm.Input[n].SendSmsInput_High == 1) || (InputAlarm.Input[n].SendSmsInput_Low == 1)) {
                      PrintVoiceCallInfiniteLoop(n);   //  Prints to the Serial Monitor 
                      PhonicCall.SetCmd_ATD();
                    }                   
                  } else {
                    UpdateShifter(n);
                    SmsCallOutState[n] = READ_PHONEBOOK;   //  Reset State
                    InputAlarm.Input[n].SmsOutProcessBusy = 0;
                  }
                }
              } else {
                PrintNoVoiceCall(n);    //  Prints to the Serial Monitor 
              }
            } else if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
              break;
            }   
            TimeOutVoiceCall = T_10SEC;
            SmsCallOutState[n]++;                    //  Set the next state. Ends a Voice Call
            break;
          case END_VOC:
            if (TimeOutVoiceCall > 0) {
              Io.LedBlink(PIN_LED8, 25, T_500MSEC);
              break;
            }
            PhonicCall.SetCmd_ATH(0);
            UpdateShifter(n);
            SmsCallOutState[n] = READ_PHONEBOOK;   //  Reset State
            InputAlarm.Input[n].SmsOutProcessBusy = 0;
            break;
          default:
            break;
        }         
      } else {
        UpdateShifter(n);  
      }                 
    }    
  }
}

void UpdateShifter(uint8_t n) {
  if (SmsCallOutShifter[n] < 7) {
    SmsCallOutShifter[n]++;  
  } else {
    SmsCallOutShifter[n] = 0;
    if ((InputAlarm.Byte[n] & 0x03) > 0) {
      if (InputAlarm.Input[n].SendSmsInput_High == 1) {
        InputAlarm.Input[n].SendSmsInput_High = 0;
        StateInput[n] = INPUT_STATE_IDLE;
      } else if (InputAlarm.Input[n].SendSmsInput_Low == 1) {
        InputAlarm.Input[n].SendSmsInput_Low = 0;
        StateInput[n] = INPUT_STATE_IDLE;
      }
      if (MaxSmsOutCounterIn[n] < Input_MaxSmsNumber[n]) {
        MaxSmsOutCounterIn[n]++;
      }
    }
  }  
}

void ReadTextSmsMessage_Input(uint8_t n) {
  Gsm.ClearBuffer(&Sms.SmsText[0], sizeof(Sms.SmsText));
  if (InputAlarm.Input[n].SendSmsInput_High == 1) {
    ReadEepromString(AddInputAlarmString.EepromAdd[n].SmsMessageInput_High, SMS_MESSAGE_MAX_LENGHT).toCharArray(Sms.SmsText, MAX_SMS_LENGHT);  
  } else if (InputAlarm.Input[n].SendSmsInput_Low == 1) {
    ReadEepromString(AddInputAlarmString.EepromAdd[n].SmsMessageInput_Low, SMS_MESSAGE_MAX_LENGHT).toCharArray(Sms.SmsText, MAX_SMS_LENGHT); 
  }
}

void PrintNumberOfSmsSent(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)MAX_SMS_TO_SEND, strlen(MAX_SMS_TO_SEND), FALSE, FALSE);
  Serial.println(Input_MaxSmsNumber[n]);
  ReadStringCmd_FLASH((uint8_t *)NUMBER_OF_SMS_SENT, strlen(NUMBER_OF_SMS_SENT), FALSE, FALSE);
  Serial.println(MaxSmsOutCounterIn[n] + 1);  
}

void PrintSendingSms(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)SEND_SMS_IN_PROGRESS, strlen(SEND_SMS_IN_PROGRESS), FALSE, FALSE);
  Serial.println(n+1);
}

void PrintSendingInfiniteSms(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)SEND_SMS_IN_PROGRESS_2, strlen(SEND_SMS_IN_PROGRESS_2), FALSE, FALSE);
  Serial.println(n+1);
}

void PrintNoSendSms(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)SEND_SMS_IN_PROGRESS_3, strlen(SEND_SMS_IN_PROGRESS_3), FALSE, FALSE);
  Serial.println(n+1);  
}

void PrintVoiceCall(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)START_VOICE, strlen(START_VOICE), FALSE, FALSE);
  Serial.println(n+1);
}

void PrintVoiceCallInfiniteLoop(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)START_VOICE_2, strlen(START_VOICE_2), FALSE, FALSE);
  Serial.println(n+1);
}

void PrintNoVoiceCall(uint8_t n) {
  ReadStringCmd_FLASH((uint8_t *)START_VOICE_3, strlen(START_VOICE_3), FALSE, FALSE);
  Serial.println(n+1);  
}
//======================================================================
