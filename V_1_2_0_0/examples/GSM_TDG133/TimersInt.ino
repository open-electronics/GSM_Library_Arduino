//======================================================================
#define Fosc          (unsigned long)16000000    //  Clock 16MHz
#define PRET          (unsigned long)256         //  Prescaler 256
#define MSEC          2                          //  Time Base: 2mSec
#define SLOWBASETIME  (0xFFFF - ((Fosc * MSEC) / (PRET * 1000)))
//======================================================================

//======================================================================
void SetupTimer5(void) {   
  // initialize timer5
  cli();                // disable all interrupts
  TCCR5A = 0x00;
  TCCR5B = 0x00;
  TCCR5C = 0x00;
  TCNT5  = SLOWBASETIME;
  TCCR5B = 0x04;
  TIMSK5 = 0x01;        // enable oveflow timer interrupt
  sei();                // enable all interrupts
}
//======================================================================

//======================================================================
ISR(TIMER5_OVF_vect) {
  TCNT5 = SLOWBASETIME;
  
  //---------------------------------------------------------------
  //  8 Bit timer 
  if (BlinkWaitPhonicCallLed > 0)  { BlinkWaitPhonicCallLed--; }
  if (TimeOutSendAtCmd       > 0)  { TimeOutSendAtCmd--;       }
  //---------------------------------------------------------------

  //---------------------------------------------------------------
  //  16 Bit timer
  if (TimeOutVoiceCall       > 0)  { TimeOutVoiceCall--;       }
  if (TimeOutWaitPhonicCall  > 0)  { TimeOutWaitPhonicCall--;  }
  if (TimeOutGateFunction    > 0)  { TimeOutGateFunction--;    }
  if (DebouncingInput[0]     > 0)  { DebouncingInput[0]--;     }
  if (DebouncingInput[1]     > 0)  { DebouncingInput[1]--;     }
  if (ToggleTimeOutRelay[0]  > 0)  { ToggleTimeOutRelay[0]--;  }
  if (ToggleTimeOutRelay[1]  > 0)  { ToggleTimeOutRelay[1]--;  }
  //---------------------------------------------------------------

  //---------------------------------------------------------------
  //  32 Bit timer
  if (InhibTimeOut_Input[0]  > 0)  { InhibTimeOut_Input[0]--;  }
  if (InhibTimeOut_Input[1]  > 0)  { InhibTimeOut_Input[1]--;  }
  if (TimeOutSendSmsReport   > 0)  { TimeOutSendSmsReport--;   }
  //---------------------------------------------------------------
}
//======================================================================
