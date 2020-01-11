//======================================================================
//  Command used to disable all reply message. This command must be positioned at the beginning of a multiple message
//  "RISP,.........."
uint8_t ProcessCmd_RISP(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_RISP, strlen(CMD_RISP), TRUE, TRUE)) == 0) {
    TDG133_Flags.Bit.ReplyMessage = 0;  //  Disable Reply Message
    StrSerialInput.remove(0, ++CommaIndex);
    PrintProcessString(CMD_RISP_CODE);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to print the EEPROM data on the Arduino serial monitor:
//  "PRNEE;pwd" -> "pwd" is the current password
uint8_t ProcessCmd_PRNEE(int8_t CommaIndex) {
  int Index;
 
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_PRN_EE, strlen(CMD_PRN_EE), TRUE, TRUE)) == 0) {   
    StrProcessCmd.remove(0, strlen(CMD_PRN_EE));    
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {     
      StrProcessCmd.remove(0, ++Index);
      if (StrProcessCmd.equals(StrSysPassword)) {
        ReadEepromRawData();
        PrintProcessString(CMD_PRNEE_CODE);     
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    }    
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to change the password:
//  "PWDxxxxx;pwd" -> "xxxxx" is the new password; "pwd" is the current password
uint8_t ProcessCmd_PWD(int8_t CommaIndex) {
  String  NewPswd;
  int     Index;
 
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_PWD, strlen(CMD_PWD), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_PWD));    
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      //  Get new and old password
      NewPswd = StrProcessCmd.substring(0, Index);
      StrProcessCmd.remove(0, ++Index);
      if (StrProcessCmd.equals(StrSysPassword)) {
        //  Delete Old Password in EEPROM
        EraseEeprom(SYS_PSWD_ADD, SYS_PSWD_MAX_LENGHT);
        //  Save New Password in EEPROM
        WriteEepromString(SYS_PSWD_ADD, NewPswd, SYS_PSWD_MAX_LENGHT);
        StrSysPassword = ReadEepromString(SYS_PSWD_ADD, SYS_PSWD_MAX_LENGHT);
        PrintProcessString(CMD_PWD_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    }    
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the list of all phone numbers currently stored in the device:
//  "ANUM?;pwd" -> "pwd" is the current password
uint8_t ProcessCmd_QANUM(int8_t CommaIndex) {
  int Index;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_ANUM, strlen(CMD_QUERY_ANUM), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_ANUM));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);   
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      if (StrProcessCmd.equals(StrSysPassword)) {
        //  Inserted valid password. Sends AT command to read phonebook memory (Only first eight entries)
        SavePhoneNumberArray(); // Create a copy of the Phone Number
        PhoneIndex    = 1;
        MaxPhoneIndex = 250;
        System_Flags.Bit.SetupAtCmdInProgress = 1;
        System_Flags.Bit.ForceNoSmsAnswer     = 1;
        StateUserAtCommand = RD_N_PHB_MEM;
        PrintProcessString(CMD_QANUM_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }         
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to find a number into a PhoneBook:
//  "FNUM?;text;pwd" -> "pwd" is the current password
uint8_t ProcessCmd_QFNUM(int8_t CommaIndex) {
  String  PhoneText;
  int     Index;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_FNUM, strlen(CMD_QUERY_FNUM), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_FNUM));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);   
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        PhoneText = char(ASCII_QUOTATION_MARKS) + StrProcessCmd.substring(0, Index) + char(ASCII_QUOTATION_MARKS);
        StrProcessCmd.remove(0, ++Index);
        PhoneText.toCharArray(PhoneBook.PhoneText, sizeof(PhoneBook.PhoneText));
        if (StrProcessCmd.equals(StrSysPassword)) {
          //  Inserted valid password. Sends AT command to read phonebook memory (Only first eight entries)
          System_Flags.Bit.StartFindEntry       = 1;
          System_Flags.Bit.SetupAtCmdInProgress = 1;
          StateUserAtCommand = FN_PHB_MEM;
          PrintProcessString(CMD_QFNUM_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }              
      } else {
        return(0);    //  Invalid command
      }  
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the list of phone numbers currently stored in the device (Only the first height locations):
//  "NUM?;pwd"   -> "pwd" is the current password
//  Command used to request a single phone numbers currently stored in the device:
//  "NUM?;x;pwd" -> "x" position in the list; "pwd" is the current password
uint8_t ProcessCmd_QNUM(int8_t CommaIndex) {
  int Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_NUM, strlen(CMD_QUERY_NUM), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_NUM));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);   
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);   
      if (Index > 0) {
        PhoneIndex = StrProcessCmd.substring(0, Index).toInt();
        StrProcessCmd.remove(0, ++Index);
        System_Flags.Bit.SinglePbRead = 1;
        System_Flags.Bit.SetupAtCmdInProgress = 1;
      }
      if (StrProcessCmd.equals(StrSysPassword)) {
        //  Inserted valid password. Sends AT command to read phonebook memory (Only first eight entries)
        StateUserAtCommand = RD_PHB_MEM;
        if (System_Flags.Bit.SinglePbRead == 0) {
          SavePhoneNumberArray(); // Create a copy of the Phone Number
          PhoneIndex    = 1;
          MaxPhoneIndex = 8;
          System_Flags.Bit.SetupAtCmdInProgress = 1;
          StateUserAtCommand = RD_N_PHB_MEM;
        }
        PrintProcessString(CMD_QNUM_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }                
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to store a phone number:
//  "NUMx+39nnnnnnnnnnn;text;pwd" -> "x" position in the list; "nnnnnnnnnnn" is the phone number with country code (+39 for Italy) (Max lenght 20 chars); "text" is the description (Max lenght 14 chars); "pwd" is the current password
//  Command used to remove a phone number:
//  "NUMx;pwd" -> "x" position in the list; "pwd" is the current password
uint8_t ProcessCmd_NUM(int8_t CommaIndex) {
  String  PhoneNumber;
  String  PhoneText;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_NUM, strlen(CMD_NUM), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_NUM));
    Index = StrProcessCmd.indexOf(ASCII_PLUS);
    if (Index >= 0) {
      PhoneIndex = StrProcessCmd.substring(0, Index).toInt();
      StrProcessCmd.remove(0, Index);
    } else {
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        //  Cancel phone number from the phonebook
        PhoneIndex = StrProcessCmd.substring(0, Index).toInt();        
        StrProcessCmd.remove(0, ++Index);       
        if (StrProcessCmd.equals(StrSysPassword)) {
          //  Inserted valid password. Sends AT command to erase phone number from SIM phonebook
          System_Flags.Bit.SetupAtCmdInProgress = 1;
          StateUserAtCommand = ER_PHB_MEM;
          PrintProcessString(CMD_NUM_CODE);
          ReadStringCmd_FLASH((uint8_t *)CMD_ERASE_PB_INDEX, strlen(CMD_ERASE_PB_INDEX), FALSE, FALSE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }    
      } else {
        return(0);    //  Invalid command 
      }
    }
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index > 0) {
      //  Get phone number
      PhoneNumber = char(ASCII_QUOTATION_MARKS) + StrProcessCmd.substring(0, Index) + char(ASCII_QUOTATION_MARKS);
      StrProcessCmd.remove(0, ++Index);
      PhoneNumber.toCharArray(PhoneBook.PhoneNumber, sizeof(PhoneBook.PhoneNumber));
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        //  Get phone number description
        PhoneText = char(ASCII_QUOTATION_MARKS) + StrProcessCmd.substring(0, Index) + char(ASCII_QUOTATION_MARKS);   
        StrProcessCmd.remove(0, ++Index);
        PhoneText.toCharArray(PhoneBook.PhoneText, sizeof(PhoneBook.PhoneText));        
        PhoneBook.PhoneNumberType = 145;
        if (StrProcessCmd.equals(StrSysPassword)) {
          //  Inserted valid password. Sends AT command to save phone number into SIM phonebook
          System_Flags.Bit.SetupAtCmdInProgress = 1;
          StateUserAtCommand = WR_PHB_MEM;
          PrintProcessString(CMD_NUM_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);    //  Inserted Invalid Password
        }
      } else {
        return(0);      //  Invalid command
      }
    } else {
      return(0);        //  Invalid command
    }
  } else {
    return(0);          //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to store the phone numbers that control the gate opener function only
//  "MAC+39xxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the phone number you want to store in the list (200 numbers max) with international code; "pwd" is the current password
uint8_t ProcessCmd_MAC(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_MAC, strlen(CMD_MAC), TRUE, TRUE)) == 0) {
    
    ReadStringCmd_FLASH((uint8_t *)CMD_NOT_IMPLEMENTED, strlen(CMD_NOT_IMPLEMENTED), FALSE, FALSE);
    //PrintProcessString(CMD_MAC_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to delete a phone number from the gate control list
//  "DAC+39xxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the phone number you want to store in the list (200 numbers max) with international code; "pwd" is the current password
//  "DAC;pwd"              -> Delete all numbers in the gate control list; "pwd" is the curren password
uint8_t ProcessCmd_DAC(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_DAC, strlen(CMD_DAC), TRUE, TRUE)) == 0) {

    ReadStringCmd_FLASH((uint8_t *)CMD_NOT_IMPLEMENTED, strlen(CMD_NOT_IMPLEMENTED), FALSE, FALSE);
    //PrintProcessString(CMD_DAC_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to restore the factory parameters of the system and delete all stored phone numbers:
//  "RES;pwd" -> "pwd" is the current password
uint8_t ProcessCmd_RES(int8_t CommaIndex) {
  int Index;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_RES, strlen(CMD_RES), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_RES));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);   
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      if (StrProcessCmd.equals(StrSysPassword)) {
        //  Inserted valid password     
        EraseEeprom(SYS_PSWD_ADD,            (ALARM_INPUT_1_HIGH_ADD   - SYS_PSWD_ADD));             //  Erase Password and Flags
        EraseEeprom(ALARM_INPUT_1_HIGH_ADD,  (ALARM_INPUT_1_LOW_ADD    - ALARM_INPUT_1_HIGH_ADD));   //  Erases the alarm text for input 1. Text for input level high 
        EraseEeprom(ALARM_INPUT_1_LOW_ADD,   (ALARM_INPUT_2_HIGH_ADD   - ALARM_INPUT_1_LOW_ADD));    //  Erases the alarm text for input 1. Text for input level low         
        EraseEeprom(ALARM_INPUT_2_HIGH_ADD,  (ALARM_INPUT_2_LOW_ADD    - ALARM_INPUT_2_HIGH_ADD));   //  Erases the alarm text for input 2. Text for input level high
        EraseEeprom(ALARM_INPUT_2_LOW_ADD,   (SYSTEM_START_UP_ADD      - ALARM_INPUT_2_LOW_ADD));    //  Erases the alarm text for input 2. Text for input level low
        EraseEeprom(SYSTEM_START_UP_ADD,     (POWER_SUPPLY_ALARM_ADD   - SYSTEM_START_UP_ADD));      //  Erases the alarm text for system startup
        EraseEeprom(POWER_SUPPLY_ALARM_ADD,  (RESTORE_POWER_SUPPLY_ADD - POWER_SUPPLY_ALARM_ADD));   //  Erases the alarm text for power supply
        EraseEeprom(RESTORE_POWER_SUPPLY_ADD, EEPROM_FREE_SPACE_ADD    - RESTORE_POWER_SUPPLY_ADD);  //  Erases the alarm text for power supply restore

        WriteEepromString(SYS_PSWD_ADD,             ReadStringCmd_FLASH((uint8_t *)FLASH_SYSTEM_PWD_CODE,       strlen(FLASH_SYSTEM_PWD_CODE),       FALSE, TRUE), sizeof(FLASH_SYSTEM_PWD_CODE));
        WriteEepromString(ALARM_INPUT_1_HIGH_ADD,   ReadStringCmd_FLASH((uint8_t *)FLASH_ALARM_INPUT_1_HIGH,    strlen(FLASH_ALARM_INPUT_1_HIGH),    FALSE, TRUE), sizeof(FLASH_ALARM_INPUT_1_HIGH));
        WriteEepromString(ALARM_INPUT_1_LOW_ADD,    ReadStringCmd_FLASH((uint8_t *)FLASH_ALARM_INPUT_1_LOW,     strlen(FLASH_ALARM_INPUT_1_LOW),     FALSE, TRUE), sizeof(FLASH_ALARM_INPUT_1_LOW));
        WriteEepromString(ALARM_INPUT_2_HIGH_ADD,   ReadStringCmd_FLASH((uint8_t *)FLASH_ALARM_INPUT_2_HIGH,    strlen(FLASH_ALARM_INPUT_2_HIGH),    FALSE, TRUE), sizeof(FLASH_ALARM_INPUT_2_HIGH));
        WriteEepromString(ALARM_INPUT_2_LOW_ADD,    ReadStringCmd_FLASH((uint8_t *)FLASH_ALARM_INPUT_2_LOW,     strlen(FLASH_ALARM_INPUT_2_LOW),     FALSE, TRUE), sizeof(FLASH_ALARM_INPUT_2_LOW));
        WriteEepromString(SYSTEM_START_UP_ADD,      ReadStringCmd_FLASH((uint8_t *)FLASH_SYSTEM_START_UP,       strlen(FLASH_SYSTEM_START_UP),       FALSE, TRUE), sizeof(FLASH_SYSTEM_START_UP));
        WriteEepromString(POWER_SUPPLY_ALARM_ADD,   ReadStringCmd_FLASH((uint8_t *)FLASH_ALARM_POWER_SUPPLY,    strlen(FLASH_ALARM_POWER_SUPPLY),    FALSE, TRUE), sizeof(FLASH_ALARM_POWER_SUPPLY));
        WriteEepromString(RESTORE_POWER_SUPPLY_ADD, ReadStringCmd_FLASH((uint8_t *)FLASH_RESTORED_POWER_SUPPLY, strlen(FLASH_RESTORED_POWER_SUPPLY), FALSE, TRUE), sizeof(FLASH_RESTORED_POWER_SUPPLY));

        eeprom_write_byte(FLAGS_ADD,                     DEFAULT_FLAGS);
        eeprom_write_byte(SMS_NOTIFICATION_ADD,          DEFAULT_SMS_NOTIFICATION);
        eeprom_write_byte(VOICE_NOTIFICATION_ADD,        DEFAULT_VOICE_NOTIFICATION);
        eeprom_write_byte(INPUT_1_INHIBITION_TIME_ADD,   DEFAULT_INHIBITION_TIME);
        eeprom_write_byte(INPUT_2_INHIBITION_TIME_ADD,   DEFAULT_INHIBITION_TIME);
        eeprom_write_byte(INPUT_1_OBSERVATION_TIME_ADD,  DEFAULT_OBSERVATION_TIME);
        eeprom_write_byte(INPUT_2_OBSERVATION_TIME_ADD,  DEFAULT_OBSERVATION_TIME);
        eeprom_write_byte(INPUT_1_MAX_NUMBER_SMS_ADD,    DEFAULT_MAX_NUMBER_SMS);
        eeprom_write_byte(INPUT_2_MAX_NUMBER_SMS_ADD,    DEFAULT_MAX_NUMBER_SMS);
        eeprom_write_byte(INPUTS_LEVEL_THRESHOLD_ADD,    0x00);       
        eeprom_write_byte(HH_REPORT_SMS_ADD,             DEFAULT_HH_REPORT_SMS);
        eeprom_write_byte(MM_REPORT_SMS_ADD,             DEFAULT_MM_REPORT_SMS);
        eeprom_write_byte(SS_REPORT_SMS_ADD,             DEFAULT_SS_REPORT_SMS);
        eeprom_write_byte(ECHO_PHONE_NUMBER_POINTER_ADD, 0x00);
        eeprom_write_byte(GATE_TIME_ADD,                 DEFAULT_GATE_TIME);

        ReadSystemDataFromEeprom();   //  Align system data in SRAM with default
        
        PhoneIndex    = 1;
        MaxPhoneIndex = 250;
        System_Flags.Bit.SetupAtCmdInProgress = 1;
        StateUserAtCommand = ER_N_PHB_MEM;
        PrintProcessString(CMD_RES_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1); 
      } else {
        return(2);    //  Inserted Invalid Password
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to erase SMS from the SIM memory:
//  "DSMSx;pwd"  -> "x" memory index; "pwd" current password
uint8_t ProcessCmd_DSMS(int8_t CommaIndex) {
  int Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_DEL_SMS, strlen(CMD_DEL_SMS), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_DEL_SMS)); 
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      SmsIndex = StrProcessCmd.substring(0, Index).toInt();
      if (SmsIndex > Sms.Sms_Mem1_Storage.Total) {
        return(0);      //  Invalid command  
      }
      StrProcessCmd.remove(0, ++Index);
      if (StrProcessCmd.equals(StrSysPassword)) {
        //  Inserted valid password
        System_Flags.Bit.SetupAtCmdInProgress = 1;
        StateUserAtCommand = ER_SMS_MEM;
        PrintProcessString(CMD_DSMS_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1); 
      } else {
        return(2);    //  Inserted Invalid Password
      }                 
    } else {
      return(0);      //  Invalid command
    }     
  } else {
    return(0);        //  Invalid command
  }  
}
//======================================================================

//======================================================================
//  Command used to erase all SMS from the SIM memory:
//  "DASMS;pwd"  -> "pwd" current password
uint8_t ProcessCmd_DASMS(int8_t CommaIndex) {
  int Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_DEL_ALL_SMS, strlen(CMD_DEL_ALL_SMS), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_DEL_ALL_SMS)); 
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      if (StrProcessCmd.equals(StrSysPassword)) {
        //  Inserted valid password
        System_Flags.Bit.SetupAtCmdInProgress = 1;
        StateUserAtCommand = ER_ALL_SMS_MEM;
        PrintProcessString(CMD_DASMS_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1); 
      } else {
        return(2);    //  Inserted Invalid Password
      }                 
    } else {
      return(0);      //  Invalid command
    }     
  } else {
    return(0);        //  Invalid command
  }  
}
//======================================================================

//======================================================================
//  Command used to allow the number in the specified position to receive SMS on input status:
//  "SMSxxxxxxxx:ON;pwd"  -> "xxxxxxxx" position of the first 8 numbers; "ON" activates the function; "pwd" current password
//  "SMSxxxxxxxx:OFF;pwd" -> "xxxxxxxx" position of the first 8 numbers; "OFF" deactivates the function; "pwd" current password
uint8_t ProcessCmd_SMS(int8_t CommaIndex) {
  String   Pswd;
  uint32_t TempData;
  uint8_t  IndexNumber;
  int      Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_SMS, strlen(CMD_SMS), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_SMS));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      TempData = StrProcessCmd.substring(0, Index).toInt();
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        if (Pswd.equals(StrSysPassword)) {
          do {
            IndexNumber = (TempData % 10);
            if (TempData > 0) {
              if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_ON, strlen(CMD_STATE_ON), TRUE, TRUE)) >= 0) {
                TDG133_Flags.Byte.Byte_1 = TDG133_Flags.Byte.Byte_1 | (1 << (IndexNumber - 1));
              } else if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_OFF, strlen(CMD_STATE_OFF), TRUE, TRUE)) >= 0) {
                TDG133_Flags.Byte.Byte_1 = TDG133_Flags.Byte.Byte_1 & ~(1 << (IndexNumber - 1));
              } else {
                return(0);  //  Invalid command
              }
              TempData = (TempData / 10);
            } else {
              return(0);    //  Invalid command
            }
          } while (TempData > 0);
          eeprom_write_byte(SMS_NOTIFICATION_ADD, TDG133_Flags.Byte.Byte_1);
          PrintProcessString(CMD_SMS_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);          
        } else {
          return(2);  //  Inserted Invalid Password
        }       
      } else {
        return(0);    //  Invalid command
      }  
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to allows the number on the specified position to receive the ringtone on the inputs status:
//  "VOCxxxxxxxx:ON;pwd"  -> "xxxxxxxx" position of the first 8 numbers; "ON" activates the function; "pwd" current password
//  "VOCxxxxxxxx:OFF;pwd" -> "xxxxxxxx" position of the first 8 numbers; "OFF" deactivates the function; "pwd" current password
uint8_t ProcessCmd_VOC(int8_t CommaIndex) {
  String   Pswd;
  uint32_t TempData;
  uint8_t  IndexNumber;
  int      Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_VOC, strlen(CMD_VOC), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_VOC));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      TempData = StrProcessCmd.substring(0, Index).toInt();
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          do {
            IndexNumber = (TempData % 10);
            if (TempData > 0) {
              if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_ON, strlen(CMD_STATE_ON), TRUE, TRUE)) >= 0) {
                TDG133_Flags.Byte.Byte_2 = TDG133_Flags.Byte.Byte_2 | (1 << (IndexNumber - 1));
              } else if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_OFF, strlen(CMD_STATE_OFF), TRUE, TRUE)) >= 0) {
                TDG133_Flags.Byte.Byte_2 = TDG133_Flags.Byte.Byte_2 & ~(1 << (IndexNumber - 1));
              } else {
                return(0);  //  Invalid command
              }
              TempData = (TempData / 10);
            } else {
              return(0);    //  Invalid command
            }
          } while (TempData > 0);
          eeprom_write_byte(VOICE_NOTIFICATION_ADD, TDG133_Flags.Byte.Byte_2);
          PrintProcessString(CMD_VOC_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        } 
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the alarm activation level:
//  "LIV?"
uint8_t ProcessCmd_QLIV(int8_t CommaIndex) { 
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_LIV, strlen(CMD_QUERY_LIV), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_LIV));
    
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_LEVEL_TEXT, strlen(QUERY_INPUT_LEVEL_TEXT), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE));
    switch (TDG133_Flags.Bit.SetLevelInput_1)
    {
      case LEVEL_HIGH_INPUT:
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_HIGH, strlen(INPUT_LEVEL_HIGH), FALSE, FALSE));
        break;
      case LEVEL_LOW_INPUT:
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_LOW, strlen(INPUT_LEVEL_LOW), FALSE, FALSE));
        break;
      case TOGGLE_INPUT:
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_TOGGLE, strlen(INPUT_LEVEL_TOGGLE), FALSE, FALSE));
        break;
      default:
        break;
    }
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_LEVEL_TEXT, strlen(QUERY_INPUT_LEVEL_TEXT), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE));
    switch (TDG133_Flags.Bit.SetLevelInput_2)
    {
      case LEVEL_HIGH_INPUT:
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_HIGH, strlen(INPUT_LEVEL_HIGH), FALSE, FALSE));
        break;
      case LEVEL_LOW_INPUT:
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_LOW, strlen(INPUT_LEVEL_LOW), FALSE, FALSE));
        break;
      case TOGGLE_INPUT:
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_TOGGLE, strlen(INPUT_LEVEL_TOGGLE), FALSE, FALSE));
        break;
      default:
        break;
    }

    PrintProcessString(CMD_QLIV_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set a HIGH/LOW/Variation level as the alarm condition for inputs IN1 or IN2:
//  "LIVx:A;pwd" -> "x" stands for input 1 or 2; "A" stands for HIGH level; "pwd" current password
//  "LIVx:B;pwd" -> "x" stands for input 1 or 2; "B" stands for LOW level; "pwd" current password
//  "LIVx:V;pwd" -> "x" stands for input 1 or 2; "V" stands for variation (LOW to HIGH or HIGH to LOW); "pwd" current password
uint8_t ProcessCmd_LIV(int8_t CommaIndex) {
  String  Pswd;
  uint8_t InputIndex;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_LIV, strlen(CMD_LIV), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_LIV));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0)  {
      InputIndex = StrProcessCmd.substring(0, Index).toInt();
      StrProcessCmd.remove(0, ++Index);
      if (InputIndex > 2) {
        //  Insert invalid input index
        ReadStringCmd_FLASH((uint8_t *)INPUT_INPUT_INDEX, strlen(INPUT_INPUT_INDEX), FALSE, FALSE);
        return(0);  //  Invalid command
      }
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          switch (InputIndex)
          {
            case 1:
              if (StrProcessCmd.equals("A")) {
                TDG133_Flags.Bit.SetLevelInput_1  = LEVEL_HIGH_INPUT;
                InputAlarm.Input[0].SetLevelInput = LEVEL_HIGH_INPUT;
              } else if (StrProcessCmd.equals("B")) {
                TDG133_Flags.Bit.SetLevelInput_1  = LEVEL_LOW_INPUT;
                InputAlarm.Input[0].SetLevelInput = LEVEL_LOW_INPUT;
              } else if (StrProcessCmd.equals("V")) {
                TDG133_Flags.Bit.SetLevelInput_1  = TOGGLE_INPUT;
                InputAlarm.Input[0].SetLevelInput = TOGGLE_INPUT;
              }
              break;
            case 2:
              if (StrProcessCmd.equals("A")) {
                TDG133_Flags.Bit.SetLevelInput_2  = LEVEL_HIGH_INPUT;
                InputAlarm.Input[1].SetLevelInput = LEVEL_HIGH_INPUT;
              } else if (StrProcessCmd.equals("B")) {
                TDG133_Flags.Bit.SetLevelInput_2  = LEVEL_LOW_INPUT;
                InputAlarm.Input[1].SetLevelInput = LEVEL_LOW_INPUT;
              } else if (StrProcessCmd.equals("V")) {
                TDG133_Flags.Bit.SetLevelInput_2  = TOGGLE_INPUT;
                InputAlarm.Input[1].SetLevelInput = TOGGLE_INPUT;
              }        
              break;
            default:
              return(0);  //  Invalid command
              break;
          }   
          eeprom_write_byte(INPUTS_LEVEL_THRESHOLD_ADD, (TDG133_Flags.Byte.Byte_3 & 0x1F)); 
          PrintProcessString(CMD_LIV_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }  
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the current inhibition time setting regarding the inputs:
//  "INI?"
uint8_t ProcessCmd_QIN(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_INI, strlen(CMD_QUERY_INI), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_INI));
    
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_INHIB_TIME, strlen(QUERY_INPUT_INHIB_TIME), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE));
    Serial.println(Input_InhibTime[0]);
    VeryLongSmsOut.concat(Input_InhibTime[0]);
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_INHIB_TIME, strlen(QUERY_INPUT_INHIB_TIME), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE));
    Serial.println(Input_InhibTime[1]);
    VeryLongSmsOut.concat(Input_InhibTime[1]);
    VeryLongSmsOut.concat("\n");
    
    PrintProcessString(CMD_QIN_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the inhibition time on Input 1 and Input 2:
//  "INIx:mm;pwd" -> "x" is the input index; "mm" is the time in prime minutes; "pwd" current password
uint8_t ProcessCmd_INI(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  uint8_t InputIndex;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_INI, strlen(CMD_INI), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_INI));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0)  {
      InputIndex = StrProcessCmd.substring(0, Index).toInt();   
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (InputIndex == 1) {
            Input_InhibTime[0] = StrProcessCmd.toInt();  
            if (Input_InhibTime[0] > MAX_INHIBITION_TIME) {
              ReadStringCmd_FLASH((uint8_t *)INVALID_INHIB_TIME, strlen(INVALID_INHIB_TIME), FALSE, FALSE);
              return(0);  //  Invalid command  
            }
            eeprom_write_byte(INPUT_1_INHIBITION_TIME_ADD, Input_InhibTime[0]);
          } else if (InputIndex == 2) {
            Input_InhibTime[1] = StrProcessCmd.toInt();  
            if (Input_InhibTime[1] > MAX_INHIBITION_TIME) {
              ReadStringCmd_FLASH((uint8_t *)INVALID_INHIB_TIME, strlen(INVALID_INHIB_TIME), FALSE, FALSE);
              return(0);  //  Invalid command  
            }
            eeprom_write_byte(INPUT_2_INHIBITION_TIME_ADD, Input_InhibTime[1]);
          } else {
            return(0);  //  Invalid command
          }
          PrintProcessString(CMD_INI_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        } 
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to reset the inhibition time if input 1 is idle:
//  "TIZ1x;pwd" -> "x" is the setting parameter (if "0" no reset; if "1" reset); "pwd" current password
uint8_t ProcessCmd_TIZ1(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TIZ1, strlen(CMD_TIZ1), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TIZ1));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      Pswd = StrProcessCmd.substring(Index + 1);
      StrProcessCmd.remove(Index);
      if (Pswd.equals(StrSysPassword)) {
        TDG133_Flags.Bit.DisableInhibTimeInput1 = StrProcessCmd.toInt();
        InputAlarm.Input[0].DisableInhibTimeInput = TDG133_Flags.Bit.DisableInhibTimeInput1;
        InhibTimeOut_Input[0] = 0;   //  Reset TimeOut Inhibition
        eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
        PrintProcessString(CMD_TIZ1_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);       
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to reset the inhibition time if input 2 is idle:
//  "TIZ2x;pwd" -> "x" is the setting parameter (if "0" no reset; if "1" reset); "pwd" current password 
uint8_t ProcessCmd_TIZ2(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TIZ2, strlen(CMD_TIZ2), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TIZ2));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0)  {
      Pswd = StrProcessCmd.substring(Index + 1);
      StrProcessCmd.remove(Index);
      if (Pswd.equals(StrSysPassword)) {
        TDG133_Flags.Bit.DisableInhibTimeInput2 = StrProcessCmd.toInt();
        InputAlarm.Input[1].DisableInhibTimeInput = TDG133_Flags.Bit.DisableInhibTimeInput2;
        InhibTimeOut_Input[1] = 0;   //  Reset TimeOut Inhibition
        eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
        PrintProcessString(CMD_TIZ2_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the current observation time setting regarding the inputs
//  "OSS?"
uint8_t ProcessCmd_QOSS(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_OSS, strlen(CMD_QUERY_OSS), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_OSS));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_OBSER_TIME, strlen(QUERY_INPUT_OBSER_TIME), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE));
    Serial.println(Input_ObserTime[0]);
    VeryLongSmsOut.concat(Input_ObserTime[0]);
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_OBSER_TIME, strlen(QUERY_INPUT_OBSER_TIME), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE));
    Serial.println(Input_ObserTime[1]);   
    VeryLongSmsOut.concat(Input_ObserTime[1]);
    VeryLongSmsOut.concat("\n");

    PrintProcessString(CMD_QOSS_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define the lenght of the observation time regarding input 1:
//  "OSSx:ss;pwd" -> "x" is the input index; "ss" is the time in seconds; "pwd" current password 
uint8_t ProcessCmd_OSS(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  uint8_t InputIndex;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_OSS, strlen(CMD_OSS), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_OSS));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      InputIndex = StrProcessCmd.substring(0, Index).toInt(); 
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (InputIndex == 1) {
            Input_ObserTime[0] = StrProcessCmd.toInt();  
            if (Input_ObserTime[0] > MAX_OBSERVATION_TIME) {
              ReadStringCmd_FLASH((uint8_t *)INVALID_OBSER_TIME, strlen(INVALID_OBSER_TIME), FALSE, FALSE);
              return(0);  //  Invalid command  
            }
            eeprom_write_byte(INPUT_1_OBSERVATION_TIME_ADD, Input_ObserTime[0]);      
          } else if (InputIndex == 2) {
            Input_ObserTime[1] = StrProcessCmd.toInt();
            if (Input_ObserTime[1] > MAX_OBSERVATION_TIME) {
              ReadStringCmd_FLASH((uint8_t *)INVALID_OBSER_TIME, strlen(INVALID_OBSER_TIME), FALSE, FALSE);
              return(0);  //  Invalid command  
            }
            eeprom_write_byte(INPUT_2_OBSERVATION_TIME_ADD, Input_ObserTime[1]);      
          } else {
            return(0);    //  Invalid command
          }
          PrintProcessString(CMD_OSS_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension presence:
//  "TIN1A:xxxx;pwd" -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and comma ",". All letters must be capitalized; "pwd" current password 
//                      The default message is "ALARM!! INPUT 1 HIGH"
uint8_t ProcessCmd_TIN1A(int8_t CommaIndex) {
  String  Pswd;
  int     Index;

  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TIN1A, strlen(CMD_TIN1A), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TIN1A));
    Index = StrProcessCmd.indexOf(ASCII_COLON);   
    if (Index >= 0) {      
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(ALARM_INPUT_1_HIGH_ADD, (ALARM_INPUT_1_LOW_ADD - ALARM_INPUT_1_HIGH_ADD));   //  Erases the alarm text for input 1. Text for input level high 
          WriteEepromString(ALARM_INPUT_1_HIGH_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TIN1A_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {   
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension absence:
//  "TIN1B:xxxx;pwd" -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accep the semi colon ";" and comma ",". All letters must be capitalized; "pwd" current password 
//                      The default message is "ALARM!!"
uint8_t ProcessCmd_TIN1B(int8_t CommaIndex) {
  String  Pswd;
  int     Index;

  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TIN1B, strlen(CMD_TIN1B), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TIN1B));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(ALARM_INPUT_1_LOW_ADD, (ALARM_INPUT_2_HIGH_ADD - ALARM_INPUT_1_LOW_ADD));    //  Erases the alarm text for input 1. Text for input level low         
          WriteEepromString(ALARM_INPUT_1_LOW_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TIN1B_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension presence:
//  "TIN2A:xxxx;pwd" -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" current password 
//                      The default message is "ALARM!! INPUT 2 HIGH"
uint8_t ProcessCmd_TIN2A(int8_t CommaIndex) {
  String  Pswd;
  int     Index;

  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TIN2A, strlen(CMD_TIN2A), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TIN2A));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index); 
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(ALARM_INPUT_2_HIGH_ADD, (ALARM_INPUT_2_LOW_ADD - ALARM_INPUT_2_HIGH_ADD));   //  Erases the alarm text for input 2. Text for input level high 
          WriteEepromString(ALARM_INPUT_2_HIGH_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TIN2A_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension absence:
//  "TIN2B:xxxx;pwd" -> "xxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" current password 
//                      The default message is "ALARM!!"
uint8_t ProcessCmd_TIN2B(int8_t CommaIndex) {
  String  Pswd;
  int     Index;

  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TIN2B, strlen(CMD_TIN2B), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TIN2B));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) { 
      StrProcessCmd.remove(0, ++Index);     
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(ALARM_INPUT_2_LOW_ADD, (ALARM_INPUT_2_HIGH_ADD - ALARM_INPUT_2_LOW_ADD));    //  Erases the alarm text for input 1. Text for input level low         
          WriteEepromString(ALARM_INPUT_2_LOW_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TIN2B_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the setting of the number of SMS to send
//  "ALN?"
uint8_t ProcessCmd_QALN(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_ALN, strlen(CMD_QUERY_ALN), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_ALN));  
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_MAX_SMS, strlen(QUERY_INPUT_MAX_SMS), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE));
    Serial.println(Input_MaxSmsNumber[0]);
    VeryLongSmsOut.concat(Input_MaxSmsNumber[0]);
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_INPUT_MAX_SMS, strlen(QUERY_INPUT_MAX_SMS), FALSE, FALSE));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE));
    Serial.println(Input_MaxSmsNumber[1]); 
    VeryLongSmsOut.concat(Input_MaxSmsNumber[1]);
    VeryLongSmsOut.concat("\n");
    
    PrintProcessString(CMD_QALN_CODE);  
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define the number of SMS that the device must send when the alarm 1 occurs:
//  "ALNy:xx;pwd" -> "y" is the input index; "xx" the number of SMS to be sent. If "xx" = 0 disables the SMS sending, if "xx" = 99 if alarm detected the system sends an infinite number of SMS until the alarm disappear; "pwd" current password
uint8_t ProcessCmd_ALN(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  uint8_t InputIndex;
      
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_ALN, strlen(CMD_ALN), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_ALN));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0)  {
      InputIndex = StrProcessCmd.substring(0, Index).toInt();
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (InputIndex == 1) {
            Input_MaxSmsNumber[0] = StrProcessCmd.toInt();
            if (Input_MaxSmsNumber[0] > MAX_NUMBER_SMS) {
              ReadStringCmd_FLASH((uint8_t *)INVALID_MAX_SMS, strlen(INVALID_MAX_SMS), FALSE, FALSE);
              return(0);  //  Invalid command  
            }
            eeprom_write_byte(INPUT_1_MAX_NUMBER_SMS_ADD, Input_MaxSmsNumber[0]);              
          } else if (InputIndex == 2) {
            Input_MaxSmsNumber[1] = StrProcessCmd.toInt();
            if (Input_MaxSmsNumber[1] > MAX_NUMBER_SMS) {
              ReadStringCmd_FLASH((uint8_t *)INVALID_MAX_SMS, strlen(INVALID_MAX_SMS), FALSE, FALSE);
              return(0);  //  Invalid command  
            }
            eeprom_write_byte(INPUT_2_MAX_NUMBER_SMS_ADD, Input_MaxSmsNumber[1]);              
          } else {
            return(0);    //  Invalid command
          }
          PrintProcessString(CMD_ALN_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to activate the specified output relay:
//  "OUTx:ON;pwd"  -> Activate output relay 1; "pwd" current password
//  "OUTx:OFF;pwd" -> Deactivate output relay 1; "pwd" current password
//  "OUTx:ss;pwd"  -> Inverts the condition of the specified relay for desired time; "ss" is a period between 1 and 59 seconds; "pwd" current password
uint8_t ProcessCmd_OUT(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  uint8_t InputIndex;
  uint8_t TimeOut;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_OUT, strlen(CMD_OUT), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_OUT));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      InputIndex = StrProcessCmd.substring(0, Index).toInt();
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_ON, strlen(CMD_STATE_ON), TRUE, TRUE)) >= 0) {
            if ((InputIndex > 0) && (InputIndex <= MAX_OUTPUT_NUM)) {
              StateRelay[InputIndex - 1] = RELAY_STATE_ON;   
            } else {
              return(0);    //  Invalid command
            }
          } else if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_OFF, strlen(CMD_STATE_OFF), TRUE, TRUE)) >= 0) {
            if ((InputIndex > 0) && (InputIndex <= MAX_OUTPUT_NUM)) {
              StateRelay[InputIndex - 1] = RELAY_STATE_OFF;   
            } else {
              return(0);    //  Invalid command
            }
          } else {
            TimeOut = StrProcessCmd.toInt();
            if ((TimeOut > 0) && (TimeOut < 60)) {
              if ((InputIndex > 0) && (InputIndex <= MAX_OUTPUT_NUM)) {
                ToggleTimeOutRelay[InputIndex - 1] = (StrProcessCmd.toInt()) * 500;
                StateRelay[InputIndex - 1] = RELAY_STATE_TOGGLE;   
              } else {
                return(0);  //  Invalid command
              }
            } else {
              return(0);    //  Invalid command
            }
          }
          PrintProcessString(CMD_OUT_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the condition of the remote outputs
//  "STA?"
uint8_t ProcessCmd_QSTA(int8_t CommaIndex) {
  uint8_t OutputIndex = 0;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_STA, strlen(CMD_QUERY_STA), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_STA));
    do {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_RELAY_STATUS, strlen(QUERY_RELAY_STATUS), FALSE, FALSE));
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE));
      if (LastStateRelay[OutputIndex] == RELAY_STATE_ON) {
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_ON, strlen(OUTPUT_RELAY_ON), FALSE, FALSE));  
      } else if (LastStateRelay[OutputIndex] == RELAY_STATE_OFF) {
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_OFF, strlen(OUTPUT_RELAY_OFF), FALSE, FALSE));  
      }
    } while (++OutputIndex < MAX_OUTPUT_NUM);

    PrintProcessString(CMD_QSTA_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the time between report SMS sending
//  "AUTOC:ON;pwd"       -> Enable function to send SMS Input/Output report; "pwd" current password
//  "AUTOC:OFF;pwd"      -> Disable function to send SMS Input/Output report; "pwd" current password
//  "AUTOC:hh:mm:ss;pwd" -> "hh" hours; "mm" minutes; "ss" seconds; "pwd" current password
uint8_t ProcessCmd_AUTOC(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  uint8_t hh;
  uint8_t mm;
  uint8_t ss;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_AUTOC, strlen(CMD_AUTOC), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_AUTOC));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_ON, strlen(CMD_STATE_ON), TRUE, TRUE)) >= 0) {
            TDG133_Flags.Bit.Sms_InOut_Report = 1;
            eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
          } else if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_STATE_OFF, strlen(CMD_STATE_OFF), TRUE, TRUE)) >= 0) {
            TDG133_Flags.Bit.Sms_InOut_Report = 0;
            eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
          } else {
            Index = StrProcessCmd.indexOf(ASCII_COLON);
            if (Index >= 0) {
              hh = StrProcessCmd.substring(0, Index).toInt();
              StrProcessCmd.remove(0, ++Index);
              Index = StrProcessCmd.indexOf(ASCII_COLON);
              if (Index >= 0) {
                mm = StrProcessCmd.substring(0, Index).toInt();
                StrProcessCmd.remove(0, ++Index);
                ss = StrProcessCmd.toInt();
                if ((hh >= 0) && (hh < 24)) {
                  if ((mm >= 0) && (mm < 60)) {
                    if ((ss >= 0) && (ss < 60)) {
                      eeprom_write_byte(HH_REPORT_SMS_ADD, hh);
                      eeprom_write_byte(MM_REPORT_SMS_ADD, mm);
                      eeprom_write_byte(SS_REPORT_SMS_ADD, ss);                  
                      Convert_HH_MM_SS_ToTimeOut(hh, mm, ss);
                      PrintProcessString(CMD_AUTOC_CODE);
                      StrSerialInput.remove(0, ++CommaIndex);
                      return(1);
                    } else {
                      return(0);  //  Invalid command
                    }
                  } else {
                    return(0);    //  Invalid command
                  }
                } else {
                  return(0);      //  Invalid command
                }
              } else {
                return(0);        //  Invalid command
              }               
            } else {
              return(0);          //  Invalid command
            }
          }
          PrintProcessString(CMD_AUTOC_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the settings of the report function
//  "AUTOC?"
uint8_t ProcessCmd_QAUTOC(int8_t CommaIndex) {
  char Temp[10];
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_AUTOC, strlen(CMD_QUERY_AUTOC), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_AUTOC));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_REPORT_SMS, strlen(QUERY_REPORT_SMS), FALSE, FALSE));
    if (TDG133_Flags.Bit.Sms_InOut_Report == 1) {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_ON, strlen(OUTPUT_RELAY_ON), FALSE, FALSE));
    } else {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_OFF, strlen(OUTPUT_RELAY_OFF), FALSE, FALSE));
    }
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_REPORT_SMS_FORMAT, strlen(QUERY_REPORT_SMS_FORMAT), FALSE, FALSE));
    if (TDG133_Flags.Bit.Sms_InOut_Report_Binary == 1) {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)SMS_FORMAT_BYNARY, strlen(SMS_FORMAT_BYNARY), FALSE, FALSE));
    } else {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)SMS_FORMAT_TEXT, strlen(SMS_FORMAT_TEXT), FALSE, FALSE));
    }
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_REPORT_SMS_TIMEOUT, strlen(QUERY_REPORT_SMS_TIMEOUT), FALSE, FALSE));
    sprintf(Temp, "%02d:%02d:%02d", eeprom_read_byte(HH_REPORT_SMS_ADD), eeprom_read_byte(MM_REPORT_SMS_ADD), eeprom_read_byte(SS_REPORT_SMS_ADD));
    Serial.println(Temp);
    VeryLongSmsOut.concat(Temp);
    VeryLongSmsOut.concat("\n");
        
    PrintProcessString(CMD_QAUTOC_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the TEXT format of the SMS in the report mode
//  "FORS:1;pwd"  -> SMS TEXT mode; "pwd" current password
//  "FORS:2;pwd"  -> SMS BINARY mode; "pwd" current password
uint8_t ProcessCmd_FORS(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_FORS, strlen(CMD_FORS), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_FORS));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (StrProcessCmd.toInt() == 1) {
            //  TEXT MODE
            TDG133_Flags.Bit.Sms_InOut_Report_Binary = 0;
          } else if (StrProcessCmd.toInt() == 2) {
            //  BINARY mode
            TDG133_Flags.Bit.Sms_InOut_Report_Binary = 1;
          } else {
            return(0);  //  Invalid command  
          }
          eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
          PrintProcessString(CMD_FORS_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }    
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to enable the Command to store the relay 1 status in case of black-out and restores it when power is back on
//  "RIP1;pwd"    -> Enable recovery relay status; "pwd" current password
//  "RIP0;pwd"    -> Disable recovery relay status; "pwd" current password
uint8_t ProcessCmd_RIP(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
    
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_RIP, strlen(CMD_RIP), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_RIP));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      Pswd = StrProcessCmd.substring(Index + 1);
      StrProcessCmd.remove(Index);
      if (Pswd.equals(StrSysPassword)) {
        if (StrProcessCmd.toInt() == 1) {
          //Enable Relay Status incase of black-out
          TDG133_Flags.Bit.RecoveryStateRelay = 1;
        } else if (StrProcessCmd.toInt() == 0) {
          //  Disable Relay Status incase of black-out
          TDG133_Flags.Bit.RecoveryStateRelay = 0;
        } else {
          return(0);  //  Invalid command
        }
        eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
        PrintProcessString(CMD_RIP_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);     //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the current setting for relay status recovery
//  "RIP?"
uint8_t ProcessCmd_QRIP(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_RIP, strlen(CMD_QUERY_RIP), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_RIP));
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_RECOVERY_RELAY, strlen(QUERY_RECOVERY_RELAY), FALSE, FALSE));
    if (TDG133_Flags.Bit.RecoveryStateRelay == 1) {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_ON, strlen(OUTPUT_RELAY_ON), FALSE, FALSE));
    } else {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_OFF, strlen(OUTPUT_RELAY_OFF), FALSE, FALSE));
    }
    
    PrintProcessString(CMD_QRIP_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to enable the function to send a start-up SMS
//  "AVV1;pwd"    -> Enable the system to send the start-up SMS; "pwd" current password
//  "AVV0;pwd"    -> Disable the system to send the start-up SMS; "pwd" current password
uint8_t ProcessCmd_AVV(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_AVV, strlen(CMD_AVV), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_AVV));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      Pswd = StrProcessCmd.substring(Index + 1);
      StrProcessCmd.remove(Index);
      if (Pswd.equals(StrSysPassword)) {
        if (StrProcessCmd.toInt() == 1) {
          //  Enable Start-Up SMS
          TDG133_Flags.Bit.StartUpSms = 1;
        } else if (StrProcessCmd.toInt() == 0) {
          //  Disable Start-Up SMS
          TDG133_Flags.Bit.StartUpSms = 0;
        } else {
          return(0);  //  Invalid command
        }   
        eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
        PrintProcessString(CMD_AVV_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the text of the message that the system sends during start-up
//  "TSU:xxxxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" current password
//                            The default message is "SYSTEM STARTUP"
uint8_t ProcessCmd_TSU(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TSU, strlen(CMD_TSU), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TSU));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0)  {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(SYSTEM_START_UP_ADD, (POWER_SUPPLY_ALARM_ADD  - SYSTEM_START_UP_ADD));
          WriteEepromString(SYSTEM_START_UP_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TSU_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    } 
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to define how relay 1 (Related to the gate control) should be enabled when there is an incoming call
//  from one of the 200 phone numbers or one of the 8 numbers in the list
//  "TAC:ss;pwd" -> "ss" is the time in seconds during which the relay should remain excited. The default value is 3 seconds; "pwd" current password
//  "TAC:00;pwd" -> Activates bistable mode; "pwd" current password
uint8_t ProcessCmd_TAC(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TAC, strlen(CMD_TAC), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TAC));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (StrProcessCmd.toInt() > 59) {
            return(0);  //  Invalid command  
          }
          Output_1_GateTimeOut = StrProcessCmd.toInt();
          eeprom_write_byte(GATE_TIME_ADD, Output_1_GateTimeOut);
          PrintProcessString(CMD_TAC_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }         
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the forward function. This function is used to forward
//  the generic SMS received at the selected number
//  "ECHO:x;pwd" -> "x" position in the list. "0" disables ECHO function; "pwd" current password
uint8_t ProcessCmd_ECHO(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_ECHO, strlen(CMD_ECHO), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_ECHO));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0)  {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0)  {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          if (StrProcessCmd.toInt() == 0) {
            //  Disable Echo function
            TDG133_Flags.Bit.EchoFunction = 0; 
          } else {
            //  Enable Echo function
            TDG133_Flags.Bit.EchoFunction = 1;
            EchoPhoneNumberPointer = StrProcessCmd.toInt();
            eeprom_write_byte(ECHO_PHONE_NUMBER_POINTER_ADD, EchoPhoneNumberPointer);
          }
          eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);
                
          PrintProcessString(CMD_ECHO_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);      
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      }
    } else {
      return(0);      //  Invalid command
    }       
  } else {
    return(0);        //  Invalid command
  } 
}
//======================================================================

//======================================================================
//  Command used to request the settings of the ECHO function
//  "ECHO?"
uint8_t ProcessCmd_QECHO(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_ECHO, strlen(CMD_QUERY_ECHO), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_ECHO));

    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_ECHO, strlen(QUERY_ECHO), FALSE, FALSE));
    if (TDG133_Flags.Bit.EchoFunction == 1) {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_ON, strlen(OUTPUT_RELAY_ON), FALSE, FALSE));
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_ECHO_PHONE_POINTER, strlen(QUERY_ECHO_PHONE_POINTER), FALSE, FALSE));
      Serial.println(EchoPhoneNumberPointer);
      VeryLongSmsOut.concat(EchoPhoneNumberPointer);
      VeryLongSmsOut.concat("\n");
    } else {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)OUTPUT_RELAY_OFF, strlen(OUTPUT_RELAY_OFF), FALSE, FALSE));
    }
    
    PrintProcessString(CMD_QECHO_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the quality of the radio signal
//  "QUAL?"
uint8_t ProcessCmd_QQUAL(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_QUAL, strlen(CMD_QUERY_QUAL), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_QUAL));
        
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_QUALITY_SIGNAL, strlen(QUERY_QUALITY_SIGNAL), FALSE, FALSE));
    if (Gsm.CSQ_Info.Rssi != 99) {
      Serial.println(Gsm.CSQ_Info.Rssi);
      VeryLongSmsOut.concat(Gsm.CSQ_Info.Rssi);
      VeryLongSmsOut.concat("\n");
    } else {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUALITY_SIGNAL_NOT_KNOWN, strlen(QUALITY_SIGNAL_NOT_KNOWN), FALSE, FALSE));
    }
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_CHANNEL_BIT_ERROR, strlen(QUERY_CHANNEL_BIT_ERROR), FALSE, FALSE));
    if (Gsm.CSQ_Info.Ber != 99) {
      Serial.println(Gsm.CSQ_Info.Ber);  
      VeryLongSmsOut.concat(Gsm.CSQ_Info.Ber);
      VeryLongSmsOut.concat("\n");
    } else {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUALITY_SIGNAL_NOT_KNOWN, strlen(QUALITY_SIGNAL_NOT_KNOWN), FALSE, FALSE));
    }
    PrintProcessString(CMD_QUAL_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to Enable/Disable the notification of the lack of power supply
//  "PWRFx;pwd"  -> "0" Disable notification; "1" Enable notification; "pwd" current password
uint8_t ProcessCmd_PWRF(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_PWRF, strlen(CMD_PWRF), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_PWRF));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      Pswd = StrProcessCmd.substring(Index + 1);
      StrProcessCmd.remove(Index);
      if (Pswd.equals(StrSysPassword)) {
        if (StrProcessCmd.toInt() == 1) {
          //  Enable Lack of power notification
          TDG133_Flags.Bit.LackOfPowerNotification = 1;
        } else if (StrProcessCmd.toInt() == 0) {
          //  Disable Lack of power notification
          TDG133_Flags.Bit.LackOfPowerNotification = 0;
        } else {
          return(0);  //  Invalid command
        }   
        eeprom_write_byte(FLAGS_ADD, TDG133_Flags.Byte.Byte_0);     
        PrintProcessString(CMD_PWRF_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    } 
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to Enable/Disable the notification of return of power supply
//  "PWRRx;pwd"  -> "0" Disable notification; "1" Enable notification; "pwd" current password
uint8_t ProcessCmd_PWRR(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_PWRR, strlen(CMD_PWRR), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_PWRR));
    Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
    if (Index >= 0) {
      Pswd = StrProcessCmd.substring(Index + 1);
      StrProcessCmd.remove(Index);
      if (Pswd.equals(StrSysPassword)) {
        if (StrProcessCmd.toInt() == 1) {
          //  Enable return of power notification
          TDG133_Flags.Bit.PowerUpSms = 1;
        } else if (StrProcessCmd.toInt() == 0) {
          //  Disable return of power notification
          TDG133_Flags.Bit.PowerUpSms = 0;
        } else {
          return(0);  //  Invalid command
        }   
        eeprom_write_byte(INPUTS_LEVEL_THRESHOLD_ADD, TDG133_Flags.Byte.Byte_3 & 0x1F);        
        PrintProcessString(CMD_PWRR_CODE);
        StrSerialInput.remove(0, ++CommaIndex);
        return(1);
      } else {
        return(2);  //  Inserted Invalid Password
      }
    } else {
      return(0);    //  Invalid command
    } 
  } else {
    return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the text of the message that the system sends during lack of power supply
//  "TPWPF:xxxxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" current password
uint8_t ProcessCmd_TPWPF(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TPWPF, strlen(CMD_TPWPF), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TPWPF));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0) {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(POWER_SUPPLY_ALARM_ADD, (RESTORE_POWER_SUPPLY_ADD - POWER_SUPPLY_ALARM_ADD));
          WriteEepromString(POWER_SUPPLY_ALARM_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TPWPF_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      } 
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to set the text of the message that the system sends during go back power supply
//  "TPWPB:xxxxxxxxxxxx;pwd" -> "xxxxxxxxxxxx" is the message you want to write (100 characters max. including spaces). The text message does not accept the semi colon ";" and all letters must be capitalized; "pwd" current password                                                  
uint8_t ProcessCmd_TPWPB(int8_t CommaIndex) {
  String  Pswd;
  int     Index;
  
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_TPWPB, strlen(CMD_TPWPB), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_TPWPB));
    Index = StrProcessCmd.indexOf(ASCII_COLON);
    if (Index >= 0)  {
      StrProcessCmd.remove(0, ++Index);
      Index = StrProcessCmd.indexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        Pswd = StrProcessCmd.substring(Index + 1);
        StrProcessCmd.remove(Index);
        if (Pswd.equals(StrSysPassword)) {
          EraseEeprom(RESTORE_POWER_SUPPLY_ADD, EEPROM_FREE_SPACE_ADD - RESTORE_POWER_SUPPLY_ADD);
          WriteEepromString(RESTORE_POWER_SUPPLY_ADD, StrProcessCmd, StrProcessCmd.length());
          PrintProcessString(CMD_TPWPB_CODE);
          StrSerialInput.remove(0, ++CommaIndex);
          return(1);
        } else {
          return(2);  //  Inserted Invalid Password
        }
      } else {
        return(0);    //  Invalid command
      } 
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request the SIM operator
//  "OPER?"
uint8_t ProcessCmd_QOPER(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_OPER, strlen(CMD_QUERY_OPER), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_OPER));

    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_OPERATOR, strlen(QUERY_OPERATOR), FALSE, FALSE));
    Serial.println((char *)(Gsm.OperatorName));
    VeryLongSmsOut.concat((char *)(Gsm.OperatorName));
    VeryLongSmsOut.concat("\n");
    
    PrintProcessString(CMD_QOPER_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
//  Command used to request TDG133 Rev and GSM Rev
//  "QREV?"
uint8_t ProcessCmd_QREV(int8_t CommaIndex) {
  if (StrProcessCmd.indexOf(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_REV, strlen(CMD_QUERY_REV), TRUE, TRUE)) == 0) {
    StrProcessCmd.remove(0, strlen(CMD_QUERY_REV));

    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_TDG133_REV, strlen(QUERY_TDG133_REV), FALSE, FALSE));
    Serial.println(TDG133_REV);
    VeryLongSmsOut.concat(TDG133_REV);
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_GMI, strlen(QUERY_GMI), FALSE, FALSE));
    Serial.println((char *)(Gsm.ME_Info.GMI_Info));
    VeryLongSmsOut.concat((char *)(Gsm.ME_Info.GMI_Info));
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_GMM, strlen(QUERY_GMM), FALSE, FALSE));
    Serial.println((char *)(Gsm.ME_Info.GMM_Info));
    VeryLongSmsOut.concat((char *)(Gsm.ME_Info.GMM_Info));
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_GMR, strlen(QUERY_GMR), FALSE, FALSE));
    Serial.println((char *)(Gsm.ME_Info.GMR_Info));
    VeryLongSmsOut.concat((char *)(Gsm.ME_Info.GMR_Info));
    VeryLongSmsOut.concat("\n");
    VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)QUERY_GSN, strlen(QUERY_GSN), FALSE, FALSE));
    Serial.println((char *)(Gsm.ME_Info.IMEI_Info));
    VeryLongSmsOut.concat((char *)(Gsm.ME_Info.IMEI_Info));
    VeryLongSmsOut.concat("\n");
    
    PrintProcessString(CMD_QREV_CODE);
    StrSerialInput.remove(0, ++CommaIndex);
    return(1);
  } else {
    return(0);  //  Invalid command
  }
}
//======================================================================

//======================================================================
void PrintProcessString(uint8_t StringCode) {
  VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_PROCESSED_CMD, strlen(CMD_PROCESSED_CMD), FALSE, FALSE));
  Serial.print(char(ASCII_QUOTATION_MARKS));
  VeryLongSmsOut.concat(char(ASCII_QUOTATION_MARKS));
  switch(StringCode)
  {
    case CMD_PWD_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_PWD, strlen(CMD_PWD), FALSE, FALSE));
      break;
    case CMD_PRNEE_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_PRN_EE, strlen(CMD_PRN_EE), FALSE, FALSE));
      break;      
    case CMD_QANUM_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_ANUM, strlen(CMD_QUERY_ANUM), FALSE, FALSE));
      break;
    case CMD_QFNUM_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_FNUM, strlen(CMD_QUERY_FNUM), FALSE, FALSE));
      break;   
    case CMD_QNUM_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_NUM, strlen(CMD_QUERY_NUM), FALSE, FALSE));
      break;
    case CMD_NUM_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_NUM, strlen(CMD_NUM), FALSE, FALSE));
      break;
    case CMD_MAC_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_MAC, strlen(CMD_MAC), FALSE, FALSE));
      break;
    case CMD_DAC_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_DAC, strlen(CMD_DAC), FALSE, FALSE));
      break;
    case CMD_RES_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_RES, strlen(CMD_RES), FALSE, FALSE));
      break;
    case CMD_DSMS_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_DEL_SMS, strlen(CMD_DEL_SMS), FALSE, FALSE));
      break;
    case CMD_DASMS_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_DEL_ALL_SMS, strlen(CMD_DEL_ALL_SMS), FALSE, FALSE));
      break;
    case CMD_SMS_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_SMS, strlen(CMD_SMS), FALSE, FALSE));
      break;
    case CMD_VOC_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_VOC, strlen(CMD_VOC), FALSE, FALSE));
      break;
    case CMD_LIV_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_LIV, strlen(CMD_LIV), FALSE, FALSE));
      break;
    case CMD_QLIV_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_LIV, strlen(CMD_QUERY_LIV), FALSE, FALSE));
      break;
    case CMD_INI_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_INI, strlen(CMD_INI), FALSE, FALSE));
      break;
    case CMD_QIN_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_INI, strlen(CMD_QUERY_INI), FALSE, FALSE));
      break;
    case CMD_TIZ1_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TIZ1, strlen(CMD_TIZ1), FALSE, FALSE));
      break;
    case CMD_TIZ2_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TIZ2, strlen(CMD_TIZ2), FALSE, FALSE));
      break;
    case CMD_OSS_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_OSS, strlen(CMD_OSS), FALSE, FALSE));
      break;
    case CMD_QOSS_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_OSS, strlen(CMD_QUERY_OSS), FALSE, FALSE));
      break;
    case CMD_TIN1A_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TIN1A, strlen(CMD_TIN1A), FALSE, FALSE));
      break;
    case CMD_TIN1B_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TIN1B, strlen(CMD_TIN1B), FALSE, FALSE));
      break;
    case CMD_TIN2A_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TIN2A, strlen(CMD_TIN2A), FALSE, FALSE));
      break;
    case CMD_TIN2B_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TIN2B, strlen(CMD_TIN2B), FALSE, FALSE));
      break;
    case CMD_ALN_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_ALN, strlen(CMD_ALN), FALSE, FALSE));
      break;
    case CMD_QALN_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_ALN, strlen(CMD_QUERY_ALN), FALSE, FALSE));
      break;
    case CMD_OUT_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_OUT, strlen(CMD_OUT), FALSE, FALSE));
      break;
    case CMD_QSTA_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_STA, strlen(CMD_QUERY_STA), FALSE, FALSE));
      break;
    case CMD_AUTOC_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_AUTOC, strlen(CMD_AUTOC), FALSE, FALSE));
      break;
    case CMD_QAUTOC_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_AUTOC, strlen(CMD_QUERY_AUTOC), FALSE, FALSE));
      break;
    case CMD_FORS_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_FORS, strlen(CMD_FORS), FALSE, FALSE));
      break;
    case CMD_RIP_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_RIP, strlen(CMD_RIP), FALSE, FALSE));
      break;
    case CMD_QRIP_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_RIP, strlen(CMD_QUERY_RIP), FALSE, FALSE));
      break;
    case CMD_AVV_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_AVV, strlen(CMD_AVV), FALSE, FALSE));
      break;
    case CMD_TSU_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TSU, strlen(CMD_TSU), FALSE, FALSE));
      break;
    case CMD_TAC_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TAC, strlen(CMD_TAC), FALSE, FALSE));
      break;
    case CMD_ECHO_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_ECHO, strlen(CMD_ECHO), FALSE, FALSE));
      break;
    case CMD_QECHO_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_ECHO, strlen(CMD_QUERY_ECHO), FALSE, FALSE));
      break;
    case CMD_QUAL_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_QUAL, strlen(CMD_QUERY_QUAL), FALSE, FALSE));
      break;
    case CMD_RISP_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_RISP, strlen(CMD_RISP), FALSE, FALSE));
      break;
    case CMD_PWRF_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_PWRF, strlen(CMD_PWRF), FALSE, FALSE));
      break;
    case CMD_PWRR_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_PWRR, strlen(CMD_PWRR), FALSE, FALSE));
      break;
    case CMD_TPWPF_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TPWPF, strlen(CMD_TPWPF), FALSE, FALSE));
      break;
    case CMD_TPWPB_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_TPWPB, strlen(CMD_TPWPB), FALSE, FALSE));
      break;
    case CMD_QOPER_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_OPER, strlen(CMD_QUERY_OPER), FALSE, FALSE));
      break;
    case CMD_QREV_CODE:
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_QUERY_REV, strlen(CMD_QUERY_REV), FALSE, FALSE));
      break;
    default:
      break;
  }
  Serial.print(char(ASCII_QUOTATION_MARKS));  
  Serial.print(char(ASCII_SPACE));
  VeryLongSmsOut.concat(char(ASCII_QUOTATION_MARKS));
  VeryLongSmsOut.concat(char(ASCII_SPACE));
  VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_PROCESSED_SUCCES, strlen(CMD_PROCESSED_SUCCES), FALSE, FALSE));
}
//======================================================================

//======================================================================
void SavePhoneNumberArray(void) {
  uint8_t  * SourceArray;  
  uint8_t  * CopyArray;
  uint8_t    Count = 0;
  
  SourceArray = &PhoneBook.PhoneNumber[0];
  CopyArray   = &PhoneNumberCopy[0];
  do {
    *(uint8_t *)CopyArray++ = *(uint8_t *)SourceArray++;
  } while (++Count < MAX_LENGHT_PN);
}
//======================================================================

//======================================================================
void RestorePhoneNumberArray(void) {
  uint8_t  * SourceArray;  
  uint8_t  * CopyArray;
  uint8_t    Count = 0;
  
  SourceArray = &PhoneBook.PhoneNumber[0];
  CopyArray   = &PhoneNumberCopy[0];
  do {
    *(uint8_t *)SourceArray++ = *(uint8_t *)CopyArray++;
  } while (++Count < MAX_LENGHT_PN);
}
//======================================================================

//======================================================================
String ReadStringCmd_FLASH(uint8_t *FlashPointer, uint8_t Lenght, boolean PrintCR, boolean NoPrint) {
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

//======================================================================
String ReadEepromString(uint16_t EeAdd, uint8_t MaxLenght) {
  String  TempData = "";
  uint8_t Count    = 0;

  do {
    if (eeprom_read_byte((uint8_t *)EeAdd) != 0x00) {
      TempData += char(eeprom_read_byte((uint8_t *)EeAdd++));    
    } else {
      break;
    }
  } while (Count++ < MaxLenght);

  return(TempData);
}
//======================================================================

//======================================================================
void WriteEepromString(uint16_t EeAdd, String Data, uint8_t MaxLenght) {
  uint8_t Count    = 0;
  
  do {
    if (Data[Count] != 0x00) {
      eeprom_write_byte(EeAdd++, Data[Count]);
    } else {
      break;
    }
  } while (Count++ < MaxLenght);
}
//======================================================================

//======================================================================
//  This function Erase Eeprom
void EraseEeprom(uint16_t EeAdd, uint8_t MaxLenght) {
  uint8_t Count    = 0;
  
  do {
    if (eeprom_read_byte(EeAdd) != 0x00) {
      eeprom_write_byte(EeAdd++, 0x00);
    }
  } while (Count++ < MaxLenght);
}
//======================================================================
