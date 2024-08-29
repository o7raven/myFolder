#include "client.h"
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
  // signal(SIGINT, sigHandler);
  // if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1){
  //     close(clientSocket);
  //     fprintf(stderr, "0x%x: client socket connect fail\n", EXIT_FAIL_SOCKET_CONNECT);
  //     return EXIT_FAIL_SOCKET_CONNECT;
  // }
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
