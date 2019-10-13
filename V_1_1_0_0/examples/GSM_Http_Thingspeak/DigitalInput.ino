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
