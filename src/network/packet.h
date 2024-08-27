#ifndef PACKET_H_
#define PACKET_H_
#include "../misc/hex.h"

#define MAX_FILENAME_LENGTH 126
typedef struct{
  char fileName[MAX_FILENAME_LENGTH];
  uint64_t contentLength;
}HEADER;

typedef struct{
  HEADER header;
  char* content;
} PACKET;
int deletePacket(PACKET* packet);
PACKET* makePacket(const char* fileName, const char* directory,int* errorCode);
int printPacket(const PACKET* packet);

#endif // !PACKET_H_
