//======================================================================
void ProcessMqttConnect(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case CONNECT_PACKET_REQ_CLIENT_ID:
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_CLIENT_ID, strlen(STR_CMD_CONNECT_CLIENT_ID), TRUE, FALSE);
      CmdSequence++;      
      break;
    case CONNECT_PACKET_WAIT_INSERT_CLIENT_ID:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > ID_CLIENT_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_CLIED_ID_TOO_LONG, strlen(STR_ERR_CLIED_ID_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.ClientIdentifier, 0, (sizeof(Mqtt.ClientIdentifier)/sizeof(Mqtt.ClientIdentifier[0])));
          strcpy(Mqtt.ClientIdentifier, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_CLIED_ID_OK, strlen(STR_CLIED_ID_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }
      break;
    case CONNECT_PACKET_REQ_KEEP_ALIVE:
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_KEEP_ALIVE, strlen(STR_CMD_CONNECT_KEEP_ALIVE), TRUE, FALSE);
      CmdSequence++;  
      break;
    case CONNECT_PACKET_WAIT_INSERT_KEEP_ALIVE:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (atol(StrSerialInput.c_str()) == 0) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_KEEP_ALIVE, strlen(STR_ERR_KEEP_ALIVE), TRUE, FALSE);
            break;
          }          
          if (atol(StrSerialInput.c_str()) > KEEP_ALIVE_MAX_VALUE) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_KEEP_ALIVE, strlen(STR_ERR_KEEP_ALIVE), TRUE, FALSE);
            break;            
          }
          Mqtt.KeepAlive = (atol(StrSerialInput.c_str()) & 0xFFFF);
          LocalReadString_FLASH((uint8_t *)STR_KEEP_ALIVE_OK, strlen(STR_KEEP_ALIVE_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }
      break;
    case CONNECT_PACKET_REQ_SET_FLAG:
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_FLAG_PARAM,         strlen(STR_CMD_CONNECT_FLAG_PARAM),         TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_RESERVED_FLAG,      strlen(STR_CMD_CONNECT_RESERVED_FLAG),      TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_CLEAN_SESSION_FLAG, strlen(STR_CMD_CONNECT_CLEAN_SESSION_FLAG), TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_WILL_FLAG,          strlen(STR_CMD_CONNECT_WILL_FLAG),          TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_WILL_QOS,           strlen(STR_CMD_CONNECT_WILL_QOS),           TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_WILL_RETAIN,        strlen(STR_CMD_CONNECT_WILL_RETAIN),        TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_PASSWORD_FLAG,      strlen(STR_CMD_CONNECT_PASSWORD_FLAG),      TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_USER_NAME_FLAG,     strlen(STR_CMD_CONNECT_USER_NAME_FLAG),     TRUE, FALSE);
      CmdSequence++;
      break;
    case CONNECT_PACKET_WAIT_INSERT_FLAG:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (ProcessConnectFlag() == 0) {
          //  Invalid command received
          LocalReadString_FLASH((uint8_t *)STR_INVALID_DATA_RECEIVED, strlen(STR_INVALID_DATA_RECEIVED), TRUE, FALSE);
          break;
        }
      }    
      break;
    case CONNECT_PACKET_REQ_WILL_TOPIC:
      if (Mqtt.ConnectFlags.Flag.Bit.WillFlag == 0) {
        CmdSequence = CONNECT_PACKET_REQ_USER_NAME;
        break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_WILL_TOPIC, strlen(STR_CMD_CONNECT_WILL_TOPIC), TRUE, FALSE);
      CmdSequence++;
      break;
    case CONNECT_PACKET_WAIT_INSERT_WILL_TOPIC:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > WILL_TOPIC_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_WILL_TOPIC_TOO_LONG, strlen(STR_ERR_WILL_TOPIC_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.WillTopic, 0, (sizeof(Mqtt.WillTopic)/sizeof(Mqtt.WillTopic[0])));
          strcpy(Mqtt.WillTopic, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_WILL_TOPIC_OK, strlen(STR_WILL_TOPIC_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }
      break;
    case CONNECT_PACKET_REQ_WILL_MESSAGE:
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_WILL_MESSAGE, strlen(STR_CMD_CONNECT_WILL_MESSAGE), TRUE, FALSE);
      CmdSequence++;
      break;
    case CONNECT_PACKET_WAIT_INSERT_WILL_MESSAGE:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > WILL_MESSAGE_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_WILL_MESSAGE_TOO_LONG, strlen(STR_ERR_WILL_MESSAGE_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.WillMessage, 0, (sizeof(Mqtt.WillMessage)/sizeof(Mqtt.WillMessage[0])));
          strcpy(Mqtt.WillMessage, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_WILL_MESSAGE_OK, strlen(STR_WILL_MESSAGE_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }
      break;
    case CONNECT_PACKET_REQ_USER_NAME:
      if (Mqtt.ConnectFlags.Flag.Bit.UserName == 0) {
        CmdSequence = CONNECT_PACKET_REQ_PASSWORD;
        break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_USER_NAME, strlen(STR_CMD_CONNECT_USER_NAME), TRUE, FALSE);
      CmdSequence++;
      break;
    case CONNECT_PACKET_WAIT_INSERT_USER_NAME:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > USER_NAME_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_USER_NAME_TOO_LONG, strlen(STR_ERR_USER_NAME_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.UserName, 0, (sizeof(Mqtt.UserName)/sizeof(Mqtt.UserName[0])));
          strcpy(Mqtt.UserName, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_USER_NAME_OK, strlen(STR_USER_NAME_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }
      break;
    case CONNECT_PACKET_REQ_PASSWORD:
      if (Mqtt.ConnectFlags.Flag.Bit.Password == 0) {
          CmdSequence = CONNECT_PACKET_GENERATE;
          break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_PASSWORD, strlen(STR_CMD_CONNECT_PASSWORD), TRUE, FALSE);
      CmdSequence++;
      break;
    case CONNECT_PACKET_WAIT_INSERT_PASSWORD:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONNECT_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > PASSWORD_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_PASSWORD_TOO_LONG, strlen(STR_ERR_PASSWORD_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.Password, 0, (sizeof(Mqtt.Password)/sizeof(Mqtt.Password[0])));
          strcpy(Mqtt.Password, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_PASSWORD_OK, strlen(STR_PASSWORD_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }    
      break;
    case CONNECT_PACKET_GENERATE:
      Generate_Connect_Packet();

      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();    
      break;
    default:
      break;
  }
}
//======================================================================

//====================================================================== 
uint8_t ProcessConnectFlag(void) {
  int     Index;
  uint8_t Result;
  
  Index = StrSerialInput.indexOf(ASCII_EQUAL);

  if (Index == -1) {
    return(0);    //  Invalid command
  } else if (Index == 1) {
    //  Sets all bits
    if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B, strlen(STR_B), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);
      if ((StrSerialInput.length() > 8) || (StrSerialInput.length() < 8)) {
         return(0);    //  Invalid command
      }
      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.UserName = Result;

      Result = CheckSubString(1, 2);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.Password = Result;

      Result = CheckSubString(2, 3);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.WillRetain = Result;
      
      if (StrSerialInput.substring(3,5).equals("00")) {
        Mqtt.ConnectFlags.Flag.Bit.WillRetain = 0;  
      } else if (StrSerialInput.substring(3,5).equals("01")) {
        Mqtt.ConnectFlags.Flag.Bit.WillRetain = 1;
      } else if (StrSerialInput.substring(3,5).equals("10")) {
        Mqtt.ConnectFlags.Flag.Bit.WillRetain = 2;
      } else {
        return(0);    //  Invalid command
      }

      Result = CheckSubString(5, 6);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.WillFlag = Result;
      
      Result = CheckSubString(6, 7);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.CleanSession = Result;
      LocalReadString_FLASH((uint8_t *)STR_CONNECT_FLAGS_OK, strlen(STR_CONNECT_FLAGS_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flags 
    } else {
      return(0);      //  Invalid command
    }
  } else if (Index == 3) {
    //  Sets bits 3 and 4
    if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B34, strlen(STR_B34), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);
      if (StrSerialInput.equals("00")) {
        Mqtt.ConnectFlags.Flag.Bit.WillQoS = 0;
      } else if (StrSerialInput.equals("01")) {
        Mqtt.ConnectFlags.Flag.Bit.WillQoS = 1;
      } else if (StrSerialInput.equals("10")) {
        Mqtt.ConnectFlags.Flag.Bit.WillQoS = 2;
      } else {
        return(0);    //  Invalid command
      }
      LocalReadString_FLASH((uint8_t *)STR_WILL_QOS_FLAG_OK, strlen(STR_WILL_QOS_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flags bit 3 and 4
    } else {
      return(0);      //  Invalid command
    }
  } else if (Index == 2) {
    //  Sets single bit
    if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B1, strlen(STR_B1), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);
      
      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.CleanSession = Result;
      LocalReadString_FLASH((uint8_t *)STR_CLEAN_SESSION_FLAG_OK, strlen(STR_CLEAN_SESSION_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 1
      
    } else if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B2, strlen(STR_B2), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);

      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.WillFlag = Result;
      LocalReadString_FLASH((uint8_t *)STR_WILL_FLAG_OK, strlen(STR_WILL_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 2
      
    } else if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B5, strlen(STR_B5), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);

      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.WillRetain = Result;
      LocalReadString_FLASH((uint8_t *)STR_WILL_RETAIN_FLAG_OK, strlen(STR_WILL_RETAIN_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 5
      
    } else if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B6, strlen(STR_B6), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);

      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.Password = Result;
      LocalReadString_FLASH((uint8_t *)STR_PASSWORD_FLAG_OK, strlen(STR_PASSWORD_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 6
      
    } else if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B7, strlen(STR_B7), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);

      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnectFlags.Flag.Bit.UserName = Result;
      LocalReadString_FLASH((uint8_t *)STR_USER_NAME_FLAG_OK, strlen(STR_USER_NAME_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 7
      
    } else {
      return(0);      //  Invalid command
    }
  } else {
    return(0);        //  Invalid command
  }
}
//======================================================================

//======================================================================
void Generate_Connect_Packet(void) {  
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);
 
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_CONNECT << 4) + (MQTT_FLAGS_CONNACK)), TRUE); 
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  Mqtt.DummyCalcPayloadLen(MQTT_CONNECT);
  PrintHexFormat("%04X", (MQTT_CONNECT_FIXED_LEN + Mqtt.MessageLen), TRUE);
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_VARIABLE_HEADER, strlen(STR_PRNT_CONNECT_VARIABLE_HEADER), FALSE, FALSE);
  PrintHexFormat("%04X", MQTT_CONNECT_PROTOCOL_NAME_LEN, FALSE);
  Serial.print("; ");
  PrintHexFormat("%02X", ASCII_M, FALSE);
  Serial.print("; ");
  PrintHexFormat("%02X", ASCII_Q, FALSE);
  Serial.print("; ");
  PrintHexFormat("%02X", ASCII_T, FALSE);
  Serial.print("; ");
  PrintHexFormat("%02X", ASCII_T, TRUE);
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PROTOCOL_LEVEL, strlen(STR_PRNT_CONNECT_PROTOCOL_LEVEL), FALSE, FALSE);
  PrintHexFormat("%02X", MQTT_CONNECT_PROTOCOL_LEVEL, TRUE);
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PROTOCOL_FLAGS, strlen(STR_PRNT_CONNECT_PROTOCOL_FLAGS), FALSE, FALSE);
  PrintHexFormat("%02X", Mqtt.ConnectFlags.Flag.FlagByte, TRUE);
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_KEEP_ALIVE, strlen(STR_PRNT_CONNECT_KEEP_ALIVE), FALSE, FALSE);
  PrintHexFormat("%04X", Mqtt.KeepAlive, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_CLIENT_ID, strlen(STR_PRNT_CONNECT_CLIENT_ID), FALSE, FALSE);
  PrintStringArray(&Mqtt.ClientIdentifier[0], (sizeof(Mqtt.ClientIdentifier)/sizeof(Mqtt.ClientIdentifier[0])));

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_WILL_TOPIC, strlen(STR_PRNT_CONNECT_WILL_TOPIC), FALSE, FALSE);
  if (Mqtt.ConnectFlags.Flag.Bit.WillFlag == 1) {
    PrintStringArray(&Mqtt.WillTopic[0], (sizeof(Mqtt.WillTopic)/sizeof(Mqtt.WillTopic[0])));  
  } else {
    Serial.println(char(ASCII_HASH));
  }
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_WILL_MESSAGE, strlen(STR_PRNT_CONNECT_WILL_MESSAGE), FALSE, FALSE);
  if (Mqtt.ConnectFlags.Flag.Bit.WillFlag == 1) {
    PrintStringArray(&Mqtt.WillMessage[0], (sizeof(Mqtt.WillMessage)/sizeof(Mqtt.WillMessage[0])));  
  } else {
    Serial.println(char(ASCII_HASH));
  }   
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_USER_NAME, strlen(STR_PRNT_CONNECT_USER_NAME), FALSE, FALSE);
  if (Mqtt.ConnectFlags.Flag.Bit.UserName == 1) {
    PrintStringArray(&Mqtt.UserName[0], (sizeof(Mqtt.UserName)/sizeof(Mqtt.UserName[0])));  
  } else {
    Serial.println(char(ASCII_HASH));
  }
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PASSWORD, strlen(STR_PRNT_CONNECT_PASSWORD), FALSE, FALSE);
  if (Mqtt.ConnectFlags.Flag.Bit.Password == 1) {
    PrintStringArray(&Mqtt.Password[0], (sizeof(Mqtt.Password)/sizeof(Mqtt.Password[0])));  
  } else {
    Serial.println(char(ASCII_HASH));
  }
  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_CONNECT_PACKET_OK, strlen(STR_CONNECT_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_CONNECT(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();
}

void PrintStringArray(uint8_t * ArrayPointer, uint8_t Length) {
  uint8_t Index = 0;
  
  do {
    if (*(uint8_t *)ArrayPointer == 0) {
      Serial.println();
      break;
    }
    Serial.print(char(*(uint8_t *)ArrayPointer++));
  } while (++Index < Length);
  if (Index == Length) {
    Serial.println();
  }
}

void PrintHexArray(uint8_t * ArrayPointer, uint8_t Length) {
  uint8_t Index = 0;
  
  do {
    PrintHexFormat("%02X", *(uint8_t *)ArrayPointer++, FALSE); 
  } while (++Index < Length);  
}

void PrintHexFormat(char Format[], uint32_t DataToPrint, boolean PrintCR) {
  char     Data[8];
 
  sprintf(Data, Format, DataToPrint);
  if (PrintCR == TRUE) {
    Serial.println(Data);
  } else {
    Serial.print(Data);
  }
}
//======================================================================
