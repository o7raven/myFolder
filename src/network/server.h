#ifndef SERVER_H_
#define SERVER_H_

#include <unistd.h>
#include <netinet/in.h>
#include <error.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "../misc/structures.h"
#include "packet.h"

#include "../error/codes.h"

typedef struct{
  
} SERVER;
SERVER makeServer(struct FLAGS* flags);
#endif // !SERVER_H_
