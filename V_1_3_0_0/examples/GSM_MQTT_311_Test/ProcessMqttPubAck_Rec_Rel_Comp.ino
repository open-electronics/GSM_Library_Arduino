//======================================================================
void ProcessMqttPubAck_Rec_Rel_Comp(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case PUBACK_PACKET_REQ_IDENTIFIER:
      LocalReadString_FLASH((uint8_t *)STR_CMD_PUB_ACK_REC_REL_COMP_IDENTIFIER, strlen(STR_CMD_PUB_ACK_REC_REL_COMP_IDENTIFIER), TRUE, FALSE);
      CmdSequence++;
      break;
    case PUBACK_PACKET_WAIT_INSERT_IDENTIFIER:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = PUBACK_PACKET_IDLE;
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
    case PUBACK_REC_REL_COMP_PACKET_GENERATE:
      Generate_PubAck_Rec_Rel_Comp_Packet();
      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();
      break;
    default:
      break;
  }
}
//======================================================================

//======================================================================
void Generate_PubAck_Rec_Rel_Comp_Packet(void) {
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  switch (MqttCmd)
  {
    case MQTT_PUBACK:
      PrintHexFormat("%02X", ((MQTT_PUBACK << 4) + (MQTT_FLAGS_PUBACK)), TRUE);
      break;
    case MQTT_PUBREC:
      PrintHexFormat("%02X", ((MQTT_PUBREC << 4) + (MQTT_FLAGS_PUBREC)), TRUE);
      break;
    case MQTT_PUBREL:
      PrintHexFormat("%02X", ((MQTT_PUBREL << 4) + (MQTT_FLAGS_PUBREL)), TRUE);
      break;
    case MQTT_PUBCOMP:
      PrintHexFormat("%02X", ((MQTT_PUBCOMP << 4) + (MQTT_FLAGS_PUBCOMP)), TRUE);
      break;  
    default:
      break;    
  }
  
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  PrintHexFormat("%02X", MQTT_PUB_ACK_REMAINING_LEN, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_PUBLISH_PACKET_IDENTIFIER, strlen(STR_PRNT_PUBLISH_PACKET_IDENTIFIER), FALSE, FALSE);
  PrintHexFormat("%04X", Mqtt.PacketIdentifier, TRUE);
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);
  
  switch (MqttCmd)
  {
    case MQTT_PUBACK:
      LocalReadString_FLASH((uint8_t *)STR_PUBACK_PACKET_OK, strlen(STR_PUBACK_PACKET_OK), FALSE, FALSE);
      Mqtt.Packets_PUBACK(&Gsm.GSM_Data_Array[0]);
      break;
    case MQTT_PUBREC:
      LocalReadString_FLASH((uint8_t *)STR_PUBREC_PACKET_OK, strlen(STR_PUBREC_PACKET_OK), FALSE, FALSE);
      Mqtt.Packets_PUBREC(&Gsm.GSM_Data_Array[0]);
      break;
    case MQTT_PUBREL:
      LocalReadString_FLASH((uint8_t *)STR_PUBREL_PACKET_OK, strlen(STR_PUBREL_PACKET_OK), FALSE, FALSE);
      Mqtt.Packets_PUBREL(&Gsm.GSM_Data_Array[0]);
      break;
    case MQTT_PUBCOMP:
      LocalReadString_FLASH((uint8_t *)STR_PUBCOMP_PACKET_OK, strlen(STR_PUBCOMP_PACKET_OK), FALSE, FALSE);
      Mqtt.Packets_PUBCOMP(&Gsm.GSM_Data_Array[0]);
      break;  
    default:
      break;
  }
  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();
}
//======================================================================
