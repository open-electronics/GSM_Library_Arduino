/*********************************************************************
 *
 *       I/O Board configuration
 *
 *********************************************************************
 * FileName:        IO_GSM.cpp
 * Revision:        1.0.0
 * Date:            08/05/2016
 * Dependencies:    SoftwareSerial.h
 *                  GenericCmd_GSM.h
 *                  Isr_GSM.h
 *                  Io_GSM.h
 *                  Uart_GSM.h
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
#include "Isr_GSM.h"
#include "Io_GSM.h"
#include "Uart_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/****************************************************************************
 * Function:        SetOutputLed
 *
 * Overview:        This function sets the hardware I/O to manage the LEDs connected
 *                  to ArduinoMega 2560 and GSM Board Rev. 1.2 
 *
 * PreCondition:    This function need the hardware R.1.2 and Arduino Mega 2560 or Fishino Mega 2560
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
void Io_GSM::SetOutputLed(void) {   
#if defined(HARDWARE_12) || defined(HARDWARE_13)
    #ifdef ARDUINO_MEGA2560_REV3
        pinMode(PIN_LED4, OUTPUT);
        pinMode(PIN_LED5, OUTPUT);
        pinMode(PIN_LED6, OUTPUT);
        pinMode(PIN_LED7, OUTPUT);
        pinMode(PIN_LED8, OUTPUT);
        pinMode(PIN_LED9, OUTPUT);
        
        digitalWrite(PIN_LED4, LOW);
        digitalWrite(PIN_LED5, LOW);
        digitalWrite(PIN_LED6, LOW);
        digitalWrite(PIN_LED7, LOW);
        digitalWrite(PIN_LED8, LOW);
        digitalWrite(PIN_LED9, LOW);
    #endif
#endif  
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetOutputTrigger
 *
 * Overview:        This function sets the hardware I/O to manage the TRIGGERS
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
void Io_GSM::SetOutputTrigger(void) {
    pinMode(TRIGGER_1, OUTPUT);
    pinMode(TRIGGER_2, OUTPUT);
    pinMode(TRIGGER_3, OUTPUT);
    
    digitalWrite(TRIGGER_1, LOW);
    digitalWrite(TRIGGER_2, LOW);
    digitalWrite(TRIGGER_3, LOW);
}
/****************************************************************************/

/****************************************************************************
 * Function:        CheckOutputLed
 *
 * Overview:        This function is used to check the led functionality
 *
 * PreCondition:    This function need the hardware R.1.2 and Arduino Mega 2560 or Fishino Mega 2560
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
void Io_GSM::CheckOutputLed(void) {
    uint8_t Count = 0;
#if defined(HARDWARE_12) || defined(HARDWARE_13)
    #ifdef ARDUINO_MEGA2560_REV3    
        do {
            digitalWrite((PIN_LED9 + Count), HIGH);
            delay(250);
        } while (++Count < 6);
        delay(1000);
        Count = 0;
        do {
            digitalWrite((PIN_LED9 + Count), LOW);
            delay(250);
        } while (++Count < 6);
    #endif
#endif  
}
/****************************************************************************/
    
/****************************************************************************
 * Function:        CheckOutputTrigger
 *
 * Overview:        This function is used to check the trigger functionality
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
void Io_GSM::CheckOutputTrigger(void) {
    uint8_t Count = 0;
    
    do {
        digitalWrite((TRIGGER_1 + Count), HIGH);
        delay(250);
    } while (++Count < 3);
    delay(1000);
    Count = 0;
    do {
        digitalWrite((TRIGGER_1 + Count), LOW);
        delay(250);
    } while (++Count < 3);  
}
/****************************************************************************/

/****************************************************************************
 * Functions:       LedOn; LedOff; TriggerOn; TriggerOff
 *
 * Overview:        These functions are used to SET/RESET led and trigger output
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           LedSelected: This parameter identify the I/O where the led is connected
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
void Io_GSM::LedOn(uint8_t LedSelected) {
#if defined(HARDWARE_12) || defined(HARDWARE_13)
    #ifdef ARDUINO_MEGA2560_REV3
        digitalWrite(LedSelected, HIGH);
    #endif
#endif
}
void Io_GSM::LedOff(uint8_t LedSelected) {
#if defined(HARDWARE_12) || defined(HARDWARE_13)
    #ifdef ARDUINO_MEGA2560_REV3
        digitalWrite(LedSelected, LOW);
    #endif
#endif          
}
void Io_GSM::TriggerOn(uint8_t TriggerSelected) {
    digitalWrite(TriggerSelected, HIGH);
}
void Io_GSM::TriggerOff(uint8_t TriggerSelected) {
    digitalWrite(TriggerSelected, LOW);
}
/****************************************************************************/

/****************************************************************************
 * Functions:       LedBlink; LedBlinkAction
 *
 * Overview:        These functions are used to blink the selected led
 *
 * PreCondition:    For the leds 4 to 9 is necessary the hardware R.1.2 and Arduino Mega 2560 or Fishino Mega 2560
 *
 * GSM cmd syntax:  None    
 *
 * Input:           LedSelected:     This parameter identify the I/O where the led is connected
 *                  DutyCycle:       This parameter identify the dutycycle of the led during blink
 *                                   section. This parameter depend of the TimeOutConstant parameter
 *                  TimeOutConstant: This parameter identify the time constant to use for led blinking
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
void Io_GSM::LedBlink(uint8_t LedSelected, uint8_t DutyCycle, uint16_t TimeOutConstant) {
    uint16_t t_ON = 0;
    
    t_ON = TimeOutConstant - (TimeOutConstant * DutyCycle) / 100;
    
    switch (LedSelected)
    {
#if defined(HARDWARE_12) || defined(HARDWARE_13)
    #ifdef ARDUINO_MEGA2560_REV3    
        case PIN_LED4:
            Isr.TimeOutBlinkLed4 = LedBlinkAction(PIN_LED4, t_ON, Isr.TimeOutBlinkLed4, TimeOutConstant);
            break;
        case PIN_LED5:
            Isr.TimeOutBlinkLed5 = LedBlinkAction(PIN_LED5, t_ON, Isr.TimeOutBlinkLed5, TimeOutConstant);
            break;
        case PIN_LED6:
            Isr.TimeOutBlinkLed6 = LedBlinkAction(PIN_LED6, t_ON, Isr.TimeOutBlinkLed6, TimeOutConstant);
            break;
        case PIN_LED7:
            Isr.TimeOutBlinkLed7 = LedBlinkAction(PIN_LED7, t_ON, Isr.TimeOutBlinkLed7, TimeOutConstant);
            break;
        case PIN_LED8:
            Isr.TimeOutBlinkLed8 = LedBlinkAction(PIN_LED8, t_ON, Isr.TimeOutBlinkLed8, TimeOutConstant);
            break;
        case PIN_LED9:
            Isr.TimeOutBlinkLed9 = LedBlinkAction(PIN_LED9, t_ON, Isr.TimeOutBlinkLed9, TimeOutConstant);
            break;
    #endif
#endif              
        case TRIGGER_1:
            Isr.TimeOutBlinkTrigger1 = LedBlinkAction(TRIGGER_1, t_ON, Isr.TimeOutBlinkTrigger1, TimeOutConstant);
            break;          
        case TRIGGER_2:
            Isr.TimeOutBlinkTrigger2 = LedBlinkAction(TRIGGER_2, t_ON, Isr.TimeOutBlinkTrigger2, TimeOutConstant);
            break;          
        case TRIGGER_3:
            Isr.TimeOutBlinkTrigger3 = LedBlinkAction(TRIGGER_3, t_ON, Isr.TimeOutBlinkTrigger3, TimeOutConstant);
            break;                      
        default:
            break;
    }
}

uint16_t Io_GSM::LedBlinkAction(uint8_t LedSelected, uint16_t t_ON, uint16_t Timer, uint16_t TimeOutConstant) {
    if (Timer >= t_ON) {
        digitalWrite(LedSelected, HIGH);
    } else {
        digitalWrite(LedSelected, LOW);
    }
    if (Timer == 0) {
        Timer = TimeOutConstant;        //  Load led blink frequency
    }
    return (Timer);
}
/****************************************************************************/
