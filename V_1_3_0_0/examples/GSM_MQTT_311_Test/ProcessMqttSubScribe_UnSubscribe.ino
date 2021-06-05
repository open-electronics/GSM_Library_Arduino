//======================================================================
void ProcessMqttSubscribe(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case SUBSCRIBE_PACKET_REQ_IDENTIFIER:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_IDENTIFIER, strlen(STR_CMD_SUBSCRIBE_IDENTIFIER), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_IDENTIFIER:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = SUBSCRIBE_PACKET_IDLE;
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
    case SUBSCRIBE_PACKET_REQ_HOW_MUCH_TOPIC:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER_QTY, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER_QTY), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_QUANTITY:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = SUBSCRIBE_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (atol(StrSerialInput.c_str()) == 0) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY, strlen(STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY), TRUE, FALSE);
            break;
          }          
          if (atol(StrSerialInput.c_str()) > SUBSCRIBE_MAX_TOPIC) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY, strlen(STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY), TRUE, FALSE);
            break;            
          }
          SubscribeTopicQty = (atol(StrSerialInput.c_str()) & 0xFFFF);
          LocalReadString_FLASH((uint8_t *)STR_SUBSCRIBE_TOPIC_FILTER_QTY_OK, strlen(STR_SUBSCRIBE_TOPIC_FILTER_QTY_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }   
      break;
    case SUBSCRIBE_PACKET_REQ_TOPIC_1:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_1:
      CheckTopicSubscribe(0);
      break;
    case SUBSCRIBE_PACKET_REQ_QOS_1:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_QOS, strlen(STR_CMD_SUBSCRIBE_QOS), TRUE, FALSE);
      CmdSequence++;   
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_QOS_1:
      RetValue = CheckQosSubscribe(0);
      if (RetValue == 1) {
        if (SubscribeTopicQty < 2) {
          CmdSequence = SUBSCRIBE_PACKET_GENERATE;
          break;
        }        
      } else if (RetValue == 2) {
          CmdSequence = SUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case SUBSCRIBE_PACKET_REQ_TOPIC_2:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_2:
      CheckTopicSubscribe(1);
      break;
    case SUBSCRIBE_PACKET_REQ_QOS_2:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_QOS, strlen(STR_CMD_SUBSCRIBE_QOS), TRUE, FALSE);
      CmdSequence++;  
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_QOS_2:
      RetValue = CheckQosSubscribe(1);
      if (RetValue == 1) {
        if (SubscribeTopicQty < 3) {
          CmdSequence = SUBSCRIBE_PACKET_GENERATE;
          break;
        }        
      } else if (RetValue == 2) {
          CmdSequence = SUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case SUBSCRIBE_PACKET_REQ_TOPIC_3:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_3:
      CheckTopicSubscribe(2);
      break;
    case SUBSCRIBE_PACKET_REQ_QOS_3:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_QOS, strlen(STR_CMD_SUBSCRIBE_QOS), TRUE, FALSE);
      CmdSequence++;  
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_QOS_3:
      RetValue = CheckQosSubscribe(2);
      if (RetValue == 1) {
        if (SubscribeTopicQty < 4) {
          CmdSequence = SUBSCRIBE_PACKET_GENERATE;
          break;
        }        
      } else if (RetValue == 2) {
          CmdSequence = SUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case SUBSCRIBE_PACKET_REQ_TOPIC_4:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_4:
      CheckTopicSubscribe(3);
      break;
    case SUBSCRIBE_PACKET_REQ_QOS_4:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_QOS, strlen(STR_CMD_SUBSCRIBE_QOS), TRUE, FALSE);
      CmdSequence++;  
      break;
    case SUBSCRIBE_PACKET_WAIT_INSERT_QOS_4:
      RetValue = CheckQosSubscribe(3);
      if (RetValue == 2) {
          CmdSequence = SUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case SUBSCRIBE_PACKET_GENERATE:
      Generate_Subscribe_Packet();

      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();   
      break;
    default:
      break;
  }
}
//======================================================================

//======================================================================
uint8_t CheckQosSubscribe(uint8_t Index) {
  uint8_t RetValue;
  
  if (TestSerialDataAvailable() == 1) {
    //  If serial data is available
    RetValue = CheckStringOkOrExit();
    if (RetValue == 1) {
      //  Typed "ok"
      return(1);
    } else if (RetValue == 2) {
      //  Typed "exit"
      return(2);
    }
    if (StrSerialInput.length() > 0) {
      if (atol(StrSerialInput.c_str()) > 2) {
        LocalReadString_FLASH((uint8_t *)STR_ERR_SUBSCRIBE_QOS, strlen(STR_ERR_SUBSCRIBE_QOS), TRUE, FALSE);
        return(0);
      }
      Mqtt.Subscribe[Index].Qos = (atoi(StrSerialInput.c_str()) & 0x0003);
      LocalReadString_FLASH((uint8_t *)STR_SUBSCRIBE_QOS_OK, strlen(STR_SUBSCRIBE_QOS_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(0);
    }
  }  
}
//======================================================================

//======================================================================
uint8_t CheckTopicSubscribe(uint8_t Index) {
  uint8_t RetValue;
  
  if (TestSerialDataAvailable() == 1) {
    //  If serial data is available
    RetValue = CheckStringOkOrExit();
    if (RetValue == 1) {
      //  Typed "ok"
      return(1);
    } else if (RetValue == 2) {
      //  Typed "exit"
      return(2);
    }
    if (StrSerialInput.length() > 0) {
      if (StrSerialInput.length() > SUBSCRIBE_TOPIC_FILTER_MAX_LENGTH) {
        LocalReadString_FLASH((uint8_t *)STR_ERR_TOPIC_FILTER_TOO_LONG, strlen(STR_ERR_TOPIC_FILTER_TOO_LONG), TRUE, FALSE);
        return;
      }
      memset(Mqtt.Subscribe[Index].TopicFilter, 0, (sizeof(Mqtt.Subscribe[Index].TopicFilter)/sizeof(Mqtt.Subscribe[Index].TopicFilter[0])));
      strcpy(Mqtt.Subscribe[Index].TopicFilter, StrSerialInput.c_str());
      LocalReadString_FLASH((uint8_t *)STR_SUBSCRIBE_TOPIC_FILTER_OK, strlen(STR_SUBSCRIBE_TOPIC_FILTER_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);                              
    }
  }
}
//======================================================================

//======================================================================
void Generate_Subscribe_Packet(void) {
  uint8_t Index = 0;
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_SUBSCRIBE << 4) + MQTT_FLAGS_SUBSCRIBE), TRUE); 
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  Mqtt.DummyCalcPayloadLen(MQTT_SUBSCRIBE);
  PrintHexFormat("%04X", (MQTT_SUBSCRIBE_FIXED_LEN + Mqtt.MessageLen), TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_SUBSCRIBE_PACKET_IDENTIFIER, strlen(STR_PRNT_SUBSCRIBE_PACKET_IDENTIFIER), FALSE, FALSE);
  PrintHexFormat("%04X", Mqtt.PacketIdentifier, TRUE);

  do {
    LocalReadString_FLASH((uint8_t *)STR_PRNT_SUBSCRIBE_TOPIC_FILTER, strlen(STR_PRNT_SUBSCRIBE_TOPIC_FILTER), FALSE, FALSE);
    PrintStringArray(&Mqtt.Subscribe[Index].TopicFilter[0], (sizeof(Mqtt.Subscribe[Index].TopicFilter)/sizeof(Mqtt.Subscribe[Index].TopicFilter[0])));
  
    LocalReadString_FLASH((uint8_t *)STR_PRNT_SUBSCRIBE_QOS, strlen(STR_PRNT_SUBSCRIBE_QOS), FALSE, FALSE);
    PrintHexFormat("%02X", Mqtt.Subscribe[Index].Qos, TRUE);    
  } while (++Index < SubscribeTopicQty);
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);
  
  LocalReadString_FLASH((uint8_t *)STR_SUBSCRIBE_PACKET_OK, strlen(STR_SUBSCRIBE_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_SUBSCRIBE(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();
}
//======================================================================

//======================================================================
void ProcessMqttUnSubscribe(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case UNSUBSCRIBE_PACKET_REQ_IDENTIFIER:
      LocalReadString_FLASH((uint8_t *)STR_CMD_UNSUBSCRIBE_IDENTIFIER, strlen(STR_CMD_UNSUBSCRIBE_IDENTIFIER), TRUE, FALSE);
      CmdSequence++;
      break;
    case UNSUBSCRIBE_PACKET_WAIT_INSERT_IDENTIFIER:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = UNSUBSCRIBE_PACKET_IDLE;
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
    case UNSUBSCRIBE_PACKET_REQ_HOW_MUCH_TOPIC:
      LocalReadString_FLASH((uint8_t *)STR_CMD_UNSUBSCRIBE_TOPIC_FILTER_QTY, strlen(STR_CMD_UNSUBSCRIBE_TOPIC_FILTER_QTY), TRUE, FALSE);
      CmdSequence++;
      break;
    case UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_QUANTITY:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = UNSUBSCRIBE_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (atol(StrSerialInput.c_str()) == 0) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY, strlen(STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY), TRUE, FALSE);
            break;
          }          
          if (atol(StrSerialInput.c_str()) > UNSUBSCRIBE_MAX_TOPIC) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY, strlen(STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY), TRUE, FALSE);
            break;            
          }
          UnSubscribeTopicQty = (atol(StrSerialInput.c_str()) & 0xFFFF);
          LocalReadString_FLASH((uint8_t *)STR_SUBSCRIBE_TOPIC_FILTER_QTY_OK, strlen(STR_SUBSCRIBE_TOPIC_FILTER_QTY_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }   
      break;
    case UNSUBSCRIBE_PACKET_REQ_TOPIC_1:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_1:
      RetValue = CheckTopicUnSubscribe(0);
      if (RetValue == 1) {
        if (UnSubscribeTopicQty < 2) {
          CmdSequence = UNSUBSCRIBE_PACKET_GENERATE;
          break;
        }        
      } else if (RetValue == 2) {
          CmdSequence = UNSUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case UNSUBSCRIBE_PACKET_REQ_TOPIC_2:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_2:
      RetValue = CheckTopicUnSubscribe(1);
      if (RetValue == 1) {
        if (UnSubscribeTopicQty < 3) {
          CmdSequence = UNSUBSCRIBE_PACKET_GENERATE;
          break;
        }        
      } else if (RetValue == 2) {
          CmdSequence = UNSUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case UNSUBSCRIBE_PACKET_REQ_TOPIC_3:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_3:
      RetValue = CheckTopicUnSubscribe(2);
      if (RetValue == 1) {
        if (UnSubscribeTopicQty < 4) {
          CmdSequence = UNSUBSCRIBE_PACKET_GENERATE;
          break;
        }        
      } else if (RetValue == 2) {
          CmdSequence = UNSUBSCRIBE_PACKET_IDLE;      
      }
      break;
    case UNSUBSCRIBE_PACKET_REQ_TOPIC_4:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_TOPIC_FILTER, strlen(STR_CMD_SUBSCRIBE_TOPIC_FILTER), TRUE, FALSE);
      CmdSequence++;
      break;
    case UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_4:
      CheckTopicUnSubscribe(3);
      break;
    case UNSUBSCRIBE_PACKET_GENERATE:
      Generate_UnSubscribe_Packet();

      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();   
      break;
    default:
      break;
  }
}
//======================================================================

//======================================================================
uint8_t CheckTopicUnSubscribe(uint8_t Index) {
  uint8_t RetValue;
  
  if (TestSerialDataAvailable() == 1) {
    //  If serial data is available
    RetValue = CheckStringOkOrExit();
    if (RetValue == 1) {
      //  Typed "ok"
      return(1);
    } else if (RetValue == 2) {
      //  Typed "exit"
      return(2);
    }
    if (StrSerialInput.length() > 0) {
      if (StrSerialInput.length() > UNSUBSCRIBE_TOPIC_FILTER_MAX_LENGTH) {
        LocalReadString_FLASH((uint8_t *)STR_ERR_TOPIC_FILTER_TOO_LONG, strlen(STR_ERR_TOPIC_FILTER_TOO_LONG), TRUE, FALSE);
        return;
      }
      memset(Mqtt.Unsubscribe[Index].TopicFilter, 0, (sizeof(Mqtt.Unsubscribe[Index].TopicFilter)/sizeof(Mqtt.Unsubscribe[Index].TopicFilter[0])));
      strcpy(Mqtt.Unsubscribe[Index].TopicFilter, StrSerialInput.c_str());
      LocalReadString_FLASH((uint8_t *)STR_UNSUBSCRIBE_TOPIC_FILTER_OK, strlen(STR_UNSUBSCRIBE_TOPIC_FILTER_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);                              
    }
  }
}
//======================================================================

//======================================================================
void Generate_UnSubscribe_Packet(void) {
  uint8_t Index = 0;
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_UNSUBSCRIBE << 4) + MQTT_FLAGS_UNSUBSCRIBE), TRUE); 
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  Mqtt.DummyCalcPayloadLen(MQTT_UNSUBSCRIBE);
  PrintHexFormat("%04X", (MQTT_UNSUBSCRIBE_FIXED_LEN + Mqtt.MessageLen), TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_SUBSCRIBE_PACKET_IDENTIFIER, strlen(STR_PRNT_SUBSCRIBE_PACKET_IDENTIFIER), FALSE, FALSE);
  PrintHexFormat("%04X", Mqtt.PacketIdentifier, TRUE);

  do {
    LocalReadString_FLASH((uint8_t *)STR_PRNT_SUBSCRIBE_TOPIC_FILTER, strlen(STR_PRNT_SUBSCRIBE_TOPIC_FILTER), FALSE, FALSE);
    PrintStringArray(&Mqtt.Unsubscribe[Index].TopicFilter[0], (sizeof(Mqtt.Unsubscribe[Index].TopicFilter)/sizeof(Mqtt.Unsubscribe[Index].TopicFilter[0])));
  } while (++Index < UnSubscribeTopicQty);
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);
  
  LocalReadString_FLASH((uint8_t *)STR_UNSUBSCRIBE_PACKET_OK, strlen(STR_UNSUBSCRIBE_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_UNSUBSCRIBE(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();
}
//======================================================================
