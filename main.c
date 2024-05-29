#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#define EXIT_SUCCESS 0
#define EXIT_FAIL 1
#define EXIT_FAIL_NOT_ENOUGH_ARGS 2
#define EXIT_FAIL_PORT 3
#define EXIT_FAIL_SOCKET_CREATE 4
#define EXIT_FAIL_SOCKET_BIND 5
#define EXIT_FAIL_SOCKET_LISTEN 6
#define EXIT_FAIL_SOCKET_ACCEPT 7
#define EXIT_FAIL_SOCKET_CONNECT 8
#define EXIT_FAIL_SOCKET_IPCONVERSION 9

struct FLAGS {
  int port;
  char *watchDirectory;
  char *type;
};

int server(const struct FLAGS flags);
int client(const struct FLAGS flags);

int main(int argc, char **argv) {
  struct FLAGS flags;
  if (argc < 7) {
    printf("Usage: %s -d [directory] -p [port] -t [type]\n", argv[0]);
    exit(EXIT_FAIL_NOT_ENOUGH_ARGS);
  }
  for (int i = 1; i < 7; i += 2) {
    const char *flag = argv[i];
    char *value = argv[i + 1];
    printf("flag: %s\n", flag);
    if (!strcmp(flag, "-p")) {
      printf("[%d] port flag reached\n", i);
      flags.port = atoi(value);
      if (flags.port == 0)
        exit(EXIT_FAIL_PORT);
    }
    if (!strcmp(flag, "-d")) {
      printf("[%d] dir flag reached\n", i);
      flags.watchDirectory = value;
    }
    if (!strcmp(flag, "-t")) {
      printf("[%d] type flag reached\n", i);
      flags.type = value;
    }
  }
  printf("Flag structure filled\nport:%d\nwatchDirectory:%s\ntype:%s\n",flags.port, flags.watchDirectory, flags.type);
  if (!strcmp(flags.type, "server")) {
    server(flags);
  } else if (!strcmp(flags.type, "client")) {
    client(flags);
  }
  exit(EXIT_SUCCESS);
}

int server(const struct FLAGS flags) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    exit(EXIT_FAIL_SOCKET_CREATE);
  }
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(flags.port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    exit(EXIT_FAIL_SOCKET_BIND);
  }
  if (listen(sockfd, 0) == -1) {
    exit(EXIT_FAIL_SOCKET_LISTEN);
  }
  struct sockaddr clientSocket;
  if (accept(sockfd, &clientSocket, &addrlen) == -1) {
    exit(EXIT_FAIL_SOCKET_ACCEPT);
  }
  puts("Connection accepted\n");
  exit(EXIT_SUCCESS);
}

int client(const struct FLAGS flags) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    exit(EXIT_FAIL_SOCKET_CREATE);
  }
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(flags.port);
  if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) == -1){
    exit(EXIT_FAIL_SOCKET_IPCONVERSION);
  } 
  if(connect(sockfd, (struct sockaddr*)&address, sizeof(address)) == -1){
    exit(EXIT_FAIL_SOCKET_CONNECT);
  }
  exit(EXIT_SUCCESS);
}
