#include "client.h"
AGENT makeClient(struct FLAGS* flags){
  AGENT agent = {0};
  puts("\n---agent---\n");
  puts("Starting agent...\n");
  // printFlags(flags);
  agent.socket= socket(AF_INET, SOCK_STREAM, 0);
  if(agent.socket == -1){
    fprintf(stderr, "0x%x: agent socket create fail\n", EXIT_FAIL_SOCKET_CREATE);
    close(agent.socket);
    exit(EXIT_FAIL_SOCKET_CREATE);
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  puts("Connecting...\n");
  if(connect(agent.socket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1){
      close(agent.socket);
      fprintf(stderr, "0x%x: agent socket connect fail\n", EXIT_FAIL_SOCKET_CONNECT);
      exit(EXIT_FAIL_SOCKET_CONNECT);
  }
  puts("connected");
  if(connect(agent.socket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1){
      close(agent.socket);
      fprintf(stderr, "0x%x: agent socket connect fail\n", EXIT_FAIL_SOCKET_CONNECT);
  }
  puts("connected22");

  // error handling needed
  // int err = EXIT_SUCCESS;
  // PACKET* receivedPacket = recvPacket(agentSocket, &err);
  // if(err != EXIT_SUCCESS){
  //   close(agentSocket);
  //   return err;
  // }
  // printPacket(receivedPacket);
  // //debug purposes
  // puts("fopen section...\n");
  // const int fileLocationLength = strlen(fileToSend)+strlen((*flags).dir)+2; 
  // char* fileLocation = malloc(fileLocationLength);
  // if(fileLocation == NULL){
  //   fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
  //   free(fileLocation);
  //   return EXIT_FAIL_MALLOC;
  // }
  // snprintf(fileLocation, fileLocationLength,"%s/%s", (*flags).dir, fileToSend); 
  // // FILE* newFile = fopen(receivedPacket->header.fileName, "wb");
  // FILE* newFile = fopen(fileLocation, "wb");
  // if(newFile == NULL){
  //   close(agentSocket);
  //   deletePacket(receivedPacket);
  //   fprintf(stderr, "0x%x: agent fail file open\nfilename:%s\n", EXIT_FAIL_FILE_OPEN, receivedPacket->header.fileName);
  //   return EXIT_FAIL_FILE_OPEN;
  // }
  // puts("passed\n");
  // puts("fwrite section...\n");
  // uint64_t bytesWritten = fwrite(receivedPacket->content, sizeof(char), receivedPacket->header.contentLength, newFile);
  // if(bytesWritten != receivedPacket->header.contentLength){
  //   close(agentSocket);
  //   deletePacket(receivedPacket);
  //   fprintf(stderr, "0x%x: agent file written characters:%"PRIu64"d\ncharacters to be written:%zu\n", EXIT_FAIL_FWRITE, bytesWritten, receivedPacket->header.contentLength);
  //   return EXIT_FAIL_FWRITE;
  // }
  // puts("passed\n");
  // fclose(newFile);
  // puts("mallocing message section...\n");
  // puts("passed\n");
  // notify(receivedPacket->header.fileName);
  // deletePacket(receivedPacket);
  return agent;
}

