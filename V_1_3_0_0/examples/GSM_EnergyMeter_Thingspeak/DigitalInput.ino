//======================================================================
//  Sets digital inputs (GSM Board)
void SetInputGsmPin(void) {
  pinMode(P3_Button, INPUT);          // set pin to input
  pinMode(P4_Button, INPUT);          // set pin to input

  digitalWrite(P3_Button, HIGH);      // turn on pullup resistors
  digitalWrite(P4_Button, HIGH);      // turn on pullup resistors
}
//======================================================================

//======================================================================
//  Sets digital inputs (Energy Meter Board)
void SetInputEnergyMeterPin(void) {
  InputPin[0] = IN_P1;
  InputPin[1] = IN_P2;
  
  pinMode(InputPin[0], INPUT);          // set pin to input
  pinMode(InputPin[1], INPUT);          // set pin to input

  digitalWrite(InputPin[0], HIGH);      // turn on pullup resistors
  digitalWrite(InputPin[1], HIGH);      // turn on pullup resistors
}
//======================================================================
