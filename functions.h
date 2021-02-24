/**
 * UDP Client in C for Windows
 * Author: Brently Maxwell
 *
 * This is the header file for functions.c.
 */

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

dataPacket* createDataSegments(char inputBuffer[]);
dataPacket* createErrorSegments();
void readInputFile(char inputBuffer[]);
int deserialize(ackPacket *ackPacket, rejectPacket *rejPacket, char buffer[]);

#endif
