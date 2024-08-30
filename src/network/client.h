#ifndef CLIENT_H_
#define CLIENT_H_

#include <unistd.h>
#include <netinet/in.h>
#include <error.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>

#include "../structures.h"
#include "packet.h"
#include "../misc/notifications.h"

#include "../error/codes.h"
static volatile int keepConnecting = 1;
AGENT makeClient(struct FLAGS* flags);
void sigHandler(int sig);
#endif // !CLIENT_H_
