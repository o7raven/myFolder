#include "communication.h"

int sendPacket(AGENT* agent){
  puts("\n---sendPacket---\n");
  ssize_t bytesSent = 0;
  size_t totalBytesSent = 0;
  size_t packetSize = sizeof(HEADER);
  while(totalBytesSent<packetSize){
    bytesSent = send(agent->socket, ((char*)&agent->packet->header)+totalBytesSent, packetSize - totalBytesSent,0);
    printf("Bytes sent: %zu\n", bytesSent);
    if(bytesSent==-1){
      deletePacket(agent->packet);
      fprintf(stderr, "0x%x: sendPacket socket send content error\n", EXIT_FAIL_SOCKET_SEND);
      return EXIT_FAIL_SOCKET_SEND;
    }
    totalBytesSent += bytesSent;
  }
  puts("packet header has been sent successfully!\n");
  bytesSent = 0;
  totalBytesSent = 0;
  packetSize = bswap_64(agent->packet->header.contentLength);
  while(totalBytesSent<packetSize){
    bytesSent = send(agent->socket, agent->packet->content+totalBytesSent,packetSize- totalBytesSent,0);
    printf("Bytes sent: %zu\n", bytesSent);
    if(bytesSent==-1){
      deletePacket(agent->packet);
      fprintf(stderr, "0x%x: sendPacket socket send content error\n", EXIT_FAIL_SOCKET_SEND);
      return EXIT_FAIL_SOCKET_SEND;
    }
    totalBytesSent += bytesSent;
  }
  puts("packet content has been sent successfully!\n");
  return EXIT_SUCCESS;
}
int recvPacket(AGENT* agent){
  puts("\n---recvPacket---\n");
  agent->packet = malloc(sizeof(PACKET));
  if(agent->packet==NULL){
    fprintf(stderr, "0x%x: recvPacket packet malloc error\n", EXIT_FAIL_MALLOC);
    // exit(EXIT_FAIL_MALLOC);
    // return agent->packet;
    return EXIT_FAIL_MALLOC;
  }
  puts("receiving header...\n");

  ssize_t bytesReceived = 0;
  size_t totalBytesReceived = 0;
  size_t packetSize = sizeof(HEADER);
  while(totalBytesReceived < packetSize){
    bytesReceived = recv(agent->socket, ((char*)&agent->packet->header)+totalBytesReceived,packetSize-totalBytesReceived, 0);
    if(bytesReceived==-1){
      fprintf(stderr, "0x%x: recvPacket socket receive header error\n", EXIT_FAIL_SOCKET_RECEIVE);
      free(agent->packet);
      // exit(EXIT_FAIL_SOCKET_RECEIVE);
      return EXIT_FAIL_SOCKET_RECEIVE;
    }
    totalBytesReceived+=bytesReceived;
  }
  agent->packet->header.contentLength = bswap_64(agent->packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  agent->packet->content = malloc(agent->packet->header.contentLength);
  if(agent->packet->content==NULL){
    fprintf(stderr,"0x%x: recvPacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    free(agent->packet);
    // exit(EXIT_FAIL_MALLOC);
    return EXIT_FAIL_MALLOC;
  }
  puts("receiving content...\n");
  bytesReceived = 0;
  totalBytesReceived = 0;
  packetSize = agent->packet->header.contentLength;
  while(totalBytesReceived<packetSize){
    bytesReceived = recv(agent->socket, agent->packet->content+totalBytesReceived,packetSize-totalBytesReceived, 0);
    if(bytesReceived==-1){
      fprintf(stderr, "0x%x: recvPacket socket receive content error\n", EXIT_FAIL_SOCKET_RECEIVE);
      deletePacket(agent->packet);
      // exit(EXIT_FAIL_SOCKET_RECEIVE);
      return EXIT_FAIL_SOCKET_RECEIVE;
    }
    totalBytesReceived+=bytesReceived;
  }
  return EXIT_SUCCESS;
}
