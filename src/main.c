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
#include <signal.h>

//windows defined for future
#if defined (_WIN32) || defined (_WIN64)
 #define WINDOWS 1
#elif defined (__unix__)
 #define UNIX 1
#endif

#ifdef WINDOWS
 #define HOMEENV "USERPROFILE"
#elif UNIX
 #define HOMEENV "HOME"
#endif


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
#define EXIT_FAIL_MALLOC 0x0F
#define EXIT_FAIL_FWRITE 0x10

#define EXIT_FAIL_FATAL 0xFF

//for development purposes later gon delete
static const char* fileToSend = "audio_testing.mp3";

static volatile int keepConnecting = 1;

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
int notify(char* message);
int printPacket(const PACKET* packet);
PACKET* makePacket(const char* fileName, const char* directory,int* errorCode);
PACKET* recvPacket(const int socketfd, int* errorCode);
int checkHex(uint64_t n);
int deletePacket(PACKET* packet);
void sigHandler(int sig);

int main(int argc, char** argv){
  if(argc < 9){
    printf("Usage: %s  --directory [what directory to watch] --type [server/client] --address --port\n", argv[0]);
    fprintf(stderr, "0x%x: not enough arguments\n", EXIT_NOT_ENOUGH_ARGS);
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
      fprintf(stderr, "0x%x: wrong --type flag\n", EXIT_FAIL_TYPE);
      exit(EXIT_FAIL_TYPE);
    }
    if(!strcmp("--address", argv[i]) || !strcmp("-a", argv[i])){
      printf("[%d] Address flag reached\n", i); 
      flags.addr = value;
    }
    if(!strcmp("--port", argv[i]) || !strcmp("-p", argv[i])){
      printf("[%d] Port flag reached\n", i); 
      flags.port = atoi(value);
      if(flags.port <= 0){
        fprintf(stderr, "0x%x: wrong port input\n", EXIT_FAIL_PORT);
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
    fprintf(stderr, "0x%x: server socket create error\n", EXIT_FAIL_SOCKET_CREATE);
    return EXIT_FAIL_SOCKET_CREATE;
  }
  if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket reuse flag fail\n", EXIT_FAIL_SOCKET_REUSE);
    return EXIT_FAIL_SOCKET_REUSE;
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket bind fail\n", EXIT_FAIL_SOCKET_BIND);
    return EXIT_FAIL_SOCKET_BIND;
  }
  if(listen(serverSocket, 1) == -1){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket listen fail\n", EXIT_FAIL_SOCKET_LISTEN);
    return EXIT_FAIL_SOCKET_LISTEN;
  }
  int clientSocket = accept(serverSocket, NULL, NULL);
  if(clientSocket == -1){
    close(serverSocket);
    close(clientSocket);
    fprintf(stderr, "0x%x: server socket accept fail\n", EXIT_FAIL_SOCKET_ACCEPT);
    return EXIT_FAIL_SOCKET_ACCEPT;
  }

  int err = EXIT_SUCCESS;
  PACKET* packet = makePacket(fileToSend, (*flags).dir, &err);
  if(err != EXIT_SUCCESS){
    close(serverSocket);
    close(clientSocket);
    return err;
  }
  if(sendPacket(clientSocket, packet) != EXIT_SUCCESS)
    return err;
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
    fprintf(stderr, "0x%x: client socket create fail\n", EXIT_FAIL_SOCKET_CREATE);
    close(clientSocket);
    return EXIT_FAIL_SOCKET_CREATE;
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  puts("Connecting...\n");
  signal(SIGINT, sigHandler);
  if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1){
      close(clientSocket);
      fprintf(stderr, "0x%x: client socket connect fail\n", EXIT_FAIL_SOCKET_CONNECT);
      return EXIT_FAIL_SOCKET_CONNECT;
  }
  // while(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1){
  //   if(keepConnecting==0){
  //     close(clientSocket);
  //     fprintf(stderr, "0x%x: client socket connect fail\n", EXIT_FAIL_SOCKET_CONNECT);
  //     return EXIT_FAIL_SOCKET_CONNECT;
  //   }
  //
  // }

  // error handling needed
  int err = EXIT_SUCCESS;
  PACKET* receivedPacket = recvPacket(clientSocket, &err);
  if(err != EXIT_SUCCESS){
    close(clientSocket);
    return err;
  }
  printPacket(receivedPacket);
  //debug purposes
  puts("fopen section...\n");
  const int fileLocationLength = strlen(fileToSend)+strlen((*flags).dir)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    return EXIT_FAIL_MALLOC;
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", (*flags).dir, fileToSend); 
  // FILE* newFile = fopen(receivedPacket->header.fileName, "wb");
  FILE* newFile = fopen(fileLocation, "wb");
  if(newFile == NULL){
    close(clientSocket);
    deletePacket(receivedPacket);
    // fclose(newFile);
    fprintf(stderr, "0x%x: client fail file open\nfilename:%s\n", EXIT_FAIL_FILE_OPEN, receivedPacket->header.fileName);
    return EXIT_FAIL_FILE_OPEN;
  }
  puts("passed\n");
  puts("fwrite section...\n");
  uint64_t bytesWritten = fwrite(receivedPacket->content, sizeof(char), receivedPacket->header.contentLength, newFile);
  if(bytesWritten != receivedPacket->header.contentLength){
    close(clientSocket);
    deletePacket(receivedPacket);
    // fclose(newFile);
    fprintf(stderr, "0x%x: client file written characters:%"PRIu64"d\ncharacters to be written:%zu\n", EXIT_FAIL_FWRITE, bytesWritten, receivedPacket->header.contentLength);
    return EXIT_FAIL_FWRITE;
  }
  puts("passed\n");
  fclose(newFile);
  puts("mallocing message section...\n");
  puts("passed\n");
  notify(receivedPacket->header.fileName);
  deletePacket(receivedPacket);
  close(clientSocket);
  return EXIT_SUCCESS;
}
PACKET* recvPacket(const int socketfd, int* errorCode){
  puts("\n---recvPacket---\n");
  PACKET* packet = malloc(sizeof(PACKET));
  if(packet==NULL){
    fprintf(stderr, "0x%x: recvPacket packet malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    return packet;
  }
  puts("receiving header...\n");

  ssize_t bytesReceived = 0;
  size_t totalBytesReceived = 0;
  size_t packetSize = sizeof(HEADER);
  while(totalBytesReceived < packetSize){
    bytesReceived = recv(socketfd, ((char*)&packet->header)+totalBytesReceived,packetSize-totalBytesReceived, 0);
    if(bytesReceived==-1){
      *errorCode = EXIT_FAIL_SOCKET_RECEIVE;
      fprintf(stderr, "0x%x: recvPacket socket receive header error\n", EXIT_FAIL_SOCKET_RECEIVE);
      free(packet);
      return NULL;
    }
    totalBytesReceived+=bytesReceived;
  }
  packet->header.contentLength = bswap_64(packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  packet->content = malloc(packet->header.contentLength);
  if(packet->content==NULL){
    fprintf(stderr,"0x%x: recvPacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    free(packet);
    return NULL;
  }
  puts("receiving content...\n");
  bytesReceived = 0;
  totalBytesReceived = 0;
  packetSize = packet->header.contentLength;
  while(totalBytesReceived<packetSize){
    bytesReceived = recv(socketfd, packet->content+totalBytesReceived,packetSize-totalBytesReceived, 0);
    if(bytesReceived==-1){
      fprintf(stderr, "0x%x: recvPacket socket receive content error\n", EXIT_FAIL_SOCKET_RECEIVE);
      *errorCode = EXIT_FAIL_SOCKET_RECEIVE;
      deletePacket(packet);
      return NULL;
    }
    totalBytesReceived+=bytesReceived;
  }
  return packet;
}
int sendPacket(const int socketfd, PACKET* packet){
  puts("\n---sendPacket---\n");
  ssize_t bytesSent = 0;
  size_t totalBytesSent = 0;
  size_t packetSize = sizeof(HEADER);
  while(totalBytesSent<packetSize){
    bytesSent = send(socketfd, ((char*)&packet->header)+totalBytesSent, packetSize - totalBytesSent,0);
    printf("Bytes sent: %zu\n", bytesSent);
    if(bytesSent==-1){
      deletePacket(packet);
      fprintf(stderr, "0x%x: sendPacket socket send content error\n", EXIT_FAIL_SOCKET_SEND);
      return EXIT_FAIL_SOCKET_SEND;
    }
    totalBytesSent += bytesSent;
  }
  puts("packet header has been sent successfully!\n");
  bytesSent = 0;
  totalBytesSent = 0;
  packetSize = bswap_64(packet->header.contentLength);
  while(totalBytesSent<packetSize){
    bytesSent = send(socketfd, packet->content+totalBytesSent,packetSize- totalBytesSent,0);
    printf("Bytes sent: %zu\n", bytesSent);
    if(bytesSent==-1){
      deletePacket(packet);
      fprintf(stderr, "0x%x: sendPacket socket send content error\n", EXIT_FAIL_SOCKET_SEND);
      return EXIT_FAIL_SOCKET_SEND;
    }
    totalBytesSent += bytesSent;
  }
  puts("packet content has been sent successfully!\n");
  return EXIT_SUCCESS;
}

PACKET* makePacket(const char* fileName, const char* directory, int* errorCode){
  puts("\n---makePacket---\n");
  const int fileLocationLength = strlen(fileToSend)+strlen(directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    free(fileLocation);
    return NULL;
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", directory, fileToSend); 
  printf(" LOCATION = %s\n", fileLocation);
  FILE* file = fopen(fileLocation, "rb");
  free(fileLocation);
  puts("passed\n");
  if(file == NULL){
    *errorCode = EXIT_FAIL_FILE_OPEN;
    fclose(file);
  }
  puts("passed\n");
  PACKET* packet = malloc(sizeof(PACKET));
  if(packet==NULL){
    fprintf(stderr,"0x%x: makePacket packet malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    return NULL;
  }
  strncpy(packet->header.fileName, fileName, MAX_FILENAME_LENGTH);
  // packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  fseek(file, 0L, SEEK_END);
  packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  packet->content = malloc(packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  if(packet->content==NULL){
    fprintf(stderr,"0x%x: makePacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    free(packet);
    return NULL;
  }
  puts("reading content...\n");
  size_t bytesRead = fread(packet->content, sizeof(char), packet->header.contentLength, file);
  if(bytesRead!= packet->header.contentLength){
    fprintf(stderr, "0x%x: makePacket file read error\n", EXIT_FAIL_FILE_READ);
    *errorCode = EXIT_FAIL_FILE_READ;
    fclose(file);
    deletePacket(packet);
    return NULL;
  }
  // packet->content[packet->header.contentLength] = '\0';
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

int notify(char* fileName){
  char* command = "notify-send  \"has been edited\""; 
  puts("whats it gon be\n");
  command = malloc(strlen(command)+strlen(fileName));
  puts("whats it gon be\n");
  if(command==NULL){
    fprintf(stderr, "0x%x: notify command malloc error\n", EXIT_FAIL_MALLOC);
    return EXIT_FAIL_MALLOC;
  }
  puts("worrked\n");
  snprintf(command, strlen(command), "notify-send %s \"has been edited\"", fileName);
  printf("%s\n", command);
  if(system(command) == -1){
    fprintf(stderr, "0x%x: notify system command error\n", EXIT_FAIL_NOTIFY_SEND);
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
void sigHandler(int sig){
  if(keepConnecting==0){
    //gonna fix later 
    exit(EXIT_FAIL_FATAL);
  }
  keepConnecting=0;
}
//https://pastebin.com/D8Z8jGfA

