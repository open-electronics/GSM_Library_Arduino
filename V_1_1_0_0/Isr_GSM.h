/*********************************************************************
 *
 *       Internet service routine
 *
 *********************************************************************
 * FileName:        Isr_GSM.h
 * Revision:        1.0.0
 * Date:            08/05/2016
 * Dependencies:    Arduino.h
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

#ifndef _Isr_GSM_H_INCLUDED__
#define _Isr_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//=======================================================================
#define TIMEBASE 2 

#define T_10MSEC    (10/TIMEBASE)     //  Defines a constant for a timeout of 10 mSec
#define T_20MSEC    (20/TIMEBASE)     //  Defines a constant for a timeout of 20 mSec
#define T_50MSEC    (50/TIMEBASE)     //  Defines a constant for a timeout of 50 mSec
#define T_100MSEC   (100/TIMEBASE)    //  Defines a constant for a timeout of 100 mSec
#define T_200MSEC   (200/TIMEBASE)    //  Defines a constant for a timeout of 100 mSec
#define T_250MSEC   (250/TIMEBASE)    //  Defines a constant for a timeout of 250 mSec
#define T_300MSEC   (300/TIMEBASE)    //  Defines a constant for a timeout of 250 mSec
#define T_500MSEC   (500/TIMEBASE)    //  Defines a constant for a timeout of 500 mSec
#define T_750MSEC   (750/TIMEBASE)    //  Defines a constant for a timeout of 750 mSec

#define T_1SEC      (1000/TIMEBASE)   //  Defines a constant for a timeout of 1,000  Sec
#define T_2SEC      (2000/TIMEBASE)   //  Defines a constant for a timeout of 2,000  Sec
#define T_2_500SEC  (2500/TIMEBASE)   //  Defines a constant for a timeout of 2,500  Sec
#define T_3SEC      (3000/TIMEBASE)   //  Defines a constant for a timeout of 3,000  Sec
#define T_4SEC      (4000/TIMEBASE)   //  Defines a constant for a timeout of 4,000  Sec
#define T_4_500SEC  (4500/TIMEBASE)   //  Defines a constant for a timeout of 4,000  Sec
#define T_4_750SEC  (4750/TIMEBASE)   //  Defines a constant for a timeout of 4,750  Sec
#define T_4_900SEC  (4900/TIMEBASE)   //  Defines a constant for a timeout of 4,900  Sec
#define T_4_950SEC  (4950/TIMEBASE)   //  Defines a constant for a timeout of 4,950  Sec
#define T_5SEC      (5000/TIMEBASE)   //  Defines a constant for a timeout of 5,000  Sec
#define T_7_500SEC  (7500/TIMEBASE)   //  Defines a constant for a timeout of 5,000  Sec

#define T_10SEC     (10000/TIMEBASE)  //  Defines a constant for a timeout of 10,000  Sec
#define T_11SEC     (11000/TIMEBASE)  //  Defines a constant for a timeout of 11,000  Sec
#define T_12SEC     (12000/TIMEBASE)  //  Defines a constant for a timeout of 12,000  Sec
#define T_13SEC     (13000/TIMEBASE)  //  Defines a constant for a timeout of 13,000  Sec
#define T_14SEC     (14000/TIMEBASE)  //  Defines a constant for a timeout of 14,000  Sec
#define T_15SEC     (15000/TIMEBASE)  //  Defines a constant for a timeout of 15,000  Sec
#define T_20SEC     (20000/TIMEBASE)  //  Defines a constant for a timeout of 20,000  Sec
#define T_25SEC     (25000/TIMEBASE)  //  Defines a constant for a timeout of 25,000  Sec
#define T_30SEC     (30000/TIMEBASE)  //  Defines a constant for a timeout of 30,000  Sec
#define T_35SEC     (35000/TIMEBASE)  //  Defines a constant for a timeout of 35,000  Sec
#define T_40SEC     (40000/TIMEBASE)  //  Defines a constant for a timeout of 40,000  Sec
#define T_45SEC     (45000/TIMEBASE)  //  Defines a constant for a timeout of 45,000  Sec
#define T_50SEC     (50000/TIMEBASE)  //  Defines a constant for a timeout of 50,000  Sec
#define T_55SEC     (55000/TIMEBASE)  //  Defines a constant for a timeout of 55,000  Sec
#define T_60SEC     (60000/TIMEBASE)  //  Defines a constant for a timeout of 60,000  Sec
#define T_70SEC     (70000/TIMEBASE)  //  Defines a constant for a timeout of 70,000  Sec
#define T_80SEC     (80000/TIMEBASE)  //  Defines a constant for a timeout of 80,000  Sec
#define T_90SEC     (90000/TIMEBASE)  //  Defines a constant for a timeout of 90,000  Sec
#define T_100SEC    (100000/TIMEBASE) //  Defines a constant for a timeout of 100,000  Sec
#define T_110SEC    (110000/TIMEBASE) //  Defines a constant for a timeout of 110,000  Sec
#define T_120SEC    (120000/TIMEBASE) //  Defines a constant for a timeout of 120,000  Sec
#define T_130SEC    (130000/TIMEBASE) //  Defines a constant for a timeout of 130,000  Sec
//=======================================================================

//=======================================================================
#define T_250MS_UART_TIMEOUT    0
#define T_1S_UART_TIMEOUT       1
#define T_2S5_UART_TIMEOUT      2
#define T_5S_UART_TIMEOUT       3
#define T_7S5_UART_TIMEOUT      4
#define T_10S_UART_TIMEOUT      5
#define T_15S_UART_TIMEOUT      6
#define T_20S_UART_TIMEOUT      7
//=======================================================================

//=======================================================================
#define Fosc          (unsigned long)16000000    //  Clock 16MHz
#define PRE           (unsigned long)256         //  Prescaler 256
#define MSEC          2                          //  Time Base: 2mSec
#define SLOWBASETIME  (0xFFFF - ((Fosc * MSEC) / (PRE * 1000)))
//=======================================================================

class Isr_GSM {
    public:
        uint16_t TimeOutBlinkLed4;          //  TimeOut blink led 4
        uint16_t TimeOutBlinkLed5;          //  TimeOut blink led 4
        uint16_t TimeOutBlinkLed6;          //  TimeOut blink led 4
        uint16_t TimeOutBlinkLed7;          //  TimeOut blink led 4
        uint16_t TimeOutBlinkLed8;          //  TimeOut blink led 4
        uint16_t TimeOutBlinkLed9;          //  TimeOut blink led 4
        uint16_t TimeOutBlinkTrigger1;      //  TimeOut blink Trigger 1
        uint16_t TimeOutBlinkTrigger2;      //  TimeOut blink Trigger 2
        uint16_t TimeOutBlinkTrigger3;      //  TimeOut blink Trigger 3
        
        uint16_t TimeOutWait;               //  Generic TimeOut
        uint16_t UartTimeOut;               //  Serial TimeOut                      
        uint16_t UartTestTimeOut;
        
        void EnableLibInterrupt(void);      //  Enable Library Interrupt
        
    private:
        void EnableTimerInterrupt(void);    //  Enable and set TIMER1 interrupt (TIMR1)
        void EnableCringInterrupt(void);    //  Enable CRING interrupt (INT0)
};

extern Isr_GSM Isr;

#endif