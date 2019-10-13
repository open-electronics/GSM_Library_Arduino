//======================================================================
//  Sets digital Output
void SetOutputPin(void) {
  Io.SetOutputLed();                 //  Sets I/O leds
  Io.SetOutputTrigger();             //  Sets I/O triggers
}
//======================================================================

//======================================================================
//  Sets digital Output
void TestLed_Trigger(void) {
  Io.CheckOutputLed();               //  Checks I/O output (LEDS)
  Io.CheckOutputTrigger();           //  Checks I/O output
}
//======================================================================
