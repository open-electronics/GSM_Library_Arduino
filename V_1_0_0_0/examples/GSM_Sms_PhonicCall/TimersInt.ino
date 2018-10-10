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

  if (TimeOutBlinkLed   > 0) { TimeOutBlinkLed--;   }
  if (DebouncingTimeOut > 0) { DebouncingTimeOut--; }
  if (TimeOutP3         > 0) { TimeOutP3--;         }
  if (TimeOutP4         > 0) { TimeOutP4--;         }
}
//======================================================================

