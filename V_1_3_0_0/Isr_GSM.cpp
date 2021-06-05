/*********************************************************************
 *
 *       Interrupt service routine
 *
 *********************************************************************
 * FileName:        Isr_GSM.cpp
 * Revision:        1.0.0
 * Date:            08/05/2016
 *
 * Revision:        1.1.0
 *                  27/11/2018
 *                  - Fixed bug INT0 or INT4 external interrupt. When SMS or phonic Call ring occur
 *                    sometime the routine for send AT commands stop. To fix bug it is necessary to reset 
 *                    the flag "Gsm.GsmFlag.Bit.GsmSendCmdInProgress" into "ResetFlags" function
 *
 * Revision:        1.2.0
 *                  18/11/2019
 *                  - Added "static void (*__Timer1_Vector)(void)"
 *                  - Added function "void Timer1_Vector(void (*function)(void))"
 *
 * Revision:        1.3.0
 *                  09/01/2021 
 *					- Added TimeOut for init process "TimeOutInit"
 *
 * Dependencies:    SoftwareSerial.h
 *                  GenericCmd_GSM.h
 *                  Io_GSM.h
 *                  Isr_GSM.h
 *                  Uart_GSM.h
 *
 * Arduino Board:   Arduino Uno, Arduino Mega 2560, Fishino Uno, Fishino Mega 2560       
 *
 * Company:         Futura Group srl
 *                  www.Futurashop.it
 *                  www.open-electronics.org
 *
 * Developer:       Destro Matteo
 *
 * Support:         info@open-electronics.org
 * 
 * Software License Agreement
 *
 * Copyright (c) 2016, Futura Group srl 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************/

#include <SoftwareSerial.h>

#include "GenericCmd_GSM.h"
#include "Io_GSM.h"
#include "Isr_GSM.h"
#include "Uart_GSM.h"
#include "GprsCmd_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/****************************************************************************
 * Function:        EnableLibInterrupt
 *
 * Overview:        This function enables Timer1 Interrupt and INT0 Interrupt (Or INT4 interrupt for Arduino Mega 2560)
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Isr_GSM::EnableLibInterrupt(void) {
    Isr.EnableTimerInterrupt();
    Isr.EnableCringInterrupt();
}
/****************************************************************************/

/****************************************************************************
 * Function:        EnableTimerInterrupt
 *
 * Overview:        This function enable and sets the Timer1 interrupt
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Isr_GSM::EnableTimerInterrupt(void) {
    cli();                  // disable all interrupts
#ifdef ARDUINO_UNO_REV3
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1   = SLOWBASETIME;
    TCCR1B |= 0x04;         // Prescaler 256
    TIMSK1 |= 0x01;         // enable oveflow timer interrupt
#endif
#ifdef ARDUINO_MEGA2560_REV3
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCCR1C = 0x00;
    TCNT1  = SLOWBASETIME;
    TCCR1B = 0x04;
    TIMSK1 = 0x01;          // enable oveflow timer interrupt
#endif
    sei();                  // enable all interrupts
}
/****************************************************************************/

/****************************************************************************
 * Function:        EnableCringInterrupt
 *
 * Overview:        This function enable CRING interrupt on INT0 for Arduino Uno R3 and INT4 for ArduinoMega 2560 R3
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Isr_GSM::EnableCringInterrupt(void) {
    cli();                      // disable all interrupts
#ifdef ARDUINO_UNO_REV3
    EIMSK  = 0x00;
    EIMSK |= (1 << INT0);       // Enable external interrupt INT0
    EICRA  = 0x00;
    EICRA |= (1 << ISC01);      // The falling edge of INT0 generates an interrupt request
#endif
#ifdef ARDUINO_MEGA2560_REV3    
    EIMSK  = 0x00;
    EIMSK |= (1 << INT4);       // Enable external interrupt INT4   
    EICRB  = 0x00;
    EICRB |= (1 << ISC41);      // The falling edge of INT4 generates an interrupt request
#endif  
    sei();                      // enable all interrupts    
}
/****************************************************************************/

/****************************************************************************
 * Function:        ISR(TIMER1_OVF_vect)
 *
 * Overview:        TIMER1 interrupt vector. This timer is set to generate an interrupt every 2mSec
 *                  This feature is used to create a moltitude of new timers using only simples variables
 *                  with a resolution of 2 msec.
 *                  For example to create a new timer of 100mSec is enough define a new variable and load
 *                  a correct value into it. In this example the correct value to load into the variable is 50.
 *                  This value is decremented every time that the interrupt occur. When this variable reach the
 *                  zero value means that the timer is expired
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#ifdef ENABLE_EXT_TIMER1  
    void Isr_GSM::Ext_Timer1_Vector(void (* volatile function)(void))
    {      
        __Ext_Timer1_Vector = function;
    }
#endif  
ISR(TIMER1_OVF_vect) {
    TCNT1 = SLOWBASETIME;    // preload timer
	
	if (Isr.TimeOutInit             > 0) { Isr.TimeOutInit--;			}		//	AT command Init TimeOut
    if (Isr.TimeOutWait             > 0) { Isr.TimeOutWait--;           }       //  Generic TimeOut
    if (Isr.UartTimeOut             > 0) { Isr.UartTimeOut--;           }       //  TimeOut for hardware/software serial COM    
    
    if (Isr.TimeOutBlinkLed4        > 0) { Isr.TimeOutBlinkLed4--;      }       //  TimeOut Blink Led 4
    if (Isr.TimeOutBlinkLed5        > 0) { Isr.TimeOutBlinkLed5--;      }       //  TimeOut Blink Led 5
    if (Isr.TimeOutBlinkLed6        > 0) { Isr.TimeOutBlinkLed6--;      }       //  TimeOut Blink Led 6
    if (Isr.TimeOutBlinkLed7        > 0) { Isr.TimeOutBlinkLed7--;      }       //  TimeOut Blink Led 7
    if (Isr.TimeOutBlinkLed8        > 0) { Isr.TimeOutBlinkLed8--;      }       //  TimeOut Blink Led 8
    if (Isr.TimeOutBlinkLed9        > 0) { Isr.TimeOutBlinkLed9--;      }       //  TimeOut Blink Led 9
    
    if (Isr.TimeOutBlinkTrigger1    > 0) { Isr.TimeOutBlinkTrigger1--;  }       //  TimeOut Blink Trigger 1
    if (Isr.TimeOutBlinkTrigger2    > 0) { Isr.TimeOutBlinkTrigger2--;  }       //  TimeOut Blink Trigger 2
    if (Isr.TimeOutBlinkTrigger3    > 0) { Isr.TimeOutBlinkTrigger3--;  }       //  TimeOut Blink Trigger 3
    
    //--------------------------------------------------------
    //  Used during engine GSM/GPRS initialization. The Trigger 3
    //  blink to indiacate init process
    if ((Gsm.GsmFlag.Bit.GsmInitInProgress == 1) || (Gsm.GsmFlag.Bit.GprsInitInProgress == 1)) {
        Io.LedBlink(TRIGGER_3, 25, T_250MSEC);
    }
    //--------------------------------------------------------
#ifdef ENABLE_EXT_TIMER1
    if (__Ext_Timer1_Vector != NULL) {
        __Ext_Timer1_Vector();
    }
#endif    
}
/****************************************************************************/

/****************************************************************************
 * Function:        ISR(INT0_vect) Or ISR(INT4_vect)
 *
 * Overview:        INT0 interrupt vector. When an SMS is received or a phonic call incoming, the INT0 falling edge is generated
 *                  The INT0 rising edge is generated when a phonic call terminate or when an SMS is recieved completely
 *
 *                  =========================================================================================================
 *                  SIMCOM SIM900 And QUECTEL M95
 *
 *                  T = 120mSec -> Received SMS
 *                  _____ <-------------> ___________
 *                       |               |
 *                       |               |
 *                       |_______________|
 *
 *                                      T > 120mSec -> Incoming Call
 *                  _____ <---------------------------------------------------------> _________
 *                       |                                                           |
 *                       |                                                           |
 *                       |___________________________________________________________|
 *
 *                  Stanby          HIGH
 *                  -----------------------------------
 *                  Voice Call      The pin is cahnged to low. When any of the following events occur, the pin will be changed to high:
 *                                  (1) Establish the call
 *                                  (2) Hang Up the call
 *                  -----------------------------------
 *                  Data Call       The pin is cahnged to low. When any of the following events occur, the pin will be changed to high:
 *                                  (1) Establish the call
 *                                  (2) Hang Up the call
 *                  -----------------------------------
 *                  SMS             The pin changed to low, and kept low for 120mSec when a SMS is received. Then it is changed to high
 *
 *                  =========================================================================================================
 *                  FIBOCOM G510
 *
 *                  T = 150mSec -> Received SMS (G510)
 *                  _____ <-------------> ___________
 *                       |               |
 *                       |               |
 *                       |_______________|
 *
 *                  T = 1Sec -> Incoming Call                 T = 1Sec
 *                  _____ <-------------> _______________ <-------------> _______________                 _______  
 *                       |               |               |               |               |               |
 *                       |               |               |               |               |               |
 *                       |_______________|               |_______________|               |_______________|
 *                                        <------------->                 <------------->
 *                                           T = 1Sec                        T = 1Sec
 *
 *                  Stanby          HIGH
 *                  -----------------------------------
 *                  Voice Call      The pin is cahnged to low, and kept low for 1Sec then the pin will be changed to high and kept high for 1Sec.
 *                                  This sequence terminate when:
 *                                  (1) Establish the call
 *                                  (2) Hang Up the call
 *                  -----------------------------------
 *                  Data Call       The pin is cahnged to low, and kept low for 1Sec then the pin will be changed to high and kept high for 1Sec.
 *                                  This sequence terminate when:
 *                                  (1) Establish the call
 *                                  (2) Hang Up the call
 *                  -----------------------------------
 *                  SMS             The pin changed to low, and kept low for 150mSec when a SMS is received. Then it is changed to high
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#ifdef ARDUINO_UNO_REV3
ISR(INT0_vect)
{
#endif
#ifdef ARDUINO_MEGA2560_REV3
ISR(INT4_vect)
{
#endif
    if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
        return;
    }
    //Gsm.ResetAllFlags();
    Gsm.ResetGsmFlags();
    Gsm.ResetSmsFlags();
    Gsm.ResetPhonicCallFlags();
    Gsm.ReadPointer = 0;
    Gsm.UartArrayPointer = &Gsm.GSM_Data_Array[0];
    
    Gsm.StateSendCmd       = CMD_IDLE;
    Gsm.StateWaitAnswerCmd = CMD_WAIT_IDLE;
    Gsm.UartState          = UART_WAITDATA_STATE;
    
    Gsm.GsmFlag.Bit.CringOccurred = 1;
}
/****************************************************************************/
