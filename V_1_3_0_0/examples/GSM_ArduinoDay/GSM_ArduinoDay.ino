/*************************************************************************************************************
#  HISTORY
#  v1.0  -  First Release
#
#  =============================================================================
#  This is an example developed for the Arduino Day 2018 and shows how to use
#  the new Arduino GSM library. It is necessary the ArduinoMega 2560 board
#
#  Developed by Matteo Destro for Futura Group srl
#
#  www.Futurashop.it
#  www.open-electronics.org
#
#  BSD license, all text above must be included in any redistribution
#
#  The code for this demo is splitted into six files:
#
#  - GSM_ArduinoDay  -> Main file project
#  - DigitalInput    -> Contains code to manage digital inputs and the respective functions to manage it
#  - DigitalOutput   -> Contains code to manage digital output and the respective functions to manage it
#  - TimerInt        -> Contains code to manage Timer5 of ATMEGA2560. Timer5 generates an interrupt every 2 mSec.
#                       With this resolution (2 mSec) it is possible to manage the timer variables used in this demo.
#                       For example the TimeOut used to filter the unwanted variations on the digital inputs (debouncing)
#
#  ArduinoMega2560 Rev. 3
#   
#  =============================================================================                    
#  
#  INSTALLATION
#
#  SUPPORT
#  
#  info@open-electronics.org
#
*********************************************************************************************************/

#include <avr/eeprom.h>
#include "Uart_GSM.h"
#include "Io_GSM.h"
#include "Isr_GSM.h"
#include "GenericCmd_GSM.h"
#include "SecurityCmd_GSM.h"
#include "PhoneBookCmd_GSM.h"
#include "SmsCmd_GSM.h"
#include "PhonicCallCmd_GSM.h"
#include "HttpCmd_GSM.h"
#include "GprsCmd_GSM.h"

SecurityCmd_GSM   Security;
PhoneBookCmd_GSM  PhoneBook;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
Isr_GSM           Isr;
Io_GSM            Io;
HttpCmd_GSM       Http;
GprsCmd_GSM       Gprs;

//======================================================================
const char SMS_PHONE_NUMBER_TEST[]  PROGMEM = "\"+393491544808\"";
const char SMS_TEST[]               PROGMEM = "Arduino Day 2018. Test di invio SMS. Ciao come va?";

const char CALL_PHONE_NUMBER_TEST[] PROGMEM = "+393491544808";
//======================================================================

//======================================================================
const char CMD_RECEIVED[]           PROGMEM = "# Command received by user -> ";
const char CMD_ERROR[]              PROGMEM = "# Command syntax error. Retry";

const char SMS_RECEIVED[]           PROGMEM = "Start process SMS received";

const char CMD_CALL[]               PROGMEM = "CmdCall";
const char CMD_SMS[]                PROGMEM = "CmdSms";
const char CMD_ERASE_SMS[]          PROGMEM = "CmdEraseSms";

const char LED9_ON[]                PROGMEM = "Led9On";
const char LED9_OFF[]               PROGMEM = "Led9Off";
const char LED9_BLINK[]             PROGMEM = "Led9Blink";
//======================================================================

#define TRUE  0
#define FALSE 1

#define SEND_SMS            0
#define SEND_GENERIC_SMS    1
#define START_CALL          2
#define START_GENERIC_CALL  3
#define STOP_CALL           4
#define NOTHING             255

#define CMD_AT_CPAS         0
#define CMD_ATQ_CSQ         1
#define CMD_ATQ_CREG        2
#define CMD_ATQ_CPIN        3
#define CMD_AT_CMGR         4
#define CMD_AT_CMGD         5
#define ERASE_SMS_RECEIVED  6

//======================================================================
//  State Machine defines
typedef void State;
typedef State (*Pstate)();

Pstate Input_Management;    //  States machine used to manage the digital inputs (P3 and P4)
//======================================================================

//======================================================================
//  Variables
uint8_t * EepromAdd;

extern uint8_t PIN1_CODE[0];

uint8_t Counter;
uint8_t StateSendCmd;

uint8_t ActionSelected;

String  SerialInput = "";
String  TempString  = "";
String  SmsReceived = "";
uint8_t StringIndex = 0;
//======================================================================

//======================================================================
//  Timer 0 variables
uint8_t  DebouncingTimeOut;    //  Debouncing TimeOut for digital inputs
uint16_t TimeOutWait;          //  TimeOut to manage pulse pressing
uint16_t TimeOutP3;            //  TimeOut to manage P3 button
uint16_t TimeOutP4;            //  TimeOut to manage P4 button

//======================================================================

//======================================================================
//  I/O Input pin define
uint8_t P3_Button = 9;         //  Digital input. Button P3. See schematic
uint8_t P4_Button = 10;        //  Digital input. Button P4. See schematic
//======================================================================

//======================================================================
//  Variables to manage the input pin (Debouncing)
union  DigInputStatus {
  uint8_t  Input;
  struct {
    uint8_t  P3_Button  : 1; //  Bit 0
    uint8_t  P4_Button  : 1; //  Bit 1
    uint8_t  Free       : 6;
  } In;
} DigInputStatus;

union  DigInputReaded {
  uint8_t  Input;
  struct {
    uint8_t  P3_Button  : 1; //  Bit 0
    uint8_t  P4_Button  : 1; //  Bit 1
    uint8_t  Free       : 6;
  } In;
} DigInputReaded;

uint8_t DigInputVar;
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Input_Management = Input_Idle;       //  Initializes states machine used to manage the digital inputs (P3 and P4)
    
  Gsm.EnableDisableDebugSerial(true);
  SetupTimer5();
  SetInputPin();
  SetOutputPin();
  TestLed_Trigger();
  Isr.EnableLibInterrupt();          // Enables Library Interrupt
  
  Gsm.SetBaudRateUart1(BAUD_57600);  //  Enables and configures the Software UART 1
  Gsm.ClearBuffer();                 //  Clear library UART Buffer
    
  Gsm.InitPowerON_GSM();             //  Start GSM initialization     
  Gsm.UartFlag.Bit.EnableUartSM = 0; // |  
  Gsm.ExecuteUartState();            // |--- Initialization Software Uart State Machine
  Gsm.UartFlag.Bit.EnableUartSM = 1; // |
 
  delay(500);
  Isr.TimeOutWait = T_15SEC;  
  Sms.SmsReceivedIndex  = 0;
  Sms.SmsWriteMemoryAdd = 0;
  
  StateSendCmd   = 0;
  ActionSelected = NOTHING;
}
//======================================================================

//======================================================================
void loop() {
  DebouncingInput();          //  Debouncing digital inputs (P3 and P4)
  Input_Management();         //  State Machine to manage Digital input
  
  ProcessStateMachineGsm();

  ProcessSerialCmd();
  ProcessUserATcmd();
  ProcessGenericATcmd();

  ProcessSmsReceived();
}
//======================================================================


//======================================================================
void ProcessStateMachineGsm(void) {
  Gsm.ExecuteUartState();
  if (Gsm.GsmFlag.Bit.GsmInitInProgress == 1) {
    Gsm.InitGsmSendCmd();
    Gsm.InitGsmWaitAnswer();
  } else {  
    Gsm.UartContinuouslyRead();
    Gsm.ProcessUnsolicitedCode();
    Gsm.GsmAnswerStateProcess();

    Io.LedBlink(PIN_LED9, 25, T_1SEC);
  }  
}
//======================================================================

//======================================================================
//  Function used to send AT command for outgoing phonic call or to send a SMS 
void ProcessUserATcmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    switch (ActionSelected)
    {
      case SEND_SMS:              //  Sends SMS
        Gsm.ReadStringFLASH((uint8_t *)SMS_PHONE_NUMBER_TEST, (uint8_t *)PhoneBook.PhoneNumber, strlen(SMS_PHONE_NUMBER_TEST));
        Gsm.ReadStringFLASH((uint8_t *)SMS_TEST, (uint8_t *)Sms.SmsText, strlen(SMS_TEST));
        Sms.SetCmd_AT_CMGS();
        ActionSelected = NOTHING;
        break;
        
      case SEND_GENERIC_SMS:      //  Sends a Generic SMS to a Generic Phone number
        Sms.SetCmd_AT_CMGS();
        ActionSelected = NOTHING;
        break;
        
      case START_CALL:            //  Start phonic call
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
          Gsm.ReadStringFLASH((uint8_t *)CALL_PHONE_NUMBER_TEST, (uint8_t *)PhoneBook.PhoneNumber, strlen(CALL_PHONE_NUMBER_TEST));
          PhonicCall.SetCmd_ATD();          
        } else if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
          Serial.println("Command aborted. Phonic call yet in progress");
          Serial.println("");
        }
        ActionSelected = NOTHING;
        break;
        
      case START_GENERIC_CALL:    //  Start phonic call to a Generic Phone Number
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 0) {
          PhonicCall.SetCmd_ATD();  
        } else if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
          Serial.println("Command aborted. Phonic call yet in progress");
          Serial.println("");
        }
        ActionSelected = NOTHING;
        break;
        
      case STOP_CALL:
        if (Gsm.SimFlag.Bit.PhoneActivityStatus == 4) {
          PhonicCall.SetCmd_ATH(0);  
        }
        ActionSelected = NOTHING;
        break;
            
      case NOTHING:
        break;  
              
      default:
        break;
    }    
  }  
} 
//======================================================================

//======================================================================
//  Function to send generic command in the loop()
void ProcessGenericATcmd(void) {
  if ((Gsm.GsmFlag.Bit.GsmSendCmdInProgress == 0) && (Gsm.GsmFlag.Bit.GsmInitInProgress == 0)) {
    if (Isr.TimeOutWait == 0) {
      Isr.TimeOutWait = T_1SEC;
      
      switch (StateSendCmd)
      {
        case CMD_AT_CPAS:
          Gsm.SetCmd_AT_CPAS();
          ++StateSendCmd;
          break;
        case CMD_ATQ_CSQ:
          Gsm.SetCmd_ATQ_CSQ();
          ++StateSendCmd;
          break;
        case CMD_ATQ_CREG:
          Gsm.SetCmd_ATQ_CREG();
          ++StateSendCmd;
          break;
        case CMD_ATQ_CPIN:
          Security.SetCmd_ATQ_CPIN();
          ++StateSendCmd;
          break;
        case CMD_AT_CMGR:
          if (Sms.SmsReceivedIndex > 0) {
            Sms.SetCmd_AT_CMGR(Sms.SmsReceivedIndex, STATE_NORMAL);
            ++StateSendCmd;
            break;
          }
          StateSendCmd = 0;
          break;
        case CMD_AT_CMGD:
            Sms.SetCmd_AT_CMGD(Sms.SmsReceivedIndex, DEF_FLAG_NORMAL);
            Sms.SmsReceivedIndex = 0;
            StateSendCmd = 0;
          break;
        case ERASE_SMS_RECEIVED:
          if (Sms.SmsWriteMemoryAdd++ < 30) {
            Sms.SetCmd_AT_CMGD(Sms.SmsWriteMemoryAdd, DEF_FLAG_NORMAL);
          } else {
            Sms.SmsWriteMemoryAdd = 0;
            StateSendCmd = 0;
          }
          break;
        default:
          break;
      }
    }
  }  
}
//======================================================================

//======================================================================
//  Functions to process the commands sent from serial port
//
//  ----------------------------------------------------------
//  How to send a command to execute a phonic call:
//
//  CmdCall: +39349*******
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to send a SMS:
//
//  CmdSms: "+39349*******"#Sms Text, max 160 chars
//
//  Note: In the command to send an SMS it is necessary add a double quotation marks. First and after telephone number
//  ----------------------------------------------------------
//
//  ----------------------------------------------------------
//  How to send a command to erase SMS received
//
//  CmdEraseSms:
//  ----------------------------------------------------------

void ProcessSerialCmd(void) {
  if (Serial.available() > 0) {
    ReadSerialCmd();
    StringIndex = CheckChar(':');
    if (StringIndex > 0) {
      TempString = SerialInput.substring(0, StringIndex);
      if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_CALL, strlen(CMD_CALL), TRUE, TRUE)) == 0) {
        //  Received command "CmdCall: +393491544808"
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        SerialInput.toCharArray(PhoneBook.PhoneNumber, sizeof PhoneBook.PhoneNumber);
        ActionSelected = START_GENERIC_CALL;        
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_SMS, strlen(CMD_SMS), TRUE, TRUE)) == 0) {
        //  Received command "CmdSms: "+393491544808"#Message Text"
        SerialInput.remove(0, StringIndex + 1);
        SerialInput.trim();
        StringIndex = CheckChar('#');
        if (StringIndex > 0) {
          TempString = SerialInput.substring(0, StringIndex);
          TempString.trim();
          TempString.toCharArray(PhoneBook.PhoneNumber, sizeof PhoneBook.PhoneNumber);
          SerialInput.remove(0, StringIndex + 1);
          SerialInput.trim();
          SerialInput.toCharArray(Sms.SmsText, sizeof Sms.SmsText);
          ActionSelected = SEND_GENERIC_SMS;  
        } else {
          LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
          return;        
        }
      } else if (TempString.compareTo(LocalReadStringFLASH((uint8_t *)CMD_ERASE_SMS, strlen(CMD_ERASE_SMS), TRUE, TRUE)) == 0) {
        StateSendCmd = ERASE_SMS_RECEIVED;
      } else {
        LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
        return;        
      }     
    } else {
      LocalReadStringFLASH((uint8_t *)CMD_ERROR, strlen(CMD_ERROR), TRUE, FALSE); 
      return;
    }
  }  
}

void ReadSerialCmd(void) {
  SerialInput = "";
  SerialInput = Serial.readString();
  SerialInput.trim();
  LocalReadStringFLASH((uint8_t *)CMD_RECEIVED, strlen(CMD_RECEIVED), FALSE, FALSE);
  Serial.println(SerialInput);
}

uint8_t CheckChar(char CharToSearch) {
  uint8_t TempIndex = 0;
  TempIndex = SerialInput.indexOf(CharToSearch, 0);
  if (TempIndex == 255) {
    return(0);
  }
  return(TempIndex);
}

String LocalReadStringFLASH(uint8_t *FlashPointer, uint8_t Lenght, boolean PrintCR, boolean NoPrint) {
  uint8_t k;
  char    myChar;
  String  TempString;
  
  for (k = 0; k < Lenght; k++) {
    myChar = pgm_read_byte_near(FlashPointer + k);
    if (NoPrint == FALSE) { 
      Serial.print(myChar);
    }
    TempString += myChar;
  }
  if (NoPrint == FALSE) { 
    if (PrintCR == TRUE) {
      Serial.print("\n");
    }
  } else {
    return(TempString); 
  }
  return("");
}
//======================================================================

//======================================================================
//  Function to process SMS received
//
//  Command Led9On    -> Led 9 on the board ON
//  Command Led9Off   -> Led 9 on the board OFF
//  Command Led9Blink -> Led 9 on the board BLINK
//
void ProcessSmsReceived(void) {
  uint8_t Counter;
  
  if (Sms.SmsFlag.Bit.SmsReadOk == 1) {
    Sms.SmsFlag.Bit.SmsReadOk = 0;
    if (Sms.SmsFlag.Bit.SmsReadStat == SMS_REC_UNREAD) {
        SmsReceived = "";
        Counter = 0;
        do {
          SmsReceived += char(Sms.SmsText[Counter]);
        } while (Counter++ < sizeof Sms.SmsText);

        LocalReadStringFLASH((uint8_t *)SMS_RECEIVED, strlen(SMS_RECEIVED), TRUE, FALSE);
        Serial.println(SmsReceived);
        
        if (SmsReceived.compareTo(LocalReadStringFLASH((uint8_t *)LED9_ON, strlen(LED9_ON), TRUE, TRUE)) == 0) {
          Io.LedOn(PIN_LED9);
          return;  
        }
        if (SmsReceived.compareTo(LocalReadStringFLASH((uint8_t *)LED9_OFF, strlen(LED9_OFF), TRUE, TRUE)) == 0) {
          Io.LedOff(PIN_LED9);
          return;
        }
        if (SmsReceived.compareTo(LocalReadStringFLASH((uint8_t *)LED9_BLINK, strlen(LED9_BLINK), TRUE, TRUE)) == 0) {
          Io.LedOn(PIN_LED9);
          delay(200);
          Io.LedOff(PIN_LED9);
          delay(200);
          Io.LedOn(PIN_LED9);
          delay(200);
          Io.LedOff(PIN_LED9);
          delay(200);
          Io.LedOn(PIN_LED9);
          delay(200);
          Io.LedOff(PIN_LED9);
          delay(200);
          Io.LedOn(PIN_LED9);
          delay(200);
          Io.LedOff(PIN_LED9);          
          return;
        }      
    }
  }
}
//======================================================================
