#ifndef PACKET_H_
#define PACKET_H_
#include "communication.h"

int deletePacket(PACKET* packet);
PACKET* makePacket(const char* fileName, const char* directory,int* errorCode);
int printPacket(const PACKET* packet);

#endif // !PACKET_H_
