//======================================================================
void ProcessMqttPublish(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case PUBLISH_PACKET_REQ_SET_FLAG:
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_FLAG_PARAM,     strlen(STR_CMD_PUBLISH_FLAG_PARAM),     TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_RETAIN_FLAG,    strlen(STR_CMD_PUBLISH_RETAIN_FLAG),    TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_QOS_LEVEL_FLAG, strlen(STR_CMD_PUBLISH_QOS_LEVEL_FLAG), TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_DUP_FLAG,       strlen(STR_CMD_PUBLISH_DUP_FLAG),       TRUE, FALSE);
      CmdSequence++;
      break;
    case PUBLISH_PACKET_WAIT_INSERT_FLAG:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = PUBLISH_PACKET_IDLE;
          break;
        }
        if (ProcessPublishFlag() == 0) {
          //  Invalid command received
          LocalReadString_FLASH((uint8_t *)STR_INVALID_DATA_RECEIVED, strlen(STR_INVALID_DATA_RECEIVED), TRUE, FALSE);
          break;
        }
      }  
      break;
    case PUBLISH_PACKET_REQ_TOPIC:
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_TOPIC, strlen(STR_CMD_PUBLISH_TOPIC), TRUE, FALSE);
      CmdSequence++;  
      break;
    case PUBLISH_PACKET_WAIT_INSERT_TOPIC:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = PUBLISH_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > PUBLISH_TOPIC_NAME_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_TOPIC_NAME_TOO_LONG, strlen(STR_ERR_TOPIC_NAME_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.PublishTopicName, 0, (sizeof(Mqtt.PublishTopicName)/sizeof(Mqtt.PublishTopicName[0])));
          strcpy(Mqtt.PublishTopicName, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_PUBLISH_TOPIC_NAME_OK, strlen(STR_PUBLISH_TOPIC_NAME_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);                              
        }
      }
      break;
    case PUBLISH_PACKET_REQ_IDENTIFIER:
      if (Mqtt.PublishFlags.Flag.Bit.QoS == 0) { 
        CmdSequence = PUBLISH_PACKET_REQ_PAYLOAD;
        break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_IDENTIFIER, strlen(STR_CMD_PUBLISH_IDENTIFIER), TRUE, FALSE);
      CmdSequence++;
      break;
    case PUBLISH_PACKET_WAIT_INSERT_IDENTIFIER:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = PUBLISH_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (atol(StrSerialInput.c_str()) == 0) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_PACKET_IDENTIFIER, strlen(STR_ERR_PACKET_IDENTIFIER), TRUE, FALSE);
            break;
          }          
          if (atol(StrSerialInput.c_str()) > PACKET_IDENTIFIER_MAX_VALUE) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_PACKET_IDENTIFIER, strlen(STR_ERR_PACKET_IDENTIFIER), TRUE, FALSE);
            break;            
          }
          Mqtt.PacketIdentifier = (atol(StrSerialInput.c_str()) & 0xFFFF);
          LocalReadString_FLASH((uint8_t *)STR_PUBLISH_PACKET_IDENTIFIER_OK, strlen(STR_PUBLISH_PACKET_IDENTIFIER_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }
      break;
    case PUBLISH_PACKET_REQ_PAYLOAD:
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_PAYLOAD, strlen(STR_CMD_PUBLISH_PAYLOAD), TRUE, FALSE);
      CmdSequence++;
      break;
    case PUBLISH_PACKET_WAIT_INSERT_PAYLOAD:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = PUBLISH_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (StrSerialInput.length() > PUBLISH_PAYLOAD_MAX_LENGTH) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_PAYLOAD_TOO_LONG, strlen(STR_ERR_PAYLOAD_TOO_LONG), TRUE, FALSE);
            break;
          }
          memset(Mqtt.PublishPayload, 0, (sizeof(Mqtt.PublishPayload)/sizeof(Mqtt.PublishPayload[0])));
          strcpy(Mqtt.PublishPayload, StrSerialInput.c_str());
          LocalReadString_FLASH((uint8_t *)STR_PUBLISH_PAYLOAD_OK, strlen(STR_PUBLISH_PAYLOAD_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);                              
        }
      }
      break;
    case PUBLISH_PACKET_GENERATE:
      Generate_Publish_Packet();

      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();    
      break;
    default:
      break;
  }
}
//======================================================================

//====================================================================== 
uint8_t ProcessPublishFlag(void) {
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
      Result = CheckSubString(3, 4);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.PublishFlags.Flag.Bit.Dup = Result;

      if (StrSerialInput.substring(4,6).equals("00")) {
        Mqtt.PublishFlags.Flag.Bit.QoS = 0;  
      } else if (StrSerialInput.substring(3,5).equals("01")) {
        Mqtt.PublishFlags.Flag.Bit.QoS = 1;
      } else if (StrSerialInput.substring(3,5).equals("10")) {
        Mqtt.PublishFlags.Flag.Bit.QoS = 2;
      } else {
        return(0);    //  Invalid command
      }

      Result = CheckSubString(6, 7);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.PublishFlags.Flag.Bit.Retain = Result;
      
      LocalReadString_FLASH((uint8_t *)STR_PUBLISH_FLAGS_OK, strlen(STR_PUBLISH_FLAGS_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flags 
    } else {
      return(0);      //  Invalid command
    }
  } else if (Index == 3) {
    //  Sets bits 1 and 2
    if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B34, strlen(STR_B34), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);
      if (StrSerialInput.equals("00")) {
        Mqtt.PublishFlags.Flag.Bit.QoS = 0;
      } else if (StrSerialInput.equals("01")) {
        Mqtt.PublishFlags.Flag.Bit.QoS = 1;
      } else if (StrSerialInput.equals("10")) {
        Mqtt.PublishFlags.Flag.Bit.QoS = 2;
      } else {
        return(0);    //  Invalid command
      }
      LocalReadString_FLASH((uint8_t *)STR_PUBLISH_QOS_FLAG_OK, strlen(STR_PUBLISH_QOS_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flags bit 3 and 4
    } else {
      return(0);      //  Invalid command
    }
  } else if (Index == 2) {
    //  Sets single bit
    if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B0, strlen(STR_B0), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);
      
      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.PublishFlags.Flag.Bit.Retain = Result;
      LocalReadString_FLASH((uint8_t *)STR_PUBLISH_RETAIN_FLAG_OK, strlen(STR_PUBLISH_RETAIN_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 1
      
    } else if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B3, strlen(STR_B3), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);

      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.PublishFlags.Flag.Bit.Dup = Result;
      LocalReadString_FLASH((uint8_t *)STR_PUBLISH_DUP_FLAG_OK, strlen(STR_PUBLISH_DUP_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 2     
    } else {
      return(0);      //  Invalid command
    }
  } else {
      return(0);      //  Invalid command
  }
}
//======================================================================

//======================================================================
void Generate_Publish_Packet(void) {  
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_PUBLISH << 4) + (Mqtt.PublishFlags.Flag.FlagByte)), TRUE); 
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  Mqtt.DummyCalcPayloadLen(MQTT_PUBLISH);
  PrintHexFormat("%04X", Mqtt.MessageLen, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_PUBLISH_TOPIC_NAME, strlen(STR_PRNT_PUBLISH_TOPIC_NAME), FALSE, FALSE);
  PrintStringArray(&Mqtt.PublishTopicName[0], (sizeof(Mqtt.PublishTopicName)/sizeof(Mqtt.PublishTopicName[0])));

  LocalReadString_FLASH((uint8_t *)STR_PRNT_PUBLISH_PACKET_IDENTIFIER, strlen(STR_PRNT_PUBLISH_PACKET_IDENTIFIER), FALSE, FALSE);
  if (Mqtt.PublishFlags.Flag.Bit.QoS > 0) {
    PrintHexFormat("%04X", Mqtt.PacketIdentifier, TRUE);
  } else {
    Serial.println(char(ASCII_HASH));
  }

  LocalReadString_FLASH((uint8_t *)STR_PRNT_PUBLISH_PAYLOAD, strlen(STR_PRNT_PUBLISH_PAYLOAD), FALSE, FALSE);
  PrintStringArray(&Mqtt.PublishPayload[0], (sizeof(Mqtt.PublishPayload)/sizeof(Mqtt.PublishPayload[0])));

  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PUBLISH_PACKET_OK, strlen(STR_PUBLISH_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_PUBLISH(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println(); 
}
//======================================================================
