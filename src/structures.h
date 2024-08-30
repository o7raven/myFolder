#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <inttypes.h>
#define MAX_FILENAME_LENGTH 126
typedef struct{
  char fileName[MAX_FILENAME_LENGTH];
  uint64_t contentLength;
}HEADER;
typedef struct{
  HEADER header;
  char* content;
} PACKET;
typedef struct{
  int serverSocket;
  int clientSocket;
  char* directory;
  PACKET* packet;
} SERVER;
typedef struct{
  int clientSocket;
  char* directory;
  PACKET* packet;
} CLIENT;
struct FLAGS {
  uint16_t port;
  char* addr;
  char* dir;
  char type;
};
#endif // !STRUCTURES_H_
