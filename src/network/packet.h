#ifndef PACKET_H_
#define PACKET_H_
#include "../misc/hex.h"
#include "../structures.h"

#define makePacket(type, fileName) _Generic(type, SERVER*: makeServerPacket, CLIENT*: makeClientPacket)(type, fileName)
int deletePacket(PACKET* packet);
int makeServerPacket(SERVER* server,const char* fileName);
int makeClientPacket(CLIENT* client, const char* fileName);
int printPacket(const PACKET* packet);

#endif // !PACKET_H_
