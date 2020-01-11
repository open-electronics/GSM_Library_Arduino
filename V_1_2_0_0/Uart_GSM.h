/*********************************************************************
 *
 *       Uart routines to manage the communication with GSM engine
 *
 *********************************************************************
 * FileName:        Uart_GSM.h
 * Revision:        1.0.0
 * Date:            08/05/2016
 * Dependencies:    Arduino.h
 *                  GenericCmd_GSM.h
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

#ifndef _Uart_sw_GSM_H_INCLUDED__
#define _Uart_sw_GSM_H_INCLUDED__

#include "Arduino.h"
#include "GenericCmd_GSM.h"

#define BAUD_2400       0x00
#define BAUD_4800       0x01
#define BAUD_9600       0x02
#define BAUD_19200      0x03
#define BAUD_38400      0x04
#define BAUD_57600      0x05
#define BAUD_115200     0x06
#define BAUD_230400     0x07
#define BAUD_250000     0x08
#define BAUD_500000     0x09
#define BAUD_1000000    0x0A

enum Uart_State {
  UART_IDLE_STATE,
  UART_SENDDATA_STATE,
  UART_WAITDATA_STATE
};

class Uart_GSM : public virtual GenericCmd_GSM {
    
    public:
        Uart_GSM();
        ~Uart_GSM();
        void    ExecuteUartState(void);
        void    UartIdle(void);
        void    UartSendData(void);
        uint8_t UartReceiveData(void);
        void    UartContinuouslyRead(void);
                
        void    EnableDisableDebugSerial(bool Set, uint8_t BaudRateSelector);   //  Enables/Disables and Sets BaudRate for serial interface used to debug code
        void    SetBaudRateUart1(bool Set, uint8_t BaudRateSelector);           //  Enables/Disables and Sets BaudRate for Software Seral interface
        uint8_t WriteData(uint8_t Lenght);                                      //  Send  Data
        void    ClearBuffer(uint8_t * Pointer, uint8_t Lenght);                 //  Clear UART Buffer
    
        uint8_t * UartArrayPointer;
        uint8_t GSM_Data_Array[255];    //  Array of byte used to send and receive data from device
        uint8_t ReadPointer;            //  Pointer for Read process
        uint8_t WritePointer;           //  Pointer for Write process
        uint8_t UartState;              //  This variable define a state of State machine
        
        union UartFlag {
            uint8_t  UartFlagByte;
            struct {
                uint8_t EnableUartSM        :1;     //  "1": UART State Machine Enable
                uint8_t SendData            :1;     //  "1": Start Send Data
                uint8_t ReceivedAnswer      :1;     //  "1": Data received. Ready to process
                uint8_t ReceivedSomething   :1;     //  "1": Received Unsolicited code
                uint8_t Free                :4;
            } Bit;  
        } UartFlag;
    
        #ifdef DEBUG_MODE
        void PrintScreenDebugMode(void);
        bool PrintBufferDebug(uint8_t Data);
        #endif
    private:
    void SelectUartTimeOut(void);
};

extern Uart_GSM Gsm;

#endif