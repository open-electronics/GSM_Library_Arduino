//======================================================================
void PrintMainMenu(void) {
  Mqtt.InitData();
  LocalReadString_FLASH((uint8_t *)STR_SEP_3,                  strlen(STR_SEP_3),                  FALSE, FALSE);
  LocalReadString_FLASH((uint8_t *)STR_SELECT_MQTT_COMMAND,    strlen(STR_SELECT_MQTT_COMMAND),    TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_CONNECT_PACKET,     strlen(STR_CMD_CONNECT_PACKET),     TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_DISCONNECT_PACKET,  strlen(STR_CMD_DISCONNECT_PACKET),  TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_PACKET,    strlen(STR_CMD_CONN_ACK_PACKET),    TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PUBLISH_PACKET,     strlen(STR_CMD_PUBLISH_PACKET),     TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PUB_ACK_PACKET,     strlen(STR_CMD_PUB_ACK_PACKET),     TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PUB_REC_PACKET,     strlen(STR_CMD_PUB_REC_PACKET),     TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PUB_REL_PACKET,     strlen(STR_CMD_PUB_REL_PACKET),     TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PUB_COMP_PACKET,    strlen(STR_CMD_PUB_COMP_PACKET),    TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_SUBSCRIBE_PACKET,   strlen(STR_CMD_SUBSCRIBE_PACKET),   TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_SUB_ACK_PACKET,     strlen(STR_CMD_SUB_ACK_PACKET),     TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_UNSUBSCRIBE_PACKET, strlen(STR_CMD_UNSUBSCRIBE_PACKET), TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_UNSUB_ACK_PACKET,   strlen(STR_CMD_UNSUB_ACK_PACKET),   TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PING_REQ_PACKET,    strlen(STR_CMD_PING_REQ_PACKET),    TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_CMD_PING_RESP_PACKET,   strlen(STR_CMD_PING_RESP_PACKET),   TRUE,  FALSE);
  LocalReadString_FLASH((uint8_t *)STR_SEP_4,                  strlen(STR_SEP_4),                  FALSE, FALSE);
  Serial.println();
}
//======================================================================

//======================================================================
void ProcessMqttIdle(void) {
  if (TestSerialDataAvailable() == 1) {
    if (StrSerialInput.equals("a")) {                 //  Selected Connect Packet
      CmdSequence = CONNECT_PACKET_REQ_CLIENT_ID;
      MqttProcessPacket = ProcessMqttConnect;
    } else if (StrSerialInput.equals("b")) {          //  Selected Disconnect Packet
      MqttProcessPacket = ProcessMqttCmdConnect;
    } else if (StrSerialInput.equals("c")) {          //  Selected ConnAck Packet
      CmdSequence = CONN_ACK_PACKET_REQ_SET_FLAG;
      MqttProcessPacket = ProcessMqttConnAck;
    } else if (StrSerialInput.equals("d")) {          //  Selected Publish Packet
      CmdSequence = PUBLISH_PACKET_REQ_SET_FLAG;
      MqttProcessPacket = ProcessMqttPublish;      
    } else if (StrSerialInput.equals("e")) {          //  Selected PubAck Packet
      CmdSequence = PUBACK_PACKET_REQ_IDENTIFIER;
      MqttCmd = MQTT_PUBACK;
      MqttProcessPacket = ProcessMqttPubAck_Rec_Rel_Comp;
    } else if (StrSerialInput.equals("f")) {          //  Selected PubRec Packet
      CmdSequence = PUBACK_PACKET_REQ_IDENTIFIER;
      MqttCmd = MQTT_PUBREC;
      MqttProcessPacket = ProcessMqttPubAck_Rec_Rel_Comp;
    } else if (StrSerialInput.equals("g")) {          //  Selected PubRel Packet
      CmdSequence = PUBACK_PACKET_REQ_IDENTIFIER;
      MqttCmd = MQTT_PUBREL;
      MqttProcessPacket = ProcessMqttPubAck_Rec_Rel_Comp;
    } else if (StrSerialInput.equals("h")) {          //  Selected PubComp Packet
      CmdSequence = PUBACK_PACKET_REQ_IDENTIFIER;
      MqttCmd = MQTT_PUBCOMP;
      MqttProcessPacket = ProcessMqttPubAck_Rec_Rel_Comp;
    } else if (StrSerialInput.equals("i")) {          //  Selected Subscribe Packet
      CmdSequence = SUBSCRIBE_PACKET_REQ_IDENTIFIER;
      MqttProcessPacket = ProcessMqttSubscribe;
    } else if (StrSerialInput.equals("j")) {          //  Selected SubAck Packet
      CmdSequence = SUBACK_PACKET_REQ_IDENTIFIER;
      MqttCmd = MQTT_SUBACK;
      MqttProcessPacket = ProcessMqttSubAck_UnSubAck;
    } else if (StrSerialInput.equals("k")) {          //  Selected Unsubscribe Packet
      CmdSequence = UNSUBSCRIBE_PACKET_REQ_IDENTIFIER;
      MqttProcessPacket = ProcessMqttUnSubscribe;
    } else if (StrSerialInput.equals("l")) {          //  Selected UnsubAck Packet
      CmdSequence = SUBACK_PACKET_REQ_IDENTIFIER;
      MqttCmd = MQTT_UNSUBACK;
      MqttProcessPacket = ProcessMqttSubAck_UnSubAck;      
    } else if (StrSerialInput.equals("m")) {          //  Selected PingReq Packet
      MqttProcessPacket = ProcessMqttPingReq;
    } else if (StrSerialInput.equals("n")) {          //  Selected PingResp Packet
      MqttProcessPacket = ProcessMqttPingResp;
    } else {
      LocalReadString_FLASH((uint8_t *)STR_CMD_INVALID_MENU_CHAR, strlen(STR_CMD_INVALID_MENU_CHAR), TRUE, FALSE);
    }
  }
}
//======================================================================

//======================================================================
//  This function checks if the strings "ok" or "Exit" was typed from user
uint8_t CheckStringOkOrExit(void) {
  if (StrSerialInput.equals(LocalReadString_FLASH((uint8_t *)STR_OK, strlen(STR_OK), FALSE, TRUE))) {
    //  Typed "OK". Changes the state machine and starts to create "CONNECT PACKET"
    CmdSequence++;
    return(1);
  } 
  if (StrSerialInput.equals(LocalReadString_FLASH((uint8_t *)STR_EXIT, strlen(STR_EXIT), FALSE, TRUE))) {
    //  Typed "EXIT". Changes the state machine and aborts process for creation of the "CONNECT PACKET"
    MqttProcessPacket = ProcessMqttIdle;
    PrintMainMenu();
    return(2);
  }
  return(0);
}
//======================================================================

//======================================================================
uint8_t CheckSubString(uint8_t StartIndex, uint8_t StopIndex) {
  if (StrSerialInput.substring(StartIndex, StopIndex).equals("1")) {
    return(1);
  } else if (StrSerialInput.substring(StartIndex, StopIndex).equals("0")) {
    return(0);
  } else {
    return(2);
  }
}
//======================================================================
