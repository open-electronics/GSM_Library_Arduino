/*********************************************************************
 *
 *       GSM Sketch - Mqtt 3.1.1 Test
 *
 *********************************************************************
 * FileName:        GSM_Mqtt_311_Test.ino
 * Revision:        1.0.0
 * Date:            18/04/2020
 * Dependencies:    MQTT_GSM.h
 *
 * Arduino Board:   Arduino Mega 2560 or Fishino Mega 2560
 *                  See file Io_GSM.h to define hardware and Arduino Board to use
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

#include <avr/eeprom.h>

#include "MQTT_GSM.h"
#include "Uart_GSM.h"
#include "Isr_GSM.h"
#include "GenericCmd_GSM.h"
#include "SecurityCmd_GSM.h"
#include "PhoneBookCmd_GSM.h"
#include "SmsCmd_GSM.h"
#include "PhonicCallCmd_GSM.h"

SecurityCmd_GSM   Security;
PhoneBookCmd_GSM  PhoneBook;
SmsCmd_GSM        Sms;
PhonicCallCmd_GSM PhonicCall;
Isr_GSM           Isr;
Mqtt_GSM          Mqtt;

#define MQTT_TEST_REV     "1.0"

#define TRUE  0
#define FALSE 1

//=======================================================================================================
//  Start Menu. Mqtt command available
const char STR_SELECT_MQTT_COMMAND[]       PROGMEM = "Selects which Mqtt packet that you want generate from the list below:";
const char STR_CMD_CONNECT_PACKET[]        PROGMEM = "a) Generate MQTT packet: \"CONNECT\"";
const char STR_CMD_DISCONNECT_PACKET[]     PROGMEM = "b) Generate MQTT packet: \"DISCONNECT\"";
const char STR_CMD_CONN_ACK_PACKET[]       PROGMEM = "c) Generate MQTT packet: \"CONNACK\"";
const char STR_CMD_PUBLISH_PACKET[]        PROGMEM = "d) Generate MQTT packet: \"PUBLISH\"";
const char STR_CMD_PUB_ACK_PACKET[]        PROGMEM = "e) Generate MQTT packet: \"PUB ACK\"";
const char STR_CMD_PUB_REC_PACKET[]        PROGMEM = "f) Generate MQTT packet: \"PUB REC\"";
const char STR_CMD_PUB_REL_PACKET[]        PROGMEM = "g) Generate MQTT packet: \"PUB REL\"";
const char STR_CMD_PUB_COMP_PACKET[]       PROGMEM = "h) Generate MQTT packet: \"PUB COMP\"";
const char STR_CMD_SUBSCRIBE_PACKET[]      PROGMEM = "i) Generate MQTT packet: \"SUBSCRIBE\"";
const char STR_CMD_SUB_ACK_PACKET[]        PROGMEM = "j) Generate MQTT packet: \"SUB ACK\"";
const char STR_CMD_UNSUBSCRIBE_PACKET[]    PROGMEM = "k) Generate MQTT packet: \"UNSUBSCRIBE\"";
const char STR_CMD_UNSUB_ACK_PACKET[]      PROGMEM = "l) Generate MQTT packet: \"UNSUB ACK\"";
const char STR_CMD_PING_REQ_PACKET[]       PROGMEM = "m) Generate MQTT packet: \"PINGREQ\"";
const char STR_CMD_PING_RESP_PACKET[]      PROGMEM = "n) Generate MQTT packet: \"PINGRESP\"";

const char STR_CMD_INVALID_MENU_CHAR[]     PROGMEM = "Invalid char was inserted ! Retry again with new char";
//=======================================================================================================

//=======================================================================================================
//  CONNECT PACKET configuration parameters: client ID, Connection Flags, Will message, User Name, Password, Keep Alive
const char STR_CMD_CONNECT_CLIENT_ID[]              PROGMEM = " - Inserts Client ID. Max length 32 chars";
const char STR_CMD_CONNECT_FLAG_PARAM[]             PROGMEM = " - Sets Flags used with \"CONNECT\" Packet [Example to set single bit: b1=0 or b1=1; Example to set all bits: b=00110100]";
const char STR_CMD_CONNECT_RESERVED_FLAG[]          PROGMEM = "   b0)  Reserved";
const char STR_CMD_CONNECT_CLEAN_SESSION_FLAG[]     PROGMEM = "   b1)  \"Clean Session\"  - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_CONNECT_WILL_FLAG[]              PROGMEM = "   b2)  \"Will Flag\"      - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_CONNECT_WILL_QOS[]               PROGMEM = "   b34) \"Will QoS\"       - Set Flag: 00; 01; 10";
const char STR_CMD_CONNECT_WILL_RETAIN[]            PROGMEM = "   b5)  \"Will Retain\"    - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_CONNECT_PASSWORD_FLAG[]          PROGMEM = "   b6)  \"Password Flag\"  - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_CONNECT_USER_NAME_FLAG[]         PROGMEM = "   b7)  \"User Name Flag\" - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_CONNECT_KEEP_ALIVE[]             PROGMEM = " - Inserts Keep Alive Time in seconds. Max value 65535";
const char STR_CMD_CONNECT_WILL_MESSAGE[]           PROGMEM = " - Inserts will message. Max length 32 chars";
const char STR_CMD_CONNECT_WILL_TOPIC[]             PROGMEM = " - Inserts will topic. Max length 32 chars";
const char STR_CMD_CONNECT_PASSWORD[]               PROGMEM = " - Inserts password. Max length 32 chars:";
const char STR_CMD_CONNECT_USER_NAME[]              PROGMEM = " - Inserts UserName. Max length 32 chars:";

const char STR_ERR_KEEP_ALIVE[]                     PROGMEM = " $ Invalid Keep Alive value inserted. Retry";
const char STR_ERR_CLIED_ID_TOO_LONG[]              PROGMEM = " $ Client ID inserted too long. Retry";
const char STR_ERR_WILL_TOPIC_TOO_LONG[]            PROGMEM = " $ Will Topic Message inserted too long. Retry";
const char STR_ERR_WILL_MESSAGE_TOO_LONG[]          PROGMEM = " $ Will Message inserted too long. Retry";
const char STR_ERR_USER_NAME_TOO_LONG[]             PROGMEM = " $ User Name inserted too long. Retry";
const char STR_ERR_PASSWORD_TOO_LONG[]              PROGMEM = " $ Password inserted too long. Retry";

const char STR_KEEP_ALIVE_OK[]                      PROGMEM = " # Keep Alive saved.";
const char STR_CLIED_ID_OK[]                        PROGMEM = " # Client ID saved.";
const char STR_WILL_TOPIC_OK[]                      PROGMEM = " # Will Topic Message saved.";
const char STR_WILL_MESSAGE_OK[]                    PROGMEM = " # Will Message saved.";
const char STR_USER_NAME_OK[]                       PROGMEM = " # User Name saved.";
const char STR_PASSWORD_OK[]                        PROGMEM = " # Password saved.";

const char STR_CONNECT_FLAGS_OK[]                   PROGMEM = "Flags \"CONNECT PACKET\" set.";
const char STR_CLEAN_SESSION_FLAG_OK[]              PROGMEM = "Flag \"Clean Session\" set.";
const char STR_WILL_FLAG_OK[]                       PROGMEM = "Flag \"Will Flag\" set.";
const char STR_WILL_QOS_FLAG_OK[]                   PROGMEM = "Flag \"Will QoS\" set.";
const char STR_WILL_RETAIN_FLAG_OK[]                PROGMEM = "Flag \"Will Retain\" set.";
const char STR_PASSWORD_FLAG_OK[]                   PROGMEM = "Flag \"Password\" set.";
const char STR_USER_NAME_FLAG_OK[]                  PROGMEM = "Flag \"User Name\" set.";

const char STR_SEP_1[]                              PROGMEM = "\n===================================================================\n";
const char STR_SEP_2[]                              PROGMEM = "===================================================================\n";
const char STR_SEP_3[]                              PROGMEM = "\n*******************************************************************\n";
const char STR_SEP_4[]                              PROGMEM = "*******************************************************************\n";
const char STR_PRNT_CONNECT_CLIENT_ID[]             PROGMEM = "Client ID         (Type String): ";
const char STR_PRNT_CONNECT_USER_NAME[]             PROGMEM = "User Name         (Type String): ";
const char STR_PRNT_CONNECT_PASSWORD[]              PROGMEM = "Password          (Type String): ";
const char STR_PRNT_CONNECT_WILL_TOPIC[]            PROGMEM = "Will Topic        (Type String): ";
const char STR_PRNT_CONNECT_WILL_MESSAGE[]          PROGMEM = "Will Message      (Type String): ";
const char STR_PRNT_CONNECT_PACKET_CODE[]           PROGMEM = "Packet Code       (Type Hex):    ";
const char STR_PRNT_CONNECT_PACKET_LENGTH[]         PROGMEM = "Packet Length     (Type Hex):    ";
const char STR_PRNT_CONNECT_VARIABLE_HEADER[]       PROGMEM = "Variable Header   (Type Hex):    ";
const char STR_PRNT_CONNECT_PROTOCOL_LEVEL[]        PROGMEM = "Protocol Level    (Type Hex):    ";
const char STR_PRNT_CONNECT_PROTOCOL_FLAGS[]        PROGMEM = "Protocol Flags    (Type Hex):    ";
const char STR_PRNT_CONNECT_KEEP_ALIVE[]            PROGMEM = "Keep Alive        (Type Hex):    ";

const char STR_CONNECT_PACKET_OK[]                  PROGMEM = "Mqtt \"CONNECT PACKET\" generated: ";

#define KEEP_ALIVE_MAX_VALUE                        65535
#define ID_CLIENT_MAX_LENGTH                        32
#define WILL_MESSAGE_MAX_LENGTH                     32
#define WILL_TOPIC_MAX_LENGTH                       32
#define USER_NAME_MAX_LENGTH                        32
#define PASSWORD_MAX_LENGTH                         32

//  Connect Packet - State machine
#define CONNECT_PACKET_IDLE                         0
#define CONNECT_PACKET_REQ_CLIENT_ID                1
#define CONNECT_PACKET_WAIT_INSERT_CLIENT_ID        2
#define CONNECT_PACKET_REQ_KEEP_ALIVE               3
#define CONNECT_PACKET_WAIT_INSERT_KEEP_ALIVE       4
#define CONNECT_PACKET_REQ_SET_FLAG                 5
#define CONNECT_PACKET_WAIT_INSERT_FLAG             6
#define CONNECT_PACKET_REQ_WILL_TOPIC               7
#define CONNECT_PACKET_WAIT_INSERT_WILL_TOPIC       8
#define CONNECT_PACKET_REQ_WILL_MESSAGE             9
#define CONNECT_PACKET_WAIT_INSERT_WILL_MESSAGE     10
#define CONNECT_PACKET_REQ_USER_NAME                11
#define CONNECT_PACKET_WAIT_INSERT_USER_NAME        12
#define CONNECT_PACKET_REQ_PASSWORD                 13
#define CONNECT_PACKET_WAIT_INSERT_PASSWORD         14
#define CONNECT_PACKET_GENERATE                     15
//=======================================================================================================

//=======================================================================================================
//  CONNACK PACKET configuration parameters: Conn ACK Flag 
const char STR_CMD_CONN_ACK_FLAG_PARAM[]            PROGMEM = " - Sets Flag used with \"CONNACK\" Packet [Example to set single bit: b0=0 or b0=1";
const char STR_CMD_CONN_ACK_SP_FLAG[]               PROGMEM = "   b0)  Session Present - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_CONN_ACK_RESERVED_FLAG[]         PROGMEM = "   b17) Reserved - Bits 1-7 are reserved and MUST be set to 0";
const char STR_CMD_CONN_ACK_RETURN_CODE[]           PROGMEM = " - Insert Connect Return Code. Code values available:";
const char STR_CMD_CONN_ACK_CONNECTION_ACCEPTED[]   PROGMEM = "   - \"0)\" Connection Accepted";
const char STR_CMD_CONN_ACK_CONNECTION_REFUSED_1[]  PROGMEM = "   - \"1)\" Connection Refused, unacceptable protocol version";
const char STR_CMD_CONN_ACK_CONNECTION_REFUSED_2[]  PROGMEM = "   - \"2)\" Connection Refused, identifier rejected";
const char STR_CMD_CONN_ACK_CONNECTION_REFUSED_3[]  PROGMEM = "   - \"3)\" Connection Refused, server unavailable";
const char STR_CMD_CONN_ACK_CONNECTION_REFUSED_4[]  PROGMEM = "   - \"4)\" Connection Refused, bad user name or password";
const char STR_CMD_CONN_ACK_CONNECTION_REFUSED_5[]  PROGMEM = "   - \"5)\" Connection Refused, not authorized";

const char STR_ERR_RETURN_CODE[]                    PROGMEM = " $ Invalid Return Code value inserted. Retry";

const char STR_SP_FLAG_OK[]                         PROGMEM = " # Flag \"SP\" set.";
const char STR_RETURN_CODE[]                        PROGMEM = " # Return Code saved.";

const char STR_PRNT_CONN_ACK_FLAGS[]                PROGMEM = "Connect ACK Flags (Type Hex):    ";
const char STR_PRNT_CONN_ACK_RETURN_CODE[]          PROGMEM = "Return Code       (Type Hex):    ";

const char STR_CONN_ACK_PACKET_OK[]                 PROGMEM = "Mqtt \"CONNACK PACKET\" generated: ";

#define RETURN_CODE_MAX_VALUE                       5

//  ConnAck Packet - State machine
#define CONN_ACK_PACKET_IDLE                        0
#define CONN_ACK_PACKET_REQ_SET_FLAG                1
#define CONN_ACK_PACKET_WAIT_INSERT_FLAG            2
#define CONN_ACK_PACKET_REQ_RETURN_CODE             3
#define CONN_ACK_PACKET_WAIT_INSERT_RETURN_CODE     4
#define CONN_ACK_PACKET_GENERATE                    5
//=======================================================================================================

//=======================================================================================================
//  DISCONNECT PACKET
const char STR_DISCONNECT_PACKET_OK[]             PROGMEM = "Mqtt \"DISCONNECT PACKET\" generated: ";
//=======================================================================================================

//=======================================================================================================
//  PUBLISH PACKET configuration parameters:
const char STR_CMD_PUBLISH_FLAG_PARAM[]             PROGMEM = " - Sets Flags used with  \"PUBLISH\" Packet [Example: b1=0 or b1=1; Example to set all bits: b=00110100]";
const char STR_CMD_PUBLISH_RETAIN_FLAG[]            PROGMEM = "   b0)  \"Retain\"     - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_PUBLISH_QOS_LEVEL_FLAG[]         PROGMEM = "   b12) \"QoS Level\"  - Set Flag: 00 (At most once delibery); 01 (At leat once delivery); 10 (Exactly once delivery)";
const char STR_CMD_PUBLISH_DUP_FLAG[]               PROGMEM = "   b3)  \"DUP Flag\"   - Set Flag: 1 ON; 0 OFF";
const char STR_CMD_PUBLISH_TOPIC[]                  PROGMEM = " - Set Topic used with \"PUBLISH\" Packet (Max length 64 char):";
const char STR_CMD_PUBLISH_IDENTIFIER[]             PROGMEM = " - Inserts Packet Identifier used with \"PUBLISH\" Packet:";
const char STR_CMD_PUBLISH_PAYLOAD[]                PROGMEM = " - Inserts Payload used with \"PUBLISH\" Packet (Max length 64 char):";

const char STR_ERR_TOPIC_NAME_TOO_LONG[]            PROGMEM = " $ Topic name inserted too long. Retry";
const char STR_ERR_PAYLOAD_TOO_LONG[]               PROGMEM = " $ Payload inserted too long. Retry";
const char STR_ERR_PACKET_IDENTIFIER[]              PROGMEM = " $ Invalid Packet Identifier value inserted. Retry";

const char STR_PUBLISH_FLAGS_OK[]                   PROGMEM = " # Flag \"PUBLISH PACKET\" set.";
const char STR_PUBLISH_QOS_FLAG_OK[]                PROGMEM = " # Flag \"PUBLISH QoS\" set.";
const char STR_PUBLISH_RETAIN_FLAG_OK[]             PROGMEM = " # Flag \"PUBLISH Retain\" set.";
const char STR_PUBLISH_DUP_FLAG_OK[]                PROGMEM = " # Flag \"PUBLISH Dup Flag\" set.";

const char STR_PUBLISH_TOPIC_NAME_OK[]              PROGMEM = " # Topic Name saved.";
const char STR_PUBLISH_PAYLOAD_OK[]                 PROGMEM = " # Payload saved.";
const char STR_PUBLISH_PACKET_IDENTIFIER_OK[]       PROGMEM = " # Packet Identifier saved.";

const char STR_PRNT_PUBLISH_TOPIC_NAME[]            PROGMEM = "Topic Name        (Type String): ";
const char STR_PRNT_PUBLISH_PACKET_IDENTIFIER[]     PROGMEM = "Packet Identifier (Type Hex):    ";
const char STR_PRNT_PUBLISH_PAYLOAD[]               PROGMEM = "Payload           (Type String): ";

const char STR_PUBLISH_PACKET_OK[]                  PROGMEM = "Mqtt \"PUBLISH PACKET\" generated: ";

#define PACKET_IDENTIFIER_MAX_VALUE                 65535
#define PUBLISH_TOPIC_NAME_MAX_LENGTH               64
#define PUBLISH_PAYLOAD_MAX_LENGTH                  128

//  publish Packet - State machine
#define PUBLISH_PACKET_IDLE                         0
#define PUBLISH_PACKET_REQ_SET_FLAG                 1
#define PUBLISH_PACKET_WAIT_INSERT_FLAG             2
#define PUBLISH_PACKET_REQ_TOPIC                    3
#define PUBLISH_PACKET_WAIT_INSERT_TOPIC            4
#define PUBLISH_PACKET_REQ_IDENTIFIER               5
#define PUBLISH_PACKET_WAIT_INSERT_IDENTIFIER       6
#define PUBLISH_PACKET_REQ_PAYLOAD                  7
#define PUBLISH_PACKET_WAIT_INSERT_PAYLOAD          8
#define PUBLISH_PACKET_GENERATE                     9
//=======================================================================================================

//=======================================================================================================
//  PUBACK-REC-REL-COMP PACKET configuration parameters:

const char STR_CMD_PUB_ACK_REC_REL_COMP_IDENTIFIER[] PROGMEM = " - Set Packet Identifier used with \"PUBACK, PUBREC, PUBREL and PUBCOMP\" Packets:";

const char STR_PUBACK_PACKET_OK[]                    PROGMEM = "Mqtt \"PUBACK PACKET\" generated: ";
const char STR_PUBREC_PACKET_OK[]                    PROGMEM = "Mqtt \"PUBREC PACKET\" generated: ";
const char STR_PUBREL_PACKET_OK[]                    PROGMEM = "Mqtt \"PUBREL PACKET\" generated: ";
const char STR_PUBCOMP_PACKET_OK[]                   PROGMEM = "Mqtt \"PUBCOMP PACKET\" generated: ";

#define PUBACK_PACKET_IDLE                           0
#define PUBACK_PACKET_REQ_IDENTIFIER                 1
#define PUBACK_PACKET_WAIT_INSERT_IDENTIFIER         2
#define PUBACK_REC_REL_COMP_PACKET_GENERATE          3
//=======================================================================================================

//=======================================================================================================
//  SUBSCRIBE PACKET configuration parameters:
const char STR_CMD_SUBSCRIBE_IDENTIFIER[]             PROGMEM = " - Inserts Packet Identifier used with \"SUBSCRIBE\" Packet:";
const char STR_CMD_SUBSCRIBE_TOPIC_FILTER_QTY[]       PROGMEM = " - How much Topic Filters do you want to use in the \"SUBSCRIBE\" Packet?. Max 4 Topic:";
const char STR_CMD_SUBSCRIBE_TOPIC_FILTER[]           PROGMEM = " - Inserts Topic Filter. Max lenght 32 chars";
const char STR_CMD_SUBSCRIBE_QOS[]                    PROGMEM = " - Inserts QoS code. Values accepted are: 0, 1 and 2.";

const char STR_ERR_SUBSCRIBE_TOPIC_FILTER_QTY[]       PROGMEM = " $ Invalid Topic Filter Quantity value inserted. Retry";
const char STR_ERR_SUBSCRIBE_QOS[]                    PROGMEM = " $ Invalid Qos code inserted. Retry";
const char STR_ERR_TOPIC_FILTER_TOO_LONG[]            PROGMEM = " $ Topic Filter inserted too long. Retry";

const char STR_SUBSCRIBE_TOPIC_FILTER_QTY_OK[]        PROGMEM = " # Topic Filter Quantity accepted.";
const char STR_SUBSCRIBE_QOS_OK[]                     PROGMEM = " # Subscribe Qos code saved.";
const char STR_SUBSCRIBE_TOPIC_FILTER_OK[]            PROGMEM = " # Subscribe Topic Filter saved.";

const char STR_PRNT_SUBSCRIBE_PACKET_IDENTIFIER[]     PROGMEM = "Packet Identifier (Type Hex):    ";
const char STR_PRNT_SUBSCRIBE_TOPIC_FILTER[]          PROGMEM = "Topic Filter      (Type String): ";
const char STR_PRNT_SUBSCRIBE_QOS[]                   PROGMEM = "QoS               (Type Hex):    ";

const char STR_SUBSCRIBE_PACKET_OK[]                  PROGMEM = "Mqtt \"SUBSCRIBE PACKET\" generated: ";

#define SUBSCRIBE_MAX_TOPIC                           2
#define SUBSCRIBE_TOPIC_FILTER_MAX_LENGTH             64

//  Subscribe Packet - State machine
#define SUBSCRIBE_PACKET_IDLE                         0
#define SUBSCRIBE_PACKET_REQ_IDENTIFIER               1
#define SUBSCRIBE_PACKET_WAIT_INSERT_IDENTIFIER       2
#define SUBSCRIBE_PACKET_REQ_HOW_MUCH_TOPIC           3
#define SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_QUANTITY   4
#define SUBSCRIBE_PACKET_REQ_TOPIC_1                  5
#define SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_1          6
#define SUBSCRIBE_PACKET_REQ_QOS_1                    7
#define SUBSCRIBE_PACKET_WAIT_INSERT_QOS_1            8
#define SUBSCRIBE_PACKET_REQ_TOPIC_2                  9
#define SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_2          10
#define SUBSCRIBE_PACKET_REQ_QOS_2                    11
#define SUBSCRIBE_PACKET_WAIT_INSERT_QOS_2            12
#define SUBSCRIBE_PACKET_REQ_TOPIC_3                  13
#define SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_3          14
#define SUBSCRIBE_PACKET_REQ_QOS_3                    15
#define SUBSCRIBE_PACKET_WAIT_INSERT_QOS_3            16
#define SUBSCRIBE_PACKET_REQ_TOPIC_4                  17
#define SUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_4          18
#define SUBSCRIBE_PACKET_REQ_QOS_4                    19
#define SUBSCRIBE_PACKET_WAIT_INSERT_QOS_4            20
#define SUBSCRIBE_PACKET_GENERATE                     21
//=======================================================================================================

//=======================================================================================================
//  UNSUBSCRIBE PACKET configuration parameters:
const char STR_CMD_UNSUBSCRIBE_IDENTIFIER[]             PROGMEM = " - Inserts Packet Identifier used with \"UNSUBSCRIBE\" Packet:";
const char STR_CMD_UNSUBSCRIBE_TOPIC_FILTER_QTY[]       PROGMEM = " - How much Topic Filters do you want to use in the \"UNSUBSCRIBE\" Packet?. Max 4 Topic:";

const char STR_UNSUBSCRIBE_TOPIC_FILTER_OK[]            PROGMEM = " # USubscribe Topic Filter saved.";

const char STR_UNSUBSCRIBE_PACKET_OK[]                  PROGMEM = "Mqtt \"UNSUBSCRIBE PACKET\" generated: ";
  
#define UNSUBSCRIBE_MAX_TOPIC                           4
#define UNSUBSCRIBE_TOPIC_FILTER_MAX_LENGTH             32

//  UnSubscribe Packet - State machine
#define UNSUBSCRIBE_PACKET_IDLE                         0
#define UNSUBSCRIBE_PACKET_REQ_IDENTIFIER               1
#define UNSUBSCRIBE_PACKET_WAIT_INSERT_IDENTIFIER       2
#define UNSUBSCRIBE_PACKET_REQ_HOW_MUCH_TOPIC           3
#define UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_QUANTITY   4
#define UNSUBSCRIBE_PACKET_REQ_TOPIC_1                  5
#define UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_1          6
#define UNSUBSCRIBE_PACKET_REQ_TOPIC_2                  7
#define UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_2          8
#define UNSUBSCRIBE_PACKET_REQ_TOPIC_3                  9
#define UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_3          10
#define UNSUBSCRIBE_PACKET_REQ_TOPIC_4                  11
#define UNSUBSCRIBE_PACKET_WAIT_INSERT_TOPIC_4          12
#define UNSUBSCRIBE_PACKET_GENERATE                     13
//=======================================================================================================

//=======================================================================================================
//  SUBBACK-UNSUBACK PACKET configuration parameters:
const char STR_CMD_SUB_UNSUB_ACK_IDENTIFIER[]           PROGMEM = " - Set Packet Identifier used with \"SUBACK and UNSUBACK\" Packets:";
const char STR_CMD_SUBACK_QOS_RETURN_CODE_QTY[]         PROGMEM = " - How much Qos return code do you want to use in the \"SUBACK\" Packet?. Max 4 Topic:";
const char STR_CMD_SUBACK_QOS[]                         PROGMEM = " - Inserts QoS return code. Values accepted are: 0, 1, 2 and 128.";

const char STR_ERR_SUBACK_QOS_RETURN_CODE_QTY[]         PROGMEM = " $ Invalid Qos Return Code quantity value inserted. Retry";
const char STR_ERR_SUBACK_QOS_RETURN_CODE[]             PROGMEM = " $ Invalid Qos return code inserted. Retry";

const char STR_SUBACK_QOS_RETURN_CODE_QTY_OK[]          PROGMEM = " # Qos Return Code Quantity accepted.";
const char STR_SUBACK_QOS_RETURN_CODE_OK[]              PROGMEM = " # SubAck Qos return code saved.";

const char STR_PRNT_SUBACK_RETURN_CODE_QOS[]            PROGMEM = "QoS Return Code   (Type Hex):    ";

const char STR_SUBACK_PACKET_OK[]                       PROGMEM = "Mqtt \"SUBACK PACKET\" generated: ";
const char STR_UNSUBACK_PACKET_OK[]                     PROGMEM = "Mqtt \"UNSUBACK PACKET\" generated: ";

#define SUBACK_MAX_QOS_RETURN_CODE                      4

#define SUBACK_PACKET_IDLE                              0
#define SUBACK_PACKET_REQ_IDENTIFIER                    1
#define SUBACK_PACKET_WAIT_INSERT_IDENTIFIER            2
#define SUBACK_PACKET_REQ_HOW_MUCH_RETURN_CODE          3
#define SUBACK_PACKET_WAIT_INSERT_RETURN_CODE_QTY       4
#define SUBACK_PACKET_REQ_QOS_1                         5
#define SUBACK_PACKET_WAIT_INSERT_QOS_1                 6
#define SUBACK_PACKET_REQ_QOS_2                         7
#define SUBACK_PACKET_WAIT_INSERT_QOS_2                 8
#define SUBACK_PACKET_REQ_QOS_3                         9
#define SUBACK_PACKET_WAIT_INSERT_QOS_3                 10
#define SUBACK_PACKET_REQ_QOS_4                         11
#define SUBACK_PACKET_WAIT_INSERT_QOS_4                 12
#define SUBACK_SUBACK_UNSUBACK_PACKET_GENERATE          13
//=======================================================================================================

//=======================================================================================================
//  PINGREQ PACKET
const char STR_PINGREQ_PACKET_OK[]   PROGMEM = "Mqtt \"PINGREQ PACKET\" generated: ";
//=======================================================================================================

//=======================================================================================================
//  PINGRESP PACKET
const char STR_PINGRESP_PACKET_OK[]  PROGMEM = "Mqtt \"PINGRESP PACKET\" generated: ";
//=======================================================================================================

//=======================================================================================================
//  Generic strings
const char STR_INVALID_DATA_RECEIVED[]       PROGMEM = " $$$ Error - Invalid command or char not supported!";
const char STR_FINISHED[]                    PROGMEM = " If finished type \"ok\" to procede or \"exit\" to abort!";

const char STR_OK[]                          PROGMEM = "ok";
const char STR_EXIT[]                        PROGMEM = "exit";

const char STR_B[]                           PROGMEM = "b";
const char STR_B0[]                          PROGMEM = "b0";
const char STR_B1[]                          PROGMEM = "b1";
const char STR_B2[]                          PROGMEM = "b2";
const char STR_B3[]                          PROGMEM = "b3";
const char STR_B4[]                          PROGMEM = "b4";
const char STR_B5[]                          PROGMEM = "b5";
const char STR_B6[]                          PROGMEM = "b6";
const char STR_B7[]                          PROGMEM = "b7";

const char STR_B12[]                         PROGMEM = "b12";
const char STR_B34[]                         PROGMEM = "b34";
//=======================================================================================================

//======================================================================
typedef void State;
typedef State (*Pstate)();

Pstate MqttProcessPacket;      //  States machine used to process Mqtt string received

//======================================================================

//======================================================================
String  StrSerialInput = "";

uint8_t CmdSequence;            //  Command Sequence Code. Used with state machine
uint8_t MqttCmd;                //  Mqtt command code
uint8_t SubscribeTopicQty;      //  Number of topic used in Subscribe command
uint8_t UnSubscribeTopicQty;    //  Number of topic used in Unsubscribe command
uint8_t QosReturnCodeQty;       //  Number of return code
//======================================================================

//======================================================================
//  Sketch Setup
void setup() {
  Mqtt.InitData();
  Isr.EnableLibInterrupt();                           //  Enables Library Interrupt
  Gsm.EnableDisableDebugSerial(true, BAUD_115200);    //  Enables and configure uart for debug 

  MqttProcessPacket = ProcessMqttIdle;
  
  delay(1000);
  PrintMainMenu();
}
//======================================================================

//======================================================================
void loop() {
  MqttProcessPacket();
}
//======================================================================

//======================================================================
String LocalReadString_FLASH(uint8_t *FlashPointer, uint8_t Lenght, boolean PrintCR, boolean NoPrint) {
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
  }
  return(TempString); 
}
//======================================================================
