//======================================================================
uint8_t TestSerialDataAvailable(void) {
  if (Serial.available() > 0) {
    ReadSerialMqttCmd();
    if (CheckSerialInputData() == 0) {
      //  Invalid string received
      LocalReadString_FLASH((uint8_t *)STR_INVALID_DATA_RECEIVED, strlen(STR_INVALID_DATA_RECEIVED), TRUE, FALSE);
      return(2);
    } else {
      //  Valid string received
      return(1);
    }
  }
  //  No data received
  return(0);
}
//======================================================================

//======================================================================
void ReadSerialMqttCmd(void) {
  StrSerialInput = "";
  StrSerialInput = Serial.readString();
  StrSerialInput.trim();
}
//======================================================================

//======================================================================
uint8_t CheckSerialInputData(void) {
  uint8_t  CharIndex = 0;
  uint8_t  CharCode  = 0;
  
  do {
    CharCode = char(StrSerialInput.charAt(CharIndex++));
    if ((CharCode < ASCII_SPACE) || (CharCode > ASCII_z)) {
      return(0);  //  Ivalid char
    }
  } while (CharIndex < StrSerialInput.length());
  return(1);
}
//======================================================================
