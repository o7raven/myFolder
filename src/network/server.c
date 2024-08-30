#include "server.h"
#include <stdio.h>
SERVER makeServer(struct FLAGS* flags){
  SERVER server = {0}; 
  puts("\n---server---\n");
  puts("Starting server...\n");
  // printFlags(flags);
  server.serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(server.serverSocket == -1){
    close(server.serverSocket);
    fprintf(stderr, "0x%x: server socket create error\n", EXIT_FAIL_SOCKET_CREATE);
    exit(EXIT_FAIL_SOCKET_CREATE);
  }
  if(setsockopt(server.serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
    close(server.serverSocket);
    fprintf(stderr, "0x%x: server socket reuse flag fail\n", EXIT_FAIL_SOCKET_REUSE);
    exit(EXIT_FAIL_SOCKET_REUSE);
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(server.serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
    close(server.serverSocket);
    fprintf(stderr, "0x%x: server socket bind fail\n", EXIT_FAIL_SOCKET_BIND);
    exit(EXIT_FAIL_SOCKET_BIND);
  }
  if(listen(server.serverSocket, 1) == -1){
    close(server.serverSocket);
    fprintf(stderr, "0x%x: server socket listen fail\n", EXIT_FAIL_SOCKET_LISTEN);
    exit(EXIT_FAIL_SOCKET_LISTEN);
  }
  server.clientSocket = accept(server.serverSocket, NULL, NULL);
  if(server.clientSocket == -1){
    close(server.serverSocket);
    close(server.clientSocket);
    fprintf(stderr, "0x%x: server socket accept fail\n", EXIT_FAIL_SOCKET_ACCEPT);
    exit(EXIT_FAIL_SOCKET_ACCEPT);
  }
  // int err = EXIT_SUCCESS;
  // PACKET* packet = makePacket(fileToSend, (*flags).dir, &err);
  // if(err != EXIT_SUCCESS){
  //   close(serverSocket);
  //   close(clientSocket);
  //   exit(err);
  // }
  // if(sendPacket(clientSocket, packet) != EXIT_SUCCESS)
  //   exit(err);
  // deletePacket(packet);
  // close(serverSocket);
  // close(clientSocket);
  // exit(EXIT_SUCCESS);
  return server;
}
