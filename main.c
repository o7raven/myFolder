#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define EXIT_SUCCESS 0
#define EXIT_FAIL_SOCKET_CREATE 1
#define EXIT_FAIL_SOCKET_BIND 2
#define EXIT_FAIL_SOCKET_LISTEN 3
#define EXIT_FAIL_SOCKET_ACCEPT 4
#define EXIT_FAIL_SOCKET_CONNECT 5

struct FLAGS {
  int port;
};

int server(struct FLAGS* flags);
int client(struct FLAGS* flags);

int main(int argc, char** argv){
  struct FLAGS flags;
  flags.port = 3512;
  client(&flags);
  exit(EXIT_SUCCESS);
}

int server(struct FLAGS* flags){
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
  char* testmsg = "123";
  send(clientSocket, testmsg, strlen(testmsg), 0);
  return 0;
}
int client(struct FLAGS* flags){
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientSocket == -1)
    exit(EXIT_FAIL_SOCKET_CREATE);
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons((*flags).port);
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  if(connect(clientSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1)
    exit(EXIT_FAIL_SOCKET_CONNECT);
  char strdata[256];
  recv(clientSocket, strdata, sizeof(strdata), 0);
  printf("%s",strdata);

  return 0;
}
