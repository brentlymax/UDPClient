/**
 * UDP Client in C for Windows
 * Author: Brently Maxwell
 *
 * This file contains the helper functions for the client.
 * The functions all relate to creating, sending and receiving
 * different packet types.
 */

#include "structs.h"
#include "functions.h"

/**
 * Segment input data into pieces that will fit into an appropriate number of packets.
 */
dataPacket* createDataSegments(char inputBuffer[]) {
	int inputEndFlag;
	int numSegs = ceil(((float) strlen(inputBuffer))/((float)MAX_PAYLOAD_LEN));
	dataPacket *dPacketArray = malloc(sizeof(dataPacket) * numSegs);

	for (int i = 0; i < numSegs; i++) {
		inputEndFlag = 0;
		dPacketArray[i].startID = PACKET_START_ID;
		dPacketArray[i].clientID = CLIENT_ID;
		dPacketArray[i].packetType = PACKET_TYPE_DATA;
		dPacketArray[i].segNum = (i + 1);
		dPacketArray[i].payloadLen = MAX_PAYLOAD_LEN;

		// Insert payload into packet byte by byte. If the last packet is shorter, the empty characters are set to null.
		for (int j = 0; j < MAX_PAYLOAD_LEN; j++) {
			dPacketArray[i].payload[j] = inputBuffer[(255 * i) + j];
			if (inputEndFlag == 1) {
				dPacketArray[i].payload[j] = '\0';
			}
			if (inputBuffer[(255 * i) + j] == '\0' && inputEndFlag != 1) {
				dPacketArray[i].payloadLen = j;
				inputEndFlag = 1;
			}
		}
		dPacketArray[i].endID = PACKET_END_ID;
	}
	return dPacketArray;
}

/**
 * Initializes 5 packets to test the 4 error cases.
 */
dataPacket* createErrorSegments() {
	int inputEndFlag;
	int numSegs = 5;
	dataPacket *dPacketArray = malloc(sizeof(dataPacket) * numSegs);

	// Packet 1: correct packet
	char payloadContents1[MAX_PAYLOAD_LEN] = "This packet is correct.";
	dPacketArray[0].startID = PACKET_START_ID;
	dPacketArray[0].clientID = CLIENT_ID;
	dPacketArray[0].packetType = PACKET_TYPE_DATA;
	dPacketArray[0].segNum = 1;
	dPacketArray[0].payloadLen = MAX_PAYLOAD_LEN;
	inputEndFlag = 0;

	for (int i = 0; i < MAX_PAYLOAD_LEN; i++) {
		dPacketArray[0].payload[i] = payloadContents1[i];
		if (payloadContents1[i] == '\0' && inputEndFlag != 1) {
			dPacketArray[0].payloadLen = i;
			inputEndFlag = 1;
		}
	}
	dPacketArray[0].endID = PACKET_END_ID;

	// Packet 2: Out of sequence packet
	char payloadContents2[MAX_PAYLOAD_LEN] = "This packet is out of sequence.";
	dPacketArray[1].startID = PACKET_START_ID;
	dPacketArray[1].clientID = CLIENT_ID;
	dPacketArray[1].packetType = PACKET_TYPE_DATA;
	dPacketArray[1].segNum = 4;
	dPacketArray[1].payloadLen = MAX_PAYLOAD_LEN;
	inputEndFlag = 0;

	for (int i = 0; i < MAX_PAYLOAD_LEN; i++) {
		dPacketArray[1].payload[i] = payloadContents2[i];
		if (payloadContents2[i] == '\0' && inputEndFlag != 1) {
			dPacketArray[1].payloadLen = i;
			inputEndFlag = 1;
		}
	}
	dPacketArray[1].endID = PACKET_END_ID;

	// Packet 3: Length Mismatch Packet
	char payloadContents3[MAX_PAYLOAD_LEN] = "This packet has a mismatched length.";
	dPacketArray[2].startID = PACKET_START_ID;
	dPacketArray[2].clientID = CLIENT_ID;
	dPacketArray[2].packetType = PACKET_TYPE_DATA;
	dPacketArray[2].segNum = 3;
	dPacketArray[2].payloadLen = MAX_PAYLOAD_LEN;
	inputEndFlag = 0;

	for (int i = 0; i < MAX_PAYLOAD_LEN; i++) {
		dPacketArray[2].payload[i] = payloadContents3[i];
		if (payloadContents3[i] == '\0' && inputEndFlag != 1) {
			dPacketArray[2].payloadLen = i;
			inputEndFlag = 1;
		}
	}
	dPacketArray[2].endID = PACKET_END_ID;
	dPacketArray[2].payloadLen = 40;

	// Packet 4: End of packet missing packet
	char payloadContents4[MAX_PAYLOAD_LEN] = "This packet is missing its end.";
	dPacketArray[3].startID = PACKET_START_ID;
	dPacketArray[3].clientID = CLIENT_ID;
	dPacketArray[3].packetType = PACKET_TYPE_DATA;
	dPacketArray[3].segNum = 4;
	dPacketArray[3].payloadLen = MAX_PAYLOAD_LEN;
	inputEndFlag = 0;

	for (int i = 0; i < MAX_PAYLOAD_LEN; i++) {
		dPacketArray[3].payload[i] = payloadContents4[i];
		if (payloadContents4[i] == '\0' && inputEndFlag != 1) {
			dPacketArray[3].payloadLen = i;
			inputEndFlag = 1;
		}
	}
	dPacketArray[3].endID = 0xF2;

	// Packet 5: Duplicate packet
	char payloadContents5[MAX_PAYLOAD_LEN] = "This packet is a duplicate.";
	dPacketArray[4].startID = PACKET_START_ID;
	dPacketArray[4].clientID = CLIENT_ID;
	dPacketArray[4].packetType = PACKET_TYPE_DATA;
	dPacketArray[4].segNum = 4;
	dPacketArray[4].payloadLen = MAX_PAYLOAD_LEN;
	inputEndFlag = 0;

	for (int i = 0; i < MAX_PAYLOAD_LEN; i++) {
		dPacketArray[4].payload[i] = payloadContents5[i];
		if (payloadContents5[i] == '\0' && inputEndFlag != 1) {
			dPacketArray[4].payloadLen = i;
			inputEndFlag = 1;
		}
	}
	dPacketArray[4].endID = PACKET_END_ID;
	return dPacketArray;
}

/**
 * Read contents of file and insert it into the input buffer.
 */
void readInputFile(char inputBuffer[]) {
	char c;
	int i = 0;
	FILE *file = fopen("Message.txt", "r");

	if (file == NULL) {
		printf("Could not open file.\n");
		return -1;
	}

	while (c != EOF) {
		c = fgetc(file);
		inputBuffer[i] = c;
		i++;
		if (i >= MAX_BUFFER_LEN) {
			printf("WARNING: File contents greater than 5 buffers.\n");
			break;
		}
	}

	inputBuffer[i] = '\0';
	fclose(file);
}

/**
 * Deserialize ACK or reject packet and give an appropriate response to client.
 */
int deserialize(ackPacket *ackPacket, rejectPacket *rejPacket, char buffer[]) {
	// If ACK received.
	if(((u_char)buffer[3] == 0xf2 && (u_char)buffer[4] == 0xff) ||
		((u_char)buffer[3] == 0xff && (u_char)buffer[4] == 0xf2)) {
		ackPacket->packetType = PACKET_TYPE_ACK;
		printf("ACK received from server.\n");
		return 1;
	}
	// Else if reject received.
	else if(((u_char)buffer[3] == 0xf3 && (u_char)buffer[4] == 0xff) ||
		((u_char)buffer[3] == 0xff && (u_char)buffer[4] == 0xf3)) {
		rejPacket->packetType = PACKET_TYPE_REJECT;
		rejPacket->subCode = buffer[5] + buffer[6] + 1;

		if (rejPacket->subCode == REJECT_SUB1) {
			printf("Packet out of sequence. Error: %1x.\n", REJECT_SUB1);
			return -1;
		}
		else if (rejPacket->subCode == REJECT_SUB2) {
			printf("Payload length mismatch. Error: %1x.\n", REJECT_SUB2);
			return -1;
		}
		else if (rejPacket->subCode == REJECT_SUB3) {
			printf("End of packet missing. Error: %1x.\n", REJECT_SUB3);
			return -1;
		}
		else if (rejPacket->subCode == REJECT_SUB4) {
			printf("Duplicate packet. Error: %1x.\n", REJECT_SUB4);
			return -1;
		}
		else {
			printf("Error: incorrect Reject code.\n");
			return -1;
		}

	}
	// Else deserialize failed.
	else {
		printf("Error: deserialize failed.\n");
		return -1;
	}
}
