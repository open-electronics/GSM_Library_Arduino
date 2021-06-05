
//======================================================================
void PrintTcpUdpState(void) {
  if (TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD != LastStateTCP_UDP) {
    LastStateTCP_UDP = TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD;
    Serial.print("TCP/UDP: ");
    Serial.println(TcpIp.TcpIpStateFlag.BitState[0].State_TCP_UPD);
  }
  if (TcpIp.TcpIpStateFlag.BitState[0].ClientState != LastStateConn) {
    LastStateConn = TcpIp.TcpIpStateFlag.BitState[0].ClientState;
    Serial.print("Client: ");
    Serial.println(TcpIp.TcpIpStateFlag.BitState[0].ClientState);
  } 
}
//======================================================================

//======================================================================
//  Function to send generic command in the loop()
void ProcessSM_MqttPacket_AT_Cmd(void) {
  uint8_t   TempData;
  uint8_t * DataPnt;
  
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0) && (Gsm.GsmFlag.Bit.GprsInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;
      
      PresentStateSendCmd = FutureStateSendCmd;
      switch (PresentStateSendCmd)
      {
        case CMD_ATQ_GMI:                                 //  Present State
          Gsm.SetCmd_ATQ_GMI(GMR_V25_FORMAT);
          FutureStateSendCmd = CMD_ATQ_GMM;               //  Future State
          break;
        case CMD_ATQ_GMM:                                 //  Present State
          Gsm.SetCmd_ATQ_GMM(GMR_V25_FORMAT);
          FutureStateSendCmd = CMD_ATQ_GMR;               //  Future State
          break;
        case CMD_ATQ_GMR:                                 //  Present State
          Gsm.SetCmd_ATQ_GMR(GMR_V25_FORMAT);
          PastStateSendCmd = CMD_ATQ_GMR;                 //  Past State
#ifdef QUECTEL_M95
          FutureStateSendCmd = CMD_AT_QIDNSIP;            //  Future State
#else
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;         //  Future State
#endif
          break;          
#ifdef QUECTEL_M95
        case CMD_AT_QIDNSIP:                              //  Present State
          TcpIp.SetCmd_AT_QIDNSIP(DOMAIN_NAME);
          PastStateSendCmd = CMD_AT_QIDNSIP;              //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;         //  Future State
          break;
#endif
        case CMD_ATQ_CIPSTATUS:                           //  Present State
          TcpIp.SetCmd_AT_CIPSTATUS(0);
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS_ANSW;    //  Future State
          Isr.TimeOutWait = T_2SEC;
          break;
        case CMD_ATQ_CIPSTATUS_ANSW:                      //  Present State
          switch (PastStateSendCmd)
          {
            case CMD_ATQ_GMR:                             //  Past State
            case CMD_AT_QIDNSIP:                          //  Past State
            case CMD_AT_CIPCLOSE:                         //  Past State
              if ((TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_IP_CLOSE_CODE)  || \
                  (TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_IP_STATUS_CODE) || \
                  (TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_CONNECT_OK_CODE)) {
                FutureStateSendCmd = CMD_AT_CIPSTART;     //  Future State
              } else {
                FutureStateSendCmd = CMD_ATQ_CIPSTATUS;   //  Future State
              }
              break;
            case CMD_AT_CIPSTART:                         //  Past State
              if ((TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_TCP_CONNECTING_CODE) || \
                  (TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_IP_STATUS_CODE)) {
                LocalReadString_FLASH((uint8_t *)STR_TCP_UDP_CONNECTING, strlen(STR_TCP_UDP_CONNECTING), TRUE, FALSE);
                FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                               //  Future State
              } else if (TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_CONNECT_OK_CODE) {
                LocalReadString_FLASH((uint8_t *)STR_TCP_UDP_CONNECTED,    strlen(STR_TCP_UDP_CONNECTED),    TRUE, FALSE);
                LocalReadString_FLASH((uint8_t *)STR_TCP_SERVER_CONNECTED, strlen(STR_TCP_SERVER_CONNECTED), TRUE, FALSE);
                FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_CONN_STP_1;                //  Future State
                Io.LedOn(PIN_LED4); //  LED 4 RED ON
              }
              break;
            case CMD_AT_CIPSEND_MQTT_CONN_STP_2:                                    //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_GET_ANSW_MQTT_CONN);           //  Future State
              break;
            case DECODE_ANSW_MQTT_CONN:                                             //  Past State
#ifdef TEST_SUBSCRIBE_QOS_0
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_SUBSCRIBE_STP_1); //  Future State
              Io.LedOn(PIN_LED6); //  LED 6 YELLOW ON
              break;
#endif              
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUB_STP_1);       //  Future State
              Io.LedOn(PIN_LED5); //  LED 5 RED ON
              break;
            case DECODE_ANSW_MQTT_PUB_QOS_1:                                        //  Past State
            case DECODE_ANSW_MQTT_PUBREL:                                           //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUB_STP_1);       //  Future State
              Io.LedOn(PIN_LED5); //  LED 5 RED ON
              break;            
            case CMD_AT_GET_ANSW_MQTT_PUB:                                          //  Past State
#ifdef TEST_MQTT_PING_CMD
              if (PingTimeOut == 0) {
                PingTimeOut = T_120SEC;
                SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PING_1);        //  Future State
                Io.LedOn(PIN_LED7); //  LED 7 YELLOW ON
                break;
              }
#endif
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUB_STP_1);       //  Future State
              Io.LedOn(PIN_LED5); //  LED 5 RED ON
              break;            
            case DECODE_ANSW_MQTT_PUB_QOS_2:                                        //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUBREL_STP_1);    //  Future State
              break;
            case DECODE_ANSW_MQTT_PING:                                             //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUB_STP_1);       //  Future State
              Io.LedOn(PIN_LED5); //  LED 5 RED ON
              break;
            case DECODE_ANSW_MQTT_SUBSCRIBE:                                        //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUB_STP_1);       //  Future State
              Io.LedOn(PIN_LED5); //  LED 5 RED ON
              break;
            case DECODE_ANSW_MQTT_CONN_ERR:                                         //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPCLOSE);                     //  Future State
              break;
            case CMD_AT_GET_ANSW_MQTT_PUB_QOS_ERR:                                  //  Past State
            case DECODE_ANSW_MQTT_PUB_QOS_1_ERR:                                    //  Past State
            case DECODE_ANSW_MQTT_PUB_QOS_2_ERR:                                    //  Past State
            case DECODE_ANSW_MQTT_PUBREL_ERR:                                       //  Past State
            case DECODE_ANSW_MQTT_PING_ERR:                                         //  Past State
            case DECODE_ANSW_MQTT_SUBSCRIBE_ERR:                                    //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_DISCONN_STP_1);   //  Future State
              break;
            case DECODE_MQTT_PUBLISH_RECEIVED:                                      //  Past State
              SetFutureStateAfterCipStatusCmd(CMD_AT_CIPSEND_MQTT_PUB_STP_1);       //  Future State
              Io.LedOn(PIN_LED5); //  LED 5 RED ON
              break;
            default:
              break;
          }         
          break;
        case CMD_AT_CIPSTART:                                                       //  Present State
          TcpIp.SetCmd_AT_CIPSTART(0, TCP_CONNECTION, DOMAIN_NAME);
          PastStateSendCmd = CMD_AT_CIPSTART;                                       //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                                   //  Future State
          break;
        case CMD_AT_CIPCLOSE:
          TcpIp.SetCmd_AT_CIPCLOSE(0, 0);
          PastStateSendCmd = CMD_AT_CIPCLOSE;                                       //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                                   //  Future State
          break;
        case CMD_AT_CIPSEND_MQTT_CONN_STP_1:                                        //  Present State
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_CONN_STP_2;                      //  Future State
          break;
        case CMD_AT_CIPSEND_MQTT_CONN_STP_2:                                        //  Present State
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetMqttConnectPacket());
            FutureStateSendCmd = CMD_AT_GET_ANSW_MQTT_CONN;                         //  Future State
            Isr.TimeOutWait = T_2SEC;
          }
          break;
        case CMD_AT_GET_ANSW_MQTT_CONN:                                             //  Present State
          SetCmdToGetAnswer();
          FutureStateSendCmd = DECODE_ANSW_MQTT_CONN;                               //  Future State
          break;          
        case DECODE_ANSW_MQTT_CONN:                                                 //  Present State
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_CONNACK);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_CONNACK) {
            if (Mqtt.DecodePacketReceived.ConnectReturnCode == MQTT_CONNACK_ACCEPTED) {
              //  Connection accepted
              LocalReadString_FLASH((uint8_t *)STR_MQTT_CONN_OK, strlen(STR_MQTT_CONN_OK), TRUE, FALSE);
              PastStateSendCmd = DECODE_ANSW_MQTT_CONN;                             //  Past State
              FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                               //  Future State
              Isr.TimeOutWait = T_2SEC;
              Io.LedOff(PIN_LED4); //  LED 4 OFF
              break;
            } else {
              if (Mqtt.DecodePacketReceived.ConnectReturnCode == MQTT_CONNACK_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION) {
                LocalReadString_FLASH((uint8_t *)STR_MQTT_CONN_KO_1, strlen(STR_MQTT_CONN_KO_1), TRUE, FALSE);
              } else if (Mqtt.DecodePacketReceived.ConnectReturnCode == MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED) {
                LocalReadString_FLASH((uint8_t *)STR_MQTT_CONN_KO_2, strlen(STR_MQTT_CONN_KO_2), TRUE, FALSE);
              } else if (Mqtt.DecodePacketReceived.ConnectReturnCode == MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE) {
                LocalReadString_FLASH((uint8_t *)STR_MQTT_CONN_KO_3, strlen(STR_MQTT_CONN_KO_3), TRUE, FALSE);
              } else if (Mqtt.DecodePacketReceived.ConnectReturnCode == MQTT_CONNACK_REFUSED_BAD_USERNAME_OR_PASSWORD) {
                LocalReadString_FLASH((uint8_t *)STR_MQTT_CONN_KO_4, strlen(STR_MQTT_CONN_KO_4), TRUE, FALSE);
              } else if (Mqtt.DecodePacketReceived.ConnectReturnCode == MQTT_CONNACK_REFUSED_NOT_AUTHORIZED) {
                LocalReadString_FLASH((uint8_t *)STR_MQTT_CONN_KO_5, strlen(STR_MQTT_CONN_KO_5), TRUE, FALSE);
              }
            }            
          }
          PastStateSendCmd = DECODE_ANSW_MQTT_CONN_ERR;                 //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                       //  Future State
          Isr.TimeOutWait = T_2SEC;
          Io.LedOff(PIN_LED4); //  LED 4 OFF
          break;
        case CMD_AT_CIPSEND_MQTT_PUB_STP_1:                             //  Present State
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_PUB_STP_2;           //  Future State
          break;  
        case CMD_AT_CIPSEND_MQTT_PUB_STP_2:                             //  Present State
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetMqttPublishPacket());
            FutureStateSendCmd = CMD_AT_GET_ANSW_MQTT_PUB;              //  Future State
            Isr.TimeOutWait = T_2SEC;
          }
          break;
        case CMD_AT_GET_ANSW_MQTT_PUB:                                  //  Present State
          if (Mqtt.PublishFlags.Flag.Bit.QoS > 0){
            SetCmdToGetAnswer();
            if (Mqtt.PublishFlags.Flag.Bit.QoS == 1) {
              FutureStateSendCmd = DECODE_ANSW_MQTT_PUB_QOS_1;          //  Future State
            } else if (Mqtt.PublishFlags.Flag.Bit.QoS == 2) {
              FutureStateSendCmd = DECODE_ANSW_MQTT_PUB_QOS_2;          //  Future State
            } else {
              LocalReadString_FLASH((uint8_t *)STR_MQTT_QOS_ERR, strlen(STR_MQTT_QOS_ERR), TRUE, FALSE);
              PastStateSendCmd = CMD_AT_GET_ANSW_MQTT_PUB_QOS_ERR;      //  Past State
              FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                   //  Future State
              Isr.TimeOutWait = T_2SEC;
            }
          } else {
            LocalReadString_FLASH((uint8_t *)STR_MQTT_PUB_SENT, strlen(STR_MQTT_PUB_SENT), TRUE, FALSE);
            PastStateSendCmd = CMD_AT_GET_ANSW_MQTT_PUB;                //  Past State
            FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                     //  Future State
            Isr.TimeOutWait = T_60SEC;
            Io.LedOff(PIN_LED5); //  LED 5 OFF
          }
          break;
        case DECODE_ANSW_MQTT_PUB_QOS_1:                                //  Present State        
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_PUBACK);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_PUBACK) {
            if (Mqtt.PacketIdentifier == Mqtt.DecodePacketReceived.PacketIdentifier) {
              LocalReadString_FLASH((uint8_t *)STR_MQTT_PUBACK_OK, strlen(STR_MQTT_PUBACK_OK), TRUE, FALSE);
              PastStateSendCmd = DECODE_ANSW_MQTT_PUB_QOS_1;            //  Past State
              FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                   //  Future State
              Isr.TimeOutWait = T_30SEC;
              Io.LedOff(PIN_LED5); //  LED 5 OFF
              break;            
            }
          }
          LocalReadString_FLASH((uint8_t *)STR_MQTT_PUBACK_KO, strlen(STR_MQTT_PUBACK_KO), TRUE, FALSE);
          PastStateSendCmd = DECODE_ANSW_MQTT_PUB_QOS_1_ERR;            //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                       //  Future State
          Isr.TimeOutWait = T_2SEC;
          Io.LedOff(PIN_LED5); //  LED 5 OFF
          break;
        case DECODE_ANSW_MQTT_PUB_QOS_2:                                //  Present State       
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_PUBREC);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_PUBREC) {
            if (Mqtt.PacketIdentifier == Mqtt.DecodePacketReceived.PacketIdentifier) {
              LocalReadString_FLASH((uint8_t *)STR_MQTT_PUBREC_OK, strlen(STR_MQTT_PUBREC_OK), TRUE, FALSE);
              FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_PUBREL_STP_1;    //  Future State
              Isr.TimeOutWait = T_2SEC;              
              break;
            }
          }
          LocalReadString_FLASH((uint8_t *)STR_MQTT_PUBREC_KO, strlen(STR_MQTT_PUBREC_KO), TRUE, FALSE);
          PastStateSendCmd = DECODE_ANSW_MQTT_PUB_QOS_2_ERR;            //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                       //  Future State
          Isr.TimeOutWait = T_2SEC;
          break;
        case CMD_AT_CIPSEND_MQTT_PUBREL_STP_1:                          //  Present State
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_PUBREL_STP_2;        //  Future State
          break;
        case CMD_AT_CIPSEND_MQTT_PUBREL_STP_2:                          //  Present State
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetMqttPubRelPacket());
            FutureStateSendCmd = CMD_AT_GET_ANSW_MQTT_PUBREL;           //  Future State
            Isr.TimeOutWait = T_2SEC;
          }
          break;
        case CMD_AT_GET_ANSW_MQTT_PUBREL:                               //  Present State
          SetCmdToGetAnswer();
          FutureStateSendCmd = DECODE_ANSW_MQTT_PUBREL;                 //  Future State
          break;
        case DECODE_ANSW_MQTT_PUBREL:                                   //  Present State
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_PUBCOMP);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_PUBCOMP) {
            if (Mqtt.PacketIdentifier == Mqtt.DecodePacketReceived.PacketIdentifier) {
              LocalReadString_FLASH((uint8_t *)STR_MQTT_PUBCOMP_OK, strlen(STR_MQTT_PUBCOMP_OK), TRUE, FALSE);
              PastStateSendCmd = DECODE_ANSW_MQTT_PUBREL;               //  Past State
              FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                   //  Future State
              Isr.TimeOutWait = T_30SEC;
              Io.LedOff(PIN_LED5); //  LED 5 OFF
              break;
            }
          }
          LocalReadString_FLASH((uint8_t *)STR_MQTT_PUBCOMP_KO, strlen(STR_MQTT_PUBCOMP_KO), TRUE, FALSE);
          PastStateSendCmd = DECODE_ANSW_MQTT_PUBREL_ERR;               //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                       //  Future State
          Isr.TimeOutWait = T_2SEC;
          Io.LedOff(PIN_LED5); //  LED 5 OFF
          break;
#ifdef TEST_MQTT_PING_CMD
        case CMD_AT_CIPSEND_MQTT_PING_1:                                //  Present State
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_PING_2;              //  Future State
          break;
        case CMD_AT_CIPSEND_MQTT_PING_2:                                //  Present State
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetMqttPingPacket());
            FutureStateSendCmd = CMD_AT_GET_ANSW_MQTT_PING;             //  Future State
            Isr.TimeOutWait = T_2SEC;
          }
          break;
        case CMD_AT_GET_ANSW_MQTT_PING:                                 //  Present State
          SetCmdToGetAnswer();
          FutureStateSendCmd = DECODE_ANSW_MQTT_PING;                   //  Future State
          break;
        case DECODE_ANSW_MQTT_PING:                                     //  Present State
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_PINGRESP);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_PINGRESP) {
            LocalReadString_FLASH((uint8_t *)STR_MQTT_PING_OK, strlen(STR_MQTT_PING_OK), TRUE, FALSE);
            PastStateSendCmd = DECODE_ANSW_MQTT_PING;                   //  Past State
            FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                     //  Future State
            Isr.TimeOutWait = T_30SEC;
            Io.LedOff(PIN_LED7); //  LED 7 OFF
            break;
          }
          LocalReadString_FLASH((uint8_t *)STR_MQTT_PING_KO, strlen(STR_MQTT_PING_KO), TRUE, FALSE);
          PastStateSendCmd = DECODE_ANSW_MQTT_PING_ERR;                 //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                       //  Future State
          Isr.TimeOutWait = T_2SEC;
          Io.LedOff(PIN_LED7); //  LED 7 OFF
          break;
#endif
#ifdef TEST_SUBSCRIBE_QOS_0
        case CMD_AT_CIPSEND_MQTT_SUBSCRIBE_STP_1:                        //  Present State
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_SUBSCRIBE_STP_2;      //  Future State
          break;
        case CMD_AT_CIPSEND_MQTT_SUBSCRIBE_STP_2:                        //  Present State
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetMqttSubscribePacket());
            FutureStateSendCmd = CMD_AT_GET_ANSW_MQTT_SUBSCRIBE;         //  Future State
            Isr.TimeOutWait = T_2SEC;
          }
          break;
        case CMD_AT_GET_ANSW_MQTT_SUBSCRIBE:                             //  Present State
          SetCmdToGetAnswer();
          FutureStateSendCmd = DECODE_ANSW_MQTT_SUBSCRIBE;               //  Future State        
          break;
        case DECODE_ANSW_MQTT_SUBSCRIBE:                                 //  Present State
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_SUBACK);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_SUBACK) {
            if (Mqtt.DecodePacketReceived.SubRetCode[SUBSCRIBE_TOPIC_FILTER_USED] != 0x80) {
              LocalReadString_FLASH((uint8_t *)STR_MQTT_SUBACK_OK, strlen(STR_MQTT_SUBACK_OK), TRUE, FALSE);
              PastStateSendCmd = DECODE_ANSW_MQTT_SUBSCRIBE;              //  Past State
              FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                     //  Future State
              Isr.TimeOutWait = T_2SEC;
              Io.LedOff(PIN_LED6); //  LED 6 OFF
              break;
            } else {
              LocalReadString_FLASH((uint8_t *)STR_MQTT_SUBACK_KO, strlen(STR_MQTT_SUBACK_KO), TRUE, FALSE);
              PastStateSendCmd = DECODE_ANSW_MQTT_SUBSCRIBE_ERR;          //  Past State
              FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                     //  Future State
              Isr.TimeOutWait = T_2SEC;
              Io.LedOff(PIN_LED6); //  LED 6 OFF
            }
            break;
          }
          LocalReadString_FLASH((uint8_t *)STR_MQTT_SUBACK_KO, strlen(STR_MQTT_SUBACK_KO), TRUE, FALSE);
          PastStateSendCmd = DECODE_ANSW_MQTT_SUBSCRIBE_ERR;              //  Past State
          FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                         //  Future State
          Isr.TimeOutWait = T_2SEC;
          Io.LedOff(PIN_LED6); //  LED 6 OFF
          break;
        case DECODE_MQTT_PUBLISH_RECEIVED:                                //  Present State
          Mqtt.DecodeMqttPacketReceived((uint8_t *)&Gsm.GSM_Data_Array[TcpIp.CipRxGetAnswer.DataStartPointer], MQTT_PUBLISH);
          if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_PUBLISH) {
            LocalReadString_FLASH((uint8_t *)STR_MQTT_PUB_RECEIVED, strlen(STR_MQTT_PUB_RECEIVED), TRUE, FALSE);
            Serial.print((char *)(Mqtt.DecodePacketReceived.TopicName));
            Serial.println();
            Serial.print((char *)(Mqtt.DecodePacketReceived.Payload));
            Serial.println();
            if (SUBSCRIBE_TOPIC_FILTER_USED == 2) {
              Mqtt.DecodeMqttPacketReceived((uint8_t *)DataPnt, MQTT_PUBLISH);
              if (Mqtt.DecodePacketReceived.MqttCmd == MQTT_PUBLISH) {
                LocalReadString_FLASH((uint8_t *)STR_MQTT_PUB_RECEIVED, strlen(STR_MQTT_PUB_RECEIVED), TRUE, FALSE);
                Serial.print((char *)(Mqtt.DecodePacketReceived.TopicName));
                Serial.println();
                Serial.print((char *)(Mqtt.DecodePacketReceived.Payload)); 
                Serial.println();
              }
            }
          }          
          PastStateSendCmd    = DECODE_MQTT_PUBLISH_RECEIVED;          //  Past State
          FutureStateSendCmd  = CMD_ATQ_CIPSTATUS;                     //  Future State
          Isr.TimeOutWait     = BckTimeOut;
          PastStateSendCmd    = BckPastStateSendCmd;
          PresentStateSendCmd = BckPresentStateSendCmd;
          FutureStateSendCmd  = BckFutureStateSendCmd;
          break;
#endif
        case CMD_AT_CIPSEND_MQTT_DISCONN_STP_1:                           //  Present State
          TcpIp.SetCmd_AT_CIPSEND(false, 0);
          FutureStateSendCmd = CMD_AT_CIPSEND_MQTT_DISCONN_STP_2;         //  Future State
          break;
        case CMD_AT_CIPSEND_MQTT_DISCONN_STP_2:                           //  Present State
          if (TcpIp.TcpIpFlag.Bit.SetsDataToSent == 1) {
            TcpIp.TcpIpFlag.Bit.SetsDataToSent = 0;
            TcpIp.SetCmd_AT_CIPSEND_Step2(SetMqttDisconnectPacket());
            PastStateSendCmd = CMD_ATQ_GMR;                               //  Past State
            FutureStateSendCmd = CMD_ATQ_CIPSTATUS;                       //  Future State
            Isr.TimeOutWait = T_2SEC;
          }
          break;
        default:
          break;      
      }
    }
  } 
}
//======================================================================

//======================================================================
void SetFutureStateAfterCipStatusCmd(uint8_t FutureState) {
  if (TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_CONNECT_OK_CODE) {
    FutureStateSendCmd = FutureState;           //  Future State
  } else {
    if (TcpIp.TcpIpStateFlag.BitState[0].State_Conn == STATE_IP_CLOSE_CODE) {
      //  Connection Closed
      PastStateSendCmd = CMD_ATQ_GMR;           //  Past State
      FutureStateSendCmd = CMD_ATQ_CIPSTATUS;   //  Future State    
    } else {
      //  To Do
    }
  }
}
//======================================================================

//======================================================================
void SetCmdToGetAnswer(void) {
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
  TcpIp.SetCmd_AT_CIPRXGET(2, 0, 255);
#endif
#ifdef QUECTEL_M95
  TcpIp.SetCmd_AT_CIPRXGET(255);
#endif  
}
//======================================================================

//======================================================================
//  This function is used to set MQTT CONNECT Packet
uint8_t SetMqttConnectPacket(void) {
  uint8_t Index;

  Mqtt.ConnectFlags.Flag.FlagByte         = 0x00;
  Mqtt.ConnectFlags.Flag.Bit.UserName     = 1;
  Mqtt.ConnectFlags.Flag.Bit.Password     = 1;
  Mqtt.ConnectFlags.Flag.Bit.CleanSession = 1;
  Mqtt.KeepAlive = 1000;
            
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])));
  Mqtt.Packets_CONNECT(&Gsm.GSM_Data_Array[0]);  
  Index = Mqtt.MessageLen;
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
  return(Index);
}
//======================================================================

//======================================================================
//  This function is used to set MQTT DISCONNECT Packet
uint8_t SetMqttDisconnectPacket(void) {
  uint8_t Index;
   
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])));
  Mqtt.Packets_DISCONNECT(&Gsm.GSM_Data_Array[0]);  
  Index = Mqtt.MessageLen;
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
  return(Index);
}
//======================================================================

//======================================================================
//  This function is used to set MQTT PUBLISH Packet
uint8_t SetMqttPublishPacket(void) {
  uint8_t Index = 0;

  memset(Mqtt.PublishPayload, 0, (sizeof(Mqtt.PublishPayload)/sizeof(Mqtt.PublishPayload[0])));
  Gsm.ReadStringFLASH((uint8_t *)STR_MQTT_FIELD_1, ((uint8_t *)Mqtt.PublishPayload), strlen(STR_MQTT_FIELD_1));
  Index = strlen(Mqtt.PublishPayload);
  Index += Gsm.ConvertNumberToString(FieldValue, ((uint8_t *)(Mqtt.PublishPayload) + Index), 0);
  Mqtt.PublishPayload[Index++] = ASCII_AMPERSAND;
  FieldValue += 10;
  Gsm.ReadStringFLASH((uint8_t *)STR_MQTT_FIELD_2, ((uint8_t *)Mqtt.PublishPayload + Index), strlen(STR_MQTT_FIELD_2));
  Index = strlen(Mqtt.PublishPayload);
  Index += Gsm.ConvertNumberToString(FieldValue, ((uint8_t *)(Mqtt.PublishPayload) + Index), 0);
  FieldValue += 10;
  Mqtt.PublishPayload[Index++] = ASCII_AMPERSAND;
  Gsm.ReadStringFLASH((uint8_t *)STR_STATUS_MQTT_PUB, ((uint8_t *)Mqtt.PublishPayload + Index), strlen(STR_STATUS_MQTT_PUB));  

  Mqtt.PublishFlags.Flag.FlagByte    = 0x00;
  Mqtt.PublishFlags.Flag.Bit.Retain  = 0;         //  Set Retain bit to zero
  Mqtt.PublishFlags.Flag.Bit.QoS     = 0;         //  Set QoS bit to zero. ThingSpeak work only with Qos = 0
  Mqtt.PublishFlags.Flag.Bit.Dup     = 0;         //  Set Dup bit to zero
  if (Mqtt.PublishFlags.Flag.Bit.QoS > 0) {
    Mqtt.PacketIdentifier = PACKET_IDENTIFIER;    //  Set packet identifier
  }
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])));
  Mqtt.Packets_PUBLISH(&Gsm.GSM_Data_Array[0]);  
  Index = Mqtt.MessageLen;
  
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
  return(Index);
}
//======================================================================

//======================================================================
//  This function is used to set MQTT SUBSCRIBE Packet
uint8_t SetMqttSubscribePacket(void) {
  uint8_t Index = 0;

  //  Enable Topic filter 1; array index = 0 (Max number of "Topic Filter" supported by library is 4. Then array index from 0 to 3)
  Mqtt.Subscribe[SUBSCRIBE_TOPIC_FILTER_USED].Flags.FlagByte     = 0x00;
  Mqtt.Subscribe[SUBSCRIBE_TOPIC_FILTER_USED].Flags.Bit.EnFilter = 1;
  Mqtt.Subscribe[SUBSCRIBE_TOPIC_FILTER_USED].Qos                = MQTT_SUB_QOS_0;       //  Set QoS = 0 for Subscribe command
  Mqtt.PacketIdentifier = PACKET_IDENTIFIER;    //  Set packet identifier
  
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])));
  Mqtt.Packets_SUBSCRIBE(&Gsm.GSM_Data_Array[0]);  
  Index = Mqtt.MessageLen;
  
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
  return(Index);
}
//======================================================================

//======================================================================
//  This function is used to set MQTT PUBREL Packet
uint8_t SetMqttPubRelPacket(void) {
  uint8_t Index = 0;

  if (Mqtt.PublishFlags.Flag.Bit.QoS > 0) {
    Mqtt.PacketIdentifier = PACKET_IDENTIFIER;    //  Set packet identifier
  }
  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])));
  Mqtt.Packets_PUBREL(&Gsm.GSM_Data_Array[0]);  
  Index = Mqtt.MessageLen;
  
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
  return(Index);
}
//======================================================================

//======================================================================
//  This function is used to set MQTT PING Packet
uint8_t SetMqttPingPacket(void) {
  uint8_t Index = 0;

  Gsm.ClearBuffer(&Gsm.GSM_Data_Array[0], (sizeof(Gsm.GSM_Data_Array)/sizeof(Gsm.GSM_Data_Array[0])));
  Mqtt.Packets_PINGREQ(&Gsm.GSM_Data_Array[0]);  
  Index = Mqtt.MessageLen;
  
  Gsm.GSM_Data_Array[Index++] = ASCII_SUB;
  return(Index);
}
//======================================================================
