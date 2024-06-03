#include <bits/types/struct_iovec.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#define EXIT_FAIL_SOCKET_CREATE 0x01
#define EXIT_FAIL_SOCKET_BIND 0x02
#define EXIT_FAIL_SOCKET_LISTEN 0x03
#define EXIT_FAIL_SOCKET_ACCEPT 0x04
#define EXIT_FAIL_SOCKET_CONNECT 0x05
#define EXIT_NOT_ENOUGH_ARGS 0x06
#define EXIT_FAIL_PORT 0x07
#define EXIT_FAIL_TYPE 0x08
#define EXIT_FAIL_FILE_OPEN 0x09
#define EXIT_FAIL_FILE_READ 0x0A

struct FLAGS {
  uint port;
  char* addr;
  char* dir;
  char type;
};

int server(struct FLAGS* flags);
int client(struct FLAGS* flags);
int sendFile(const int* socketfd, const char* fileName);
int recvFile(const int* socketfd);

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

  printf("Flags structure filled:directory:%s type:%c address:%s port:%d\n", flags.dir, flags.type, flags.addr, flags.port);
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
  puts("\nStarting server...\n");
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
  sendFile(&serverSocket, "file.txt");
  return 0;
}
int client(struct FLAGS* flags){
  puts("\nStarting client...\n");
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientSocket == -1)
    exit(EXIT_FAIL_SOCKET_CREATE);
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1)
    exit(EXIT_FAIL_SOCKET_CONNECT);
  recvFile(&clientSocket);

  return 0;
}
int recvFile(const int* socketfd){
  char data[sizeof(size_t)];
  recv(*socketfd, data, sizeof(data), 0);
  printf("%s", data);
  return 0;
}

int sendFile(const int* socketfd, const char* fileName){
  FILE* file = fopen(fileName, "r");
  if(file == NULL)
    exit(EXIT_FAIL_FILE_OPEN);
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  fseek(file, 0L, SEEK_SET);
  printf("\nFile size: %lu\n", fileSize);
  char* buffer = malloc(fileSize);
  size_t newLen = fread(buffer, sizeof(char), fileSize, file);
  if(ferror(file)!=0)
    exit(EXIT_FAIL_FILE_READ);
  else
     buffer[newLen++] = '\0';
  printf("Seding buffer:%s\n", buffer);
  send(*socketfd, buffer, strlen(buffer), 0);
  free(buffer);
  fclose(file);
  return 0;
}
