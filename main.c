/**
 * UDP Client in C for Windows
 * Author: Brently Maxwell
 *
 * This file contains the main() function.
 * It starts the client using Winsock.
 * The client sends data packet(s) to the server
 * and receives responses based on the validity
 * of each sent packet.
 */

#include <time.h>
#include "structs.h"
#include "functions.h"

#define MAX_RESEND_ATTEMPTS 3

int main()
{
	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;
	int serverPort = 5150;
	int clientPort = 5151;
	int serverAddrLen;
	char inputBuffer[MAX_BUFFER_LEN];
	char outputBuffer[MAX_BUFFER_LEN];
	int numSegs;
	int resendAttemptNum = 0;
	dataPacket* sendPacket;
	ackPacket* recvACKPacket;
	rejectPacket* recvRejPacket;
	TIMEVAL timeout;
	fd_set socks;
	serverAddrLen = sizeof(SOCKADDR_IN);
	recvACKPacket = malloc(sizeof(ackPacket));
	printf("Welcome to the Client.\n");

	// Initialize Winsock 2.2.
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("WSAStartup failed. Error: %1d.\n", WSAGetLastError());
		return -1;
	} else
		printf("WSAStartup succeeded. Status: %s.\n", wsaData.szSystemStatus);

	// Open socket.
	if ((clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed. Error: %1d.\n", WSAGetLastError());
		WSACleanup();
		return -1;
	} else
		printf("socket() succeeded.\n");

	// Set up address structures, then bind the client to the socket.
	memset((char*)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = htonl(0x7f000001);
	memset((char*)&clientAddr, 0, sizeof(clientAddr));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(clientPort);
	clientAddr.sin_addr.s_addr = htons(0);

	if ((bind(clientSocket, (SOCKADDR*)&clientAddr, sizeof(clientAddr))) == SOCKET_ERROR)
	{
		printf("bind() failed. Error: %1d.\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	} else
		printf("bind() succeeded.\n");

	// Loop and send/receive data to/from the server.
	printf("Ready to transmit data.\n");
	while (1) {
		// Read input from file.
		printf("Reading input file.\n");
		readInputFile(inputBuffer);

		// Alternatively, read input from user.
		// printf("Please enter a message or press CTRL + C to quit.\n");
		// fgets(inputBuffer, MAX_BUFFER_LEN, stdin);

		// Create data packet segments.
		sendPacket = createDataSegments(inputBuffer);
		numSegs = ceil(((float) strlen(inputBuffer))/((float)MAX_PAYLOAD_LEN));

		// Alternatively, create error packet segments for testing.
		// sendPacket = createErrorSegments();
		// numSegs = 5;

		// Loop and attempt to send each segment to server.
		for (int i = 0; i < numSegs; i++) {
			if (sendto(clientSocket, &sendPacket[i], sizeof(dataPacket), 0, (SOCKADDR*)&serverAddr, serverAddrLen) < 0) {
				printf("Failed to send data to server. Error: %1d.\n\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			} else
				printf("Data sent to server.\n\n");

			// Wait for ACK, resend data up to 3 times if timeout happens.
			while (resendAttemptNum < MAX_RESEND_ATTEMPTS) {
				FD_ZERO(&socks);
				FD_SET(clientSocket, &socks);
				timeout.tv_sec = 3;
				timeout.tv_usec = 0;
				int selectTimeout = select(clientSocket + 1, &socks, NULL, NULL, &timeout);

				// If select() error throw WSA error.
				if (selectTimeout == SOCKET_ERROR) {
					printf("Timeout select() call failed. Error: %1d.\n", WSAGetLastError());
				// Else if timeout occurs, resend packet.
				} else if (selectTimeout == 0) {
					printf("Timeout waiting for ACK. Resending packet.\n");
					if (sendto(clientSocket, &sendPacket[i], sizeof(dataPacket), 0, (SOCKADDR*)&serverAddr, serverAddrLen) < 0) {
						printf("Failed to send data to server. Error: %1d.\n\n", WSAGetLastError());
						closesocket(clientSocket);
						WSACleanup();
						return -1;
					} else
						printf("Data sent to server.\n\n");
				// Else receive ACK or reject from server.
				} else {
					char recvBuffer[MAX_BUFFER_LEN];
					if (recvfrom(clientSocket, recvBuffer, MAX_BUFFER_LEN - 1, 0, (SOCKADDR*)&serverAddr, &serverAddrLen) < 0) {
						printf("Failed to receive Response from server. Error: %1d.\n", WSAGetLastError());
						closesocket(clientSocket);
						WSACleanup();
						return -1;
					} else {
						recvACKPacket = malloc(sizeof(ackPacket));
						recvRejPacket = malloc(sizeof(rejectPacket));
						deserialize(recvACKPacket, recvRejPacket, recvBuffer);
						free(recvACKPacket);
						free(recvRejPacket);
						break;
					}
				}
				resendAttemptNum++;
				if (resendAttemptNum >= MAX_RESEND_ATTEMPTS) {
					printf("Server did not respond...\n");
					return -1;
				}
			}
		}
		break;
	}
return 0;
}
