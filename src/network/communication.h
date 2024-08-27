#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILENAME_LENGTH 126
typedef struct{
  char fileName[MAX_FILENAME_LENGTH];
  uint64_t contentLength;
}HEADER;

typedef struct{
  HEADER header;
  char* content;
} PACKET;

int sendPacket(const int socketfd, PACKET* packet);
PACKET* recvPacket(const int socketfd, int* errorCode);

#endif
