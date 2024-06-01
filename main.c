#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define EXIT_SUCCESS 0
#define EXIT_FAIL_SOCKET_CREATE 1
#define EXIT_FAIL_SOCKET_BIND 2
#define EXIT_FAIL_SOCKET_LISTEN 3
#define EXIT_FAIL_SOCKET_ACCEPT 4
#define EXIT_FAIL_SOCKET_CONNECT 5
#define EXIT_NOT_ENOUGH_ARGS 6
#define EXIT_FAIL_PORT 7
#define EXIT_FAIL_TYPE 8

struct FLAGS {
  uint port;
  char* addr;
  char* dir;
  char type;
};

int server(struct FLAGS* flags);
int client(struct FLAGS* flags);

int main(int argc, char** argv){
  if(argc < 9){
    printf("Usage: %s  --directory [what directory to watch] --type [server/client] --address --port\n", argv[0]);
    exit(EXIT_NOT_ENOUGH_ARGS);
  }
  struct FLAGS flags = {0};
  for(int i = 1; i < 9; i+=2){
    const char* flag = argv[i];
    char* value = argv[i+1];
    printf("Flag: %s\n", flag);
    if(!strcmp("--directory", argv[i]) || !strcmp("-d", argv[i])){
      printf("[%d] Directory flag reached\n", i); 
      flags.dir = value;
    }
    if(!strcmp("--type", argv[i]) || !strcmp("-t", argv[i])){
      printf("[%d] Type flag reached\n", i); 
      if(!strcmp("server", value)){
        flags.type = 0;
        continue;
      }
      if(!strcmp("client", value)){
        flags.type = 1;
        continue;
      }
      puts("\nAborting, wrong \"--type\" flag!\n");
      exit(EXIT_FAIL_TYPE);
    }
    if(!strcmp("--address", argv[i]) || !strcmp("-a", argv[i])){
      printf("[%d] Address flag reached\n", i); 
      flags.addr = value;
    }
    if(!strcmp("--port", argv[i]) || !strcmp("-p", argv[i])){
      printf("[%d] Port flag reached\n", i); 
      flags.port = atoi(value);
      if(flags.port == 0){
        exit(EXIT_FAIL_PORT);
      }
    }
  }
  printf("Flags structure filled:directory:%s\ntype:%c\naddress:%s\nport:%d", flags.dir, flags.type, flags.addr, flags.port);
  switch (flags.type) {
    case 0:
      server(&flags);
      break;
    case 1:
      client(&flags);
      break;
  }
  exit(EXIT_SUCCESS);
}

int server(struct FLAGS* flags){
  puts("Starting server...");
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket == -1)
    exit(EXIT_FAIL_SOCKET_CREATE);
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    exit(EXIT_FAIL_SOCKET_BIND);
  if(listen(serverSocket, 1) == -1)
    exit(EXIT_FAIL_SOCKET_LISTEN);
  int clientSocket = accept(serverSocket, NULL, NULL);
  if(clientSocket == -1)
    exit(EXIT_FAIL_SOCKET_ACCEPT);
  char* testmsg = "123";
  send(clientSocket, testmsg, strlen(testmsg), 0);
  return 0;
}
int client(struct FLAGS* flags){
  puts("Starting client...");
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientSocket == -1)
    exit(EXIT_FAIL_SOCKET_CREATE);
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1)
    exit(EXIT_FAIL_SOCKET_CONNECT);
  char strdata[256];
  recv(clientSocket, strdata, sizeof(strdata), 0);
  printf("%s",strdata);

  return 0;
}
