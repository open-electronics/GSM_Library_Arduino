//======================================================================
//  Sets digital inputs
void SetGSMBoardInputPin(void) {
  pinMode(P3_Button, INPUT);          // set pin to input
  pinMode(P4_Button, INPUT);          // set pin to input

  digitalWrite(P3_Button, HIGH);      // turn on pullup resistors
  digitalWrite(P4_Button, HIGH);      // turn on pullup resistors
}
//======================================================================

//======================================================================
void SetTDG133_Input(void) {
  uint8_t n = 0;

  InputPin[0] = PIN_INPUT_1;
  InputPin[1] = PIN_INPUT_2;
  do {
    pinMode(InputPin[n], INPUT);  // set pin to input
    
    if (InputAlarm.Input[n].SetLevelInput == 0) {
      //  Input active HIGH. Set start state to 0
      TDG133_Input[n].In.InputStatus = 0;
    } else if (InputAlarm.Input[n].SetLevelInput == 1) {
      //  Input active LOW. Set start state to 1
      TDG133_Input[n].In.InputStatus = 1;
    } else {
      //  Input active variation
      TDG133_Input[n].In.InputStatus = digitalRead(InputPin[n]); 
    }    
  } while (++n < MAX_INPUT_NUM);
}
//======================================================================

//======================================================================
void SetLedAlarmInputs(void) {
  InputLedPin[0] = PIN_LED6;
  InputLedPin[1] = PIN_LED7;
}
//======================================================================

//======================================================================
//  Debouncing digital inputs (Observation time)
void DebouncingTDG133_Input(void) {
  uint8_t n = 0;

  do {
    TDG133_Input[n].In.InputReaded = digitalRead(InputPin[n]);
    if (TDG133_Input[n].In.InputReaded != TDG133_Input[n].In.InputStatus) {    
      if (TDG133_Input[n].In.InputReaded != TDG133_Input[n].In.InputVar) {
        TDG133_Input[n].In.InputVar = TDG133_Input[n].In.InputReaded;
        DebouncingInput[n] = Input_ObserTime[n] * T_1SEC;
      } else {
        if (DebouncingInput[n] == 0) {
          TDG133_Input[n].In.InputVar    = TDG133_Input[n].In.InputReaded;
          TDG133_Input[n].In.InputStatus = TDG133_Input[n].In.InputReaded;
        }
      }
    } else {
        TDG133_Input[n].In.InputVar = TDG133_Input[n].In.InputStatus;
    }    
  } while (++n < MAX_INPUT_NUM);
}
//======================================================================

//======================================================================
void ManageTDG133_Input(uint8_t n) {  
  switch (StateInput[n])
  {
    case INPUT_STATE_IDLE:
      if (InhibTimeOut_Input[n] == 0) {
        //  Execute code only if Inhibition TimeOut is equal to 0
        if (InputAlarm.Input[n].SetLevelInput == 0) {
          //  Input active HIGH
          if (TDG133_Input[n].In.InputStatus == 1) {
            //  The input 1 is in active state (HIGH)
            InputAlarm.Input[n].SendSmsInput_High = 1;
            Io.LedOn(InputLedPin[n]);  
            StateInput[n] = INPUT_STATE_WAIT;
            
            ReadStringCmd_FLASH((uint8_t *)INPUT_ACTIVATED,  strlen(INPUT_ACTIVATED), FALSE, FALSE);
            if (n == 0) {
              ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE);  
            } else {
              ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE);  
            }
            ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_HIGH, strlen(INPUT_LEVEL_HIGH), FALSE, FALSE);
          } else {
              MaxSmsOutCounterIn[n] = 0;   //  Reset Counter outgoing SMS Input n when allarm is not present
          }
        } else if (InputAlarm.Input[n].SetLevelInput == 1) {
          //  Input active LOW
          if (TDG133_Input[n].In.InputStatus == 0) {
            //  The input 1 is in active state (LOW)
            InputAlarm.Input[n].SendSmsInput_Low = 1;
            Io.LedOn(InputLedPin[n]);
            StateInput[n] = INPUT_STATE_WAIT;
            
            ReadStringCmd_FLASH((uint8_t *)INPUT_ACTIVATED, strlen(INPUT_ACTIVATED), FALSE, FALSE);
            if (n == 0) {
              ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE);  
            } else {
              ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE);  
            }
            ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_LOW, strlen(INPUT_LEVEL_LOW), FALSE, FALSE);
          } else {
              MaxSmsOutCounterIn[n] = 0;   //  Reset Counter outgoing SMS Input 1 when allarm is not present
          }
        } else {
          //  Input active on the VARIATION
          if (TDG133_Input[n].In.InputStatus != TDG133_Input[n].In.LastInputStatus) {
            //  The input 1 is changed (VARIATION)
            TDG133_Input[n].In.LastInputStatus = TDG133_Input[n].In.InputStatus;
            if (TDG133_Input[n].In.InputStatus == 0) {
              InputAlarm.Input[n].SendSmsInput_Low = 1;  
            } else {
              InputAlarm.Input[n].SendSmsInput_High = 1;
            }
            ReadStringCmd_FLASH((uint8_t *)INPUT_ACTIVATED, strlen(INPUT_ACTIVATED),    FALSE, FALSE);
            if (n == 0) {
              ReadStringCmd_FLASH((uint8_t *)INPUT_1_TEXT, strlen(INPUT_1_TEXT), FALSE, FALSE);  
            } else {
              ReadStringCmd_FLASH((uint8_t *)INPUT_2_TEXT, strlen(INPUT_2_TEXT), FALSE, FALSE);  
            }
            ReadStringCmd_FLASH((uint8_t *)INPUT_LEVEL_TOGGLE, strlen(INPUT_LEVEL_TOGGLE), FALSE, FALSE);
    
            UpdateInhibTimeOut_Input(n);
          }
        }
      }    
      break;
    case INPUT_STATE_WAIT:
      if (InputAlarm.Input[n].SetLevelInput == 0) {
        //  Input active HIGH
        if (TDG133_Input[n].In.InputStatus == 0) {
          //  The input 1 is in normal state
          UpdateInhibTimeOut_Input(n);
          Io.LedOff(InputLedPin[n]);
          StateInput[n] = INPUT_STATE_IDLE;
        }
      } else if (InputAlarm.Input[n].SetLevelInput == 1) {
        //  Input active LOW
        if (TDG133_Input[n].In.InputStatus == 1) {
          //  The input 1 is in normal state
          UpdateInhibTimeOut_Input(n);
          Io.LedOff(InputLedPin[n]);
          StateInput[n] = INPUT_STATE_IDLE;
        }
      } 
      break;
  }
}
//======================================================================

//======================================================================
void UpdateInhibTimeOut_Input(uint8_t n) {
  if (InputAlarm.Input[n].DisableInhibTimeInput == 0) {
    InhibTimeOut_Input[n] = Input_InhibTime[n] * T_60SEC;   
  } else {
    InhibTimeOut_Input[n] = 0;
  }
}
//======================================================================

//======================================================================
void SetStateInput(void) {
  StateInput[0] = INPUT_STATE_IDLE;
  StateInput[1] = INPUT_STATE_IDLE;  
}
//======================================================================
