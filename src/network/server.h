#ifndef SERVER_H_
#define SERVER_H_

#include <unistd.h>
#include <netinet/in.h>
#include <error.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "../structures.h"
#include "packet.h"

#include "../error/codes.h"

SERVER makeServer(struct FLAGS* flags);
int deleteSever(SERVER server);
#endif // !SERVER_H_
