#include "server.h"
AGENT makeServer(struct FLAGS* flags){
  AGENT agent = {0}; 
  agent.directory = (*flags).dir;
  puts("\n---agent---\n");
  puts("Starting agent...\n");
  // printFlags(flags);
  agent.socket = socket(AF_INET, SOCK_STREAM, 0);
  if(agent.socket == -1){
    close(agent.socket);
    fprintf(stderr, "0x%x: agent socket create error\n", EXIT_FAIL_SOCKET_CREATE);
    exit(EXIT_FAIL_SOCKET_CREATE);
  }
  if(setsockopt(agent.socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
    close(agent.socket);
    fprintf(stderr, "0x%x: agent socket reuse flag fail\n", EXIT_FAIL_SOCKET_REUSE);
    exit(EXIT_FAIL_SOCKET_REUSE);
  }
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(bind(agent.socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
    close(agent.socket);
    fprintf(stderr, "0x%x: agent socket bind fail\n", EXIT_FAIL_SOCKET_BIND);
    exit(EXIT_FAIL_SOCKET_BIND);
  }
  if(listen(agent.socket, 1) == -1){
    close(agent.socket);
    fprintf(stderr, "0x%x: agent socket listen fail\n", EXIT_FAIL_SOCKET_LISTEN);
    exit(EXIT_FAIL_SOCKET_LISTEN);
  }
  agent.socket = accept(agent.socket, NULL, NULL);
  if(agent.socket == -1){
    close(agent.socket);
    close(agent.socket);
    fprintf(stderr, "0x%x: agent socket accept fail\n", EXIT_FAIL_SOCKET_ACCEPT);
    exit(EXIT_FAIL_SOCKET_ACCEPT);
  }
  // int err = EXIT_SUCCESS;
  // PACKET* packet = makePacket(fileToSend, (*flags).dir, &err);
  // if(err != EXIT_SUCCESS){
  //   close(agentSocket);
  //   close(socket);
  //   exit(err);
  // }
  // if(sendPacket(socket, packet) != EXIT_SUCCESS)
  //   exit(err);
  // deletePacket(packet);
  // close(agentSocket);
  // close(socket);
  // exit(EXIT_SUCCESS);
  return agent;
}
