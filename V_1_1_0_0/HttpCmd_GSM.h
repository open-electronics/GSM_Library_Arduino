/*********************************************************************
 *
 *       Https command for GSM
 *
 *********************************************************************
 * FileName:        HttpCmd_GSM.h
 * Revision:        1.0.0
 * Date:            13/01/2019
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

#ifndef _HttpCmd_GSM_H_INCLUDED__
#define _HttpCmd_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  SERVER REMOTE URL, PROXY SERVER AND PORT - STORED IN EEPROM
const uint8_t  EEMEM HTTP_URL_LINK[128] = "\"UrlLink.org\"";        //  Url Link. Inserts the URL to connect
const uint8_t  EEMEM HTTP_PROXY_IP[18]  = "\"100.100.100.100\"";    //  Proxy IP Address. Inserts Proxy IP Address
const uint8_t  EEMEM HTTP_PROXY_PORT[8] = "\"8888\"";               //  Proxy port. Inserts the Proxy Port
const uint8_t  EEMEM HTTP_UA[16]        = "\"SIMCom_MODULE\"";      //  User Agent
//======================================================================

//======================================================================
//  AT COMMAND - STORED IN FLASH MEMORY
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    const char AT_SAPBR_1[]     PROGMEM = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";          //  Command - Configure bearer profile 1 (Step 1)
    const char AT_SAPBR_2[]     PROGMEM = "AT+SAPBR=3,1,";                                  //  Command - Configure bearer profile 1 (Step 2) - [Add APN data]
    const char AT_SAPBR_3[]     PROGMEM = "AT+SAPBR=1,1\r\n";                               //  Command - To Open a GPRS context
    const char AT_SAPBR_4[]     PROGMEM = "AT+SAPBR=2,1\r\n";                               //  Command - To Query the GPRS context

    const char AT_HTTPINIT[]    PROGMEM = "AT+HTTPINIT\r\n";                                //  Command - To Init HTTP service
    const char AT_HTTPTERM[]    PROGMEM = "AT+HTTPTERM\r\n";                                //  Command - To Terminate HTTP service
    const char AT_HTTPPARA[]    PROGMEM = "AT+HTTPPARA=";                                   //  Command - To Set HTTP Parameters Value
    const char AT_HTTPACTION[]  PROGMEM = "AT+HTTPACTION=";                                 //  Command - To Set HTTP Method Action (GET; POST; HEAD; DELETE) 
    const char AT_HTTPREAD[]    PROGMEM = "AT+HTTPREAD";                                    //  Command - To Read the HTTP Server Response
    const char AT_HTTPDATA[]    PROGMEM = "AT+HTTPDATA=";                                   //  Command - To Input HTTP data
    const char AT_HTTPSTATUS[]  PROGMEM = "AT+HTTPSTATUS?\r\n";                             //  Command - To Query the HTTP status
#endif
//======================================================================

//======================================================================
//  AT COMMAND ANSWER - STORED IN FLASH MEMORY
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    const char AT_SAPBR[]       PROGMEM = "+SAPBR:";                                        // Key word for answer at command query AT+SAPBR (Step 4)   
    const char AT_HTTP_ACTION[] PROGMEM = "+HTTPACTION:";
    const char AT_HTTP_STATUS[] PROGMEM = "+HTTPSTATUS:";
    
    const char AT_HTTP_GET[]    PROGMEM = "GET";
    const char AT_HTTP_POST[]   PROGMEM = "POST";
    const char AT_HTTP_HEAD[]   PROGMEM = "HEAD";
    const char AT_HTTP_DELETE[] PROGMEM = "DELETE";
#endif
//======================================================================

//======================================================================
//  AT+SAPBR Parameters
const char SAPBR_PARAM_CONTYPE[]        PROGMEM = "\"CONTYPE\"";    //  Type of Internet connection. Value refer to <ConParamValue_ConType>
const char SAPBR_PARAM_APN[]            PROGMEM = "\"APN\"";        //  point name string: maximum 64 characters
const char SAPBR_PARAM_USER[]           PROGMEM = "\"USER\"";       //  User name string: maximum 32 characters
const char SAPBR_PARAM_PWD[]            PROGMEM = "\"PWD\"";        //  Password string: maximum 32 characters
const char SAPBR_PARAM_PHONENUM[]       PROGMEM = "\"PHONENUM\"";   //  Phone number for CSD call
const char SAPBR_PARAM_RATE[]           PROGMEM = "\"RATE\"";       //  CSD connection rate. For value refer to <ConParamValue_Rate>

const char SAPBR_PARAM_CSD[]            PROGMEM = "\"CSD\"";        //  Circuit-switched data call.
const char SAPBR_PARAM_GPRS[]           PROGMEM = "\"GPRS\"";       //  GPRS connection

#define SAPBR_PARAM_CONTYPE_CODE        0
#define SAPBR_PARAM_APN_CODE            1
#define SAPBR_PARAM_USER_CODE           2
#define SAPBR_PARAM_PWD_CODE            3
#define SAPBR_PARAM_PHONENUM_CODE       4
#define SAPBR_PARAM_RATE_CODE           5

#define SAPBR_PARAM_CSD_CODE            6
#define SAPBR_PARAM_GPRS_CODE           7
//======================================================================

//======================================================================
//  AT+HTTPPARA Parameters
const char HTTP_PARAM_CID[]             PROGMEM = "\"CID\"";        //  (Mandatory Parameter) Bearer profile identifier
const char HTTP_PARAM_URL[]             PROGMEM = "\"URL\"";        //  (Mandatory Parameter) HTTP client URL "http://'server'/'path':'tcpPort' " "server": FQDN or IP-address "path": path of file or directory "tcpPort": default value is 80.
const char HTTP_PARAM_UA[]              PROGMEM = "\"UA\"";         //  The user agent string which is set by the application to identify the mobile. Usually this parameter is set as operation system and software version information. Default value is "SIMCom_MODULE"
const char HTTP_PARAM_PROIP[]           PROGMEM = "\"PROIP\"";      //  The IP address of HTTP proxy server
const char HTTP_PARAM_PROPORT[]         PROGMEM = "\"PROPORT\"";    //  The port of HTTP proxy server
const char HTTP_PARAM_REDIR[]           PROGMEM = "\"REDIR\"";      //  This flag controls the redirection mechanism of the SIM800 when it is acting as HTTP client (numeric). If the server sends a redirect code (range 30x), the client will automatically send a new HTTP request when the flag is set to (1). Default value is 0 (no redirection)
const char HTTP_PARAM_BREAK[]           PROGMEM = "\"BREAK\"";      //  Parameter for HTTP method "GET", used for resuming broken transfer
const char HTTP_PARAM_BREAKEND[]        PROGMEM = "\"BREAKEND\"";   //  Parameter for HTTP method "GET", used for resuming broken transfer. which is used together with "BREAK", If the value of "BREAKEND" is bigger than "BREAK", the transfer scope is from "BREAK" to "BREAKEND". If the value of "BREAKEND" is smaller than "BREAK", the transfer scope is from "BREAK" to the end of the file
const char HTTP_PARAM_TIMEOUT[]         PROGMEM = "\"TIMEOUT\"";    //  If both "BREAKEND" and "BREAK" are 0, the resume broken transfer function is disabled. HTTP session timeout value, scope: 30-1000 second. Default value is 120 seconds. HTTP Parameter value. Type and supported content depend on related <HTTPParamTag>
const char HTTP_PARAM_CONTENT[]         PROGMEM = "\"CONTENT\"";    //  Used to set the “Content-Type” field in HTTP header
const char HTTP_PARAM_USERDATA[]        PROGMEM = "\"USERDATA\"";   //  User data

#define HTTP_PARAM_CID_CODE             0
#define HTTP_PARAM_URL_CODE             1
#define HTTP_PARAM_UA_CODE              2
#define HTTP_PARAM_PROIP_CODE           3
#define HTTP_PARAM_PROPORT_CODE         4
#define HTTP_PARAM_REDIR_CODE           5
#define HTTP_PARAM_BREAK_CODE           6
#define HTTP_PARAM_BREAKEND_CODE        7
#define HTTP_PARAM_TIMEOUT_CODE         8
#define HTTP_PARAM_CONTENT_CODE         9
#define HTTP_PARAM_USERDATA_CODE        10
//======================================================================

//======================================================================
//  AT+HTTPACTION Parameters
#define HTTP_ACTION_METHOD_GET          0
#define HTTP_ACTION_METHOD_POST         1
#define HTTP_ACTION_METHOD_HEAD         2
#define HTTP_ACTION_METHOD_DELETE       3

//  HTTP Status Code responded by remote server, it identifier refer to HTTP1.1(RFC2616)

#define HTTP_ACTION_STATUS_CODE_CONTINUE                        100
#define HTTP_ACTION_STATUS_CODE_SWITCHING_PROTOCOL              101
#define HTTP_ACTION_STATUS_CODE_OK                              200
#define HTTP_ACTION_STATUS_CODE_CREATED                         201
#define HTTP_ACTION_STATUS_CODE_ACCEPTED                        202
#define HTTP_ACTION_STATUS_CODE_NON_AUTHO_INFO                  203
#define HTTP_ACTION_STATUS_CODE_NO_CONTENT                      204
#define HTTP_ACTION_STATUS_CODE_RESET_CONTENT                   205
#define HTTP_ACTION_STATUS_CODE_PARTIAL_CONTENT                 206
#define HTTP_ACTION_STATUS_CODE_MULTIPLE_CHOICE                 300
#define HTTP_ACTION_STATUS_CODE_MOVED_PERMANENTLY               301
#define HTTP_ACTION_STATUS_CODE_FOUND                           302
#define HTTP_ACTION_STATUS_CODE_SEE_OTHER                       303
#define HTTP_ACTION_STATUS_CODE_NOT_MODIFIED                    304
#define HTTP_ACTION_STATUS_CODE_USE_PROXY                       305
#define HTTP_ACTION_STATUS_CODE_TEMPORARY_REDIRECT              307
#define HTTP_ACTION_STATUS_CODE_BADREQUEST                      400
#define HTTP_ACTION_STATUS_CODE_UNAUTHORIZED                    401
#define HTTP_ACTION_STATUS_CODE_PAYMENT_REQUIRED                402
#define HTTP_ACTION_STATUS_CODE_FORBIDDEN                       403
#define HTTP_ACTION_STATUS_CODE_NOT_FOUND                       404
#define HTTP_ACTION_STATUS_CODE_METHOD_NOT_ALLOWED              405
#define HTTP_ACTION_STATUS_CODE_NOT_ACCEPTABLE                  406
#define HTTP_ACTION_STATUS_CODE_PROXY_AUTHENTICATION_REQUIRED   407
#define HTTP_ACTION_STATUS_CODE_REQUEST_TIMEOUT                 408
#define HTTP_ACTION_STATUS_CODE_CONFLICT                        409
#define HTTP_ACTION_STATUS_CODE_GONE                            410
#define HTTP_ACTION_STATUS_CODE_LENGHT_REQUIRED                 411
#define HTTP_ACTION_STATUS_CODE_PRECONDITION_FAILED             412
#define HTTP_ACTION_STATUS_CODE_REQUEST_ENTITY_TOO_LARGE        413
#define HTTP_ACTION_STATUS_CODE_REQUEST_URI_TOO_LARGE           414
#define HTTP_ACTION_STATUS_CODE_UNSUPPORTED_MEDIA_TYPE          415
#define HTTP_ACTION_STATUS_CODE_REQUESTED_RANGE_NOT_SATISFIABLE 416
#define HTTP_ACTION_STATUS_CODE_EXPECTATION_FAILED              417
#define HTTP_ACTION_STATUS_CODE_INTERNAL_SERVER_ERROR           500
#define HTTP_ACTION_STATUS_CODE_NOT_IMPLEMENTED                 501
#define HTTP_ACTION_STATUS_CODE_BAD_GATEWAY                     502
#define HTTP_ACTION_STATUS_CODE_SERVICE_UNAVAILABLE             503
#define HTTP_ACTION_STATUS_CODE_GATEWAY_TIMEOUT                 504
#define HTTP_ACTION_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED      505
#define HTTP_ACTION_STATUS_CODE_NOT_HTTP_PDU                    600
#define HTTP_ACTION_STATUS_CODE_NETWORK_ERROR                   601
#define HTTP_ACTION_STATUS_CODE_NO_MEMORY                       602
#define HTTP_ACTION_STATUS_CODE_DNS_ERROR                       603
#define HTTP_ACTION_STATUS_CODE_STACK_BUSY                      604
//======================================================================

//======================================================================
//  AT+SAPBR_3 OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define SAPBR_2_APN_OFFSET      13
#endif
//======================================================================

//======================================================================
//  AT+HTTPARA OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define HTTP_PARA_OFFSET        12
#endif
//======================================================================

//======================================================================
//  AT+HTTPACTION OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define HTTP_ACTION_OFFSET      14
#endif
//======================================================================

//======================================================================
//  AT+HTTPDATA OFFSET
#if defined(SIMCOM_SIM800C) || defined(SIMCOM_SIM900) || defined(SIMCOM_SIM928A)
    #define HTTP_DATA_OFFSET        12
#endif
//======================================================================
    
//======================================================================
//  SEND CMD STATE STEP
#define CMD_HTTP_IDLE                       0x00

#define WAIT_ANSWER_CMD_AT_HTTP_STATUS      0x01
#define WAIT_ANSWER_CMD_AT_HTTP_INIT        0x02
#define WAIT_ANSWER_CMD_AT_HTTP_TERM        0x03
#define WAIT_ANSWER_CMD_AT_HTTP_PARA        0x04
#define WAIT_ANSWER_CMD_AT_HTTP_ACTION      0x05
//======================================================================

//======================================================================
//  INIT HTTP STATE STEP
//#define   INIT_IDLE                       0x00    //  See GenericCmd_GSM.h
#define INIT_CMD_AT_SAPBR_1                 0x01
#define INIT_CMD_AT_SAPBR_2                 0x02
#define INIT_CMD_AT_SAPBR_3                 0x03
#define INIT_CMD_AT_SAPBR_4                 0x04

//#define INIT_WAIT_IDLE                    0x00    //  See GenericCmd_GSM.h
#define INIT_WAIT_ANSWER_CMD_AT_SAPBR_1     0x01
#define INIT_WAIT_ANSWER_CMD_AT_SAPBR_2     0x02
#define INIT_WAIT_ANSWER_CMD_AT_SAPBR_3     0x03
#define INIT_WAIT_ANSWER_CMD_AT_SAPBR_4     0x04
//======================================================================

class HttpCmd_GSM {
    
public:
    //-------------------------------------------------------------------------------------
    struct {
        uint16_t StartAddHttpUrlLink;
        uint16_t StartAddHttpProxyIp;
        uint16_t StartAddHttpProxyPort;
        uint16_t StartAddHttpUA;
    } HttpEepromAdd;
    void HttpEepromStartAddSetup(void);
    //-------------------------------------------------------------------------------------
    
    uint8_t SetCmd_AT_HTTP_STATUS(void);
    uint8_t SetCmd_AT_HTTP_INIT(void);
    uint8_t SetCmd_AT_HTTP_TERM(void);
    uint8_t SetCmd_AT_HTTP_PARA(uint8_t ParamTagCode);
    uint8_t SetCmd_AT_HTTP_ACTION(uint8_t Method);
    
    union HttpFlag {
        uint32_t  HttpDoubleWord;
        struct {
            uint8_t  EnableHttpFunctions    :1;  // "1": Enable HTTP functions. Sets this flag in the Setup() function if you want to use HTTP functions
            uint8_t  AddHttpParameters      :1;  // "1": Add HTTP parameters at the URL read from the EEPROM
            uint8_t  HttpRedir              :1;  // "1": Enable REDIR function; "0" Disable REDIR function
            uint8_t  HttpStatusMode         :2;  // "0": GET
                                                 // "1": POST
                                                 // "2": HEAD
                                                 // "3": DELETE
            uint8_t  HttpStatus             :2;  // "0": IDLE
                                                 // "1": POST
                                                 // "2": HEAD
            uint8_t  Free_1                 :1;  // Free
            uint16_t HttpStatusCode         :10; // "100": Continue
                                                 // "101": Switching Protocols
                                                 // "200": OK
                                                 // "201": Created
                                                 // "202": Accepted
                                                 // "203": Non-Authoritative Information
                                                 // "204": No Content
                                                 // "205": Reset Content
                                                 // "206": Partial Content
                                                 // "300": Multiple Choices
                                                 // "301": Moved Permanently
                                                 // "302": Found
                                                 // "303": See Other
                                                 // "304": Not Modified
                                                 // "305": Use Proxy
                                                 // "307": Temporary Redirect
                                                 // "400": Bad Request
                                                 // "401": Unauthorized
                                                 // "402": Payment Required
                                                 // "403": Forbidden
                                                 // "404": Not Found
                                                 // "405": Method Not Allowed
                                                 // "406": Not Acceptable
                                                 // "407": Proxy Authentication Required
                                                 // "408": Request Time-out
                                                 // "409": Conflict
                                                 // "410": Gone
                                                 // "411": Length Required
                                                 // "412": Precondition Failed
                                                 // "413": Request Entity Too Large
                                                 // "414": Request-URI Too Large
                                                 // "415": Unsupported Media Type
                                                 // "416": Requested range not satisfiable
                                                 // "417": Expectation Failed
                                                 // "500": Internal Server Error
                                                 // "501": Not Implemented
                                                 // "502": Bad Gateway
                                                 // "503": Service Unavailable
                                                 // "504": Gateway Time-out
                                                 // "505": HTTP Version not supported
                                                 // "600": Not HTTP PDU
                                                 // "601": Network Error
                                                 // "602": No memory
                                                 // "603": DNS Error
                                                 // "604": Stack Busy   
            uint8_t  HttpActionMethod       :2;  // "0": GET
                                                 // "1": POST
                                                 // "2": HEAD
                                                 // "3": DELETE
            uint8_t  Free_2                 :4;  // Free
            uint8_t  Free_3                 :8;  // Free
        } Bit;  
    } HttpFlag;
    //-------------------------------------------------------------------------------------

    uint8_t  UrlParameters[64];             //  Array to store parameters used in the URL
    
    uint16_t HttpBreak;
    uint16_t HttpBreakEnd;
    uint16_t HttpTimeOut;
    
    void InitHttpSendCmd(void);             //  Send init command (Init mode only)
    void InitHttpWaitAnswer(void);          //  Wait init answer (Init mode only)
    void HttpWaitAnswer(void);  
    void HttpRetrySendCmd(void);
    
private:
    uint8_t SetHttpParaParam(uint8_t Index, uint8_t ParamCode);
};

extern HttpCmd_GSM Http;

#endif