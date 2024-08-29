#ifndef CLIENT_H_
#define CLIENT_H_

#include <unistd.h>
#include <netinet/in.h>
#include <error.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "../misc/structures.h"
#include "packet.h"
#include "../misc/notifications.h"

#include "../error/codes.h"
int client(struct FLAGS* flags);
#endif // !CLIENT_H_
