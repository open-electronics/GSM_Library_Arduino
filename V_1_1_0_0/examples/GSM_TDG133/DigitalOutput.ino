//======================================================================
//  Sets GSM Board digital Output
void SetGsmBoardOutputPin(void) {
  Io.SetOutputLed();                 //  Sets I/O leds
  Io.SetOutputTrigger();             //  Sets I/O triggers
}
//======================================================================

//======================================================================
//  Sets GSM Board digital Output
void TestGsmBoardLed_Trigger(void) {
  Io.CheckOutputLed();               //  Checks I/O output (LEDS)
  Io.CheckOutputTrigger();           //  Checks I/O output
}
//======================================================================

//======================================================================
//  Sets Relay digital Output (TDG133)
void SetTDG133_Relay(void) {
  OutputPin[0]    = PIN_RELAY_1;
  OutputPin[1]    = PIN_RELAY_2;
  OutputLedPin[0] = PIN_LED4;
  OutputLedPin[1] = PIN_LED5;
  
  pinMode(OutputPin[0], OUTPUT);
  pinMode(OutputPin[1], OUTPUT);
 
  digitalWrite(OutputPin[0], LOW);
  digitalWrite(OutputPin[1], LOW);
  
  digitalWrite(OutputLedPin[0], LOW);
  digitalWrite(OutputLedPin[1], LOW);
}
//======================================================================

//======================================================================
//  Sets Relay digital Output (TDG133)
void ManageTDG133_Relay(uint8_t n) {
  switch (StateRelay[n])
  {
    case RELAY_STATE_ON:
      digitalWrite(OutputPin[n], HIGH);
      Io.LedOn(OutputLedPin[n]);        
      LastStateRelay[n] = RELAY_STATE_ON;
      StateRelay[n]     = RELAY_STATE_NOTHING;
      SaveRelayStateIntoEeprom(n);
      break;
    case RELAY_STATE_OFF:
      digitalWrite(OutputPin[n], LOW);
      Io.LedOff(OutputLedPin[n]);
      LastStateRelay[n] = RELAY_STATE_OFF;
      StateRelay[n]     = RELAY_STATE_NOTHING;
      SaveRelayStateIntoEeprom(n);
      break;
    case RELAY_STATE_TOGGLE:
      CheckStateRelay(n);
      StateRelay[n] = RELAY_STATE_TOGGLE_WAIT;
      break;
    case RELAY_STATE_TOGGLE_WAIT:
      if (ToggleTimeOutRelay[n] == 0) {
        CheckStateRelay(n);
        StateRelay[n] = RELAY_STATE_NOTHING;
      }
      break;
    case RELAY_STATE_TOGGLE_ONLY:
      CheckStateRelay(n);
      StateRelay[n] = RELAY_STATE_NOTHING;
      break;
    case RELAY_STATE_NOTHING:
      break;     
    default:
      break;
  }
}

void CheckStateRelay(uint8_t n) {
  if (LastStateRelay[n] == RELAY_STATE_ON) {
    digitalWrite(OutputPin[n], LOW);
    Io.LedOff(OutputLedPin[n]);
    LastStateRelay[n] = RELAY_STATE_OFF;
  } else if (LastStateRelay[n] == RELAY_STATE_OFF) {
    digitalWrite(OutputPin[n], HIGH);
    Io.LedOn(OutputLedPin[n]);
    LastStateRelay[n] = RELAY_STATE_ON;
  }  
}

void SaveRelayStateIntoEeprom(uint8_t n) {
  uint8_t TempData;
  
  if (TDG133_Flags.Bit.RecoveryStateRelay == 1) {
    TempData = eeprom_read_byte(OUTPUT_RELAY_STATE_ADD);
    if (LastStateRelay[n] == RELAY_STATE_ON) {
      TempData |= (0x01 << n);
      if (TempData != eeprom_read_byte(OUTPUT_RELAY_STATE_ADD)) {
        eeprom_write_byte(OUTPUT_RELAY_STATE_ADD, TempData);  
      }
    } else if (LastStateRelay[n] == RELAY_STATE_OFF) {
      TempData &= ~(0x01 << n);
      if (TempData != eeprom_read_byte(OUTPUT_RELAY_STATE_ADD)) {
        eeprom_write_byte(OUTPUT_RELAY_STATE_ADD, TempData);  
      }      
    }
  }  
}

void RestoreStateIntoEeprom(void) {
  uint8_t n = 0;
  uint8_t TempData;
  
  if (TDG133_Flags.Bit.RecoveryStateRelay == 1) {
    TempData = eeprom_read_byte(OUTPUT_RELAY_STATE_ADD);
    do {
      if ((TempData & (0x01 << n)) > 0) {
        digitalWrite(OutputPin[n], HIGH);
        Io.LedOn(OutputLedPin[n]);          
        LastStateRelay[n] = RELAY_STATE_ON;
        StateRelay[n]     = RELAY_STATE_NOTHING;
      } else {
        digitalWrite(OutputPin[n], LOW);
        Io.LedOff(OutputLedPin[n]);        
        LastStateRelay[n] = RELAY_STATE_OFF;
        StateRelay[n]     = RELAY_STATE_NOTHING;
      }
    } while (++n < MAX_OUTPUT_NUM);
  } else {
    StateRelay[0] = RELAY_STATE_OFF;
    StateRelay[1] = RELAY_STATE_OFF;
  }
}
//======================================================================
