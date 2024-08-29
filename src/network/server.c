#include "server.h"
SERVER makeServer(struct FLAGS* flags){
  puts("\n---server---\n");
  puts("Starting server...\n");
  // printFlags(flags);
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket == -1){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket create error\n", EXIT_FAIL_SOCKET_CREATE);
    exit(EXIT_FAIL_SOCKET_CREATE);
  }
  if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket reuse flag fail\n", EXIT_FAIL_SOCKET_REUSE);
    exit(EXIT_FAIL_SOCKET_REUSE);
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket bind fail\n", EXIT_FAIL_SOCKET_BIND);
    exit(EXIT_FAIL_SOCKET_BIND);
  }
  if(listen(serverSocket, 1) == -1){
    close(serverSocket);
    fprintf(stderr, "0x%x: server socket listen fail\n", EXIT_FAIL_SOCKET_LISTEN);
    exit(EXIT_FAIL_SOCKET_LISTEN);
  }
  int clientSocket = accept(serverSocket, NULL, NULL);
  if(clientSocket == -1){
    close(serverSocket);
    close(clientSocket);
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
  close(serverSocket);
  close(clientSocket);
  exit(EXIT_SUCCESS);
}
