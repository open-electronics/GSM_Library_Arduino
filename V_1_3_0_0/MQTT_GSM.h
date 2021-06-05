/*********************************************************************
 *
 *       MQTT for GSM
 *
 *********************************************************************
 * FileName:        MQTT_GSM.h
 * Revision:        1.0.0
 * Date:            10/04/2020
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

#ifndef _MQTT_GSM_H_INCLUDED__
#define _MQTT_GSM_H_INCLUDED__

#include "Arduino.h"
#include "Uart_GSM.h"

//======================================================================
//  MQTT ARRAY
const uint8_t  EEMEM MQTT_CLIENT_IDENTIFIER[32]        = "4CEE7E16DB6144DCB97E6FFE94FE9363";							//  The Client Identifier (ClientId) identifies the Client to the Server
const uint8_t  EEMEM MQTT_WILL_TOPIC[32]               = "Device Disconnected"; 										//	The Will Topic MUST be a UTF-8 encoded string
const uint8_t  EEMEM MQTT_WILL_MESSAGE[32]             = "Device Disconnected";											//	The Will Message
const uint8_t  EEMEM MQTT_USER_NAME[32]                = "Paperino"; 													//	The User Name
const uint8_t  EEMEM MQTT_PASSWORD[32]                 = "Topolino";													//	The Password
const uint8_t  EEMEM MQTT_PUBLISH_TOPIC_NAME[64]       = "channels/999968/publish/8KKSM25MBCSBSWWF";					//  Publis topic name
const uint8_t  EEMEM MQTT_SUBSCRIBE_TOPIC_FILTER_1[64] = "channels/999968/subscribe/fields/field1/PODDI37ZU2FS1WDD";	//	Subscribe topic filter 1
const uint8_t  EEMEM MQTT_SUBSCRIBE_TOPIC_FILTER_2[64] = "channels/999968/subscribe/fields/field2/PODDI37ZU2FS1WDD";	//	Subscribe topic filter 2
const uint8_t  EEMEM MQTT_SUBSCRIBE_TOPIC_FILTER_3[64] = "channels/999968/subscribe/fields/+/PODDI37ZU2FS1WDD";			//	Subscribe topic filter 3
const uint8_t  EEMEM MQTT_SUBSCRIBE_TOPIC_FILTER_4[64] = "channels/999968/subscribe/json/PODDI37ZU2FS1WDD";				//	Subscribe topic filter 4
//======================================================================

//============================================================================================================================================
//  MQTT Version 3.1.1

//======================================================================
//  Structure of an MQTT Control Packet
//
//  -----------------------------------------------------------
// |    Fixed header, present in all MQTT Control Packets      |
//  -----------------------------------------------------------
// |    Variable header, present in some MQTT Control Packets  |
//  ----------------------------------------------------------- 
// |    Payload, present in some MQTT Control Packets          |
//  ----------------------------------------------------------- 
//
//======================================================================

//======================================================================
//  MQTT Fixed Header
//
//  -----------------------------------------------------------------------------
// |     Bit     |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
//  -----------------------------------------------------------------------------
// | Byte 1      | MQTT Control Packet type      | Flags specific to each MQTT   |
// |             |                               | Control Packet Type           |
//  -----------------------------------------------------------------------------
// | Byte 2...   |                      Remaining Length                         |     
//  -----------------------------------------------------------------------------
//
//======================================================================

//======================================================================
//  MQTT Control Packet Type (MSB Nibble Byte 1)
#define MQTT_RESERVED_1         0x00
#define MQTT_CONNECT            0x01    //  Client to Server;                     Clent request to connect to Server
#define MQTT_CONNACK            0x02    //  Server to Client;                     Connect Acknowledgement
#define MQTT_PUBLISH            0x03    //  Client to Server or Server to Client; Publish Message
#define MQTT_PUBACK             0x04    //  Client to Server or Server to Client; Publish Acknowledgement
#define MQTT_PUBREC             0x05    //  Client to Server or Server to Client; Publish received (assured delivery part 1)
#define MQTT_PUBREL             0x06    //  Client to Server or Server to Client; Publish release  (assured delivery part 2)
#define MQTT_PUBCOMP            0x07    //  Client to Server or Server to Client; Publish complete (assured delivery part 3)
#define MQTT_SUBSCRIBE          0x08    //  Client to Server;                     Client subscribe request
#define MQTT_SUBACK             0x09    //  Server to Client;                     Subscribe acknowledgment
#define MQTT_UNSUBSCRIBE        0x0A    //  Client to Server;                     Unsubscribe request
#define MQTT_UNSUBACK           0x0B    //  Server to Client;                     Unsubscribe acknowledgment
#define MQTT_PINGREQ            0x0C    //  Client to Server;                     Ping request
#define MQTT_PINGRESP           0x0D    //  Server to Client;                     Ping response
#define MQTT_DISCONNECT         0x0E    //  Client to Server;                     Client is disconnecting
#define MQTT_RESERVED_2         0x0F
//======================================================================

//======================================================================
//  MQTT Control Packet Type Flags (LSB Nibble Byte 1)
#define MQTT_FLAGS_CONNECT      0x00    //  Reserved
#define MQTT_FLAGS_CONNACK      0x00    //  Reserved
#define MQTT_FLAGS_PUBLISH      0x00    //  Used in MQTT Rev. 3.1.1
                                        //  0b0000xxxx
                                        //    ||||||||
                                        //    |||||||+-----> RETAIN   -> Publish Retain Flag
                                        //    |||||++------> QoS      -> Publish Quality of Service
                                        //    ||||+--------> DUP      -> Duplicate delivery of a PUBLISH Control Packet
                                        //    ++++---------> Nothing
#define MQTT_FLAGS_PUBACK       0x00    //  Reserved
#define MQTT_FLAGS_PUBREC       0x00    //  Reserved
#define MQTT_FLAGS_PUBREL       0x02    //  Reserved
#define MQTT_FLAGS_PUBCOMP      0x00    //  Reserved
#define MQTT_FLAGS_SUBSCRIBE    0x02    //  Reserved
#define MQTT_FLAGS_SUBACK       0x00    //  Reserved
#define MQTT_FLAGS_UNSUBSCRIBE  0x02    //  Reserved
#define MQTT_FLAGS_UNSUBACK     0x00    //  Reserved                                        
#define MQTT_FLAGS_PINGREQ      0x00    //  Reserved
#define MQTT_FLAGS_PINGRESP     0x00    //  Reserved
#define MQTT_FLAGS_DISCONNECT   0x00    //  Reserved
//======================================================================

//======================================================================
//  MQTT Remaining Lenght
//
// The Remaining Length is the number of bytes remaining within the current packet, including data in the variable header and the payload.
// The Remaining Length does not include the bytes used to encode the Remaining Length.
// The Remaining Length is encoded using a variable length encoding scheme which uses a single byte for values up to 127. Larger values are
// handled as follows. The least significant seven bits of each byte encode the data, and the most significant bit is used to indicate that
// there are following bytes in the representation. Thus each byte encodes 128 values and a "continuation bit". The maximum number of bytes
// in the Remaining Length field is four.
//
// Example 1: Encode lenght 321 bytes
//            First Step  -> 321 % 128 = 65
//            Second Step -> 321 \ 128 = 2
//            Are necessary two bytes to encode decimal value 321
//
//                   MSB          LSB
//             --------------------------
//            | 0xC1 [65+128] |   0x02   |
//             --------------------------
//            |    11000001   | 00000010 |
//             --------------------------
//
//            Decode -> 0xC1 And 0x7F = 0x41 [65 in decimal format] => 65 + 128 x 2 = 321
//
// Example 2: Encode lenght 845 bytes
//            First Step  -> 845 % 128 = 77
//            Second Step -> 845 \ 128 = 6
//            Are necessary two bytes to encode decimal value 845
//
//                  MSB           LSB
//             --------------------------
//            | 0xCD [77+128] |   0x06   |
//             --------------------------
//            |   11001101    | 00000110 |
//             --------------------------
//
//            Decode -> 0xCD And 0x7F = 0x4D [77 in decimal format] => 77 + 128 x 6 = 845
//
// Example 3: Encode lenght 25589 bytes
//            First  Step -> 25589 % 128 = 117
//            Second Step -> 25589 \ 128 = 199
//            Third  Step -> 199   % 128 = 71
//            Fourth Step -> 199   \ 128 = 1
//            Are necessary three bytes to encode decimal value 25589
//
//                    MSB                          LSB
//             -------------------------------------------
//            | 0xF5 [117+128] | 0xC7 [71+128] |   0x01   |
//             -------------------------------------------
//            |    11110101    |   11000111    | 00000001 |
//             -------------------------------------------
//
//            Decode -> 0xF5 And 0x7F = 0x75 [117 in decimal format]
//            Decode -> 0xC7 And 0x7F = 0x47 [71  in decimal format] => 117 + 128 x (71 + 128 x 1) = 25589
//
// Example 4: Encode lenght 3152752 bytes
//            First  Step -> 3152752 % 128 = 112
//            Second Step -> 3152752 \ 128 = 24630
//            Third  Step -> 24630   % 128 = 54
//            Fourth Step -> 24630   \ 128 = 192
//            Fifth  Step -> 192     % 128 = 64
//            Sixth  Step -> 192     \ 128 = 1
//            Are necessary four bytes to encode decimal value 3152752
//
//                    MSB                                          LSB
//             -----------------------------------------------------------
//            | 0xF0 [112+128] | 0xB6 [54+128] | 0xC0 [64+128] |   0x01   |
//             -----------------------------------------------------------
//            |    11110000    |   10110110    |   11000000    | 00000001 |
//             -----------------------------------------------------------
//
//            Decode -> 0xF0 And 0x7F = 0x70 [112 in decimal format]
//            Decode -> 0xB6 And 0x7F = 0x36 [54  in decimal format]
//            Decode -> 0xC0 And 0x7F = 0x40 [64  in decimal format] => 112 + 128 x [54 + 128 x (64 + 128 x 1)] = 3152752
//
//======================================================================

//======================================================================
//  MQTT Variable Header
//
// Some types of MQTT Control Packets contain a variable header component. It resides between the fixed header
// and the payload. The content of the variable header varies depending on the Packet type
//
//  ---------------------------------------------
// |     Bit     | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//  ---------------------------------------------
// | Byte 1      |     Packet Identifier MSB     |
//  ---------------------------------------------
// | Byte 2...   |     Packet Identifier LSB     |
//  ---------------------------------------------
//
// Control Packets that require a Packet Identifier are listed below:
//
// - PUBLISH (if QoS > 0)
// - PUBACK
// - PUBREC
// - PUBREL
// - PUBCOMP
// - SUBSCRIBE
// - SUBACK
// - UNSUBSCRIBE
// - UNSUBACK
//
//======================================================================

//======================================================================
//  MQTT Payload
//
// Some MQTT Control Packets contain a payload as the final part of the packet
// In the case of the PUBLISH packet this is the Application Message.
// Below a list of Control Packets that contain a Payload:
//
// - CONNECT      (Required)
// - PUBLISH      (Optional)
// - SUBSCRIBE    (Required) 
// - SUBACK       (Required)
// - UNSUBSCRIBE  (Required)
//
//======================================================================

//======================================================================
//  Costants used with the MQTT Control Packets CONNECT
//
#define MQTT_CONNECT_FIXED_LEN          10      //  Fixed length to add a Payload Length
#define MQTT_CONNECT_PROTOCOL_NAME_LEN  4       //  Protocol Name Length
#define MQTT_CONNECT_PROTOCOL_LEVEL     4       //  The 8 bit unsigned value that represents the revision level of the protocol used by the Client. The value of the
                                                //  Protocol Level field for the version 3.1.1 of the protocol is 4 (0x04). The Server MUST respond to the CONNECT
                                                //  Packet with a CONNACK return code 0x01 (unacceptable protocol level) and then disconnect the Client if the Protocol
                                                //  Level is not supported by the Server [MQTT-3.1.2-2]
												//  Note: The value of the Protocol Level field for the version 5.0.0 of the protocol is 5 (0x05).
#define MQTT_CONNECT_KEEP_ALIVE         200     //  The Keep Alive is a time interval measured in seconds. Expressed as a 16-bit word, it is the maximum time interval that
                                                //  is permitted to elapse between the point at which the Client finishes transmitting one Control Packet and the point it
                                                //  starts sending the next. It is the responsibility of the Client to ensure that the interval between Control Packets
                                                //  being sent does not exceed the Keep Alive value. In the absence of sending any other Control Packets, the Client MUST
                                                //  send a PINGREQ Packet [MQTT-3.1.2-23]
                                                //  The Client can send PINGREQ at any time, irrespective of the Keep Alive value, and use the PINGRESP to determine that
                                                //  the network and the Server are working.
                                                //  If the Keep Alive value is non-zero and the Server does not receive a Control Packet from the Client within one and
                                                //  a half times the Keep Alive time period, it MUST disconnect the Network Connection to the Client as if the network
                                                //  had failed [MQTT-3.1.2-24]
                                                //  If a Client does not receive a PINGRESP Packet within a reasonable amount of time after it has sent a PINGREQ, it
                                                //  SHOULD close the Network Connection to the Server
                                                //  A Keep Alive value of zero (0) has the effect of turning off the keep alive mechanism. This means that, in this case,
                                                //  the Server is not required to disconnect the Client on the grounds of inactivity.

//======================================================================

//======================================================================
//  Costants used with the MQTT CONNACK Packet
//
#define MQTT_CONNACK_REMAINING_LEN      2

#define MQTT_CONNACK_ACCEPTED                                 0x00      //  Connection accepted
#define MQTT_CONNACK_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION    0x01      //  The Server does not support the level of the MQTT protocol requested by the Client
#define MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED              0x02      //  The Client identifier is correct UTF-8 but not allowed by the Server
#define MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE               0x03      //  The Network Connection has been made but the MQTT service is unavailable
#define MQTT_CONNACK_REFUSED_BAD_USERNAME_OR_PASSWORD         0x04      //  The data in the user name or password is malformed
#define MQTT_CONNACK_REFUSED_NOT_AUTHORIZED                   0x05      //  The Client is not authorized to connect
//======================================================================

//======================================================================
//	Constants used with the MQTT PUBLISH ACK - REC - REL or COMP Packet  
#define MQTT_PUB_ACK_REMAINING_LEN      2
#define MQTT_PUB_REC_REMAINING_LEN      2
#define MQTT_PUB_REL_REMAINING_LEN      2
#define MQTT_PUB_COMP_REMAINING_LEN     2
#define MQTT_SUB_ACK_FIXED_LEN          2
#define MQTT_UNSUB_ACK_REMAINING_LEN    2

#define MQTT_PUB_QOS_0         			0
#define MQTT_PUB_QOS_1         			1
#define MQTT_PUB_QOS_2         			2

#define MQTT_SUB_QOS_0         			0
#define MQTT_SUB_QOS_1         			1
#define MQTT_SUB_QOS_2         			2
//======================================================================

//======================================================================
//  Costants used with the MQTT SUBSCRIBE Packet
//
#define MQTT_SUBSCRIBE_MAX_TOPIC	  		4		//	Max number of subscribe topic for this library
#define MQTT_UNSUBSCRIBE_MAX_TOPIC	  		4		//	Max number of unsubscribe topic for this library

#define MQTT_SUBSCRIBE_FIXED_LEN      		2
#define MQTT_UNSUBSCRIBE_FIXED_LEN    		2

#define MQTT_CLIENT_LEN						32		// Client identifier array length
#define MQTT_WILL_TOPIC_LEN					32		// Will Topic array length
#define MQTT_WILL_MESS_LEN					32		// Will message array length
#define MQTT_USERNAME_LEN					32		// User Name array length
#define MQTT_PSWD_LEN						32		// Password array length

#define MQTT_TOPIC_NAME_LEN					64		// Publish and Subscribe Topic Name array length
#define MQTT_PAYLOAD_LEN					192		// Publish Payload array length

#define MQTT_SUBACK_SUCCESS_QOS_0           0x00
#define MQTT_SUBACK_SUCCESS_QOS_1           0x01
#define MQTT_SUBACK_SUCCESS_QOS_2           0x02
#define MQTT_SUBACK_FAILURE                 0x80
//======================================================================

//======================================================================
#define BYTE_TYPE		0x00
#define INTEGER_TYPE	0x01
#define CHAR_TYPE	    0x02
#define STRING_TYPE		0x03		//	Used in "Mqtt_GSM::AddDataToPayload" function to define string data type
//======================================================================

//============================================================================================================================================
class Mqtt_GSM {
  
public:
    //-------------------------------------------------------------------------------------
    struct {
        uint16_t StartAddClientIdentifier;
        uint16_t StartAddWillTopic;
        uint16_t StartAddWillMessage;
        uint16_t StartAddUserName;
		uint16_t StartAddPassword;
		uint16_t StartAddPublishTopicName;
		uint16_t StartAddSubscribeTopicFilter[MQTT_SUBSCRIBE_MAX_TOPIC];
    } EepromAdd;
    void EepromStartAddSetup(void);
	void ReadEepromData(void);
    //-------------------------------------------------------------------------------------
  
    uint32_t MessageLen;		//	Message Length (Fixed Header Length + Payload Length)
	uint16_t PacketIdentifier;	//	Packet Identifier
	int16_t  ReturnCode;	    //  Connect Return code values
	
    //-------------------------------------------------------------------------------------
    // The payload of the CONNECT Packet contains one or more length-prefixed fields, whose presence is determined by the flags in the variable header.
    // These fields, if present, MUST appear in the order Client Identifier, Will Topic, Will Message, User Name, Password [MQTT-3.1.3-1].
    uint16_t KeepAlive;
    uint8_t  ClientIdentifier[MQTT_CLIENT_LEN]; // The Client Identifier (ClientId) identifies the Client to the Server
												// The Server MUST allow ClientIds which are between 1 and 23 UTF-8 encoded bytes in length, and that contain
												// only the characters "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" [MQTT-3.1.3-5]
    uint8_t  WillTopic[MQTT_WILL_TOPIC_LEN];    // If the Will Flag is set to 1, the Will Topic is the next field in the payload. The Will Topic MUST be
												// a UTF-8 encoded string as defined in Section 1.5.3 [MQTT-3.1.3-10].
    uint8_t  WillMessage[MQTT_WILL_MESS_LEN];   // This field consists of a two byte length followed by the payload for the Will Message expressed as a
												// sequence of zero or more bytes. The length gives the number of bytes in the data that follows and
												// does not include the 2 bytes taken up by the length itself
    uint8_t  UserName[MQTT_USERNAME_LEN];       // The User Name MUST be a UTF-8 encoded string as defined in Section 1.5.3 [MQTT-3.1.3-11]. It can be used
												// by the Server for authentication and authorization.
    uint8_t  Password[MQTT_PSWD_LEN];           // The Password field contains 0 to 65535 bytes of binary data prefixed with a two byte length field which
												// indicates the number of bytes used by the binary data (it does not include the two bytes taken up by
												// the length field itself).
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------	
    union ConnectFlags {
        union Flag {
            uint8_t  FlagByte;
            struct {
                uint8_t Reserved        :1;
                uint8_t CleanSession    :1;     // This bit specifies the handling of the Session state. This bit is used to control the lifetime of the Session state
                                                // If CleanSession is set to 0, the Server MUST resume communications with the Client based on state from the current
                                                // Session (as identified by the Client identifier). If there is no Session associated with the Client identifier the
                                                // Server MUST create a new Session. The Client and Server MUST store the Session after the Client and Server are
                                                // disconnected [MQTT-3.1.2-4]. After the disconnection of a Session that had CleanSession set to 0, the Server MUST
                                                // store further QoS 1 and QoS 2 messages that match any subscriptions that the client had at the time of disconnection 
                                                // as part of the Session state [MQTT-3.1.2-5]. It MAY also store QoS 0 messages that meet the same criteria
                                                // If CleanSession is set to 1, the Client and Server MUST discard any previous Session and start a new one. This
                                                // Session lasts as long as the Network Connection. State data associated with this Session MUST NOT be reused in any
                                                // subsequent Session [MQTT-3.1.2-6]
                uint8_t WillFlag        :1;     // If the Will Flag is set to 1 this indicates that, if the Connect request is accepted, a Will Message MUST be stored
                                                // on the Server and associated with the Network Connection. The Will Message MUST be published when the Network Connection
                                                // is subsequently closed unless the Will Message has been deleted by the Server on receipt of a DISCONNECT Packet [MQTT-3.1.2-8]
                                                // If the Will Flag is set to 1, the Will QoS and Will Retain fields in the Connect Flags will be used by the Server, and the
                                                // Will Topic and Will Message fields MUST be present in the payload [MQTT-3.1.2-9]. The Will Message MUST be removed from the
                                                // stored Session state in the Server once it has been published or the Server has received a DISCONNECT packet from the
                                                // Client [MQTT-3.1.2-10]
                                                // If the Will Flag is set to 0 the Will QoS and Will Retain fields in the Connect Flags MUST be set to zero and the Will Topic
                                                // and Will Message fields MUST NOT be present in the payload [MQTT-3.1.2-11]
                                                // If the Will Flag is set to 0, a Will Message MUST NOT be published when this Network Connection ends [MQTT-3.1.2-12]
                uint8_t WillQoS         :2;     // These two bits specify the QoS level to be used when publishing the Will Message
                                                // If the Will Flag is set to 0, then the Will QoS MUST be set to 0 (0x00) [MQTT-3.1.2-13]
                                                // If the Will Flag is set to 1, the value of Will QoS can be 0 (0x00), 1 (0x01), or 2 (0x02). It MUST NOT be 3 (0x03) [MQTT-3.1.2-14]
                uint8_t WillRetain      :1;     // This bit specifies if the Will Message is to be Retained when it is published
                                                // If the Will Flag is set to 0, then the Will Retain Flag MUST be set to 0 [MQTT-3.1.2-15]
                                                // If the Will Flag is set to 1:
                                                //      - If Will Retain is set to 0, the Server MUST publish the Will Message as a non-retained message [MQTT-3.1.2-16]
                                                //      - If Will Retain is set to 1, the Server MUST publish the Will Message as a retained message [MQTT-3.1.2-17]
                uint8_t Password        :1;     // If the User Name Flag is set to 0, a user name MUST NOT be present in the payload [MQTT-3.1.2-18]
                                                // If the User Name Flag is set to 1, a user name MUST be present in the payload [MQTT-3.1.2-19]
                uint8_t UserName        :1;     // If the Password Flag is set to 0, a password MUST NOT be present in the payload [MQTT-3.1.2-20]
                                                // If the Password Flag is set to 1, a password MUST be present in the payload [MQTT-3.1.2-21]
                                                // If the User Name Flag is set to 0, the Password Flag MUST be set to 0 [MQTT-3.1.2-22]
                
            } Bit;  
        } Flag; 
    } ConnectFlags;
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------	
    union ConnAckFlags {
        union Flag {
            uint8_t  FlagByte;
            struct {
                uint8_t Sp          :1;         // Session Present Flag
                                                // If the Server accepts a connection with CleanSession set to 1, the Server MUST set Session Present to 0 in the CONNACK packet
                                                // in addition to setting a zero return code in the CONNACK packet [MQTT-3.2.2-680 1]
                                                // If the Server accepts a connection with CleanSession set to 0, the value set in Session Present depends on whether the Server
                                                // already has stored Session state for the supplied client ID. If the Server has stored Session state, it MUST set Session
                                                // Present to 1 in the CONNACK packet [MQTT-3.2.2-2]. If the Server does not have stored Session state, it MUST set Session
                                                // Present to 0 in the CONNACK packet. This is in addition to setting a zero return code in the CONNACK packet [MQTT-3.2.2-3]
                uint8_t Reserved    :7;
                
            } Bit;  
        } Flag; 
    } ConnAckFlags;
    //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------	
	uint8_t  PublishTopicName[MQTT_TOPIC_NAME_LEN];	// The Topic Name identifies the information channel to which payload data is published
	uint8_t  PublishPayload[MQTT_PAYLOAD_LEN];		// The Payload contains the Application Message that is being published. The content and format of the
													// data is application specific. The length of the payload can be calculated by subtracting the length of the
													// variable header from the Remaining Length field that is in the Fixed Header. It is valid for a PUBLISH
													// Packet to contain a zero length payload
	
    union PublishFlags {
        union Flag {
            uint8_t  FlagByte;
            struct {
                uint8_t Retain      :1;     // If the RETAIN flag is 0, in a PUBLISH Packet sent by a Client to a Server, the Server MUST NOT store
											// the message and MUST NOT remove or replace any existing retained message
											// If the RETAIN flag is set to 1, in a PUBLISH Packet sent by a Client to a Server, the Server MUST store
											// the Application Message and its QoS, so that it can be delivered to future subscribers whose
											// subscriptions match its topic name [MQTT-3.3.1-5]. When a new subscription is established, the last
											// retained message, if any, on each matching topic name MUST be sent to the subscriber [MQTT-3.3.1-6].
											// If the Server receives a QoS 0 message with the RETAIN flag set to 1 it MUST discard any message
											// previously retained for that topic. It SHOULD store the new QoS 0 message as the new retained
											// message for that topic, but MAY choose to discard it at any time - if this happens there will be no retained
											// message for that topic [MQTT-3.3.1-7]. See Section 4.1 for more information on storing state.
											// When sending a PUBLISH Packet to a Client the Server MUST set the RETAIN flag to 1 if a message is
											// sent as a result of a new subscription being made by a Client [MQTT-3.3.1-8]. It MUST set the RETAIN
											// flag to 0 when a PUBLISH Packet is sent to a Client because it matches an established subscription
											// regardless of how the flag was set in the message it received [MQTT-3.3.1-9].
											// A PUBLISH Packet with a RETAIN flag set to 1 and a payload containing zero bytes will be processed as
											// normal by the Server and sent to Clients with a subscription matching the topic name. Additionally any
											// existing retained message with the same topic name MUST be removed and any future subscribers for
											// the topic will not receive a retained message [MQTT-3.3.1-10].
											// not set in the message received by existing Clients. A zero byte retained message MUST NOT be stored
											// as a retained message on the Server [MQTT-3.3.1-11]
				uint8_t QoS         :2;		// This field indicates the level of assurance for delivery of an Application Message. The QoS levels are
											// listed below:
											//    - 0 At most once delivery
											//    - 1 At least once delivery
											//    - 2 Exactly once delivery
											//    - 3 Reserved - must not be used
				uint8_t Dup         :1;		// If the DUP flag is set to 0, it indicates that this is the first occasion that the Client or Server has attempted
											// to send this MQTT PUBLISH Packet. If the DUP flag is set to 1, it indicates that this might be re-delivery
											// of an earlier attempt to send the Packet
                uint8_t Reserved    :4;
                
            } Bit;  
        } Flag; 
    } PublishFlags;
    //-------------------------------------------------------------------------------------
    
	//-------------------------------------------------------------------------------------
	//	Subscribe and Unsubscribe variables. The maximum number of topic managed with this library is 4
	//	The maximum length of a single topic filter is 64. Pay attention at the maximum length of the
	//	UART array used
	
	struct {
		uint8_t  TopicFilter[MQTT_TOPIC_NAME_LEN];
		uint8_t  TopicFilterLen;
		int8_t   Qos;
		uint8_t  AckRetCode;
		union Flags {       
			uint8_t  FlagByte;
			struct {
				uint8_t EnFilter	:1;
				uint8_t Free 	   	:7; // Free
			} Bit;      
		} Flags;
	} Subscribe[MQTT_SUBSCRIBE_MAX_TOPIC];
	
	struct {
		uint8_t  TopicFilter[MQTT_TOPIC_NAME_LEN];
		uint8_t  TopicFilterLen;
		union Flags {       
			uint8_t  FlagByte;
			struct {
				uint8_t DisFilter	:1;
				uint8_t Free 	 	:7; // Free
			} Bit;      
		} Flags;
	} Unsubscribe[MQTT_SUBSCRIBE_MAX_TOPIC];
	//-------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------
	//	Structure used to decode Mqttt cmd received from the server
    struct {
        uint8_t   MqttCmd;									// Mqtt received
        uint32_t  MessageLen;								// Mqtt message Length (Fixed Header Length + Payload Length)
        uint16_t  PacketIdentifier;							// Mqtt Packet Identifier
		uint16_t  TopicLen;									// Mqtt Topic length (Mqtt PUBLISH cmd))
		uint8_t	  TopicName[MQTT_TOPIC_NAME_LEN];			// Mqtt Topic Name (Mqtt PUBLISH cmd))
		uint8_t   Payload[MQTT_PAYLOAD_LEN];				// Mqtt Payload (Mqtt PUBLISH cmd))
		uint8_t   ConnectReturnCode;						// 0x00 - Connection accepted
															// 0x01 - The Server does not support the level of the MQTT protocol requested by the Client
															// 0x02 - The Client identifier is correct UTF-8 but not allowed by the Server
															// 0x03 - The Network Connection has been made but the MQTT service is unavailable
															// 0x04 - The data in the user name or password is malformed
															// 0x05 - The Client is not authorized to connect
		uint8_t	  SubRetCode[MQTT_SUBSCRIBE_MAX_TOPIC];		// 0x00 - Success - Maximum QoS 0
															// 0x01 - Success - Maximum QoS 1
															// 0x02 - Success - Maximum QoS 2
															// 0x80 - Failure
		uint8_t	  UnsubRetCode[MQTT_SUBSCRIBE_MAX_TOPIC];	// 0x00 - Success - Maximum QoS 0
															// 0x01 - Success - Maximum QoS 1
															// 0x02 - Success - Maximum QoS 2
															// 0x80 - Failure
		union Flags {       
			uint8_t  FlagByte;
			struct {
				uint8_t ConnectAckSpFlag	:1;
				uint8_t Free 	    		:2; // Free
                uint8_t PubRetain		    :1;
				uint8_t PubQoS         		:2;
				uint8_t PubDup         		:1;
				uint8_t MalformedLen		:1; // If "1" then the message received it was coded with a malformed length
			} Bit;      
		} Flags;
    } DecodePacketReceived;
	//-------------------------------------------------------------------------------------
    
	void      InitData(void);

    void      Packets_CONNECT(uint8_t * DataPnt);
	void      Packets_DISCONNECT(uint8_t * DataPnt);
	void      Packets_CONNACK(uint8_t * DataPnt);
	void      Packets_PUBLISH(uint8_t * DataPnt);
	void      Packets_PUBACK(uint8_t * DataPnt);
	void      Packets_PUBREC(uint8_t * DataPnt);
	void      Packets_PUBREL(uint8_t * DataPnt);
	void      Packets_PUBCOMP(uint8_t * DataPnt);
	void      Packets_SUBSCRIBE(uint8_t * DataPnt);
	void      Packets_SUBACK(uint8_t * DataPnt);
	void      Packets_UNSUBSCRIBE(uint8_t * DataPnt);
	void      Packets_UNSUBACK(uint8_t * DataPnt);
    void      Packets_PINGREQ(uint8_t * DataPnt);
	void      Packets_PINGRESP(uint8_t * DataPnt);
	
	uint8_t * DecodeMqttPacketReceived(uint8_t * DataPnt, uint8_t MqttCmd);
	
	void      DummyCalcPayloadLen(uint8_t MqttCmd);
	
private:
	uint32_t  CalcPayloadLen(uint8_t MqttCmd);
	uint8_t   CalcStringArrayLen(uint8_t * ArrayPnt, uint8_t ArrayLen);
    uint8_t * EncodePacketLen(uint8_t * DataPnt, uint32_t PayloadLen);
	uint8_t * DecodePacketLen(uint8_t * DataPnt);
	uint8_t * AddDataToPayload(uint8_t * DataPnt, uint8_t * ArrayPnt, uint16_t ArrayLen, uint8_t MqttCmd, uint8_t DataType);
	uint8_t * ReadPacketIdentifier(uint8_t * DataPnt);
	uint8_t * ReadTopicLen(uint8_t * DataPnt);
	uint8_t * ExtractCharArray(uint8_t * DataPntIn, uint8_t * DataPntOut, uint16_t MaxLenOut, uint16_t DataLenIn);
};

extern Mqtt_GSM Mqtt;  

#endif