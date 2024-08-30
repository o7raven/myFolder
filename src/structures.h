#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <inttypes.h>
#include "network/packet.h"
typedef struct{
  int serverSocket;
  int clientSocket;
  PACKET* packet;
} SERVER;
typedef struct{
  int clientSocket;
  PACKET* packet;
} CLIENT;
struct FLAGS {
  uint16_t port;
  char* addr;
  char* dir;
  char type;
};
#endif // !STRUCTURES_H_
