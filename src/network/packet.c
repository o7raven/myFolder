#include "packet.h"
#include <string.h>
int makeServerPacket(SERVER* server,const char* fileName){
  puts("\n---makePacket---\n");
  const int fileLocationLength = strlen(fileName)+strlen(server->directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    exit(EXIT_FAIL_MALLOC);
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", server->directory, fileName); 
  printf(" LOCATION = %s\n", fileLocation);
  FILE* file = fopen(fileLocation, "rb");
  free(fileLocation);
  puts("passed\n");
  if(file == NULL){
    exit(EXIT_FAIL_FILE_OPEN);
  }
  puts("passed\n");
  server->packet = malloc(sizeof(PACKET));
  if(server->packet==NULL){
    fprintf(stderr,"0x%x: makePacket packet malloc error\n", EXIT_FAIL_MALLOC);
    exit(EXIT_FAIL_MALLOC);
  }
  strncpy(server->packet->header.fileName, fileName, MAX_FILENAME_LENGTH);
  // packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  fseek(file, 0L, SEEK_END);
  server->packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  server->packet->content = malloc(server->packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  if(server->packet->content==NULL){
    fprintf(stderr,"0x%x: makePacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    free(server->packet);
    exit(EXIT_FAIL_MALLOC);
  }
  puts("reading content...\n");
  size_t bytesRead = fread(server->packet->content, sizeof(char), server->packet->header.contentLength, file);
  if(bytesRead!= server->packet->header.contentLength){
    fprintf(stderr, "0x%x: makePacket file read error\n", EXIT_FAIL_FILE_READ);
    fclose(file);
    deletePacket(server->packet);
    exit(EXIT_FAIL_FILE_READ);
  }
  // packet->content[packet->header.contentLength] = '\0';
  fclose(file);
  printPacket(server->packet);
  checkHex(server->packet->header.contentLength);
  server->packet->header.contentLength = bswap_64(server->packet->header.contentLength);
  return EXIT_SUCCESS;
}

int makeClientPacket(CLIENT* client,const char* fileName){
  puts("\n---makePacket---\n");
  const int fileLocationLength = strlen(fileName)+strlen(client->directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    exit(EXIT_FAIL_MALLOC);
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", client->directory, fileName); 
  printf(" LOCATION = %s\n", fileLocation);
  FILE* file = fopen(fileLocation, "rb");
  free(fileLocation);
  puts("passed\n");
  if(file == NULL){
    exit(EXIT_FAIL_FILE_OPEN);
  }
  puts("passed\n");
  client->packet = malloc(sizeof(PACKET));
  if(client->packet==NULL){
    fprintf(stderr,"0x%x: makePacket packet malloc error\n", EXIT_FAIL_MALLOC);
    exit(EXIT_FAIL_MALLOC);
  }
  strncpy(client->packet->header.fileName, fileName, MAX_FILENAME_LENGTH);
  // packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  fseek(file, 0L, SEEK_END);
  client->packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  client->packet->content = malloc(client->packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  if(client->packet->content==NULL){
    fprintf(stderr,"0x%x: makePacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    free(client->packet);
    exit(EXIT_FAIL_MALLOC);
  }
  puts("reading content...\n");
  size_t bytesRead = fread(client->packet->content, sizeof(char), client->packet->header.contentLength, file);
  if(bytesRead!= client->packet->header.contentLength){
    fprintf(stderr, "0x%x: makePacket file read error\n", EXIT_FAIL_FILE_READ);
    fclose(file);
    deletePacket(client->packet);
    exit(EXIT_FAIL_FILE_READ);
  }
  // packet->content[packet->header.contentLength] = '\0';
  fclose(file);
  printPacket(client->packet);
  checkHex(client->packet->header.contentLength);
  client->packet->header.contentLength = bswap_64(client->packet->header.contentLength);
  return EXIT_SUCCESS;
}
int deletePacket(PACKET* packet){
  free(packet->content);
  free(packet);
  return EXIT_SUCCESS;
}

int printPacket(const PACKET* packet){
  puts("\n---printPacket---\n");
  puts("\t---Header\n");
  printf("\t\tfileName: %s\n\t\tcontentLength: %"PRIu64"\n", packet->header.fileName, packet->header.contentLength);
  puts("\t---Content\n");
  printf("\t\t%s\n", packet->content);
  return EXIT_SUCCESS;
}
