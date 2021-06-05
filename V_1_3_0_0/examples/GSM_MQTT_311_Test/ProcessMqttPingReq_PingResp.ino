//======================================================================
void ProcessMqttPingReq(void) {
  Generate_PingReq_Packet();
  MqttProcessPacket = ProcessMqttIdle;
  PrintMainMenu(); 
}
//======================================================================

//======================================================================
void Generate_PingReq_Packet(void) {
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);
 
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_PINGREQ << 4) + (MQTT_FLAGS_PINGREQ)), TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  PrintHexFormat("%04X", 0, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PINGREQ_PACKET_OK, strlen(STR_PINGREQ_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_PINGREQ(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();  
}
//======================================================================


//======================================================================
void ProcessMqttPingResp(void) {
  Generate_PingResp_Packet();
  MqttProcessPacket = ProcessMqttIdle;
  PrintMainMenu(); 
}
//======================================================================

//======================================================================
void Generate_PingResp_Packet(void) {
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);
 
  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_PINGRESP << 4) + (MQTT_FLAGS_PINGRESP)), TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  PrintHexFormat("%04X", 0, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PINGRESP_PACKET_OK, strlen(STR_PINGRESP_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_PINGRESP(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();  
}
//======================================================================
