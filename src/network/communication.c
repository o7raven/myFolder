#include "communication.h"

int sendPacket(AGENT* agent){
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

PACKET* recvPacket(AGENT* agent){
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
