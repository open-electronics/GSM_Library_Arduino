//======================================================================
void ProcessStateMachineGsm(void) {
  Gsm.ExecuteUartState();
  if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
    Gsm.InitGsmSendCmd();
    Gsm.InitGsmWaitAnswer();
  } else {  
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();

    if (System_Flags.Bit.TestFirstSimPbIndex == 0) {
      ProcessSystemATcmd();         //  State machine to process System AT command
      ProcessUserATcmd();           //  State machine to process User AT command
      ProcessSerialCmd();           //  Process config commands received from IDE Serial Monitor or SMS

      SendSmsPowerUp();             //  Send SMS Power-Up (If enabled)
      if (TDG133_Flags.Bit.PowerUpSms == 0) {
        SendSmsStartUp();           //  Send SMS Start-Up (If enabled)
        if (TDG133_Flags.Bit.StartUpSms == 0) {
          if ((InputAlarm.Input[0].SmsOutProcessBusy == 0) && (InputAlarm.Input[1].SmsOutProcessBusy == 0)) {
            if ((System_Flags.Bit.IncomingCallInProgress == 0) && (System_Flags.Bit.IncomingSmsInProgress == 0) && \
                (System_Flags.Bit.OutGoingEchoSms        == 0) && (System_Flags.Bit.OutGoingAnswerSms     == 0)) {
              //  Process Outgoing SMS input report (When timeout expired)
              TestTimeOutSmsInputReport();
            }
            if ((System_Flags.Bit.SmsReportInProgress == 0) && (System_Flags.Bit.IncomingSmsInProgress == 0) && \
                (System_Flags.Bit.OutGoingEchoSms     == 0) && (System_Flags.Bit.OutGoingAnswerSms     == 0)) {
              //  Process Incoming Call 
              IncomingCall_Management();
            }
            if ((System_Flags.Bit.IncomingCallInProgress == 0) && (System_Flags.Bit.SmsReportInProgress == 0) && \
                (System_Flags.Bit.OutGoingEchoSms        == 0) && (System_Flags.Bit.OutGoingAnswerSms   == 0)) {
              //  Process Incoming SMS
              IncomingSms_Management();
            }
            if ((System_Flags.Bit.IncomingCallInProgress == 0) && (System_Flags.Bit.IncomingSmsInProgress == 0) && \
                (System_Flags.Bit.SmsReportInProgress    == 0) && (System_Flags.Bit.OutGoingAnswerSms     == 0)) {
              //  Process Outgoing Echo SMS
              OutcomingEchoSms();
            }
            if ((System_Flags.Bit.IncomingCallInProgress == 0) && (System_Flags.Bit.SmsReportInProgress == 0) && \
                (System_Flags.Bit.OutGoingEchoSms        == 0) && (System_Flags.Bit.SmsReportInProgress == 0)) {
              //  Process Outgoing Answer SMS
              OutcomingAnswerSms(); 
            }            
          }
          if ((System_Flags.Bit.SmsReportInProgress == 0) && (System_Flags.Bit.IncomingCallInProgress == 0)) {
            if (InputAlarm.Input[1].SmsOutProcessBusy != 1) {
              SendSmsInputState(0);         //  Send SMS Input 1 State  
            }
            if (InputAlarm.Input[0].SmsOutProcessBusy != 1) {
              SendSmsInputState(1);         //  Send SMS Input 2 State
            }            
          }
        }
      }      
    } else {
      //  PhoneBook check. Check if at the index = 1 of the PhoneBook is present a valid Phone Number
      //  This check is esecuted only after initialization of GSM engine at the power up of system
      //  If in the PhoneBook is not present a valid phone number then the system wait up to 2 minutes
      //  waiting for an incoming call to save, in the first index of the PhoneBook, a valid phone number
      //  This Phone Number will be named "Admin"
      TestSimPhoneBookLed();        //  Leds yellow LD6 and LD7 blinks alternately
      TestSimPhoneBook();           //  State Machine to process Sim Test
    }
    
    if (System_Flags.Bit.SetupAtCmdInProgress == 0) {
      Io.LedBlink(TRIGGER_3, 25, T_2SEC);
    }
  } 
}
//======================================================================

//======================================================================
//  State machine used to manage Answer SMS after setting command
void SendAnswerSms_Idle(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (System_Flags.Bit.SetupAtCmdInProgress != 1) {
      if (System_Flags.Bit.SendAnswerSms != 0) {
        System_Flags.Bit.SendAnswerSms     = 0;
        System_Flags.Bit.OutGoingAnswerSms = 1;
        
        OutcomingAnswerSms = SendAnswerSms;
      }    
    } 
  }
}
void SendAnswerSms(void) {
  uint16_t StringLength;
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    StringLength = VeryLongSmsOut.length();
    Gsm.ClearBuffer(&Sms.SmsText[0], sizeof(Sms.SmsText));
    VeryLongSmsOut.toCharArray(Sms.SmsText, MAX_SMS_LENGHT);  
    Sms.SetCmd_AT_CMGS();
    
    if (StringLength > MAX_SMS_LENGHT) {
      VeryLongSmsOut.remove(0, (MAX_SMS_LENGHT - 1));
    } else {
      System_Flags.Bit.OutGoingAnswerSms = 0;
      OutcomingAnswerSms = SendAnswerSms_Idle;
    }
  }
}
//======================================================================

//======================================================================
//  State machine used to manage Outgoing Echo SMS
void OutgoingEchoSms_Idle(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (System_Flags.Bit.OutGoingEchoSms == 1) {
      PhoneBook.SetCmd_AT_CPBR(EchoPhoneNumberPointer, false);  //  AT cmd used to read location of the PhoneBook
      OutcomingEchoSms = OutgoingSendEchoSms;
    }
  }
}
void OutgoingSendEchoSms(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    Sms.SetCmd_AT_CMGS();
    System_Flags.Bit.OutGoingEchoSms = 0;
    OutcomingEchoSms = OutgoingEchoSms_Idle;
  }
}
//======================================================================

//======================================================================
//  State machine used to manage Incoming Call
void IncomingCall_Idle(void) { 
  if (Gsm.GsmFlag.Bit.IncomingCall == 1) {
    Gsm.GsmFlag.Bit.IncomingCall = 0;
    System_Flags.Bit.IncomingCallInProgress = 1;
    TimeOutVoiceCall = T_3SEC;
    IncomingCall_Management = HangUpIncomingCall;  //  Next State of the State Machine
  }
}
void HangUpIncomingCall(void) {
  Io.LedBlink(PIN_LED9, 25, T_500MSEC);
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (TimeOutVoiceCall == 0) {
      PhonicCall.SetCmd_ATH(0);
      IncomingCall_Management = IncomingCall_CheckNumber;  //  Next State of the State Machine 
    }
  }
}
void IncomingCall_CheckNumber(void) {
  Io.LedBlink(PIN_LED9, 25, T_500MSEC); 
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    PhoneBook.IndexLocation = 0;
    PhoneBook.SetCmd_AT_CPBF();
    IncomingCall_Management = IncomingCall_ExecuteCmd;  //  Next State of the State Machine
  }
}
void IncomingCall_ExecuteCmd(void) {
  Io.LedBlink(PIN_LED9, 25, T_500MSEC);
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (PhoneBook.IndexLocation > 0) {
      System_Flags.Bit.IncomingCallInProgress = 0;
      if (Output_1_GateTimeOut > 0) {
        //  Pulse mode
        TimeOutGateFunction = Output_1_GateTimeOut * T_1SEC; 
        StateRelay[0] = RELAY_STATE_ON;
        IncomingCall_Management = IncomingCall_EndCmd;  //  Next State of the State Machine
      } else {
        //  Bistable mode
        StateRelay[0] = RELAY_STATE_TOGGLE_ONLY;
        IncomingCall_Management = IncomingCall_Idle;  //  Idle State Machine    
        Io.LedOff(PIN_LED9); 
      }
    } else {
      IncomingCall_Management = IncomingCall_Idle;  //  Idle State Machine    
      Io.LedOff(PIN_LED9); 
    }    
  }
}
void IncomingCall_EndCmd(void) {
  if (TimeOutGateFunction == 0) {
    StateRelay[0] = RELAY_STATE_OFF;
    IncomingCall_Management = IncomingCall_Idle;  //  Idle State Machine
    Io.LedOff(PIN_LED9); 
  }
}
//======================================================================

//======================================================================
//  State machine used to check if a valid phone number was saved into first index of PhoneBook
void TestPhoneBook_Idle(void) { }
void TestPhoneBook_Step1(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    PhoneIndex = 1;
    PhoneBook.SetCmd_AT_CPBR(PhoneIndex++, false);  //  AT cmd used to read first location of the PhoneBook
    TestSimPhoneBook = TestPhoneBook_Step2;         //  Next State of the State Machine
  }
}

void TestPhoneBook_Step2(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
      //  Not found a valid Phone Number in the first index of the PhoneBook  
      PrintPhoneBookData();
      TimeOutWaitPhonicCall = T_120SEC;
      TestSimPhoneBook = WaitPhonicCall_Step1;  //  Next State of the State Machine
    } else {
      //  Found a valid phone number into first index of PhoneBook
      PrintPhoneBookData();
      System_Flags.Bit.TestFirstSimPbIndex = 0;
      TestSimPhoneBook = TestPhoneBook_Idle;    //  IDLE State of the State Machine
      Io.LedOff(PIN_LED6);  
      Io.LedOff(PIN_LED7); 
    }
  }
}

void WaitPhonicCall_Step1(void) {
  if (TimeOutWaitPhonicCall == 0) {
    //  TimeOut expired (2 minutes)
    System_Flags.Bit.TestFirstSimPbIndex = 0;
    TestSimPhoneBook = TestPhoneBook_Idle;  //  IDLE State of the State Machine
    Io.LedOff(PIN_LED6);  
    Io.LedOff(PIN_LED7);      
  }
  if (Gsm.GsmFlag.Bit.IncomingCall == 1) {
    //  IncomingCall in progress
    if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
      PhonicCall.SetCmd_ATH(0);
      Gsm.GsmFlag.Bit.IncomingCall = 0;
      TestSimPhoneBook = WaitPhonicCall_Step2;  //  Next State of the State Machine
    }
  }
}

void WaitPhonicCall_Step2(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    // "NUMx+39nnnnnnnnnnn;text;pwd"    
    StrSerialInput = ReadStringCmd_FLASH((uint8_t *)CMD_NUM, strlen(CMD_NUM), TRUE, TRUE);
    StrSerialInput.concat("1");
    StrSerialInput.concat((char *)(PhoneBook.PhoneNumber));
    StrSerialInput.remove(StrSerialInput.indexOf(ASCII_QUOTATION_MARKS), 1);  
    StrSerialInput.remove(StrSerialInput.indexOf(ASCII_QUOTATION_MARKS), 1);  
    StrSerialInput.concat((char)(ASCII_SEMI_COLON));
    StrSerialInput.concat(ReadStringCmd_FLASH((uint8_t *)ADMIN, strlen(ADMIN), TRUE, TRUE));
    StrSerialInput.concat((char)(ASCII_SEMI_COLON));
    StrSerialInput.concat(StrSysPassword);
    StrSerialInput.trim();
    StrSerialInput.toUpperCase(); 
    ProcessReceivedStringCmd();
    System_Flags.Bit.TestFirstSimPbIndex = 0;
    TestSimPhoneBook = TestPhoneBook_Idle;  //  IDLE State of the State Machine
    Io.LedOff(PIN_LED6);  
    Io.LedOff(PIN_LED7); 
  }
}

void TestSimPhoneBookLed(void) {
  if (BlinkWaitPhonicCallLed == 0) {
    BlinkWaitPhonicCallLed = T_500MSEC;
  }
  if (BlinkWaitPhonicCallLed > T_250MSEC) {
    Io.LedOn(PIN_LED6);  
    Io.LedOff(PIN_LED7);      
  } else {
    Io.LedOff(PIN_LED6);  
    Io.LedOn(PIN_LED7);          
  }
}
//======================================================================

//======================================================================
//  State machine used to process a incoming SMS
void ReadNewSmsIdle(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (Gsm.GsmFlag.Bit.SmsReceived == 1) {
      Gsm.GsmFlag.Bit.SmsReceived = 0;
      if (Sms.SmsReceivedIndex > 0) {
        Io.LedOn(PIN_LED9);
        Sms.SetCmd_AT_CMGR(Sms.SmsReceivedIndex, STATE_CHANGE);
        IncomingSms_Management = FindPhoneNumberIntoSimMemory;   
        System_Flags.Bit.IncomingSmsInProgress = 1;
      } 
    }
  }
}

void FindPhoneNumberIntoSimMemory(void) {
  if (Sms.SmsFlag.Bit.SmsReadStat == SMS_REC_UNREAD) {
    //  Process SMS received
    if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
      System_Flags.Bit.PhoneBookIndexFound  = 0;
      System_Flags.Bit.PhoneBookIndexHigher = 0;
      PhoneBook.IndexLocation = 0;
      PhoneBook.SetCmd_AT_CPBF();
      IncomingSms_Management = DeleteSmsReceived;
    }
  }
}

void DeleteSmsReceived(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (PhoneBook.IndexLocation > 0) {
      //  Phone number found into PhoneBook 
      System_Flags.Bit.PhoneBookIndexFound  = 1;
      System_Flags.Bit.PhoneBookIndexHigher = 0;
      if (PhoneBook.IndexLocation > 8) {
        //  Found Phone Number into SIM PhoneBook - Index major of 8
        System_Flags.Bit.PhoneBookIndexHigher = 1;
      }
    } else {
      //  Phone number not found into PhoneBook 
      System_Flags.Bit.PhoneBookIndexFound = 0;
    }
    Sms.SetCmd_AT_CMGD(Sms.SmsReceivedIndex, DEF_FLAG_NORMAL);
    Sms.SmsReceivedIndex = 0;
    IncomingSms_Management = ProcessCmdReceived;
    Io.LedOff(PIN_LED9);
  }
}

void ProcessCmdReceived(void) {
  StrSerialInput = (char *)(Sms.SmsText);
  StrSerialInput.trim();
  StrSerialInput.toUpperCase();  
  ProcessReceivedStringCmd();
  System_Flags.Bit.IncomingSmsInProgress = 0;
  IncomingSms_Management = ReadNewSmsIdle;
}
//======================================================================


//======================================================================
void ProcessSystemATcmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0) && (System_Flags.Bit.SetupAtCmdInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;

      switch (StateSystemAtCmd)
      {
        case ATQ_CREG_CMD:
          Gsm.SetCmd_ATQ_CREG();
          StateSystemAtCmd++;
          break;
        case ATQ_CSQ_CMD:
          Gsm.SetCmd_ATQ_CSQ();
          StateSystemAtCmd++;
          break;
        case ATQ_CPAS_CMD:
          Gsm.SetCmd_ATQ_CPAS();
          StateSystemAtCmd++;
          break;
        case ATQ_COPS_CMD:
          Gsm.SetCmd_ATQ_COPS();
          StateSystemAtCmd++;
          break;
        case ATQ_CPMS_CMD:
          Sms.SetCmd_AT_CPMS(0, 0, 0, true);
          StateSystemAtCmd++;
          break;
        case ATQ_GMI_CMD:
          Gsm.SetCmd_ATQ_GMI(GMI_V25_FORMAT);
          StateSystemAtCmd++;
          break;
        case ATQ_GMM_CMD:
          Gsm.SetCmd_ATQ_GMM(GMM_V25_FORMAT);
          StateSystemAtCmd++;
          break;
        case ATQ_GMR_CMD:
          Gsm.SetCmd_ATQ_GMR(GMR_V25_FORMAT);
          StateSystemAtCmd++;
          break;
        case ATQ_GSN_CMD:
          Gsm.SetCmd_ATQ_GSN(GSN_V25_FORMAT);
          StateSystemAtCmd = 0;
          break;
        default:
          break;
      }
    }
  }    
}
//======================================================================

//======================================================================
//  Function used to process user AT command
void ProcessUserATcmd(void) {
  uint8_t Count;
  if (TimeOutSendAtCmd > 0) {
    return;
  }
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    switch (StateUserAtCommand)
    {
      case RD_N_PHB_MEM:
        if (PhoneIndex > 1) {
          //  Prints data read from phonebook memory
          PrintPhoneBookData();
        }
        if (PhoneIndex > MaxPhoneIndex) {
          System_Flags.Bit.SetupAtCmdInProgress = 0;
          System_Flags.Bit.ForceNoSmsAnswer     = 0;
          RestorePhoneNumberArray();
          StateUserAtCommand = NOTHING;
          break;
        }
        PhoneBook.SetCmd_AT_CPBR(PhoneIndex++, false);
        TimeOutSendAtCmd = T_50MSEC;
        break;
      case RD_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBR(PhoneIndex, false);
        StateUserAtCommand = NOTHING;
        if (System_Flags.Bit.SinglePbRead == 1) {
          System_Flags.Bit.SinglePbRead = 0;
          PhoneIndex++;
          MaxPhoneIndex = 0;
          StateUserAtCommand = RD_N_PHB_MEM;
          break;
        }
        System_Flags.Bit.SetupAtCmdInProgress = 0;
        break;
      case FN_PHB_MEM:
        if (System_Flags.Bit.StartFindEntry == 1) {
          System_Flags.Bit.StartFindEntry = 0;
          PhoneBook.SetCmd_AT_CPBF();
          break;  
        } else {
          if (PhoneBook.IndexLocation > 0) {
            //  Found Entry  
            VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_FOUND_PB_TEXT, strlen(CMD_FOUND_PB_TEXT), FALSE, FALSE));
            Serial.println(PhoneBook.IndexLocation);
            VeryLongSmsOut.concat(PhoneBook.IndexLocation);
            VeryLongSmsOut.concat("\n");
            PhoneBook.IndexLocation = 0;
          } else {
            //  Not Found Entry
            VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_NOT_FOUND_PB_TEXT, strlen(CMD_NOT_FOUND_PB_TEXT), FALSE, FALSE));
          }
        }
        System_Flags.Bit.SetupAtCmdInProgress = 0;
        StateUserAtCommand = NOTHING;
        break;
      case WR_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBW(PhoneIndex, CPBW_WRITE_ENTRY);
        System_Flags.Bit.SetupAtCmdInProgress = 0;
        StateUserAtCommand = NOTHING;
        break;
      case ER_N_PHB_MEM:
        if (PhoneIndex > MaxPhoneIndex) {
          PhoneIndex = 1;
          System_Flags.Bit.SetupAtCmdInProgress = 0;
          StateUserAtCommand = NOTHING;
          break;
        }
        ReadStringCmd_FLASH((uint8_t *)CMD_ERASE_PB_INDEX_2, strlen(CMD_ERASE_PB_INDEX_2), FALSE, FALSE);
        Serial.println(PhoneIndex); 
        PhoneBook.SetCmd_AT_CPBW(PhoneIndex++, CPBW_ERASE_ENTRY);
        TimeOutSendAtCmd = T_50MSEC;
        break;      
      case ER_PHB_MEM:
        PhoneBook.SetCmd_AT_CPBW(PhoneIndex, CPBW_ERASE_ENTRY);
        System_Flags.Bit.SetupAtCmdInProgress = 0;
        StateUserAtCommand = NOTHING;
        break;
      case ER_SMS_MEM:
        Sms.SetCmd_AT_CMGD(SmsIndex, DEF_FLAG_NORMAL);
        System_Flags.Bit.SetupAtCmdInProgress = 0;
        StateUserAtCommand = NOTHING;
        break;
      case ER_ALL_SMS_MEM:
        Sms.SetCmd_AT_CMGD(1, DEF_FLAG_ALL);
        System_Flags.Bit.SetupAtCmdInProgress = 0;
        StateUserAtCommand = NOTHING;
        break;  
      case NOTHING:
        break;
      default:
        break;
    }    
  }  
} 
//======================================================================

//======================================================================
void PrintPhoneBookData(void) {
  if (PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation == 1) {
    //  Phonebook memeory empty
    PhoneBook.PhoneBookFlag.Bit.EmptyMemoryLocation = 0;
    if (System_Flags.Bit.ForceNoSmsAnswer != 1) {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_NUMBER_1, strlen(CMD_READ_PB_NUMBER_1), FALSE, FALSE));
      Serial.print(PhoneIndex - 1);
      VeryLongSmsOut.concat(PhoneIndex - 1);
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_EMPTY, strlen(CMD_READ_PB_EMPTY), FALSE, FALSE));
    } else {
      ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_NUMBER_1, strlen(CMD_READ_PB_NUMBER_1), FALSE, FALSE);
      Serial.print(PhoneIndex - 1);
      ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_EMPTY, strlen(CMD_READ_PB_EMPTY), FALSE, FALSE);
    }
  } else {
    if (System_Flags.Bit.ForceNoSmsAnswer != 1) {
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_NUMBER_1, strlen(CMD_READ_PB_NUMBER_1), FALSE, FALSE));
      Serial.print(PhoneIndex - 1);
      VeryLongSmsOut.concat(PhoneIndex - 1);
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_NUMBER_2, strlen(CMD_READ_PB_NUMBER_2), FALSE, FALSE));
      Serial.println((char *)PhoneBook.PhoneNumber);
      VeryLongSmsOut.concat((char *)PhoneBook.PhoneNumber);
      VeryLongSmsOut.concat("\n");
      VeryLongSmsOut.concat(ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_TEXT, strlen(CMD_READ_PB_TEXT), FALSE, FALSE));
      Serial.println((char *)PhoneBook.PhoneText);
      VeryLongSmsOut.concat((char *)PhoneBook.PhoneText);
      VeryLongSmsOut.concat("\n");      
    } else {
      ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_NUMBER_1, strlen(CMD_READ_PB_NUMBER_1), FALSE, FALSE);
      Serial.print(PhoneIndex - 1);
      ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_NUMBER_2, strlen(CMD_READ_PB_NUMBER_2), FALSE, FALSE);
      Serial.println((char *)PhoneBook.PhoneNumber);
      ReadStringCmd_FLASH((uint8_t *)CMD_READ_PB_TEXT, strlen(CMD_READ_PB_TEXT), FALSE, FALSE);
      Serial.println((char *)PhoneBook.PhoneText);
    }
  }
}
//======================================================================
