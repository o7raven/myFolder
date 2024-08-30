#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <byteswap.h>
#include <inttypes.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../error/codes.h"
#include "../structures.h"
#include "packet.h"

int sendServerPacket(SERVER* server);
int sendClientPacket(CLIENT* client);
PACKET* recvServerPacket(SERVER* server);
PACKET* recvClientPacket(CLIENT* client);

#endif
