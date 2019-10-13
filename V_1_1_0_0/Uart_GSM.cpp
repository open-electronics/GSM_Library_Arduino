/*********************************************************************
 *
 *       Uart routines to manage the communication with GSM engine
 *
 *********************************************************************
 * FileName:        Uart_GSM.cpp
 * Revision:        1.0.0
 * Date:            08/05/2016
 *
 * Revision:        1.1.0
 *                  25/11/2018
 *                  - Fixed bug in function "UartContinuouslyRead". Communication timeout not used. This bug could stop the UART communication
 *
 * Dependencies:    SoftwareSerial.h
 *                  Io_GSM.h
 *                  Isr_GSM.h
 *                  Uart_GSM.h
 *                  PhoneBookCmd_GSM.h
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

#include "Io_GSM.h"
#include "Isr_GSM.h"
#include "Uart_GSM.h"
#include "GenericCmd_GSM.h"
#include "PhoneBookCmd_GSM.h"
#include "GprsCmd_GSM.h"
#include "HttpCmd_GSM.h"

#ifdef SOFTWARE_UART1
    #include <SoftwareSerial.h>
#endif

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//========================================================================================================================
// NOTE: If overflow occurred increase the "_SS_MAX_RX_BUFF" buffer lenght in the "SoftwareSerial.h" file.
//       The change for software serial ports require a simple modification of the file:
//
//       <base Arduino folder>\hardware\arduino\avr\libraries\SoftwareSerial\src\SoftwareSerial.h
//
// For example:
//
// Change -> __#define _SS_MAX_RX_BUFF 64
//
// With   -> __#define _SS_MAX_RX_BUFF 128 or 256
//
//
// NOTE: If hardware serial port is used and during trasmission an overflow occurred then the value of RX buffer
//       must be changed. The parameter is located into file "HardwareSerial.h"
//       The file is stored in this path:
//
//       <base Arduino folder>\hardware\arduino\avr\cores\arduino
//
// For example:
//
// Change -> __#define SERIAL_RX_BUFFER_SIZE 64
//
// With   -> __#define SERIAL_RX_BUFFER_SIZE 128 or 256
//
//========================================================================================================================

#ifdef SOFTWARE_UART1
    SoftwareSerial Serial1_SW = SoftwareSerial(PIN_RX1_SW, PIN_TX1_SW);     //  Software serial for SIM900/M95/G510
#endif

Uart_GSM Gsm;
Uart_GSM::Uart_GSM(){};
Uart_GSM::~Uart_GSM(){};

/****************************************************************************
 * Function:        EnableDisableDebugSerial
 *
 * Overview:        This routine is used to Enable/Disable the serial port for debug
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None    
 *
 * Input:           Set: If "true" enable the serial debug, if "false" disable debug
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
void Uart_GSM::EnableDisableDebugSerial(bool Set, uint8_t BaudRateSelector) {
    if (Set == true) {
        switch (BaudRateSelector)
        {
            case BAUD_9600:
                Serial.begin(9600);     //  Debug Serial
                break;
            case BAUD_19200:
                Serial.begin(19200);    //  Debug Serial
                break;
            case BAUD_38400:
                Serial.begin(38400);    //  Debug Serial
                break;
            case BAUD_57600:
                Serial.begin(57600);    //  Debug Serial
                break;
            case BAUD_115200:
                Serial.begin(115200);   //  Debug Serial
                break;  
            case BAUD_230400:
                Serial.begin(230400);   //  Debug Serial
                break;
            case BAUD_250000:
                Serial.begin(250000);   //  Debug Serial
                break;
            case BAUD_500000:
                Serial.begin(500000);   //  Debug Serial
                break;
            case BAUD_1000000:
                Serial.begin(1000000);  //  Debug Serial
                break;
            default:
                Serial.begin(57600);    //  Debug Serial
                break;          
        }
    } else {
        Serial.end();           
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        SetBaudRateUart1
 *
 * Overview:        This routine is used to Initialize the Serial port (Software mode)
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           BaudRateSelector: Bauderate code. Use defined directive
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
void Uart_GSM::SetBaudRateUart1(bool Set, uint8_t BaudRateSelector) {
    if (Set == true) {
        switch (BaudRateSelector)
        {
            case BAUD_2400:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(2400);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(2400);
                #endif
                break;
            case BAUD_4800:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(4800);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(4800);
                #endif
                break;
            case BAUD_9600:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(9600);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(9600);
                #endif
                break;
            case BAUD_19200:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(19200);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(19200);
                #endif
                break;
            case BAUD_38400:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(38400);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(38400);
                #endif
                break;
            case BAUD_57600:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(57600);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(57600);
                #endif
                break;
            case BAUD_115200:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(115200);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(115200);
                #endif
                break;  
            default:
                #ifdef SOFTWARE_UART1
                    Serial1_SW.begin(115200);
                #endif
                #ifdef HARDWARE_UART1
                    Serial1.begin(115200);
                #endif
                break;
        }       
    } else {
#ifdef SOFTWARE_UART1
        Serial1_SW.end();
#endif
#ifdef HARDWARE_UART1
        Serial1.end();
#endif      
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        ExecuteUartState
 *
 * Overview:        This function process the state of the Uart State Machine.
 *                  There are three state: IDLE, SEND DATA and WAIT DATA.
 *                  Normally the system is in the "Idle Mode" and if necessary the
 *                  system switch in "Send Data Mode" to send data at GSM engine or switch in 
 *                  "Wait Data Mode" to receive data from GSM engine
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
void Uart_GSM::ExecuteUartState(void) {
    if (UartFlag.Bit.EnableUartSM == 1) {
        switch (Gsm.UartState) 
        {
            case UART_IDLE_STATE:
                UartIdle();
                break;      
            case UART_SENDDATA_STATE:
#ifdef UART_DEBUG
                //  The Print Screen debug mode use the "Gsm.TempStringCompare" array. Be carefully
                Gsm.ReadStringFLASH((uint8_t *)STR_UART_SEND_DATA_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_SEND_DATA_DEBUG));
                Gsm.PrintScreenDebugMode();
#endif
                UartSendData();
                break;  
            case UART_WAITDATA_STATE:
#ifdef UART_DEBUG
                //  The Print Screen debug mode use the "Gsm.TempStringCompare" array. Be carefully
                Gsm.ReadStringFLASH((uint8_t *)STR_UART_WAIT_DATA_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_WAIT_DATA_DEBUG));
                Gsm.PrintScreenDebugMode();
#endif
                UartReceiveData();
                Gsm.UartFlag.Bit.ReceivedAnswer = 1;
                Gsm.UartState = UART_IDLE_STATE;
#ifdef UART_DEBUG
                //  The Print Screen debug mode use the "Gsm.TempStringCompare" array. Be carefully
                Gsm.ReadStringFLASH((uint8_t *)STR_UART_IDLE_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_IDLE_DEBUG));
                Gsm.PrintScreenDebugMode();
#endif
                break;
        }
    } else {
        Gsm.UartState = UART_IDLE_STATE;
    }   
}
/****************************************************************************/

/****************************************************************************
 * Function:        UartIdle
 *
 * Overview:        Uart Idle Mode state
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
void Uart_GSM::UartIdle(void) {
    if ((UartFlag.Bit.SendData == 1) && (Gsm.WritePointer > 0)) {
        Gsm.UartState = UART_SENDDATA_STATE;
    }
}
/****************************************************************************/

/****************************************************************************
 * Function:        UartSendData
 *
 * Overview:        Uart Send Data Mode state
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
void Uart_GSM::UartSendData(void) {
    WriteData(Gsm.WritePointer);
    Gsm.WritePointer = 0;
    Gsm.ReadPointer  = 0;
    Gsm.UartFlag.Bit.SendData = 0;
    Gsm.UartState = UART_WAITDATA_STATE;
}
/****************************************************************************/

/****************************************************************************
 * Function:        WriteData
 *
 * Overview:        This function write data on the serial port
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return the number of byte wrote on the serial port 
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t Uart_GSM::WriteData(uint8_t Lenght) {
    uint8_t  Counter;
    
#ifdef GSM_AT_CMD_DEBUG
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SEP1, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SEP1));
    Gsm.PrintScreenDebugMode();
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SENT_DATA_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SENT_DATA_DEBUG));
    Gsm.PrintScreenDebugMode();
#endif
    UartArrayPointer = &Gsm.GSM_Data_Array[0];
    Counter = 0;
    do {
#ifdef SOFTWARE_UART1
        Serial1_SW.write(*(uint8_t *)UartArrayPointer);
#endif
#ifdef HARDWARE_UART1
        Serial1.write(*(uint8_t *)UartArrayPointer);
#endif
#ifdef GSM_AT_CMD_DEBUG
        if (Gsm.PrintBufferDebug((*(uint8_t *)UartArrayPointer)) == false) {
            break;
        }
#endif
        *UartArrayPointer++ = 0;
    } while (Counter++ < (Lenght - 1)); 
#ifdef GSM_AT_CMD_DEBUG 
    Serial.print("\n");
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SEP2, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SEP2));
    Gsm.PrintScreenDebugMode();
    Serial.print("\n");
#endif
    return(Counter);
}
/****************************************************************************/

/****************************************************************************
 * Function:        UartReceiveData
 *
 * Overview:        This function received data from serial port
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          Return the number of byte wrote on the serial port 
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t Uart_GSM::UartReceiveData(void) {
    char c = 0;

    SelectUartTimeOut();
    Gsm.ReadPointer = 0;
    Gsm.UartArrayPointer = &Gsm.GSM_Data_Array[0];
#ifdef SOFTWARE_UART1
    while (Serial1_SW.available() || (Isr.UartTimeOut > 0)) {
    #ifdef UART_DEBUG
        if (Serial1_SW.overflow()) {
            Gsm.ReadStringFLASH((uint8_t *)STR_UART_BUFFER_OVERFLOW_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_UART_BUFFER_OVERFLOW_DEBUG));
            Gsm.PrintScreenDebugMode();         
        }
    #endif
        c = Serial1_SW.read();
        if (c != -1) {
            if (Gsm.ReadPointer < (sizeof(Gsm.GSM_Data_Array) - 1)) {
                *(uint8_t *)Gsm.UartArrayPointer++ = c;
                *(uint8_t *)Gsm.UartArrayPointer   = ASCII_NULL;
                Gsm.ReadPointer++;
            }   
            SelectUartTimeOut();
        } else {
            if ((Gsm.GsmFlag.Bit.GsmReset != 1) && (Gsm.GsmFlag.Bit.GsmPowerOn != 1) && (Gsm.GsmFlag.Bit.GsmPowerOff != 1)) {
                if (Isr.UartTimeOut < Isr.UartTestTimeOut) {
                    if ((Gsm.GSM_Data_Array[Gsm.ReadPointer - 1] == ASCII_LINE_FEED) && \
                        (Gsm.GSM_Data_Array[Gsm.ReadPointer - 2] == ASCII_CARRIAGE_RET)) {
                        break;
                    }
                }
            }
        }
    }
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut
#endif
#ifdef HARDWARE_UART1
    while (Serial1.available() || (Isr.UartTimeOut > 0)) {
        c = Serial1.read();
        if (c != -1) {
            if (Gsm.ReadPointer < (sizeof(Gsm.GSM_Data_Array) - 1)) {
                *(uint8_t *)Gsm.UartArrayPointer++ = c;
                *(uint8_t *)Gsm.UartArrayPointer = ASCII_NULL;
                Gsm.ReadPointer++;
            }
            SelectUartTimeOut();
        } else {
            if ((Gsm.GsmFlag.Bit.GsmReset != 1) && (Gsm.GsmFlag.Bit.GsmPowerOn != 1) && (Gsm.GsmFlag.Bit.GsmPowerOff != 1)) {
                if (Isr.UartTimeOut < Isr.UartTestTimeOut) {
                    if ((Gsm.GSM_Data_Array[Gsm.ReadPointer - 1] == ASCII_LINE_FEED) && \
                        (Gsm.GSM_Data_Array[Gsm.ReadPointer - 2] == ASCII_CARRIAGE_RET)) {
                        break;
                    }
                }
            }
        }
    }
    Gsm.GsmFlag.Bit.UartTimeOutSelect = T_250MS_UART_TIMEOUT;   // Default Uart TimeOut
#endif
#ifdef GSM_AT_CMD_DEBUG
    uint8_t Count = 0;

    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SEP1, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SEP1));
    Gsm.PrintScreenDebugMode();
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_RECEIVED_DATA_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_RECEIVED_DATA_DEBUG));
    Gsm.PrintScreenDebugMode();
    Count = 0;
    do {
        if (Gsm.PrintBufferDebug(Gsm.GSM_Data_Array[Count]) == false) {
            break;
        }                   
    } while (Count++ < (sizeof(Gsm.GSM_Data_Array) - 1));

    Serial.print("\n");
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_RECEIVED_CHAR_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_RECEIVED_CHAR_DEBUG));
    Gsm.PrintScreenDebugMode();     
    Serial.print(Gsm.ReadPointer);
    Serial.print("\n");
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_SEP2, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_SEP2));
    Gsm.PrintScreenDebugMode();
    Serial.print("\n");
#endif
}

void Uart_GSM::SelectUartTimeOut(void) {
    switch (Gsm.GsmFlag.Bit.UartTimeOutSelect)
    {
        case T_250MS_UART_TIMEOUT:
            Isr.UartTimeOut = T_250MSEC;
            break;
        case T_1S_UART_TIMEOUT:
            Isr.UartTimeOut = T_1SEC;
            break;
        case T_2S5_UART_TIMEOUT:
            Isr.UartTimeOut = T_2_500SEC;
            break;
        case T_5S_UART_TIMEOUT:
            Isr.UartTimeOut = T_5SEC;
            break;
        case T_7S5_UART_TIMEOUT:
            Isr.UartTimeOut = T_7_500SEC;
            break;
        case T_10S_UART_TIMEOUT:
            Isr.UartTimeOut = T_10SEC;
            break;
        case T_15S_UART_TIMEOUT:
            Isr.UartTimeOut = T_15SEC;
            break;
        case T_20S_UART_TIMEOUT:
            Isr.UartTimeOut = T_20SEC;
            break;
        default:
            break;
    }
    Isr.UartTestTimeOut = (Isr.UartTimeOut / 5);        //  20% of Isr.UartTimeOut
}
/****************************************************************************/

/****************************************************************************
 * Function:        UartContinuouslyRead
 *
 * Overview:        This function is used to intercept an Unsolicited Result Code
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
void Uart_GSM::UartContinuouslyRead(void) {
    uint8_t Count = 0;
    char    c = 0;
    
    if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 1) || (Gsm.GsmFlag.Bit.CringOccurred == 1)) {
        return;
    }
#ifdef SOFTWARE_UART1
    if (!Serial1_SW.available()) {
        return;
    }
    Gsm.UartFlag.Bit.ReceivedSomething = 1;
    Gsm.ReadPointer = 0;
    Gsm.UartArrayPointer = &Gsm.GSM_Data_Array[0];  
    Isr.UartTimeOut = T_1SEC;
    while ((Serial1_SW.available() > 0) || (Isr.UartTimeOut > 0)) {
        c = Serial1_SW.read();
        if (c != -1) {
            if (Gsm.ReadPointer < (sizeof(Gsm.GSM_Data_Array) - 1)) {
                *(uint8_t *)Gsm.UartArrayPointer++ = c;
                *(uint8_t *)Gsm.UartArrayPointer   = ASCII_NULL;
                Gsm.ReadPointer++;
            }
        }
    }   
#endif
#ifdef HARDWARE_UART1
    if (!Serial1.available()) {
        return;
    }
    Gsm.UartFlag.Bit.ReceivedSomething = 1;
    Gsm.ReadPointer = 0;
    Gsm.UartArrayPointer = &Gsm.GSM_Data_Array[0];
    Isr.UartTimeOut = T_1SEC;
    while ((Serial1.available() > 0) || (Isr.UartTimeOut > 0)) {
        c = Serial1.read();
        if (c != -1) {
            if (Gsm.ReadPointer < (sizeof(Gsm.GSM_Data_Array) - 1)) {
                *(uint8_t *)Gsm.UartArrayPointer++ = c;
                *(uint8_t *)Gsm.UartArrayPointer   = ASCII_NULL;
                Gsm.ReadPointer++;
            }
        }
    }
#endif  
#ifdef GSM_AT_CMD_DEBUG
    Gsm.ReadStringFLASH((uint8_t *)STR_GSM_UNSOLICITED_DEBUG, (uint8_t *)Gsm.TempStringCompare, strlen(STR_GSM_UNSOLICITED_DEBUG));
    Gsm.PrintScreenDebugMode();
    Count = 0;
    do {
        if (Gsm.PrintBufferDebug(Gsm.GSM_Data_Array[Count]) == false) {
            break;
        }                   
    } while (Count++ < (sizeof(Gsm.GSM_Data_Array) - 1));
    Serial.print("\n");
#endif
}
/****************************************************************************/

/****************************************************************************
 * Function:        PrintBufferDebug
 *
 * Overview:        This function is used to print on serial monitor the data received from serial port
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           Data: Data to print
 *
 * Command Note:    None
 *
 * Output:          bool: "true" data print success, "false" data print failed
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
#ifdef DEBUG_MODE
bool Uart_GSM::PrintBufferDebug(uint8_t Data) {
    if ((Data >= ASCII_SPACE) && (Data < ASCII_DEL)) {
        Serial.print(char(Data));
    } else {
        if (Data == ASCII_LINE_FEED) {
            Serial.print("<LF>");
        } else if (Data == ASCII_CARRIAGE_RET) {
            Serial.print("<CR>");
        } else if (Data == ASCII_SUB) {
            Serial.print("<SUB>");
        } else if (Data == ASCII_ESC) {
            Serial.print("<ESC>");
        } else {
            Serial.print(char(ASCII_SPACE));
            return(false);
        }
    }
    return(true);
}
#endif
/****************************************************************************/

/****************************************************************************
 * Function:        ClearBuffer
 *
 * Overview:        This function is used to clear UART buffer
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
void Uart_GSM::ClearBuffer(uint8_t * Pointer, uint8_t Lenght) {
    uint8_t Counter = 0;

    do {        
        *(uint8_t *)Pointer++ = ASCII_NULL;
    } while (Counter++ < Lenght);
}
/****************************************************************************/

/****************************************************************************
 * Function:        PrintScreenDebugMode
 *
 * Overview:        This function is used to print on serial monitor the strings
 *                  in debug mode. The strings are usefully for code debug and
 *                  are stored in FLASH memory 
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
#ifdef DEBUG_MODE
void Uart_GSM::PrintScreenDebugMode(void) {
    uint8_t *Pointer;
    uint8_t  Counter;
    char     c;
    
    Pointer = &Gsm.TempStringCompare[0];
    Counter = 0;
    do {
        if (*(uint8_t *)Pointer != 0) {
            if (((*(uint8_t *)Pointer == ASCII_CARRIAGE_RET)) || ((*(uint8_t *)Pointer == ASCII_NULL))) {
                break;
            }           
            Serial.print(char(*(uint8_t *)Pointer));
            *Pointer++ = 0;
        } else {
            break;
        }
    } while (Counter++ < (sizeof(Gsm.TempStringCompare) - 1));
}
#endif
/****************************************************************************/
