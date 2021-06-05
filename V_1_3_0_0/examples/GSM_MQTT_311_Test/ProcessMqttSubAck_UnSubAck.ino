//======================================================================
void ProcessMqttSubAck_UnSubAck(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case SUBACK_PACKET_REQ_IDENTIFIER:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUB_UNSUB_ACK_IDENTIFIER, strlen(STR_CMD_SUB_UNSUB_ACK_IDENTIFIER), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBACK_PACKET_WAIT_INSERT_IDENTIFIER:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = SUBACK_PACKET_IDLE;
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
    case SUBACK_PACKET_REQ_HOW_MUCH_RETURN_CODE:
      if (MqttCmd == MQTT_UNSUBACK) {
        CmdSequence = SUBACK_SUBACK_UNSUBACK_PACKET_GENERATE;
        break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBACK_QOS_RETURN_CODE_QTY, strlen(STR_CMD_SUBACK_QOS_RETURN_CODE_QTY), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBACK_PACKET_WAIT_INSERT_RETURN_CODE_QTY:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = SUBACK_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {
          if (atoi(StrSerialInput.c_str()) == 0) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_SUBACK_QOS_RETURN_CODE_QTY, strlen(STR_ERR_SUBACK_QOS_RETURN_CODE_QTY), TRUE, FALSE);
            break;
          }          
          if (atoi(StrSerialInput.c_str()) > SUBACK_MAX_QOS_RETURN_CODE) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_SUBACK_QOS_RETURN_CODE_QTY, strlen(STR_ERR_SUBACK_QOS_RETURN_CODE_QTY), TRUE, FALSE);
            break;            
          }
          QosReturnCodeQty = (atoi(StrSerialInput.c_str()) & 0x000F);
          LocalReadString_FLASH((uint8_t *)STR_SUBACK_QOS_RETURN_CODE_QTY_OK, strlen(STR_SUBACK_QOS_RETURN_CODE_QTY_OK), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
        }
      }   
      break;
    case SUBACK_PACKET_REQ_QOS_1:
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBACK_QOS, strlen(STR_CMD_SUBACK_QOS), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBACK_PACKET_WAIT_INSERT_QOS_1:
      CheckQosReturnCodeSubAck(0);
      break;
    case SUBACK_PACKET_REQ_QOS_2:
      if (QosReturnCodeQty < 2) {
        CmdSequence = SUBACK_SUBACK_UNSUBACK_PACKET_GENERATE;
        break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBACK_QOS, strlen(STR_CMD_SUBACK_QOS), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBACK_PACKET_WAIT_INSERT_QOS_2:
      CheckQosReturnCodeSubAck(1);
      break;
    case SUBACK_PACKET_REQ_QOS_3:
      if (QosReturnCodeQty < 3) {
        CmdSequence = SUBACK_SUBACK_UNSUBACK_PACKET_GENERATE;
        break;
      }
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBACK_QOS, strlen(STR_CMD_SUBACK_QOS), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBACK_PACKET_WAIT_INSERT_QOS_3:
      CheckQosReturnCodeSubAck(2);
      break;
    case SUBACK_PACKET_REQ_QOS_4:
      if (QosReturnCodeQty < 4) {
        CmdSequence = SUBACK_SUBACK_UNSUBACK_PACKET_GENERATE;
        break;
      }    
      LocalReadString_FLASH((uint8_t *)STR_CMD_SUBACK_QOS, strlen(STR_CMD_SUBACK_QOS), TRUE, FALSE);
      CmdSequence++;
      break;
    case SUBACK_PACKET_WAIT_INSERT_QOS_4:
      CheckQosReturnCodeSubAck(3);
      break;
    case SUBACK_SUBACK_UNSUBACK_PACKET_GENERATE:
      Generate_SubAck_UnSubAck_Packet();
      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();
      break;
    default:
      break;
  }
}
//======================================================================

//======================================================================
uint8_t CheckQosReturnCodeSubAck(uint8_t Index) {
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
      if (atoi(StrSerialInput.c_str()) > MQTT_SUBACK_SUCCESS_QOS_2) {
        if (atoi(StrSerialInput.c_str()) != MQTT_SUBACK_FAILURE) {
          LocalReadString_FLASH((uint8_t *)STR_ERR_SUBACK_QOS_RETURN_CODE, strlen(STR_ERR_SUBACK_QOS_RETURN_CODE), TRUE, FALSE);
          return(0); 
        }
      }
      Mqtt.Subscribe[Index].AckRetCode = atoi(StrSerialInput.c_str());
      LocalReadString_FLASH((uint8_t *)STR_SUBACK_QOS_RETURN_CODE_OK, strlen(STR_SUBACK_QOS_RETURN_CODE_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(0);
    }
  }  
}
//======================================================================

//======================================================================
void Generate_SubAck_UnSubAck_Packet(void) {
  uint8_t Index = 0;
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  switch (MqttCmd)
  {
    case MQTT_SUBACK:
      PrintHexFormat("%02X", ((MQTT_SUBACK << 4) + (MQTT_FLAGS_SUBACK)), TRUE);
      break;
    case MQTT_UNSUBACK:
      PrintHexFormat("%02X", ((MQTT_UNSUBACK << 4) + (MQTT_FLAGS_UNSUBACK)), TRUE);
      break; 
    default:
      break;    
  }
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  switch (MqttCmd)
  {
    case MQTT_SUBACK:
      Mqtt.DummyCalcPayloadLen(MQTT_SUBACK);
      PrintHexFormat("%04X", (MQTT_SUB_ACK_FIXED_LEN + Mqtt.MessageLen), TRUE);
      break;
    case MQTT_UNSUBACK:
      PrintHexFormat("%04X", MQTT_UNSUB_ACK_REMAINING_LEN, TRUE);
      break; 
    default:
      break;    
  }
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_PUBLISH_PACKET_IDENTIFIER, strlen(STR_PRNT_PUBLISH_PACKET_IDENTIFIER), FALSE, FALSE);
  PrintHexFormat("%04X", Mqtt.PacketIdentifier, TRUE);

  if (MqttCmd == MQTT_SUBACK) {
    do {
      LocalReadString_FLASH((uint8_t *)STR_PRNT_SUBACK_RETURN_CODE_QOS, strlen(STR_PRNT_SUBACK_RETURN_CODE_QOS), FALSE, FALSE);
      PrintHexFormat("%02X", Mqtt.Subscribe[Index].AckRetCode, TRUE);    
    } while (++Index < QosReturnCodeQty);    
  }
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);

  switch (MqttCmd)
  {
    case MQTT_SUBACK:
      LocalReadString_FLASH((uint8_t *)STR_SUBACK_PACKET_OK, strlen(STR_SUBACK_PACKET_OK), FALSE, FALSE);
      Mqtt.Packets_SUBACK(&Gsm.GSM_Data_Array[0]);
      break;
    case MQTT_UNSUBACK:
      LocalReadString_FLASH((uint8_t *)STR_UNSUBACK_PACKET_OK, strlen(STR_UNSUBACK_PACKET_OK), FALSE, FALSE);
      Mqtt.Packets_UNSUBACK(&Gsm.GSM_Data_Array[0]);
      break;
    default:
      break;
  }

  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();
}
//======================================================================
