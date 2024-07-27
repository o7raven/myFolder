#include <bits/pthreadtypes.h>
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

#define MAX_FILENAME_LENGTH 126

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
#define EXIT_FAIL_SOCKET_REUSE 0x0E

const char* notificationCommand = "notify-send "; 

typedef struct{
  char fileName[MAX_FILENAME_LENGTH];
  uint64_t contentLength;
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
int sendPacket(const int socketfd, const PACKET* packet);
PACKET* recvPacket(const int socketfd);
int printFlags(const struct FLAGS* flags);
int notify(const char* message);
int printPacket(const PACKET* packet);
PACKET* makePacket(const char* fileName);
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
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
      exit(EXIT_FAIL_SOCKET_REUSE);
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
  PACKET* packet = makePacket("server.txt");
  sendPacket(clientSocket, packet);
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
  PACKET* receivedPacket = recvPacket(clientSocket);
  puts("receivedpacket:");
  printPacket(receivedPacket);
  free(receivedPacket);
  // FILE* newFile = fopen("receivedFile.txt", "w");
  // if(newFile == NULL)
  //   exit(EXIT_FAIL_FILE_OPEN);
  // fprintf(newFile,"%s", receivedFile);
  // fclose(newFile);
  return 0;
}
PACKET* recvPacket(const int socketfd){
  puts("\n---recvPacket---\n");
  PACKET* packet = malloc(sizeof(PACKET));
  if(recv(socketfd, &packet->header,sizeof(HEADER), 0) == -1)
    exit(EXIT_FAIL_SOCKET_RECEIVE);
  packet->content = malloc(packet->header.contentLength+1);
  if(recv(socketfd, packet->content,packet->header.contentLength, 0) == -1)
    exit(EXIT_FAIL_SOCKET_RECEIVE);
  packet->content[packet->header.contentLength] = '\0';
  return packet;
}
int sendPacket(const int socketfd, const PACKET* packet){
  puts("\n---sendPacket---\n");
  printPacket(packet);
  if(send(socketfd, &packet->header, sizeof(HEADER),0) == -1)
    exit(EXIT_FAIL_SOCKET_SEND);
  if(send(socketfd, packet->content,packet->header.contentLength,0) == -1)
    exit(EXIT_FAIL_SOCKET_SEND);
  puts("packet has been sent successfully!\n");
  return 0;
}

PACKET* makePacket(const char* fileName){
  puts("\n---makePacket---\n");
  FILE* file = fopen(fileName, "r");
  if(file == NULL)
    exit(EXIT_FAIL_FILE_OPEN);
  PACKET* packet = malloc(sizeof(PACKET));
  strncpy(packet->header.fileName, fileName, MAX_FILENAME_LENGTH-1);
  packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  // printf("Filename: %s\n", packet->header.fileName);

  fseek(file, 0L, SEEK_END);
  packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  // printf("Content length: %lu\n", packet->header.contentLength);
  packet->content = malloc(packet->header.contentLength);
  fread(packet->content, sizeof(char), packet->header.contentLength, file);
  if(ferror(file)!=0){
    printf("error reading file\n");
    exit(EXIT_FAIL_FILE_READ);
  }else
    packet->content[packet->header.contentLength] = '\0';
  fclose(file);
  return packet;
}
int printPacket(const PACKET* packet){
  puts("\n---printPacket---\n");
  puts("\t---Header\n");
  printf("\t\tfileName: %s\n\t\tcontentLength: %zu\n", packet->header.fileName, packet->header.contentLength);
  puts("\t---Content\n");
  printf("\t\t%s\n", packet->content);
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


size_t fromLSBtoMSB(size_t n){

}
