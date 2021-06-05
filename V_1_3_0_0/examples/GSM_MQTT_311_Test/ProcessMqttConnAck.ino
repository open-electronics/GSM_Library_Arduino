//======================================================================
void ProcessMqttConnAck(void) {
  uint8_t RetValue;
  
  switch (CmdSequence) {
    case CONN_ACK_PACKET_REQ_SET_FLAG:
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_FLAG_PARAM,    strlen(STR_CMD_CONN_ACK_FLAG_PARAM),    TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_SP_FLAG,       strlen(STR_CMD_CONN_ACK_SP_FLAG),       TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_RESERVED_FLAG, strlen(STR_CMD_CONN_ACK_RESERVED_FLAG), TRUE, FALSE);
      CmdSequence++;
      break;
    case CONN_ACK_PACKET_WAIT_INSERT_FLAG:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONN_ACK_PACKET_IDLE;
          break;
        }
        if (ProcessConnAckFlag() == 0) {
          //  Invalid command received
          LocalReadString_FLASH((uint8_t *)STR_INVALID_DATA_RECEIVED, strlen(STR_INVALID_DATA_RECEIVED), TRUE, FALSE);
          break;
        }
      }
      break;
    case CONN_ACK_PACKET_REQ_RETURN_CODE:
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_RETURN_CODE,          strlen(STR_CMD_CONN_ACK_RETURN_CODE),          TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_CONNECTION_ACCEPTED,  strlen(STR_CMD_CONN_ACK_CONNECTION_ACCEPTED),  TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_CONNECTION_REFUSED_1, strlen(STR_CMD_CONN_ACK_CONNECTION_REFUSED_1), TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_CONNECTION_REFUSED_2, strlen(STR_CMD_CONN_ACK_CONNECTION_REFUSED_2), TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_CONNECTION_REFUSED_3, strlen(STR_CMD_CONN_ACK_CONNECTION_REFUSED_3), TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_CONNECTION_REFUSED_4, strlen(STR_CMD_CONN_ACK_CONNECTION_REFUSED_4), TRUE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_CMD_CONN_ACK_CONNECTION_REFUSED_5, strlen(STR_CMD_CONN_ACK_CONNECTION_REFUSED_5), TRUE, FALSE);
      CmdSequence++;
      break;
    case CONN_ACK_PACKET_WAIT_INSERT_RETURN_CODE:
      if (TestSerialDataAvailable() == 1) {
        //  If serial data is available
        RetValue = CheckStringOkOrExit();
        if (RetValue == 1) {
          //  Typed "ok"
          break;
        } else if (RetValue == 2) {
          //  Typed "exit"
          CmdSequence = CONN_ACK_PACKET_IDLE;
          break;
        }
        if (StrSerialInput.length() > 0) {        
          if (atol(StrSerialInput.c_str()) > RETURN_CODE_MAX_VALUE) {
            LocalReadString_FLASH((uint8_t *)STR_ERR_RETURN_CODE, strlen(STR_ERR_RETURN_CODE), TRUE, FALSE);
            break;            
          }
          Mqtt.ReturnCode = (atol(StrSerialInput.c_str()) & 0xFF);
          LocalReadString_FLASH((uint8_t *)STR_RETURN_CODE, strlen(STR_RETURN_CODE), FALSE, FALSE);
          LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);          
        }
      }
      break;
    case CONN_ACK_PACKET_GENERATE:
      Generate_ConnAck_Packet();

      MqttProcessPacket = ProcessMqttIdle;
      PrintMainMenu();    
      break;
    default:
      break;
  }
}
//======================================================================

//======================================================================
uint8_t ProcessConnAckFlag(void) {
  int     Index;
  uint8_t Result;
  
  Index = StrSerialInput.indexOf(ASCII_EQUAL);

  if (Index == -1) {
    return(0);    //  Invalid command
  } else if (Index == 2) {
    //  Sets single bit
    if (StrSerialInput.indexOf(LocalReadString_FLASH((uint8_t *)STR_B0, strlen(STR_B0), TRUE, TRUE)) == 0) {
      StrSerialInput.remove(0, ++Index);
      Result = CheckSubString(0, 1);
      if (Result == 2) {return(0);}   //  Invalid command 
      Mqtt.ConnAckFlags.Flag.Bit.Sp = Result;
      LocalReadString_FLASH((uint8_t *)STR_SP_FLAG_OK, strlen(STR_SP_FLAG_OK), FALSE, FALSE);
      LocalReadString_FLASH((uint8_t *)STR_FINISHED, strlen(STR_FINISHED), TRUE, FALSE);
      return(1);      //  Set Flag bit 1            
    } else {
      return(0);    //  Invalid command
    }
  } else {
    return(0);    //  Invalid command
  }
}
//======================================================================

//======================================================================
void Generate_ConnAck_Packet(void) {
  LocalReadString_FLASH((uint8_t *)STR_SEP_1, strlen(STR_SEP_1), FALSE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_CODE, strlen(STR_PRNT_CONNECT_PACKET_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", ((MQTT_CONNACK << 4) + (MQTT_FLAGS_CONNACK)), TRUE); 

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONNECT_PACKET_LENGTH, strlen(STR_PRNT_CONNECT_PACKET_LENGTH), FALSE, FALSE);
  PrintHexFormat("%02X", MQTT_CONNACK_REMAINING_LEN, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONN_ACK_FLAGS, strlen(STR_PRNT_CONN_ACK_FLAGS), FALSE, FALSE);
  PrintHexFormat("%02X", Mqtt.ConnAckFlags.Flag.FlagByte, TRUE);

  LocalReadString_FLASH((uint8_t *)STR_PRNT_CONN_ACK_RETURN_CODE, strlen(STR_PRNT_CONN_ACK_RETURN_CODE), FALSE, FALSE);
  PrintHexFormat("%02X", Mqtt.ReturnCode, TRUE);
  
  LocalReadString_FLASH((uint8_t *)STR_SEP_2, strlen(STR_SEP_2), TRUE, FALSE);

  LocalReadString_FLASH((uint8_t *)STR_CONN_ACK_PACKET_OK, strlen(STR_CONN_ACK_PACKET_OK), FALSE, FALSE);
  Mqtt.Packets_CONNACK(&Gsm.GSM_Data_Array[0]);  
  PrintHexArray(&Gsm.GSM_Data_Array[0], Mqtt.MessageLen);
  Serial.println();
}
//======================================================================
