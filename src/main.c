#include <bits/pthreadtypes.h>
#include <bits/types/struct_iovec.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <byteswap.h>
//#include <intin.h> for Windows
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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
int sendPacket(const int socketfd, PACKET* packet);
int printFlags(const struct FLAGS* flags);
int notify(const char* message);
int printPacket(const PACKET* packet);
PACKET* makePacket(const char* fileName, int* errorCode);
PACKET* recvPacket(const int socketfd, int* errorCode);
int checkHex(uint64_t n);
int deletePacket(PACKET* packet);

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
  switch (flags.type) {
    case 0:
      exit(server(&flags));
    case 1:
      exit(client(&flags));
  }
}

int server(struct FLAGS* flags){
  puts("\n---server---\n");
  puts("Starting server...\n");
  printFlags(flags);
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket == -1){
    close(serverSocket);
    return EXIT_FAIL_SOCKET_CREATE;
  }
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
    close(serverSocket);
    return EXIT_FAIL_SOCKET_REUSE;
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
    close(serverSocket);
    return EXIT_FAIL_SOCKET_BIND;
  }
  if(listen(serverSocket, 1) == -1){
    close(serverSocket);
    return EXIT_FAIL_SOCKET_LISTEN;
  }
  int clientSocket = accept(serverSocket, NULL, NULL);
  if(clientSocket == -1){
    close(serverSocket);
    close(clientSocket);
    return EXIT_FAIL_SOCKET_ACCEPT;
  }

  int err = EXIT_SUCCESS;
  PACKET* packet = makePacket("server.txt", &err);
  if(err != EXIT_SUCCESS){
    close(serverSocket);
    close(clientSocket);
    return err;
  }
  if(sendPacket(clientSocket, packet) != EXIT_SUCCESS){
    return err;
  }

  deletePacket(packet);
  close(serverSocket);
  close(clientSocket);
  return EXIT_SUCCESS;
}
int client(struct FLAGS* flags){
  puts("\n---client---\n");
  puts("Starting client...\n");
  printFlags(flags);
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientSocket == -1){
    close(clientSocket);
    return EXIT_FAIL_SOCKET_CREATE;
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1){
    close(clientSocket);
    return EXIT_FAIL_SOCKET_CONNECT;
  }

  // error handling needed
  int err = EXIT_SUCCESS;
  PACKET* receivedPacket = recvPacket(clientSocket, &err);
  if(err != EXIT_SUCCESS){
    close(clientSocket);
    return err;
  }
  printPacket(receivedPacket);
  FILE* newFile = fopen(receivedPacket->header.fileName, "w");
  if(newFile == NULL){
    close(clientSocket);
    deletePacket(receivedPacket);
    fclose(newFile);
    return EXIT_FAIL_FILE_OPEN;
  }
  fprintf(newFile,"%s", receivedPacket->content);
  fclose(newFile);
  deletePacket(receivedPacket);
  close(clientSocket);
  return EXIT_SUCCESS;
}
PACKET* recvPacket(const int socketfd, int* errorCode){
  puts("\n---recvPacket---\n");
  PACKET* packet = malloc(sizeof(PACKET));
  puts("receiving header...\n");
  if(recv(socketfd, &packet->header,sizeof(HEADER), 0) == -1){
    *errorCode = EXIT_FAIL_SOCKET_RECEIVE;
    free(packet);
    return NULL;
  }
  packet->header.contentLength = bswap_64(packet->header.contentLength);
  packet->content = malloc(packet->header.contentLength+1);
  puts("receiving content...\n");
  if(recv(socketfd, packet->content,packet->header.contentLength, 0) == -1){
    *errorCode = EXIT_FAIL_SOCKET_RECEIVE;
    deletePacket(packet);
    return NULL;
  }
  packet->content[packet->header.contentLength] = '\0';
  return packet;
}
int sendPacket(const int socketfd, PACKET* packet){
  puts("\n---sendPacket---\n");
  if(send(socketfd, &packet->header, sizeof(HEADER),0) == -1){
    deletePacket(packet);
    return EXIT_FAIL_SOCKET_SEND;
  }
  if(send(socketfd, packet->content,packet->header.contentLength,0) == -1){
    deletePacket(packet);
    return EXIT_FAIL_SOCKET_SEND;
  }
  puts("packet has been sent successfully!\n");
  return EXIT_SUCCESS;
}

PACKET* makePacket(const char* fileName, int* errorCode){
  puts("\n---makePacket---\n");
  FILE* file = fopen(fileName, "r");
  if(file == NULL){
    *errorCode = EXIT_FAIL_FILE_OPEN;
    fclose(file);
  }
  PACKET* packet = malloc(sizeof(PACKET));
  strncpy(packet->header.fileName, fileName, MAX_FILENAME_LENGTH-1);
  packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  fseek(file, 0L, SEEK_END);
  packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  packet->content = malloc(packet->header.contentLength);
  fread(packet->content, sizeof(char), packet->header.contentLength, file);
  if(ferror(file)!=0){
    *errorCode = EXIT_FAIL_FILE_READ;
    fclose(file);
    deletePacket(packet);
    return NULL;
  }else
    packet->content[packet->header.contentLength] = '\0';
  fclose(file);
  printPacket(packet);
  checkHex(packet->header.contentLength);
  packet->header.contentLength = bswap_64(packet->header.contentLength);
  return packet;
}
int deletePacket(PACKET* packet){
  free(packet->content);
  free(packet);
  return EXIT_SUCCESS;
}

int printPacket(const PACKET* packet){
  puts("\n---printPacket---\n");
  puts("\t---Header\n");
  printf("\t\tfileName: %s\n\t\tcontentLength: %"PRIu64"\n", packet->header.fileName, packet->header.contentLength);
  puts("\t---Content\n");
  printf("\t\t%s\n", packet->content);
  return EXIT_SUCCESS;
}

int notify(const char* message){
  char* command = malloc(strlen(notificationCommand)+strlen(message));
  strcpy(command, notificationCommand);
  strcat(command, message);
  printf("%s", command);
  if(system(command) == -1){
    free(command);
    return EXIT_FAIL_NOTIFY_SEND;
  }
  free(command);
  return EXIT_SUCCESS;
}
int printFlags(const struct FLAGS* flags){
  printf("PORT: %d\nADDR: %s\nDIR: %s\n", flags->port, flags->addr, flags->dir);
  if(flags->type == 0)
    puts("TYPE: server\n");
  else
    puts("TYPE: client\n");
  return EXIT_SUCCESS;
}

int checkHex(uint64_t n)
{
  uint64_t swapped = bswap_64(n);
  printf("original: 0x%016"PRIx64"\nSwapped: 0x%016"PRIx64"\n", n, swapped);
  return EXIT_SUCCESS;
}
