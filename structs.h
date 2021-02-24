/**
 * UDP Client in C for Windows
 * Author: Brently Maxwell
 *
 * This is the header file for the structs that represent
 * the different packet types used by the client and server.
 */

#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_LEN 1276				// Max length of buffer (5 packets)
#define PACKET_START_ID 0xFFFF          // Start of packet identifier
#define PACKET_END_ID 0xFFFF            // End of packet identifier
#define CLIENT_ID 0x25                  // Client ID
#define MAX_PAYLOAD_LEN 0xFF            // Max payload length
#define PACKET_TYPE_DATA 0xFFF1         // Packet type: DATA
#define PACKET_TYPE_ACK 0xFFF2          // Packet type: ACK
#define PACKET_TYPE_REJECT 0xFFF3       // Packet type: REJECT
#define REJECT_SUB1 0xFFF4              // Reject sub code: Out of sequence
#define REJECT_SUB2 0xFFF5              // Reject sub code: Length mismatch
#define REJECT_SUB3 0xFFF6              // Reject sub code: End of packet missing
#define REJECT_SUB4 0xFFF7              // Reject sub code: Duplicate packet

// Data packets sent by the client.
typedef struct dataPacket
{
	uint16_t startID;
	uint8_t clientID;
	uint16_t packetType;
	uint8_t segNum;
	uint8_t payloadLen;
	char payload[MAX_PAYLOAD_LEN];
	uint16_t endID;
} dataPacket;

// Acknowledgment packets sent by server when it receives a correct packet.
typedef struct ackPacket
{
	uint16_t startID;
	uint8_t clientID;
	uint16_t packetType;
	uint8_t recvSegNum;
	uint16_t endID;
} ackPacket;

// Reject packets sent by server when it receives an invalid packet.
typedef struct rejectPacket
{
	uint16_t startID;
	uint8_t clientID;
	uint16_t packetType;
	uint16_t subCode;
	uint8_t recvSegNum;
	uint16_t endID;
} rejectPacket;

// Buffer for packets that contain pointers to any of the other 3 packet types.
typedef struct dataBuffer {
	void *data;
	size_t size;
	int next;
} dataBuffer;

#endif
