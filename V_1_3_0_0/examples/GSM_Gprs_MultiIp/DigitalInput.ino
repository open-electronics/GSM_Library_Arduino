
//======================================================================
//  Sets digital inputs
void SetInputPin(void) {
  pinMode(P3_Button, INPUT);          // set pin to input
  pinMode(P4_Button, INPUT);          // set pin to input

  digitalWrite(P3_Button, HIGH);      // turn on pullup resistors
  digitalWrite(P4_Button, HIGH);      // turn on pullup resistors
 
  DigInputStatus.Input = 0xFF;  
}
//======================================================================

//======================================================================
//  Debouncing digital inputs
void DebouncingInput(void) {
  DigInputReaded.In.P3_Button = digitalRead(P3_Button);
  DigInputReaded.In.P4_Button = digitalRead(P4_Button);

  if (DigInputReaded.Input != DigInputStatus.Input) {
    if (DigInputReaded.Input != DigInputVar) {
       DigInputVar = DigInputReaded.Input;
       DebouncingTimeOut = T_50MSEC;
    } else {
      if (DebouncingTimeOut == 0) {
         DigInputVar = DigInputReaded.Input;
         DigInputStatus.Input = DigInputReaded.Input;
      }
    }
  } else {
      DigInputVar = DigInputStatus.Input;
  }
}
//======================================================================

//======================================================================
void Input_Idle(void) {  
  if (DigInputStatus.In.P3_Button != 1) {
    TimeOutP3 = T_3SEC;
    Input_Management = Input_WaitP3;   
    return;
  }
  if (DigInputStatus.In.P4_Button != 1) {
    TimeOutP4 = T_3SEC;
    Input_Management = Input_WaitP4;   
    return;
  }  
}
//======================================================================

//======================================================================
//  Manage P3 functions
void Input_WaitP3(void) {
  if ((TimeOutP3 == 0) && (DigInputStatus.In.P3_Button != 1)) {
    Input_Management = Input_ReleasedP3;  //  Button released after the 3 seconds timer espired
  } else if ((TimeOutP3 > 0) && (DigInputStatus.In.P3_Button != 0)) {
    if ((TcpIp.TcpIpStateFlag.BitState[0].ClientState == CLIENT_CONNECTED_CODE) || \
        (TcpIp.TcpIpStateFlag.BitState[0].ClientState == CLIENT_CONNECTING_CODE)) {
      TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
      StateSendCmd = CMD_AT_CIPCLOSE; 
    }
    Input_Management = Input_Idle;        //  Button released before the 3 seconds timer espired
  }
}

void Input_ReleasedP3(void) {
  if (DigInputStatus.In.P3_Button != 0) {
    TcpIp.TcpIpFlag.Bit.IndexConnection = 0;
    StateSendCmd = CMD_AT_CIPSTART;
    Input_Management = Input_Idle;
  }
}
//======================================================================

//======================================================================
//  Manage P4 functions
void Input_WaitP4(void) {
  if ((TimeOutP4 == 0) && (DigInputStatus.In.P4_Button != 1)) {
    Input_Management = Input_ReleasedP4;    //  Button released after the 3 seconds timer espired
  } else if ((TimeOutP4 > 0) && (DigInputStatus.In.P4_Button != 0)) {
    if ((TcpIp.TcpIpStateFlag.BitState[1].ClientState == CLIENT_CONNECTED_CODE) || \
        (TcpIp.TcpIpStateFlag.BitState[1].ClientState == CLIENT_CONNECTING_CODE)) {
      TcpIp.TcpIpFlag.Bit.IndexConnection = 1;
      StateSendCmd = CMD_AT_CIPCLOSE; 
    }
    Input_Management = Input_Idle;          //  Button released before the 3 seconds timer espired
  }
}

void Input_ReleasedP4(void) {
  if (DigInputStatus.In.P4_Button != 0) {
    TcpIp.TcpIpFlag.Bit.IndexConnection = 1;
    StateSendCmd = CMD_AT_CIPSTART;
    Input_Management = Input_Idle;
  }
}
//======================================================================
