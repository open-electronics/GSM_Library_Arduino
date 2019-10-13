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
  if (DigInputStatus.In.P3_Button == 0) {
    LocalReadStringFLASH((uint8_t *)STR_P3_PRESSED, strlen(STR_P3_PRESSED), TRUE, FALSE);
    Io.TriggerOn(TRIGGER_1); 
    Input_Management = Input_WaitP3;   
    return;
  }
  if (DigInputStatus.In.P4_Button == 0) {
    LocalReadStringFLASH((uint8_t *)STR_P4_PRESSED, strlen(STR_P4_PRESSED), TRUE, FALSE);
    Io.TriggerOn(TRIGGER_2); 
    Input_Management = Input_WaitP4;   
    return;
  }  
}
//======================================================================

//======================================================================
//  Manage P3 functions
void Input_WaitP3(void) {
  if (DigInputStatus.In.P3_Button == 1) {
    LocalReadStringFLASH((uint8_t *)STR_P3_RELEASED, strlen(STR_P3_RELEASED), TRUE, FALSE);
    Io.TriggerOff(TRIGGER_1);
    Input_Management = Input_Idle;        //  Button released 
  }
}

//======================================================================

//======================================================================
//  Manage P4 functions
void Input_WaitP4(void) {
  if (DigInputStatus.In.P4_Button == 1) {
    LocalReadStringFLASH((uint8_t *)STR_P4_RELEASED, strlen(STR_P4_RELEASED), TRUE, FALSE);
    Io.TriggerOff(TRIGGER_2);
    Input_Management = Input_Idle;        //  Button released 
  }
}
//======================================================================
