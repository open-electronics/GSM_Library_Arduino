/*********************************************************************
 *
 *       Tcpip command (Supported SIMCOM SIMCOM_SIM800C, SIMCOM_SIM900, SIMCOM_SIM928A and QUECTEL M95)
 *                                FIBOCOM G510 not supported
 *
 *********************************************************************
 * FileName:        TcpIpCmd_GSM.h
 * Revision:        1.0.0 (First issue)
 * Date:            15/10/2018
 * Dependencies:    Arduino.h
 *                  Uart_Gsm.h
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
 * Copyright (c) 2018, Futura Group srl 
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

#ifndef _TcpIpCmd_GSM_H_INCLUDED__
#define _TcpIpCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  SERVER REMOTE IP and PORT - STORED IN EEPROM
const uint8_t  EEMEM REMOTE_SERVER_IP_0[18]      = "\"79.35.125.98\"";          //  Remote server IP. Inserts your remote server IP
const uint8_t  EEMEM REMOTE_SERVER_IP_1[18]      = "\"79.35.125.98\"";          //  Remote server IP. Inserts your remote server IP
const uint8_t  EEMEM REMOTE_SERVER_IP_2[18]      = "\"100.100.100.100\"";       //  Remote server IP. Inserts your remote server IP
const uint8_t  EEMEM REMOTE_DOMAIN_SERVER_0[128] = "\"YourDomainServer.org\"";  //  Remote Domain Server. Inserts your remote domani server
const uint8_t  EEMEM REMOTE_DOMAIN_SERVER_1[128] = "\"YourDomainServer.org\"";  //  Remote Domain Server. Inserts your remote domani server
const uint8_t  EEMEM REMOTE_DOMAIN_SERVER_2[128] = "\"YourDomainServer.org\"";  //  Remote Domain Server. Inserts your remote domani server
const uint8_t  EEMEM REMOTE_SERVER_PORT_0[8]     = "\"1080\"";                  //  Remote port. Inserts your remote port
const uint8_t  EEMEM REMOTE_SERVER_PORT_1[8]     = "\"11000\"";                 //  Remote port. Inserts your remote port
const uint8_t  EEMEM REMOTE_SERVER_PORT_2[8]     = "\"65535\"";                 //  Remote port. Inserts your remote port
//======================================================================

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    const char AT_CIPMUX[]      PROGMEM = "AT+CIPMUX=0\r\n";    //  Command - Start up Multi-IP Connection; "0" Single IP connection; "1" Multi IP connection
    const char AT_CSTT[]        PROGMEM = "AT+CSTT=";           //  Command - Start Task and Set APN, USER NAME, PASSWORD
    const char AT_CIICR[]       PROGMEM = "AT+CIICR\r\n";       //  Command - Bring up Wireless Connection with GPRS or CSD
    const char AT_CIFSR[]       PROGMEM = "AT+CIFSR\r\n";       //  Command - Get Local IP Address
    const char AT_CIPHEAD[]     PROGMEM = "AT+CIPHEAD=1\r\n";   //  Command - Add an IP Head at the Beginning of a Package Received
    const char AT_CIPSRIP[]     PROGMEM = "AT+CIPSRIP=1\r\n";   //  Command - Show Remote IP Address and Port When Received Data

    const char AT_CIPSTART[]    PROGMEM = "AT+CIPSTART=";       //  Command - Start up TCP or UDP Connection
    const char AT_CIPSEND[]     PROGMEM = "AT+CIPSEND";         //  Command - Send Data Through TCP or UDP Connection
    const char AT_CIPCLOSE[]    PROGMEM = "AT+CIPCLOSE=";       //  Command - Close TCP or UDP Connection

    const char ATQ_CIPSEND[]    PROGMEM = "AT+CIPSEND?";        //  Command - Query to check max data lenght
    const char ATQ_CIPSTATUS[]  PROGMEM = "AT+CIPSTATUS";       //  Command - Query Current Connection Status
#endif
#ifdef QUECTEL_M95
    const char AT_CIPMUX[]      PROGMEM = "AT+QIMUX=0\r\n";     //  Command - Start up Multi-IP Connection; "0" Single IP connection; "1" Multi IP connection
    const char AT_CSTT[]        PROGMEM = "AT+QIREGAPP=";       //  Command - Start Task and Set APN, USER NAME, PASSWORD
    const char AT_CIICR[]       PROGMEM = "AT+QIACT\r\n";       //  Command - Bring up Wireless Connection with GPRS or CSD
    const char AT_CIFSR[]       PROGMEM = "AT+QILOCIP\r\n";     //  Command - Get Local IP Address
    const char AT_CIPHEAD[]     PROGMEM = "AT+QIHEAD=1\r\n";    //  Command - Add an IP Head at the Beginning of a Package Received
    const char AT_CIPSRIP[]     PROGMEM = "AT+QISHOWRA=1\r\n";  //  Command - Show Remote IP Address and Port When Received Data

    const char AT_CIPSTART[]    PROGMEM = "AT+QIOPEN=";         //  Command - Start up TCP or UDP Connection
    const char AT_CIPSEND[]     PROGMEM = "AT+QISEND";          //  Command - Send Data Through TCP or UDP Connection
    const char AT_CIPCLOSE[]    PROGMEM = "AT+QICLOSE=";        //  Command - Close TCP or UDP Connection

    const char ATQ_CIPSEND[]    PROGMEM = "AT+QISEND?";         //  Command - Query to check max data lenght
    const char ATQ_CIPSTATUS[]  PROGMEM = "AT+QISTATE\r\n";     //  Command - Query Current Connection Status
#endif
//======================================================================

//======================================================================
//  AT COMMAND ANSWER - STORED IN FLASH MEMORY
const char AT_ANSW_CIPSHUT[]            PROGMEM = "SHUT OK";    // Key word for answer at command CLCK
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    const char AT_ANSW_CIPSTATUS[]      PROGMEM = "+CIPSTATUS:";
    const char AT_ANSW_CIPSEND[]        PROGMEM = "SEND OK";
#endif
#ifdef QUECTEL_M95
    const char AT_ANSW_QISTATE[]        PROGMEM = "+QISTATE:";
#endif

const char GSM_SET_AS_SERVER[]          PROGMEM = "S:";         //  GSM Module is set as Server
const char GSM_SET_AS_CLIENT[]          PROGMEM = "C:";         //  GSM Module is set as Client

const char TCP_STR[]                    PROGMEM = "\"TCP\"";
const char UDP_STR[]                    PROGMEM = "\"UDP\"";
const char URL_STR[]                    PROGMEM = "\"URL\"";
const char GET_STR[]                    PROGMEM = "GET /";
const char POST_STR[]                   PROGMEM = "POST /";
const char HTTP_STR[]                   PROGMEM = "HTTP/1.1\r\n";
const char HOST_STR[]                   PROGMEM = "Host:";
const char CONTENT_TYPE_STR[]           PROGMEM = "Content-Type:application/x-www-form-urlencoded<CR><LF>";
const char CONTENT_LENGHT_STR[]         PROGMEM = "Content-Lenght:";
const char KEEP_ALIVE_STR[]             PROGMEM = "Connection:keep-alive\r\n\r\n";

const char TCP_UDP_ALREADY_CONNECT[]    PROGMEM = "ALREADY CONNECT";
const char TCP_UDP_CONNECT_OK[]         PROGMEM = "CONNECT OK";
const char TCP_UDP_CONNECT_FAIL[]       PROGMEM = "CONNECT FAIL";
const char TCP_UDP_CLOSE_OK[]           PROGMEM = "CLOSE OK";
const char TCP_UDP_STATE[]              PROGMEM = "STATE:";

const char SERVER_OPENING[]             PROGMEM = "OPENING";
const char SERVER_LISTENING[]           PROGMEM = "LISTENING";
const char SERVER_CLOSING[]             PROGMEM = "CLOSING";

const char CLIENT_INITIAL[]             PROGMEM = "INITIAL";
const char CLIENT_CONNECTING[]          PROGMEM = "CONNECTING";
const char CLIENT_CONNECTED[]           PROGMEM = "CONNECTED";
const char CLIENT_REMOTE_CLOSING[]      PROGMEM = "REMOTE CLOSING";
const char CLIENT_CLOSING[]             PROGMEM = "CLOSING";
const char CLIENT_CLOSED[]              PROGMEM = "CLOSED";

const char STATE_IP_INITIAL[]           PROGMEM = "IP INITIAL";             // The TCP/IP stack is in IDLE state. Single and Multi IP state
const char STATE_IP_START[]             PROGMEM = "IP START";               // The TCP/IP stack has been registered. Single and Multi IP state
const char STATE_IP_CONFIG[]            PROGMEM = "IP CONFIG";              // It has been start-up to activate GPRS/CSD context. Single and Multi IP state
const char STATE_IP_GPRSACT[]           PROGMEM = "IP GPRSACT";             // GPRS/CSD context has been activated successfully. Single and Multi IP state
const char STATE_IP_CLOSE[]             PROGMEM = "IP CLOSE";               // The TCP/UDP connections has been closed
const char STATE_IP_STATUS[]            PROGMEM = "IP STATUS";              // The local IP address has been gotten by the command AT+QILOCIP. Single and Multi IP state
const char STATE_IP_PROCESSING[]        PROGMEM = "IP PROCESSING";          // Data phase. Processing the existing connection now. Only for Multi-IP state
const char STATE_TCP_CONNECTING[]       PROGMEM = "TCP CONNECTING";         // It is trying to establish a TCP connection. Only for Single-IP state
const char STATE_UDP_CONNECTING[]       PROGMEM = "UDP CONNECTING";         // It is trying to establish a UDP connection. Only for Single-IP state
const char STATE_SERVER_LISTENING[]     PROGMEM = "SERVER LISTENING";       // Only for Single-IP state
const char STATE_CONNECT_OK[]           PROGMEM = "CONNECT OK";             // The TCP/UDP connection has been established successfully. Only for Single-IP state
const char STATE_TCP_CLOSING[]          PROGMEM = "TCP CLOSING";            // Only for Single-IP state
const char STATE_UDP_CLOSING[]          PROGMEM = "UDP CLOSING";            // Only for Single-IP state
const char STATE_TCP_CLOSED[]           PROGMEM = "TCP CLOSED";             // Only for Single-IP state
const char STATE_UDP_CLOSED[]           PROGMEM = "UDP CLOSED";             // Only for Single-IP state
const char STATE_PDP_DEACT[]            PROGMEM = "PDP DEACT";              // GPRS/CSD context was deactivated because of unknown reason 

#define TCP_UDP_NOT_CONNECTED_CODE      0
#define TCP_UDP_ALREADY_CONNECT_CODE    1
#define TCP_UDP_CONNECT_OK_CODE         2
#define TCP_UDP_CONNECT_FAIL_CODE       3

#define SERVER_OPENING_CODE             0
#define SERVER_LISTENING_CODE           1
#define SERVER_CLOSING_CODE             2

#define CLIENT_INITIAL_CODE             0
#define CLIENT_CONNECTING_CODE          1
#define CLIENT_CONNECTED_CODE           2
#define CLIENT_REMOTE_CLOSING_CODE      3
#define CLIENT_CLOSING_CODE             4
#define CLIENT_CLOSED_CODE              5

#define STATE_IP_INITIAL_CODE           0
#define STATE_IP_START_CODE             1
#define STATE_IP_CONFIG_CODE            2
#define STATE_IP_GPRSACT_CODE           3
#define STATE_IP_CLOSE_CODE             4
#define STATE_IP_STATUS_CODE            5
#define STATE_IP_PROCESSING_CODE        6
#define STATE_TCP_CONNECTING_CODE       7
#define STATE_UDP_CONNECTING_CODE       8
#define STATE_SERVER_LISTENING_CODE     9
#define STATE_CONNECT_OK_CODE           10
#define STATE_TCP_CLOSING_CODE          11
#define STATE_UDP_CLOSING_CODE          12
#define STATE_TCP_CLOSED_CODE           13
#define STATE_UDP_CLOSED_CODE           14
#define STATE_PDP_DEACT_CODE            15
//======================================================================

//======================================================================
//  AT+CSTT OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CSTT_APN_OFFSET     8
#endif
#ifdef QUECTEL_M95
    #define CSTT_APN_OFFSET     12
#endif
//======================================================================

//======================================================================
//  AT+CIPSTART OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CIPSTART_N_OFFSET   12
#endif
#ifdef QUECTEL_M95
    #define CIPSTART_N_OFFSET   10
#endif

#define TCP_CONNECTION  0
#define UDP_CONNECTION  1
//======================================================================

//======================================================================
//  AT+CIPCLOSE OFFSET (This offset is necessary only if AT+CMUX or AT+QIMUX was set as 1. Otherwise the parameters MUST be omitted)
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CIPCLOSE_OFFSET 12
#endif
#ifdef QUECTEL_M95
    #define CIPCLOSE_OFFSET 11
#endif
//======================================================================

//======================================================================
//  AT+CIPMUX OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CIPMUX_OFFSET       10
#endif
#ifdef QUECTEL_M95
    #define CIPMUX_OFFSET       9
#endif

#define SINLGE_IP_CONN          0
#define MULTI_IP_CONN           1
//======================================================================

//======================================================================
//  AT+CIPSEND OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CIPSEND_N_OFFSET    10
#endif
#ifdef QUECTEL_M95
    #define CIPSEND_N_OFFSET    9
#endif
//======================================================================

//======================================================================
//  AT+CLPORT OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CLPORT_N_OFFSET     10
#endif
#ifdef QUECTEL_M95
    #define CLPORT_N_OFFSET     11
#endif
//======================================================================

//======================================================================
//  AT+CIPSTATUS OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define CIPSTATUS_OFFSET    12
#endif
//======================================================================

//======================================================================
//  SEND CMD STATE STEP
#define CMD_TCP_IP_IDLE                     0x00

#define WAIT_ANSWER_CMD_AT_CIPSTART         0x01
#define WAIT_ANSWER_CMD_AT_CIPCLOSE         0x02
#define WAIT_ANSWER_CMD_AT_CIPSEND_STEP1    0x03
#define WAIT_ANSWER_CMD_AT_CIPSEND_STEP2    0x04
#define WAIT_ANSWER_CMD_ATQ_CIPSEND         0x05
#define WAIT_ANSWER_CMD_ATQ_CIPSTATUS       0x06
//======================================================================

class TcpIpCmd_GSM {
    
public:
    //-------------------------------------------------------------------------------------
    struct {
        uint16_t StartAddRemoteServerIp[3];
        uint16_t StartAddRemoteDomainServer[3];
        uint16_t StartAddRemoteServerPort[3];
    } TcpIpEepromAdd;
    void TcpIpEepromStartAddSetup(void);
    //-------------------------------------------------------------------------------------
    
    uint8_t SetCmd_AT_CIPSTATUS(uint8_t n);
    uint8_t SetCmd_AT_CIPSTART(uint8_t n, uint8_t ConnectionMode, uint8_t IP_Domain);
    uint8_t SetCmd_AT_CIPCLOSE(uint8_t n, uint8_t id);
    uint8_t SetCmd_AT_CIPSEND(bool Query, uint8_t n);
    void    SetCmd_AT_CIPSEND_Step2(uint8_t Index);
    
    union TcpIpStateFlag {
        uint16_t  TcpIpStateWord[3];
        struct {
            uint8_t ServerState             :2; //  "00"   -> OPENING
                                                //  "01"   -> LISTENING
                                                //  "10"   -> CLOSING
            uint8_t ClientState             :3; //  "000"  -> INITIAL
                                                //  "001"  -> CONNECTING
                                                //  "010"  -> CONNECTED
                                                //  "011"  -> REMOTE CLOSING
                                                //  "100"  -> CLOSING
                                                //  "101"  -> CLOSED
            uint8_t State_Conn              :4; //  "0000" -> IP INITIAL
                                                //  "0001" -> IP START
                                                //  "0010" -> IP CONFIG
                                                //  "0011" -> IP GPRSACT
                                                //  "0100" -> IP CLOSE
                                                //  "0101" -> IP STATUS
                                                //  "0110" -> TCP CONNECTING
                                                //  "0111" -> UDP CONNECTING
                                                //  "1000" -> SERVER LISTENING
                                                //  "1001" -> CONNECRT OK
                                                //  "1010" -> TCP CLOSING
                                                //  "1011" -> UDP CLOSING
                                                //  "1100" -> TCP CLOSED
                                                //  "1101" -> UDP CLOSED
                                                //  "1110" -> PDP DEACT
                                                //  "1111" -> IP PROCESSING (only for multi IP)
            uint8_t State_TCP_UPD           :2; //  "00"   -> TCP/UDP Not connected yet or closed
                                                //  "01"   -> TCP/UDP Already connected
                                                //  "10"   -> TCP/UDP Connected ok
                                                //  "11"   -> TCP/UDP Connected fail 
            uint8_t Free                    :5; //  Free
        } BitState[3];
    } TcpIpStateFlag;
    
    union TcpIpFlag {       
        uint8_t  TcpIpByte;
        struct {        
            uint8_t ApnUserNamePassword     :1; //  "1" send User Name and Password with APN. "0" No User Name and Password for APN 
            uint8_t MultiIp                 :1; //  "1" Multi IP connection, "0" single IP connection
            uint8_t SetsDataToSent          :1; //  "1" Request to set data to send
            uint8_t IndexConnection         :2; //  "00" -> connessione 0
                                                //  "01" -> connessione 1
                                                //  "10" -> connessione 2
            uint8_t Free                    :3; //  Free
        } Bit;      
    } TcpIpFlag;
    //-------------------------------------------------------------------------------------

    uint8_t Local_IP_Address[18];           // String type, IP address. Format: "<n>.<n>.<n>.<n>" where <n>=0..255
        
    void    TcpIpWaitAnswer(void);  
    void    TcpIpRetrySendCmd(void);
    
private:
    void    ProcessSimcomCipStatusAnswer(void);
    void    ProcessQuectelCipStatusAnswer(void);
};

extern TcpIpCmd_GSM TcpIp;

#endif