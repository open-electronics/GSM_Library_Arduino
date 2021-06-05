/*********************************************************************
 *
 *       MQTT for GSM
 *
 *********************************************************************
 * FileName:        MQTT_GSM.cpp
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

#include "GenericCmd_GSM.h"
#include "MQTT_GSM.h"
#include "TcpIpCmd_GSM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/****************************************************************************
 * Function:        Packets_CONNECT (Client to Server)
 *
 * Overview:        This function is used to create a MQTT CONNECT Packet.
 *                  After a Network Connection is established by a Client to a Server, the first Packet sent
 *                  from the Client to the Server MUST be a CONNECT Packet
 *                  A Client can only send the CONNECT Packet once over a Network Connection. The Server MUST
 *                  process a second CONNECT Packet sent from a Client as a protocol violation and disconnect the Client
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_CONNECT(uint8_t * DataPnt) {
	Mqtt.MessageLen = (MQTT_CONNECT_FIXED_LEN + Mqtt.CalcPayloadLen(MQTT_CONNECT));
    *(uint8_t *)DataPnt++ = (MQTT_CONNECT << 4) | (MQTT_FLAGS_CONNECT & 0x0F);
    DataPnt = Mqtt.EncodePacketLen(DataPnt, Mqtt.MessageLen);
    *(uint8_t *)DataPnt++ = MQTT_CONNECT_PROTOCOL_NAME_LEN & 0xF0;      
    *(uint8_t *)DataPnt++ = MQTT_CONNECT_PROTOCOL_NAME_LEN;
    *(uint8_t *)DataPnt++ = ASCII_M;
    *(uint8_t *)DataPnt++ = ASCII_Q;
    *(uint8_t *)DataPnt++ = ASCII_T;
    *(uint8_t *)DataPnt++ = ASCII_T;
    *(uint8_t *)DataPnt++ = MQTT_CONNECT_PROTOCOL_LEVEL;
    *(uint8_t *)DataPnt++ = Mqtt.ConnectFlags.Flag.FlagByte;
    *(uint8_t *)DataPnt++ = (Mqtt.KeepAlive >> 8);
    *(uint8_t *)DataPnt++ = (Mqtt.KeepAlive & 0xFF);
    	
	//--------------------------------------
	//	Add MQTT CLIENT ID
	DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.ClientIdentifier[0], (Mqtt.CalcStringArrayLen(&Mqtt.ClientIdentifier[0], (sizeof(Mqtt.ClientIdentifier)/sizeof(Mqtt.ClientIdentifier[0])))), MQTT_CONNECT, STRING_TYPE);
	//--------------------------------------
	
	if (Mqtt.ConnectFlags.Flag.Bit.WillFlag == 1) {
		//--------------------------------------
		//	Add MQTT WILL TOPIC
		DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.WillTopic[0], (Mqtt.CalcStringArrayLen(&Mqtt.WillTopic[0], (sizeof(Mqtt.WillTopic)/sizeof(Mqtt.WillTopic[0])))), MQTT_CONNECT, STRING_TYPE);
		//--------------------------------------
		
		//--------------------------------------
		//	Add MQTT WILL MESSAGE
		DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.WillMessage[0], (Mqtt.CalcStringArrayLen(&Mqtt.WillMessage[0], (sizeof(Mqtt.WillMessage)/sizeof(Mqtt.WillMessage[0])))), MQTT_CONNECT, STRING_TYPE);
		//--------------------------------------
	}
	if (Mqtt.ConnectFlags.Flag.Bit.UserName == 1) {
		//--------------------------------------
		//	Add MQTT USER NAME		
		DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.UserName[0], (Mqtt.CalcStringArrayLen(&Mqtt.UserName[0], (sizeof(Mqtt.UserName)/sizeof(Mqtt.UserName[0])))), MQTT_CONNECT, STRING_TYPE);
		//--------------------------------------
	}
	if (Mqtt.ConnectFlags.Flag.Bit.Password == 1) {
		//--------------------------------------
		//	Add MQTT PASSWORD		
		DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.Password[0], (Mqtt.CalcStringArrayLen(&Password[0], (sizeof(Mqtt.Password)/sizeof(Mqtt.Password[0])))), MQTT_CONNECT, STRING_TYPE);
		//--------------------------------------
	}
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_CONNACK  (Server To Client)
 *
 * Overview:        The CONNACK Packet is the packet sent by the Server in response to a CONNECT Packet received
 *                  from a Client. The first packet sent from the Server to the Client MUST be a CONNACK Packet [MQTT-657 3.2.0-1].
 *                  If the Client does not receive a CONNACK Packet from the Server within a reasonable amount of time,
 *                  the Client SHOULD close the Network Connection. A "reasonable" amount of time depends on the type of
 *                  application and the communications infrastructure.
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_CONNACK(uint8_t * DataPnt) {
    *(uint8_t *)DataPnt++ = (MQTT_CONNACK << 4) | (MQTT_FLAGS_CONNACK & 0x0F);
    *(uint8_t *)DataPnt++ = MQTT_CONNACK_REMAINING_LEN;
    *(uint8_t *)DataPnt++ = Mqtt.ConnAckFlags.Flag.FlagByte;      
    *(uint8_t *)DataPnt++ = Mqtt.ReturnCode;
	
	Mqtt.MessageLen = 4;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_DISCONNECT (Client to Server)
 *
 * Overview:        This function is used to create a MQTT DISCONNECT Packet.
 *                  The DISCONNECT Packet is the final Control Packet sent from the Client to the Server.
 *                  It indicates that the Client is disconnecting cleanly.
 *                  After sending a DISCONNECT Packet the Client:
 *                  	- MUST close the Network Connection
 *                  	- MUST NOT send any more Control Packets on that Network Connection
 *                  On receipt of DISCONNECT the Server:
 *                  	- MUST discard any Will Message associated with the current connection without
 *                        publishing it, as described in Section 3.1.2.5
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_DISCONNECT(uint8_t * DataPnt) {	
	*(uint8_t *)DataPnt++ = (MQTT_DISCONNECT << 4) | (MQTT_FLAGS_DISCONNECT & 0x0F);
	*(uint8_t *)DataPnt   = 0;
	
	Mqtt.MessageLen = 2;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PUBLISH (Client to Server) or (Server to Client)
 *
 * Overview:        This function is used to create a MQTT PUBLISH Packet.
 *                  A PUBLISH Control Packet is sent from a Client to a Server or
 *                  from Server to a Client to transport an Application Message.
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt    = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PUBLISH(uint8_t * DataPnt) {	
	Mqtt.MessageLen = Mqtt.CalcPayloadLen(MQTT_PUBLISH);
    *(uint8_t *)DataPnt++ = (MQTT_PUBLISH << 4) | (Mqtt.PublishFlags.Flag.FlagByte & 0x0F);
    DataPnt = Mqtt.EncodePacketLen(DataPnt, Mqtt.MessageLen);
	DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.PublishTopicName[0], (Mqtt.CalcStringArrayLen(&Mqtt.PublishTopicName[0], (sizeof(Mqtt.PublishTopicName)/sizeof(Mqtt.PublishTopicName[0])))), MQTT_PUBLISH, STRING_TYPE);
	if (Mqtt.PublishFlags.Flag.Bit.QoS > 0) {
		*(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);
		*(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	}
	DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.PublishPayload[0], (Mqtt.CalcStringArrayLen(&Mqtt.PublishPayload[0], (sizeof(Mqtt.PublishPayload)/sizeof(Mqtt.PublishPayload[0])))), (MQTT_PUBLISH + 0x10), STRING_TYPE);
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PUBACK  (Server To Client)
 *
 * Overview:        The PUBACK Packet is the response to a PUBLISH Packet with QoS level 1
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PUBACK(uint8_t * DataPnt) {	
    *(uint8_t *)DataPnt++ = (MQTT_PUBACK << 4) | (MQTT_FLAGS_PUBACK & 0x0F);
    *(uint8_t *)DataPnt++ = MQTT_PUB_ACK_REMAINING_LEN;
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	Mqtt.MessageLen = 4;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PUBREC ((QoS 2 publish received, part 1) (Server To Client)
 *
 * Overview:        The PUBREC Packet is the response to a PUBLISH Packet with QoS 2. It is the
 *                  second packet of the QoS 2 protocol exchange.
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PUBREC(uint8_t * DataPnt) {	
    *(uint8_t *)DataPnt++ = (MQTT_PUBREC << 4) | (MQTT_FLAGS_PUBREC & 0x0F);
    *(uint8_t *)DataPnt++ = MQTT_PUB_REC_REMAINING_LEN;
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	Mqtt.MessageLen = 4;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PUBREL ((QoS 2 publish received, part 2) (Client To Server)
 *
 * Overview:        A PUBREL Packet is the response to a PUBREC Packet. It is the third packet
 *                  of the QoS 2 protocol exchange
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PUBREL(uint8_t * DataPnt) {
    *(uint8_t *)DataPnt++ = (MQTT_PUBREL << 4) | (MQTT_FLAGS_PUBREL & 0x0F);
    *(uint8_t *)DataPnt++ = MQTT_PUB_REL_REMAINING_LEN;
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	Mqtt.MessageLen = 4;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PUBCOMP  (Server To Client)
 *
 * Overview:        The PUBCOMP Packet is the response to a PUBREL Packet. It is
 *                  the fourth and final packet of the QoS 2 protocol exchange
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PUBCOMP(uint8_t * DataPnt) {	
    *(uint8_t *)DataPnt++ = (MQTT_PUBCOMP << 4) | (MQTT_FLAGS_PUBCOMP & 0x0F);
    *(uint8_t *)DataPnt++ = MQTT_PUB_COMP_REMAINING_LEN;
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	Mqtt.MessageLen = 4;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_SUBSCRIBE (Client to Server)
 *
 * Overview:        This function is used to create a MQTT SUBSCRIBE Packet.
 *                  A SUBSCRIBE Control Packet is sent from a Client to a Server to create one or
 *                  more Subscriptions. Each Subscription registers a Client’s interest in one or
 *                  more Topics. The Server sends PUBLISH Packets to the Client in order to forward
 *                  Application Messages that were published to Topics that match these Subscriptions.
 *                  The SUBSCRIBE Packet also specifies (for each Subscription) the maximum QoS with
 *                  which the Server can send Application Messages to the Client 
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_SUBSCRIBE(uint8_t * DataPnt) {
	uint8_t Count = 0;
	
	Mqtt.MessageLen = (MQTT_SUBSCRIBE_FIXED_LEN + Mqtt.CalcPayloadLen(MQTT_SUBSCRIBE));
    *(uint8_t *)DataPnt++ = (MQTT_SUBSCRIBE << 4) | (MQTT_FLAGS_SUBSCRIBE & 0x0F);
    DataPnt = Mqtt.EncodePacketLen(DataPnt, Mqtt.MessageLen);
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	//--------------------------------------
	//	Payload
	do {
		if (Mqtt.Subscribe[Count].Flags.Bit.EnFilter != 0) {
			DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.Subscribe[Count].TopicFilter[0], (Mqtt.CalcStringArrayLen(&Mqtt.Subscribe[Count].TopicFilter[0], (sizeof(Mqtt.Subscribe[Count].TopicFilter)/sizeof(Mqtt.Subscribe[Count].TopicFilter[0])))), MQTT_SUBSCRIBE, STRING_TYPE);
			*(uint8_t *)DataPnt++ = Mqtt.Subscribe[Count].Qos;
		}
	} while (++Count < (sizeof(Mqtt.Subscribe)/sizeof(Mqtt.Subscribe[0])));
	//--------------------------------------
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_SUBACK  (Server To Client)
 *
 * Overview:        The SUBACK is sent by the Server to the Client to confirm receipt and
 *                  processing of a SUBSCRIBE Packet.
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_SUBACK(uint8_t * DataPnt) {
	uint8_t Count = 0;
	
	Mqtt.MessageLen = (MQTT_SUB_ACK_FIXED_LEN + Mqtt.CalcPayloadLen(MQTT_SUBACK));
    *(uint8_t *)DataPnt++ = (MQTT_SUBACK << 4) | (MQTT_FLAGS_SUBACK & 0x0F);
    DataPnt = Mqtt.EncodePacketLen(DataPnt, Mqtt.MessageLen);
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	//--------------------------------------
	//	Payload
	do {
		switch (Mqtt.Subscribe[Count].AckRetCode)
		{
			case MQTT_SUBACK_SUCCESS_QOS_0:
			case MQTT_SUBACK_SUCCESS_QOS_1:
			case MQTT_SUBACK_SUCCESS_QOS_2:
			case MQTT_SUBACK_FAILURE:
				*(uint8_t *)DataPnt++ = Mqtt.Subscribe[Count].AckRetCode;
				break;
			default:
				*(uint8_t *)DataPnt++ = MQTT_SUBACK_FAILURE;
				break;
		}		
	} while (++Count < (sizeof(Mqtt.Subscribe)/sizeof(Mqtt.Subscribe[0])));
	//--------------------------------------		
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_UNSUBSCRIBE (Client to Server)
 *
 * Overview:        This function is used to create a MQTT SUBSCRIBE Packet.
 *                  A SUBSCRIBE Control Packet is sent by the Client to the Server, to unsubscribe from topics
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_UNSUBSCRIBE(uint8_t * DataPnt) {
	uint8_t Count = 0;
		
	Mqtt.MessageLen = (MQTT_UNSUBSCRIBE_FIXED_LEN + Mqtt.CalcPayloadLen(MQTT_UNSUBSCRIBE));
    *(uint8_t *)DataPnt++ = (MQTT_UNSUBSCRIBE << 4) | (MQTT_FLAGS_UNSUBSCRIBE & 0x0F);
    DataPnt = Mqtt.EncodePacketLen(DataPnt, Mqtt.MessageLen);
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	//--------------------------------------
	//	Payload
	do {
		if (Mqtt.Unsubscribe[Count].Flags.Bit.DisFilter != 0) {
			DataPnt = Mqtt.AddDataToPayload(DataPnt, &Mqtt.Unsubscribe[Count].TopicFilter[0], (Mqtt.CalcStringArrayLen(&Mqtt.Unsubscribe[Count].TopicFilter[0], (sizeof(Mqtt.Unsubscribe[Count].TopicFilter)/sizeof(Mqtt.Unsubscribe[Count].TopicFilter[0])))), MQTT_UNSUBSCRIBE, STRING_TYPE);
		}
	} while (++Count < (sizeof(Mqtt.Unsubscribe)/sizeof(Mqtt.Unsubscribe[0])));
	//--------------------------------------
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_UNSUBACK  (Server To Client)
 *
 * Overview:        The UNSUBACK Packet is sent by the Server to the Client to confirm receipt of an UNSUBSCRIBE Packet
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_UNSUBACK(uint8_t * DataPnt) {
	uint8_t Count = 0;
		
    *(uint8_t *)DataPnt++ = (MQTT_UNSUBACK << 4) | (MQTT_FLAGS_UNSUBACK & 0x0F);
    *(uint8_t *)DataPnt++ = MQTT_UNSUB_ACK_REMAINING_LEN;
    *(uint8_t *)DataPnt++ = ((Mqtt.PacketIdentifier >> 8) & 0xFF);      
    *(uint8_t *)DataPnt++ = (Mqtt.PacketIdentifier & 0xFF);
	
	Mqtt.MessageLen = 4;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PINGREQ
 *
 * Overview:        The PINGREQ Packet is sent from a Client to the Server. It can be used to:
 *                  1) Indicate to the Server that the Client is alive in the absence of any other Control Packets being
 *                     sent from the Client to the Server.
 *                  2) Request that the Server responds to confirm that it is alive.
 *                  3) Exercise the network to indicate that the Network Connection is active
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PINGREQ(uint8_t * DataPnt) {	
    *(uint8_t *)DataPnt++ = (MQTT_PINGREQ << 4) | (MQTT_FLAGS_PINGREQ & 0x0F);
    *(uint8_t *)DataPnt++ = 0;
	
	Mqtt.MessageLen = 2;
}
/****************************************************************************/

/****************************************************************************
 * Function:        Packets_PINGRESP
 *
 * Overview:        A PINGRESP Packet is sent by the Server to the Client in response to a PINGREQ Packet. It indicates
 *                  that the Server is alive
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array where save MQTT packet
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
void Mqtt_GSM::Packets_PINGRESP(uint8_t * DataPnt) {	
    *(uint8_t *)DataPnt++ = (MQTT_PINGRESP << 4) | (MQTT_FLAGS_PINGRESP & 0x0F);
    *(uint8_t *)DataPnt++ = 0;
	
	Mqtt.MessageLen = 2;
}
/****************************************************************************/

/****************************************************************************
 * Function:        DecodeMqttCmdReceived
 *
 * Overview:        This function is used to decode MQTT answer received related to
 *                  the MQTT command sent
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt = Pointer to an array which contains the MQTT answer
 *					MqttCmd   = Mqtt command code
 *
 * Command Note:    None
 *
 * Output:          Memory Pointer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a public function
 *****************************************************************************/
uint8_t *  Mqtt_GSM::DecodeMqttPacketReceived(uint8_t * DataPnt, uint8_t MqttCmd) {
	uint8_t Count = 0;

	switch(MqttCmd) {
		case MQTT_CONNACK:		//	Check CONNACK answer
			Mqtt.DecodePacketReceived.MqttCmd    = ((*(uint8_t *)DataPnt++) >> 4);		//	Read Mqtt command
			Mqtt.DecodePacketReceived.MessageLen = *(uint8_t *)DataPnt++;				//	Read Message Length
			
			Mqtt.DecodePacketReceived.Flags.Bit.ConnectAckSpFlag = 0;
			if (((*(uint8_t *)DataPnt++) & 0x01) > 0) {
				Mqtt.DecodePacketReceived.Flags.Bit.ConnectAckSpFlag = 1;
			}
			Mqtt.DecodePacketReceived.ConnectReturnCode = *(uint8_t *)DataPnt;			//	Read PUBLISH cmd return code
			break;
		case MQTT_PUBLISH:		//	Check PUBLISH answer with QoS = 0 related at SUBSCRIBE command sent
			Mqtt.DecodePacketReceived.Flags.Bit.PubRetain = (*(uint8_t *)DataPnt & 0x01);			//	Read Flag Retain
			Mqtt.DecodePacketReceived.Flags.Bit.PubQoS    = ((*(uint8_t *)DataPnt >> 1) & 0x03);	//	Read flags QoS
			Mqtt.DecodePacketReceived.Flags.Bit.PubDup    = ((*(uint8_t *)DataPnt >> 3) & 0x01);	//	Read flag DUP
			Mqtt.DecodePacketReceived.MqttCmd             = ((*(uint8_t *)DataPnt++) >> 4);			//	Read Mqtt command
			DataPnt = Mqtt.DecodePacketLen((uint8_t *)DataPnt);										//	Read Message Length
			DataPnt = Mqtt.ReadTopicLen((uint8_t *)DataPnt);										//	Read Topic Length
			DataPnt = Mqtt.ExtractCharArray((uint8_t *)DataPnt, (uint8_t *)(&Mqtt.DecodePacketReceived.TopicName[0]), MQTT_TOPIC_NAME_LEN, Mqtt.DecodePacketReceived.TopicLen);		//	Read Topic Name		
			if (Mqtt.DecodePacketReceived.Flags.Bit.PubQoS > 0) {
				DataPnt = Mqtt.ReadPacketIdentifier((uint8_t *)DataPnt);							//	Read Packet Identifier
			}
			DataPnt = Mqtt.ExtractCharArray((uint8_t *)DataPnt, (char *)(&Mqtt.DecodePacketReceived.Payload[0]), MQTT_PAYLOAD_LEN, (Mqtt.DecodePacketReceived.MessageLen - Mqtt.DecodePacketReceived.TopicLen - 2));	//  Read Payload
			break;
		case MQTT_PUBACK:		//	Check PUBACK  answer with QoS = 1
		case MQTT_PUBREC:		//	Check PUBREC  answer with QoS = 2
		case MQTT_PUBCOMP:		//  Check PUBCOMP answer with QoS = 2
			Mqtt.DecodePacketReceived.MqttCmd    = ((*(uint8_t *)DataPnt++) >> 4);		//	Read Mqtt command
			Mqtt.DecodePacketReceived.MessageLen = *(uint8_t *)DataPnt++;				//	Read Message Length
			DataPnt = Mqtt.ReadPacketIdentifier((uint8_t *)DataPnt);					//	Read Packet Identifier
			break;
		case MQTT_SUBACK:		//	Check SUBACK answer
			Mqtt.DecodePacketReceived.MqttCmd    = ((*(uint8_t *)DataPnt++) >> 4);		//	Read Mqtt command
			Mqtt.DecodePacketReceived.MessageLen = *(uint8_t *)DataPnt++;				//	Read Message Length
			DataPnt = Mqtt.ReadPacketIdentifier((uint8_t *)DataPnt);					//	Read Packet Identifier
			do {
				Mqtt.DecodePacketReceived.SubRetCode[Count] = *(uint8_t *)DataPnt++;	//  Read SUBSCRIBE Return Code
			} while (++Count < (Mqtt.DecodePacketReceived.MessageLen - 2));
			break;
		case MQTT_UNSUBACK:		//	Check UNSUBACK answer
			Mqtt.DecodePacketReceived.MqttCmd    = ((*(uint8_t *)DataPnt++) >> 4);		//	Read Mqtt command
			Mqtt.DecodePacketReceived.MessageLen = *(uint8_t *)DataPnt++;				//	Read Message Length
			DataPnt = Mqtt.ReadPacketIdentifier((uint8_t *)DataPnt);					//	Read Packet Identifier
			do {
				Mqtt.DecodePacketReceived.UnsubRetCode[Count] = *(uint8_t *)DataPnt++;	//  Read UNSUBSCRIBE Return Code
			} while (++Count < (Mqtt.DecodePacketReceived.MessageLen - 2));
			break;
		case MQTT_PINGRESP:		//	Check PINGRESP answer
			Mqtt.DecodePacketReceived.MqttCmd    = ((*(uint8_t *)DataPnt++) >> 4);		//	Read Mqtt command
			Mqtt.DecodePacketReceived.MessageLen = *(uint8_t *)DataPnt;					//	Read Message Length
			break;
		default:
			break;
	}
	return(DataPnt);
}
uint8_t * Mqtt_GSM::ReadPacketIdentifier(uint8_t * DataPnt) {
	Mqtt.DecodePacketReceived.PacketIdentifier = *(uint8_t *)DataPnt++;
	Mqtt.DecodePacketReceived.PacketIdentifier = ((Mqtt.DecodePacketReceived.PacketIdentifier << 8) & 0xFF00);
	Mqtt.DecodePacketReceived.PacketIdentifier = Mqtt.DecodePacketReceived.PacketIdentifier | *(uint8_t *)DataPnt++;
	return(DataPnt);
}
uint8_t * Mqtt_GSM::ReadTopicLen(uint8_t * DataPnt) {
	Mqtt.DecodePacketReceived.TopicLen = *(uint8_t *)DataPnt++;
	Mqtt.DecodePacketReceived.TopicLen = ((Mqtt.DecodePacketReceived.TopicLen << 8) & 0xFF00);
	Mqtt.DecodePacketReceived.TopicLen = Mqtt.DecodePacketReceived.TopicLen | *(uint8_t *)DataPnt++;
	return(DataPnt);
}
uint8_t * Mqtt_GSM::ExtractCharArray(uint8_t * DataPntIn, uint8_t * DataPntOut, uint16_t MaxLenOut, uint16_t DataLenIn) {
    uint16_t Counter = 0;
    do {
		if (Counter > MaxLenOut) {
			break;
		}
		*(uint8_t *)DataPntOut++ = *(uint8_t *)DataPntIn++;
    } while (++Counter < DataLenIn);	
	return(DataPntIn);
}
/****************************************************************************/

/****************************************************************************
 * Function:        CalcPayloadLen
 *
 * Overview:        This function is used to calculate the payload length
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           MqttCmd = MQTT command code
 *
 * Command Note:    None
 *
 * Output:          Packet Length
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
uint32_t Mqtt_GSM::CalcPayloadLen(uint8_t MqttCmd) {
	uint32_t TempLen = 0;
	uint8_t  Count = 0;
	
	switch (MqttCmd)
	{
		case MQTT_CONNECT:
			TempLen = 0x02 + Mqtt.CalcStringArrayLen(&Mqtt.ClientIdentifier[0], (sizeof(Mqtt.ClientIdentifier)/sizeof(Mqtt.ClientIdentifier[0])));
			if (Mqtt.ConnectFlags.Flag.Bit.WillFlag == 0) {
				Mqtt.ConnectFlags.Flag.Bit.WillQoS = 0;
				Mqtt.ConnectFlags.Flag.Bit.WillRetain = 0;
			} else {
				TempLen = TempLen + 0x02 + Mqtt.CalcStringArrayLen(&Mqtt.WillTopic[0], (sizeof(Mqtt.WillTopic)/sizeof(Mqtt.WillTopic[0])));
				TempLen = TempLen + 0x02 + Mqtt.CalcStringArrayLen(&Mqtt.WillMessage[0], (sizeof(Mqtt.WillMessage)/sizeof(Mqtt.WillMessage[0])));
			}
			if (Mqtt.ConnectFlags.Flag.Bit.UserName == 1) {
				TempLen = TempLen + 0x02 + Mqtt.CalcStringArrayLen(&Mqtt.UserName[0], (sizeof(Mqtt.UserName)/sizeof(Mqtt.UserName[0])));
			}
			if (Mqtt.ConnectFlags.Flag.Bit.Password == 1) {
				TempLen = TempLen + 0x02 + Mqtt.CalcStringArrayLen(&Mqtt.Password[0], (sizeof(Mqtt.Password)/sizeof(Mqtt.Password[0])));
			}
			break;
		case MQTT_PUBLISH:
			TempLen = 0x02 + Mqtt.CalcStringArrayLen(&Mqtt.PublishTopicName[0], (sizeof(Mqtt.PublishTopicName)/sizeof(Mqtt.PublishTopicName[0])));
			if (Mqtt.PublishFlags.Flag.Bit.QoS > 0) {
				TempLen += 2;	// Add Packet Identifier Length
			} else {
				Mqtt.PublishFlags.Flag.Bit.Dup = 0; 
			}
			TempLen = TempLen + Mqtt.CalcStringArrayLen(&Mqtt.PublishPayload[0], (sizeof(Mqtt.PublishPayload)/sizeof(Mqtt.PublishPayload[0])));
			break;
		case MQTT_SUBSCRIBE:
			do {
				if (Mqtt.Subscribe[Count].Flags.Bit.EnFilter != 0) {				
					Mqtt.Subscribe[Count].TopicFilterLen = Mqtt.CalcStringArrayLen(&Mqtt.Subscribe[Count].TopicFilter[0], (sizeof(Mqtt.Subscribe[Count].TopicFilter)/sizeof(Mqtt.Subscribe[Count].TopicFilter[0])));
					if (Mqtt.Subscribe[Count].TopicFilterLen > 0) {
						TempLen = TempLen + 0x03 + Mqtt.Subscribe[Count].TopicFilterLen;
					}
				}
			} while (++Count < (sizeof(Mqtt.Subscribe)/sizeof(Mqtt.Subscribe[0])));
			break;
		case MQTT_UNSUBSCRIBE:
			do {
				if (Mqtt.Unsubscribe[Count].Flags.Bit.DisFilter != 0) {				
					Mqtt.Unsubscribe[Count].TopicFilterLen = Mqtt.CalcStringArrayLen(&Mqtt.Unsubscribe[Count].TopicFilter[0], (sizeof(Mqtt.Unsubscribe[Count].TopicFilter)/sizeof(Mqtt.Unsubscribe[Count].TopicFilter[0])));
					if (Mqtt.Unsubscribe[Count].TopicFilterLen > 0) {
						TempLen = TempLen + 0x02 + Mqtt.Unsubscribe[Count].TopicFilterLen;
					}
				}
			} while (++Count < (sizeof(Mqtt.Unsubscribe)/sizeof(Mqtt.Unsubscribe[0])));
			break;
		case MQTT_SUBACK:
			do {
				switch (Mqtt.Subscribe[Count].AckRetCode)
				{
					case MQTT_SUBACK_SUCCESS_QOS_0:
					case MQTT_SUBACK_SUCCESS_QOS_1:
					case MQTT_SUBACK_SUCCESS_QOS_2:
					case MQTT_SUBACK_FAILURE:
						TempLen++;
						break;
					default:
						break;
				}
			} while (++Count < (sizeof(Mqtt.Subscribe)/sizeof(Mqtt.Subscribe[0])));
			break;
		default:
			break;
	}
	return(TempLen);
}
/****************************************************************************/

/****************************************************************************
 * Function:        DummyCalcPayloadLen
 *
 * Overview:        This function is used to calculate the payload length
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           MqttCmd = MQTT command code
 *
 * Command Note:    None
 *
 * Output:          None
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
void Mqtt_GSM::DummyCalcPayloadLen(uint8_t MqttCmd) {
	Mqtt.MessageLen = Mqtt.CalcPayloadLen(MqttCmd);
}
/****************************************************************************/
	
/****************************************************************************
 * Function:        EncodePacketLen
 *
 * Overview:        This function is used to format the Remaining Length
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt    = Pointer to an array where save MQTT packet
 *                    PayloadLen = Payload lenght
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *
 *  -------------------------------------------------------------------------
 *   MQTT Remaining Lenght. How to calc it
 *  
 *  The Remaining Length is the number of bytes remaining within the current packet, including data in the variable header and the payload.
 *  The Remaining Length does not include the bytes used to encode the Remaining Length.
 *  The Remaining Length is encoded using a variable length encoding scheme which uses a single byte for values up to 127. Larger values are
 *  handled as follows. The least significant seven bits of each byte encode the data, and the most significant bit is used to indicate that
 *  there are following bytes in the representation. Thus each byte encodes 128 values and a "continuation bit". The maximum number of bytes
 *  in the Remaining Length field is four.
 *  
 *  Example 1: Encode lenght 321 bytes
 *             First  Step -> 321 % 128 = 65		    Remainder
 *             Second Step -> 321 \ 128 = 2            Quotient
 *             Are necessary two bytes to encode decimal value 321
 *  
 *                    MSB          LSB
 *              --------------------------
 *             | 0xC1 [65+128] |   0x02   |
 *              --------------------------
 *             |    11000001   | 00000010 |
 *              --------------------------
 *  
 *             Decode -> 0xC1 And 0x7F = 0x41 [65 in decimal format] => 65 + 128 x 2 = 321
 *  
 *  Example 2: Encode lenght 845 bytes
 *             First  Step -> 845 % 128 = 77		    Remainder
 *             Second Step -> 845 \ 128 = 6            Quotient
 *             Are necessary two bytes to encode decimal value 845
 *  
 *                   MSB           LSB
 *              --------------------------
 *             | 0xCD [77+128] |   0x06   |
 *              --------------------------
 *             |   11001101    | 00000110 |
 *              --------------------------
 *  
 *             Decode -> 0xCD And 0x7F = 0x4D [77 in decimal format] => 77 + 128 x 6 = 845
 *  
 *  Example 3: Encode lenght 25589 bytes
 *             First  Step -> 25589 % 128 = 117		Remainder
 *             Second Step -> 25589 \ 128 = 199        Quotient
 *             Third  Step -> 199   % 128 = 71         Remainder
 *             Fourth Step -> 199   \ 128 = 1          Quotient
 *             Are necessary three bytes to encode decimal value 25589
 *                                                     
 *                     MSB                          LSB
 *              -------------------------------------------
 *             | 0xF5 [117+128] | 0xC7 [71+128] |   0x01   |
 *              -------------------------------------------
 *             |    11110101    |   11000111    | 00000001 |
 *              -------------------------------------------
 *  
 *             Decode -> 0xF5 And 0x7F = 0x75 [117 in decimal format]
 *             Decode -> 0xC7 And 0x7F = 0x47 [71  in decimal format] => 117 + 128 x (71 + 128 x 1) = 25589
 *  
 *  Example 4: Encode lenght 3152752 bytes
 *             First  Step -> 3152752 % 128 = 112		Remainder
 *             Second Step -> 3152752 \ 128 = 24630	Quotient
 *             Third  Step -> 24630   % 128 = 54		Remainder
 *             Fourth Step -> 24630   \ 128 = 192		Quotient
 *             Fifth  Step -> 192     % 128 = 64		Remainder
 *             Sixth  Step -> 192     \ 128 = 1		Quotient
 *             Are necessary four bytes to encode decimal value 3152752
 *  
 *                     MSB                                          LSB
 *              -----------------------------------------------------------
 *             | 0xF0 [112+128] | 0xB6 [54+128] | 0xC0 [64+128] |   0x01   |
 *              -----------------------------------------------------------
 *             |    11110000    |   10110110    |   11000000    | 00000001 |
 *              -----------------------------------------------------------
 *  
 *             Decode -> 0xF0 And 0x7F = 0x70 [112 in decimal format]
 *             Decode -> 0xB6 And 0x7F = 0x36 [54  in decimal format]
 *             Decode -> 0xC0 And 0x7F = 0x40 [64  in decimal format] => 112 + 128 x [54 + 128 x (64 + 128 x 1)] = 3152752
 *  
 *  -------------------------------------------------------------------------
 *****************************************************************************/
uint8_t * Mqtt_GSM::EncodePacketLen(uint8_t * DataPnt, uint32_t PayloadLen) {
    do {
		Mqtt.MessageLen++;	//	Add the ref to the packet length parameter
        *(uint8_t *)DataPnt = PayloadLen % 0x80;
        PayloadLen          = PayloadLen / 0x80;
        if (PayloadLen > 0x00) {
            *(uint8_t *)DataPnt |= 0x80;
            DataPnt++;
        } else {
            DataPnt++;
        }
    } while (PayloadLen > 0x00);
	Mqtt.MessageLen++;	//	Add the Mqtt command code to the packet length
    return(DataPnt);
}
/****************************************************************************/

/****************************************************************************
 * Function:        EncodePacketLen
 *
 * Overview:        This function is used to format the Remaining Length
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           * DataPnt    = Pointer to an array where save MQTT packet
 *                    PayloadLen = Payload lenght
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
uint8_t * Mqtt_GSM::DecodePacketLen(uint8_t * DataPnt) {
	uint32_t Multiplier = 1;
	
	Mqtt.DecodePacketReceived.Flags.Bit.MalformedLen = 0;	//	Set bit to "0"
	Mqtt.DecodePacketReceived.MessageLen 			  = 0;
	
    do {	
		Mqtt.DecodePacketReceived.MessageLen += (*(uint8_t *)DataPnt & 0x7F) * Multiplier;
		Multiplier *= 0x80;
		if (Multiplier > 0x200000) {
			//	Malformed Remaining length
			Mqtt.DecodePacketReceived.Flags.Bit.MalformedLen = 1;
			break;
		}
    } while ((*(uint8_t *)DataPnt++ & 0x80) > 0);
    return(DataPnt);	
}
/****************************************************************************/
	
/****************************************************************************
 * Function:        AddDataToPayload
 *
 * Overview:        This function is used to add data Payload
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           DataPnt  = Array pointer (Destination Array)
 *                  ArrayPnt = Array Pointer (Starting Array)
 *					ArrayLen = Length of array than contain the data
 *                  MqttCmd  = MQTT code
 *					DataType = Type of array to process
 *
 * Command Note:    None
 *
 * Output:          Return Array Pointer
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
uint8_t *  Mqtt_GSM::AddDataToPayload(uint8_t * DataPnt, uint8_t * ArrayPnt, uint16_t ArrayLen, uint8_t MqttCmd, uint8_t DataType) {
	uint16_t Count = 0;

	switch (MqttCmd)
	{
		case MQTT_CONNECT:
		case MQTT_PUBLISH:
		case MQTT_UNSUBSCRIBE:
		case MQTT_SUBSCRIBE:
			*(uint8_t *)DataPnt++ = (ArrayLen >> 8);
			*(uint8_t *)DataPnt++ = (ArrayLen & 0xFF);
			break;
		default:
			break;
	}

	do {
		switch (DataType)
		{
			case BYTE_TYPE:
				break;
			case INTEGER_TYPE:
				break;
			case CHAR_TYPE:
				break;
			case STRING_TYPE:
				if (*(uint8_t *)ArrayPnt == 0x00) {
					break;
				}
				break;
		}
		*(uint8_t *)DataPnt++ = *(uint8_t *)ArrayPnt++;
	} while (++Count < ArrayLen);
	return(DataPnt);
}
/****************************************************************************/

/****************************************************************************
 * Function:        CalcStringArrayLen
 *
 * Overview:        This function is used to calc the length of an array (String Type)
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           ArrayPnt = Array pointer
 *					ArrayLen = Length of array than contain the data
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a private function
 *****************************************************************************/
uint8_t Mqtt_GSM::CalcStringArrayLen(uint8_t * ArrayPnt, uint8_t ArrayLen) {
	uint16_t Count = 0;

	do {
		if (*(uint8_t *)ArrayPnt++ == 0x00) { break; }
	} while (++Count < ArrayLen);
	return(Count);	
}
/****************************************************************************/

/****************************************************************************
 * Function:        InitData
 *
 * Overview:        This function is used to initialize the arrays
 *
 * PreCondition:    None
 *
 * GSM cmd syntax:  None
 *
 * Input:           None
 *
 * Command Note:    None
 *
 * Output:          none
 *
 * GSM answer det:  None
 *
 * Side Effects:    None
 *
 * Note:            This is a Public function
 *****************************************************************************/
void Mqtt_GSM::InitData(void) {
	uint8_t Count   = 0;
	uint8_t Count_2 = 0;
	
	Mqtt.MessageLen                 = 0;	// Default value
	Mqtt.PacketIdentifier           = 0;	// Default value
	Mqtt.ReturnCode                 = 255;	// Default value
	Mqtt.KeepAlive                  = 600;	// Default value
	Mqtt.ConnectFlags.Flag.FlagByte = 0;	// Default value
	Mqtt.ConnAckFlags.Flag.FlagByte = 0;    // Default value
	Mqtt.PublishFlags.Flag.FlagByte = 0;	// Default value
	
	Mqtt.DecodePacketReceived.MqttCmd           = 0;
	Mqtt.DecodePacketReceived.MessageLen        = 0;
	Mqtt.DecodePacketReceived.PacketIdentifier  = 0;
	Mqtt.DecodePacketReceived.TopicLen          = 0;
	Mqtt.DecodePacketReceived.ConnectReturnCode = 255;
	Mqtt.DecodePacketReceived.Flags.FlagByte    = 0;
	do {
		Mqtt.DecodePacketReceived.SubRetCode[Count]   = 255;
		Mqtt.DecodePacketReceived.UnsubRetCode[Count] = 255;
	} while (++Count < (sizeof(Mqtt.DecodePacketReceived.SubRetCode)/sizeof(Mqtt.DecodePacketReceived.SubRetCode[0])));

	Count = 0;	
	do {
		Mqtt.ClientIdentifier[Count] = 0;
		Mqtt.WillTopic[Count]        = 0;
		Mqtt.WillMessage[Count]      = 0;
		Mqtt.UserName[Count]         = 0;
		Mqtt.Password[Count]         = 0;
	} while (++Count < (sizeof(Mqtt.ClientIdentifier)/sizeof(Mqtt.ClientIdentifier[0])));
	
	Count = 0;
	do {
		Mqtt.PublishTopicName[Count] = 0;
	} while (++Count < (sizeof(Mqtt.PublishTopicName)/sizeof(Mqtt.PublishTopicName[0])));	

	Count = 0;
	do {
		Mqtt.PublishPayload[Count] = 0;
	} while (++Count < (sizeof(Mqtt.PublishPayload)/sizeof(Mqtt.PublishPayload[0])));
	
	Count = 0;
	do {
		Mqtt.Subscribe[Count].TopicFilterLen        = 0;
		Mqtt.Unsubscribe[Count].TopicFilterLen      = 0;
		Mqtt.Subscribe[Count].Qos                   = 255;
		Mqtt.Subscribe[Count].AckRetCode            = 0;
		Mqtt.Subscribe[Count].Flags.Bit.EnFilter    = 0;
		Mqtt.Unsubscribe[Count].Flags.Bit.DisFilter = 0;
	} while (++Count < (sizeof(Mqtt.Subscribe)/sizeof(Mqtt.Subscribe[0])));
	
	Count = 0;
	do {
		do {
			Mqtt.Subscribe[Count].TopicFilter[Count_2]   = 0;
			Mqtt.Unsubscribe[Count].TopicFilter[Count_2] = 0;
		} while (++Count_2 < (sizeof(Mqtt.Subscribe[0].TopicFilter)/sizeof(Mqtt.Subscribe[0].TopicFilter[0])));
	} while (++Count < (sizeof(Mqtt.Subscribe)/sizeof(Mqtt.Subscribe[0])));
}
/****************************************************************************/

/****************************************************************************
 * Function:        ReadEepromData
 *
 * Overview:        This function reads data from EEPROM
 *
 * PreCondition:    This function need Arduino Uno, Fishino Uno, Arduino Mega 2560 or Fishino Mega 2560
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
 void Mqtt_GSM::ReadEepromData(void) {
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.ClientIdentifier[0],         Mqtt.EepromAdd.StartAddClientIdentifier,        (sizeof(MQTT_CLIENT_IDENTIFIER)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.WillTopic[0],                Mqtt.EepromAdd.StartAddWillTopic,               (sizeof(MQTT_WILL_TOPIC)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.WillMessage[0],              Mqtt.EepromAdd.StartAddWillMessage,             (sizeof(MQTT_WILL_MESSAGE)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.UserName[0],                 Mqtt.EepromAdd.StartAddUserName,                (sizeof(MQTT_USER_NAME)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.Password[0],                 Mqtt.EepromAdd.StartAddPassword,                (sizeof(MQTT_PASSWORD)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.PublishTopicName[0],         Mqtt.EepromAdd.StartAddPublishTopicName,        (sizeof(MQTT_PUBLISH_TOPIC_NAME)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.Subscribe[0].TopicFilter[0], Mqtt.EepromAdd.StartAddSubscribeTopicFilter[0], (sizeof(MQTT_SUBSCRIBE_TOPIC_FILTER_1)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.Subscribe[1].TopicFilter[0], Mqtt.EepromAdd.StartAddSubscribeTopicFilter[1], (sizeof(MQTT_SUBSCRIBE_TOPIC_FILTER_2)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.Subscribe[2].TopicFilter[0], Mqtt.EepromAdd.StartAddSubscribeTopicFilter[2], (sizeof(MQTT_SUBSCRIBE_TOPIC_FILTER_3)));
	Gsm.LoadDataFromEepromIntoArray(&Mqtt.Subscribe[3].TopicFilter[0], Mqtt.EepromAdd.StartAddSubscribeTopicFilter[3], (sizeof(MQTT_SUBSCRIBE_TOPIC_FILTER_4)));
}
/****************************************************************************/
	 
/****************************************************************************
 * Function:        EepromStartAddSetup
 *
 * Overview:        This function sets the Start address for ......... saved on the EEPROM memory
 *
 * PreCondition:    This function need Arduino Uno, Fishino Uno, Arduino Mega 2560 or Fishino Mega 2560
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
 void Mqtt_GSM::EepromStartAddSetup(void) {
    Mqtt.EepromAdd.StartAddWillTopic               = &MQTT_WILL_TOPIC[0];
    Mqtt.EepromAdd.StartAddClientIdentifier        = &MQTT_CLIENT_IDENTIFIER[0];
    Mqtt.EepromAdd.StartAddWillMessage             = &MQTT_WILL_MESSAGE[0];
    Mqtt.EepromAdd.StartAddUserName                = &MQTT_USER_NAME[0];
	Mqtt.EepromAdd.StartAddPassword                = &MQTT_PASSWORD[0];
	Mqtt.EepromAdd.StartAddPublishTopicName        = &MQTT_PUBLISH_TOPIC_NAME[0];
	Mqtt.EepromAdd.StartAddSubscribeTopicFilter[0] = &MQTT_SUBSCRIBE_TOPIC_FILTER_1[0];
	Mqtt.EepromAdd.StartAddSubscribeTopicFilter[1] = &MQTT_SUBSCRIBE_TOPIC_FILTER_2[0];
	Mqtt.EepromAdd.StartAddSubscribeTopicFilter[2] = &MQTT_SUBSCRIBE_TOPIC_FILTER_3[0];
	Mqtt.EepromAdd.StartAddSubscribeTopicFilter[3] = &MQTT_SUBSCRIBE_TOPIC_FILTER_4[0];
 }
/****************************************************************************/