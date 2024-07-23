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

#define HEADER_SIZE_BYTES 128
#define MAX_FILENAME_LENGTH 120
#define MAX_PACKET_SIZE sizeof(size_t)

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
#define EXIT_FAIL_SOCKET_RECEIVE 0x0B
#define EXIT_FAIL_SOCKET_SEND 0x0C
#define EXIT_FAIL_NOTIFY_SEND 0x0D

const char* notificationCommand = "notify-send "; 

typedef struct{
  char fileName[MAX_FILE_NAME_SIZE];
  size_t contentLength;
}HEADER;

typedef struct{
  HEADER header;
  char* content;
} PACKET;

struct FLAGS {
  uint port;
  char* addr;
  char* dir;
  char type;
};

int server(struct FLAGS* flags);
int client(struct FLAGS* flags);
int sendFile(const int* socketfd, const PACKET packet);
int recvFile(const int* socketfd, PACKET* receivedPacket);
int printFlags(const struct FLAGS* flags);
int notify(const char* message);
int printPacket(const PACKET* packet);
PACKET makePacket(const char* fileName);

int main(int argc, char** argv){
  makePacket("server.txt");
  return 0;
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

  // printf("Flags structure filled:directory:%s type:%d address:%s port:%d\n", flags.dir, flags.type, flags.addr, flags.port);
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
  puts("\n---server---\n");
  puts("Starting server...\n");
  printFlags(flags);
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
  // sendFile(&clientSocket, "server.txt");
  return 0;
}
int client(struct FLAGS* flags){
  puts("\n---client---\n");
  puts("Starting client...\n");
  printFlags(flags);
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientSocket == -1)
    exit(EXIT_FAIL_SOCKET_CREATE);
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1)
    exit(EXIT_FAIL_SOCKET_CONNECT);
  PACKET receivedPacket;
  recvFile(&clientSocket, &receivedPacket);
  // FILE* newFile = fopen("receivedFile.txt", "w");
  // if(newFile == NULL)
  //   exit(EXIT_FAIL_FILE_OPEN);
  // fprintf(newFile,"%s", receivedFile);
  // fclose(newFile);
  return 0;
}
int recvFile(const int* socketfd, PACKET* receivedPacket){
  puts("\n---recvFile---\n");
  if(recv(*socketfd, receivedPacket,MAX_PACKET_SIZE, 0) == -1)
    exit(EXIT_FAIL_SOCKET_RECEIVE);
  return 0;
}

PACKET makePacket(const char* fileName){
  FILE* file = fopen(fileName, "r");
  if(file == NULL)
    exit(EXIT_FAIL_FILE_OPEN);
  PACKET packet;
  strncpy(packet.header.fileName, fileName, MAX_FILENAME_LENGTH-1);
  printf("filename: %s\n", packet.header.fileName);
  fseek(file, 0L, SEEK_END);
  // size_t fileSize = ftell(file);
  // fseek(file, 0L, SEEK_SET);
  // printf("File size: %lu\n", fileSize);
  // char* buffer = malloc(fileSize+1);
  // size_t newLen = fread(buffer, sizeof(char), fileSize, file);
  // if(ferror(file)!=0){
  //   printf("error reading file\n");
  //   exit(EXIT_FAIL_FILE_READ);
  // }else
  //   buffer[fileSize] = '\0';
  // free(buffer);
  fclose(file);
  return packet;
}

int sendFile(const int* socketfd, const PACKET packet){
  puts("\n---sendFile---\n");
  // printf("Sending buffer:%s\n", buffer);
  // if(send(*socketfd, buffer, MAX_FILE_SIZE,0) == -1)
  // {
  //   free(buffer);
  //   fclose(file);
  //   exit(EXIT_FAIL_SOCKET_SEND);
  // }
  // free(buffer);
  // fclose(file);
  return 0;
}
int notify(const char* message){
  char* command = malloc(strlen(notificationCommand)+strlen(message));
  strcpy(command, notificationCommand);
  strcat(command, message);
  printf("%s", command);
  if(system(command) == -1){
    free(command);
    exit(EXIT_FAIL_NOTIFY_SEND);
  }
  free(command);
  return 0;
}
int printFlags(const struct FLAGS* flags){
  printf("PORT: %d\nADDR: %s\nDIR: %s\n", flags->port, flags->addr, flags->dir);
  if(flags->type == 0)
    puts("TYPE: server\n");
  else
    puts("TYPE: client\n");
  return 0;
}
