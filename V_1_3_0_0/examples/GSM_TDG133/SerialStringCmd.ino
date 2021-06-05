//======================================================================
void ProcessSerialCmd(void) {
  if (Serial.available() > 0) {
    Io.LedOn(TRIGGER_3);
    ReadSerialCmd();
    ProcessReceivedStringCmd();
    if (System_Flags.Bit.SetupAtCmdInProgress == 0) {
      Io.LedOff(TRIGGER_3);  
    }
  }
}
//======================================================================

//======================================================================
void ReadSerialCmd(void) {
  StrSerialInput = "";
  StrSerialInput = Serial.readString();
  StrSerialInput.trim();
  StrSerialInput.toUpperCase();
  ReadStringCmd_FLASH((uint8_t *)CMD_RECEIVED, strlen(CMD_RECEIVED), FALSE, FALSE);
  Serial.println(StrSerialInput);
}
//======================================================================

//======================================================================
uint8_t TestSerialInputData(void) {
  uint8_t  CharIndex = 0;
  uint8_t  CharCode  = 0;
  
  do {
    CharCode = char(StrSerialInput.charAt(CharIndex++));
    if ((CharCode < ASCII_SPACE) || (CharCode > ASCII_z)) {
      return(0);  //  Ivalid char
    }
  } while (CharIndex < StrSerialInput.length());
  return(1);
}
//======================================================================

//======================================================================
void ProcessReceivedStringCmd(void) {
  int8_t  Index;
  int8_t  Index_2;
  uint8_t CmdCount;
  uint8_t Response;
  String  TempPswd = "";
  
  if (TestSerialInputData() == 0) {
    //  Cmd Text contain invalid char
    ReadStringCmd_FLASH((uint8_t *)CMD_INVALID_CHAR, strlen(CMD_INVALID_CHAR), FALSE, FALSE);
    return; 
  }
  VeryLongSmsOut = "";
  TDG133_Flags.Bit.ReplyMessage = 1;
  do {
    Index = StrSerialInput.indexOf(ASCII_COMMA);
    if (Index >= 0) {
      //--------------------------------------------------------
      //  Process Multi-commands 
      StrProcessCmd = StrSerialInput.substring(0, Index);
      Index_2 = StrSerialInput.lastIndexOf(ASCII_SEMI_COLON);
      if (Index_2 >= 0) {
        //  Found password on the end of string command
        if (StrSerialInput.indexOf(ASCII_SEMI_COLON) != Index_2) {
          //  More password in the string command. Error, when is used a multi string command only one password is necessary
          VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_INVALID_PASSWORD, strlen(CMD_INVALID_PASSWORD), TRUE, FALSE));
          return;
        }        
        TempPswd = StrSerialInput.substring(Index_2 + 1);
        StrSerialInput.remove(Index_2);    
        StrProcessCmd.concat(char(ASCII_SEMI_COLON));
        StrProcessCmd.concat(TempPswd);
      } else {
        //  Password not found
        if (TempPswd.length() > 0) {
          //  Check if the password is stored into Temp String
          StrProcessCmd.concat(char(ASCII_SEMI_COLON));
          StrProcessCmd.concat(TempPswd);           
        }
      }
      //--------------------------------------------------------
    } else {
      //  Process Single command
      Index = StrSerialInput.length();
      StrProcessCmd = StrSerialInput;
      if (TempPswd.length() > 0) {
        //  Check if the password is stored into Temp String
        StrProcessCmd.concat(char(ASCII_SEMI_COLON));
        StrProcessCmd.concat(TempPswd);           
      }
    }
    CmdCount = 0;
    do {
      switch (CmdCount)
      {
        case CMD_PWD_CODE:        //  Code 00 -> Command used to change the password
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_PWD_CODE)); 
          Response = ProcessCmd_PWD(Index);
          break;
        case CMD_PRNEE_CODE:      //  Code 01 -> Command used to print eeprom data
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_PRNEE_CODE)); 
          Response = ProcessCmd_PRNEE(Index);
          break;
        case CMD_QANUM_CODE:      //  Code 02 -> Command used to request all phone numbers currently stored in the device (250 locations)
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QANUM_CODE)); 
          Response = ProcessCmd_QANUM(Index);
          break;
        case CMD_QFNUM_CODE:      //  Code 03 -> Command used to find a number into a PhoneBook
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QFNUM_CODE)); 
          Response = ProcessCmd_QFNUM(Index);
          break;
        case CMD_QNUM_CODE:       //  Code 04 -> Command used to request the list of phone numbers currently stored in the device (Only first 8 locations)
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QNUM_CODE)); 
          Response = ProcessCmd_QNUM(Index);
          break;
        case CMD_NUM_CODE:        //  Code 05 -> Command used to store a phone number
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_NUM_CODE)); 
          Response = ProcessCmd_NUM(Index);
          break;
        case CMD_MAC_CODE:        //  Code 6 -> Not implemented
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_MAC_CODE)); 
          Response = ProcessCmd_MAC(Index);
          break;
        case CMD_DAC_CODE:        //  Code 7 -> Not implemented
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_DAC_CODE)); 
          Response = ProcessCmd_DAC(Index);
          break;
        case CMD_RES_CODE:        //  Code 08 -> Command used to reset the initial settings of the system and delete all stored phone numbers
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_RES_CODE)); 
          Response = ProcessCmd_RES(Index);
          break;
        case CMD_DSMS_CODE:       //  Code 09 -> Command used to delete SMS by SIM memory  
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_DSMS_CODE)); 
          Response = ProcessCmd_DSMS(Index);
          break;
        case CMD_DASMS_CODE:      //  Code 10 -> Command used to delete all SMS by SIM memory
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_DASMS_CODE)); 
          Response = ProcessCmd_DASMS(Index);
          break;
        case CMD_SMS_CODE:        //  Code 11 -> Command used to allow the number in the specified position to receive SMS on input status
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_SMS_CODE)); 
          Response = ProcessCmd_SMS(Index);
          break;
        case CMD_VOC_CODE:        //  Code 12 -> Command used to allows the number on the specified position to receive the ringtone on the inputs status
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_VOC_CODE)); 
          Response = ProcessCmd_VOC(Index);
          break;
        case CMD_QLIV_CODE:       //  Code 13 -> Command used to request the alarm activation level
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QLIV_CODE)); 
          Response = ProcessCmd_QLIV(Index);
          break;
        case CMD_LIV_CODE:        //  Code 14 -> Command used to set a HIGH, LOW or TOGGLE level as the alarm condition for inputs IN1 or IN2
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_LIV_CODE)); 
          Response = ProcessCmd_LIV(Index);
          break;
        case CMD_QIN_CODE:        //  Code 15 -> Command used to request the current inhibition time setting regarding the inputs
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QIN_CODE)); 
          Response = ProcessCmd_QIN(Index);
          break;
        case CMD_INI_CODE:        //  Code 16 -> Command used to set the inhibition time on input 1
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_INI_CODE)); 
          Response = ProcessCmd_INI(Index);
          break;
        case CMD_TIZ1_CODE:       //  Code 17 -> Command used to reset/disable the inhibition time if input 1 is idle
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TIZ1_CODE)); 
          Response = ProcessCmd_TIZ1(Index);
          break;
        case CMD_TIZ2_CODE:       //  Code 18 -> Command used to reset/disable the inhibition time if input 2 is idle
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TIZ2_CODE)); 
          Response = ProcessCmd_TIZ2(Index);
          break;
        case CMD_QOSS_CODE:       //  Code 19 -> Command used to request the current observation time setting regarding the inputs
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QOSS_CODE)); 
          Response = ProcessCmd_QOSS(Index);
          break;
        case CMD_OSS_CODE:        //  Code 20 -> Command used to define the lenght of the observation time regarding input 1
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_OSS_CODE)); 
          Response = ProcessCmd_OSS(Index);
          break;
        case CMD_TIN1A_CODE:      //  Code 21 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension presence:
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TIN1A_CODE)); 
          Response = ProcessCmd_TIN1A(Index);
          break;
        case CMD_TIN1B_CODE:      //  Code 22 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension absence:
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TIN1B_CODE)); 
          Response = ProcessCmd_TIN1B(Index);
          break;
        case CMD_TIN2A_CODE:      //  Code 23 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension presence:
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TIN2A_CODE)); 
          Response = ProcessCmd_TIN2A(Index);
          break;
        case CMD_TIN2B_CODE:      //  Code 24 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension absence:
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TIN2B_CODE)); 
          Response = ProcessCmd_TIN2B(Index);
          break;
        case CMD_QALN_CODE:       //  Code 25 -> Command used to request the current setting for the number of sms thath must be send when an alarm occurs
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QALN_CODE)); 
          Response = ProcessCmd_QALN(Index);
          break;
        case CMD_ALN_CODE:        //  Code 26 -> Command used to define the number of SMS that the device must send when the alarm occurs
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_ALN_CODE)); 
          Response = ProcessCmd_ALN(Index);
          break;
        case CMD_OUT_CODE:        //  Code 27 -> Command used to activate the specified output relay
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_OUT_CODE));         
          Response = ProcessCmd_OUT(Index);
          break;
        case CMD_QSTA_CODE:       //  Code 28 -> Command used to request the condition of the remote outputs
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QSTA_CODE)); 
          Response = ProcessCmd_QSTA(Index);
          break;
        case CMD_QAUTOC_CODE:     //  Code 29 -> Command used to request the settings of the report function
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QAUTOC_CODE)); 
          Response = ProcessCmd_QAUTOC(Index);
          break;
        case CMD_AUTOC_CODE:      //  Code 30 -> Command used to set the time between report SMS sending
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_AUTOC_CODE)); 
          Response = ProcessCmd_AUTOC(Index);
          break;
        case CMD_FORS_CODE:       //  Code 31 -> Command used to set the TEXT format of the SMS in the report mode
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_FORS_CODE)); 
          Response = ProcessCmd_FORS(Index);
          break;
        case CMD_QRIP_CODE:       //  Code 32 -> Command used to request the current setting for relay status recovery
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QRIP_CODE)); 
          Response = ProcessCmd_QRIP(Index);
          break;
        case CMD_RIP_CODE:        //  Code 33 -> Command used to enable the Command to store the relay 1 status in case of black-out and restores it when power is back on
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_RIP_CODE)); 
          Response = ProcessCmd_RIP(Index);
          break;
        case CMD_AVV_CODE:        //  Code 34 -> Command used to enable the function to send a start-up SMS
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_AVV_CODE)); 
          Response = ProcessCmd_AVV(Index);
          break;
        case CMD_TSU_CODE:        //  Code 35 -> Command used to set the text of the message that the system sends during start-up
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TSU_CODE)); 
          Response = ProcessCmd_TSU(Index);
          break;
        case CMD_TAC_CODE:        //  Code 36 -> Command used to define how relay 1 (Related to the gate control) should be enabled when there is an incoming call from one of the 200 phone numbers or one of the 8 numbers in the list
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TAC_CODE)); 
          Response = ProcessCmd_TAC(Index);
          break;
        case CMD_QECHO_CODE:      //  Code 37 -> Command used to request the settings of the ECHO function
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QECHO_CODE)); 
          Response = ProcessCmd_QECHO(Index);
          break;
        case CMD_ECHO_CODE:       //  Code 38 -> Command used to set the forward function. This function is used to forward the generic SMs received at the selected number
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_ECHO_CODE)); 
          Response = ProcessCmd_ECHO(Index);
          break;
        case CMD_QUAL_CODE:       //  Code 39 -> Command used to request the quality of the radio signal
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QUAL_CODE)); 
          Response = ProcessCmd_QQUAL(Index);
          break;
        case CMD_RISP_CODE:       //  Code 40 -> Command used to disable all reply message. This command must be positioned at the beginning of a multiple message
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_RISP_CODE)); 
          Response = ProcessCmd_RISP(Index);
          break;
        case CMD_PWRF_CODE:       //  Code 41 -> Command used to Enable/Disable the notification of the lack of power supply
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_PWRF_CODE)); 
          Response = ProcessCmd_PWRF(Index);
          break;
        case CMD_PWRR_CODE:       //  Code 42 -> Command used to Enable/Disable the notification of return of power supply
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_PWRR_CODE)); 
          Response = ProcessCmd_PWRR(Index);
          break;
        case CMD_TPWPF_CODE:      //  Code 43 -> Command used to set the text of the message that the system sends during lack of power supply
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TPWPF_CODE)); 
          Response = ProcessCmd_TPWPF(Index);
          break;
        case CMD_TPWPB_CODE:      //  Code 44 -> Command used to set the text of the message that the system sends during go back power supply
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_TPWPB_CODE)); 
          Response = ProcessCmd_TPWPB(Index);
          break;
        case CMD_QOPER_CODE:      //  Code 45 -> Command used to request the SIM Operator Name
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QOPER_CODE)); 
          Response = ProcessCmd_QOPER(Index);
          break;
        case CMD_QREV_CODE:       //  Code 46 -> Command used to request the TDG133 Firmware Rev and GSM Rev 
          StrProcessCmd.concat(CheckPwdOrNotPwd(CMD_QREV_CODE)); 
          Response = ProcessCmd_QREV(Index);
          break;
        default:
          break;
      }
      if (Response == 1) {
        //  Command received and processed with success
        break;
      } else if (Response == 2) {
        //  Invalid password
        StrSerialInput = "";
        VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_INVALID_PASSWORD, strlen(CMD_INVALID_PASSWORD), TRUE, FALSE));
        break;
      } else {
        //  Received invalid command
        if (CmdCount == MAX_CMD_CODE) {
          VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE));
          StrSerialInput.remove(0, ++Index);
          if (System_Flags.Bit.IncomingSmsInProgress == 1) {
            System_Flags.Bit.OutGoingEchoSms = 1;
          }
        }
      }
    } while (CmdCount++ < MAX_CMD_CODE);
  } while (StrSerialInput.length() > 0);

  System_Flags.Bit.PhoneBookIndexFound  = 0;
  System_Flags.Bit.PhoneBookIndexHigher = 0;
  PhoneBook.IndexLocation = 0;

  if (System_Flags.Bit.IncomingSmsInProgress == 1) {
    System_Flags.Bit.SendAnswerSms = 0;
    if (TDG133_Flags.Bit.ReplyMessage != 0) {
      System_Flags.Bit.SendAnswerSms = 1;
    }
  }
}
//======================================================================

//======================================================================
String CheckPwdOrNotPwd(uint8_t Cmdcode) {
  int8_t  Index;
  
  switch (Cmdcode)
  {
    //-----------------------------------------------------------------------------------------------------------------------------------------------------  
    //  Commands that must have a password
    case CMD_PWD_CODE:            //  Code 00 -> Command used to change the password
    case CMD_PRNEE_CODE:          //  Code 01 -> Command used to print eeprom data
    case CMD_QANUM_CODE:          //  Code 02 -> Command used to request all phone numbers currently stored in the device (250 locations)
    case CMD_QFNUM_CODE:          //  Code 03 -> Command used to find a number into a PhoneBook
    case CMD_QNUM_CODE:           //  Code 04 -> Command used to request the list of phone numbers currently stored in the device (Only first 8 locations)
    case CMD_NUM_CODE:            //  Code 05 -> Command used to store a phone number
    //case CMD_MAC_CODE:          //  Code 06 -> Not implemented
    //case CMD_DAC_CODE:          //  Code 07 -> Not implemented 
    case CMD_RES_CODE:            //  Code 08 -> Command used to reset the initial settings of the system and delete all stored phone numbers
    case CMD_SMS_CODE:            //  Code 11 -> Command used to allow the number in the specified position to receive SMS on input status
    case CMD_VOC_CODE:            //  Code 12 -> Command used to allows the number on the specified position to receive the ringtone on the inputs status
      return("");
      break;
    //-----------------------------------------------------------------------------------------------------------------------------------------------------  
      
    //-----------------------------------------------------------------------------------------------------------------------------------------------------  
    //  Commands that must have a password if SIM PhoneBook index is major of 8
    case CMD_DSMS_CODE:           //  Code 09 -> Command used to delete SMS by SIM memory  
    case CMD_DASMS_CODE:          //  Code 10 -> Command used to delete all SMS by SIM memory
    case CMD_LIV_CODE:            //  Code 14 -> Command used to set a HIGH, LOW or TOGGLE level as the alarm condition for inputs IN1 or IN2
    case CMD_INI_CODE:            //  Code 16 -> Command used to set the inhibition time on input 1
    case CMD_TIZ1_CODE:           //  Code 17 -> Command used to reset/disable the inhibition time if input 1 is idle
    case CMD_TIZ2_CODE:           //  Code 18 -> Command used to reset/disable the inhibition time if input 2 is idle
    case CMD_OSS_CODE:            //  Code 20 -> Command used to define the lenght of the observation time regarding input 1
    case CMD_TIN1A_CODE:          //  Code 21 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension presence:
    case CMD_TIN1B_CODE:          //  Code 22 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 1 gets the alert of tension absence:
    case CMD_TIN2A_CODE:          //  Code 23 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension presence:
    case CMD_TIN2B_CODE:          //  Code 24 -> Command used to define the message the device sends to the numbers in the list enabled to receive alarm SMS when the input 2 gets the alert of tension absence:
    case CMD_ALN_CODE:            //  Code 26 -> Command used to define the number of SMS that the device must send when the alarm occurs
    case CMD_OUT_CODE:            //  Code 27 -> Command used to activate the specified output relay
    case CMD_AUTOC_CODE:          //  Code 30 -> Command used to set the time between report SMS sending
    case CMD_FORS_CODE:           //  Code 31 -> Command used to set the TEXT format of the SMS in the report mode
    case CMD_RIP_CODE:            //  Code 33 -> Command used to enable the Command to store the relay 1 status in case of black-out and restores it when power is back on
    case CMD_AVV_CODE:            //  Code 34 -> Command used to enable the function to send a start-up SMS
    case CMD_TSU_CODE:            //  Code 35 -> Command used to set the text of the message that the system sends during start-up
    case CMD_TAC_CODE:            //  Code 36 -> Command used to define how relay 1 (Related to the gate control) should be enabled when there is an incoming call from one of the 200 phone numbers or one of the 8 numbers in the list
    case CMD_ECHO_CODE:           //  Code 38 -> Command used to set the forward function. This function is used to forward the generic SMs received at the selected number
    case CMD_RISP_CODE:           //  Code 40 -> Command used to disable all reply message. This command must be positioned at the beginning of a multiple message
    case CMD_PWRF_CODE:           //  Code 41 -> Command used to Enable/Disable the notification of the lack of power supply
    case CMD_PWRR_CODE:           //  Code 42 -> Command used to Enable/Disable the notification of return of power supply
    case CMD_TPWPF_CODE:          //  Code 43 -> Command used to set the text of the message that the system sends during lack of power supply
    case CMD_TPWPB_CODE:          //  Code 44 -> Command used to set the text of the message that the system sends during go back power supply
      Index = StrProcessCmd.lastIndexOf(ASCII_SEMI_COLON);
      if (Index >= 0) {
        return("");
      } else {
        if (System_Flags.Bit.PhoneBookIndexFound == 1) {
          if (System_Flags.Bit.PhoneBookIndexHigher == 0) {
            //  First 8 SIM PhoneBook index
            return(char(ASCII_SEMI_COLON) + StrSysPassword);  
          }          
        }     
      }
      break;
    //-----------------------------------------------------------------------------------------------------------------------------------------------------  
    
    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    //  Commands that not require a password
    case CMD_QLIV_CODE:           //  Code 13 -> Command used to request the alarm activation level
    case CMD_QIN_CODE:            //  Code 15 -> Command used to request the current inhibition time setting regarding the inputs
    case CMD_QOSS_CODE:           //  Code 19 -> Command used to request the current observation time setting regarding the inputs
    case CMD_QALN_CODE:           //  Code 25 -> Command used to request the current setting for the number of sms thath must be send when an alarm occurs
    case CMD_QSTA_CODE:           //  Code 28 -> Command used to request the condition of the remote outputs
    case CMD_QAUTOC_CODE:         //  Code 29 -> Command used to request the settings of the report function
    case CMD_QRIP_CODE:           //  Code 32 -> Command used to request the current setting for relay status recovery
    case CMD_QECHO_CODE:          //  Code 37 -> Command used to request the settings of the ECHO function
    case CMD_QUAL_CODE:           //  Code 39 -> Command used to request the quality of the radio signal
    case CMD_QOPER_CODE:          //  Code 45 -> Command used to request the SIM Operator Name
    case CMD_QREV_CODE:           //  Code 46 -> Command used to request the TDG133 Firmware Rev and GSM Rev 
      return("");
      break;
    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    default:
      return("");
      break;
  }
  return("");
}
//======================================================================
