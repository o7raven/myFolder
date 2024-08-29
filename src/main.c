#include <string.h>
#include <signal.h>
#include <unistd.h>

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
#define MAX_FILENAME_LENGTH 126

//for development purposes later gon delete
static const char* fileToSend = "audio_testing.mp3";

static volatile int keepConnecting = 1;


#include "network/communication.h"
#include "network/packet.h"
#include "misc/structures.h"
#include "network/server.h"

int client(struct FLAGS* flags);
int printFlags(const struct FLAGS* flags);
int notify(char* message);
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
  if(flags.type == 0)
    makeServer(&flags);
  else
     client(&flags);
  
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
    fprintf(stderr, "0x%x: client fail file open\nfilename:%s\n", EXIT_FAIL_FILE_OPEN, receivedPacket->header.fileName);
    return EXIT_FAIL_FILE_OPEN;
  }
  puts("passed\n");
  puts("fwrite section...\n");
  uint64_t bytesWritten = fwrite(receivedPacket->content, sizeof(char), receivedPacket->header.contentLength, newFile);
  if(bytesWritten != receivedPacket->header.contentLength){
    close(clientSocket);
    deletePacket(receivedPacket);
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
void sigHandler(int sig){
  if(keepConnecting==0){
    //gonna fix later 
    exit(EXIT_FAIL_FATAL);
  }
  keepConnecting=0;
}
