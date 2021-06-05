//======================================================================
//  Sets digital Output
void SetOutputPin(void) {
  Io.SetOutputLed();                 // Set Led nets
  Io.SetOutputTrigger();             // Set trigger nets
}
//======================================================================

//======================================================================
//  Sets digital Output
void TestLed_Trigger(void) {
  Io.CheckOutputLed();               // Check led nets
  Io.CheckOutputTrigger();           // Check Trigger nets
}
//======================================================================
