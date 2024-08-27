#include "communication.h"

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
