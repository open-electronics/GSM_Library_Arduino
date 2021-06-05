//======================================================================
//  Function to send generic command in the loop()
void ProcessGprsTcpIpCmd(void) {
  uint8_t   Index;
  uint8_t   TempData;
  uint8_t * DataPnt;
  
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0) && (Gsm.GsmFlag.Bit.GprsInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;

      StateSendCmd = FutureStateCmd;
      switch (StateSendCmd)
      {
        case CMD_ATQ_GMI:
          Gsm.SetCmd_ATQ_GMI(GMR_V25_FORMAT);
          FutureStateCmd = CMD_ATQ_GMM;
          break;
        case CMD_ATQ_GMM:
          Gsm.SetCmd_ATQ_GMM(GMR_V25_FORMAT);
          FutureStateCmd = CMD_ATQ_GMR;
          break;
        case CMD_ATQ_GMR:
          Gsm.SetCmd_ATQ_GMR(GMR_V25_FORMAT);
          FutureStateCmd = CMD_AT_CIPPING;
          break;          
        case CMD_AT_CIPPING:
#ifdef QUECTEL_M95
          TcpIp.SetCmd_AT_CIPPING(1, 5, 32, 50, 64, 0);
#endif
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
          TcpIp.SetCmd_AT_CIPPING(1, 4, 32, 100, 64, 0);
#endif          
          FutureStateCmd = CMD_AT_VIEW_ANSW;
          break;
        case CMD_AT_VIEW_ANSW:
#ifdef QUECTEL_M95
          if (TcpIp.TcpIpPing.Flags.Bit.DataOk == 1) {
            TcpIp.TcpIpPing.Flags.Bit.DataOk = 0;
            if (TcpIp.TcpIpPing.Flags.Bit.PingOk == 1) {
              TcpIp.TcpIpPing.Flags.Bit.PingOk = 0;
              LocalReadString_FLASH((uint8_t *)STR_PING_OK, strlen(STR_PING_OK), TRUE, FALSE);

              LocalReadString_FLASH((uint8_t *)STR_PING_TIME, strlen(STR_PING_TIME), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.PingTime);
              LocalReadString_FLASH((uint8_t *)STR_PING_TTL, strlen(STR_PING_TTL), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.Ttl);
            } else if (TcpIp.TcpIpPing.Flags.Bit.PingFinishedNormally == 1) {
              TcpIp.TcpIpPing.Flags.Bit.PingFinishedNormally = 0;
              LocalReadString_FLASH((uint8_t *)STR_PING_FINISHED, strlen(STR_PING_FINISHED), TRUE, FALSE);
              
              LocalReadString_FLASH((uint8_t *)STR_PING_SENT, strlen(STR_PING_SENT), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.PingSent);
              LocalReadString_FLASH((uint8_t *)STR_PING_RCVD, strlen(STR_PING_RCVD), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.PingReceived);
              LocalReadString_FLASH((uint8_t *)STR_PING_LOST, strlen(STR_PING_LOST), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.PingLost);
              LocalReadString_FLASH((uint8_t *)STR_PING_MIN_T, strlen(STR_PING_MIN_T), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.MinRespTime);
              LocalReadString_FLASH((uint8_t *)STR_PING_MAX_T, strlen(STR_PING_MAX_T), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.MaxRespTime);
              LocalReadString_FLASH((uint8_t *)STR_PING_AVG_T, strlen(STR_PING_AVG_T), FALSE, FALSE);
              Serial.println(TcpIp.TcpIpPing.AvgRespTime);  
            } else if (TcpIp.TcpIpPing.Flags.Bit.PingTimeOut == 1) {
              TcpIp.TcpIpPing.Flags.Bit.PingTimeOut = 0;
              LocalReadString_FLASH((uint8_t *)STR_PING_TIMEOUT, strlen(STR_PING_TIMEOUT), TRUE, FALSE);
            } else if (TcpIp.TcpIpPing.Flags.Bit.PingStackBusy == 1) {
              TcpIp.TcpIpPing.Flags.Bit.PingStackBusy = 0;
              LocalReadString_FLASH((uint8_t *)STR_PING_STACK_BUSY, strlen(STR_PING_STACK_BUSY), TRUE, FALSE);
            } else if (TcpIp.TcpIpPing.Flags.Bit.HostNotFound == 1) {
              TcpIp.TcpIpPing.Flags.Bit.HostNotFound = 0;
              LocalReadString_FLASH((uint8_t *)STR_PING_NOT_FOUND, strlen(STR_PING_NOT_FOUND), TRUE, FALSE);
            } else if (TcpIp.TcpIpPing.Flags.Bit.PdpContextFailed == 1) {
              TcpIp.TcpIpPing.Flags.Bit.PdpContextFailed = 0;
              LocalReadString_FLASH((uint8_t *)STR_PING_CONTEXT_FAILED, strlen(STR_PING_CONTEXT_FAILED), TRUE, FALSE);
            }
          }
#endif
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
          do {
            if (TcpIp.TcpIpPing[Index].Flags.Bit.DataOk == 1) {
              TcpIp.TcpIpPing[Index].Flags.Bit.DataOk = 0;
              if (TcpIp.TcpIpPing[Index].Flags.Bit.PingOk == 1) {
                TcpIp.TcpIpPing[Index].Flags.Bit.PingOk = 0;
                LocalReadString_FLASH((uint8_t *)STR_PING_OK, strlen(STR_PING_OK), FALSE, FALSE);
                Serial.println(Index);
                
                LocalReadString_FLASH((uint8_t *)STR_PING_TIME, strlen(STR_PING_TIME), FALSE, FALSE);
                Serial.println(TcpIp.TcpIpPing[Index].PingTime);
                LocalReadString_FLASH((uint8_t *)STR_PING_TTL, strlen(STR_PING_TTL), FALSE, FALSE);
                Serial.println(TcpIp.TcpIpPing[Index].Ttl);
              } else if (TcpIp.TcpIpPing[Index].Flags.Bit.PingTimeOut == 1) {
                TcpIp.TcpIpPing[Index].Flags.Bit.PingTimeOut = 0;
                TcpIp.TcpIpPing[Index].Flags.Bit.PingTimeOut = 0;
                LocalReadString_FLASH((uint8_t *)STR_PING_TIMEOUT, strlen(STR_PING_TIMEOUT), FALSE, FALSE);
                Serial.println(Index);
              }
            }            
          } while (Index++ < MAX_PING_NUM);
#endif
          break;
        default:
          break;      
      }
    }
  } 
}
//======================================================================
