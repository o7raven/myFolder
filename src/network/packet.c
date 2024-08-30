#include "packet.h"
#include <string.h>
int makePacket(AGENT* agent,const char* fileName){
  puts("\n---makePacket---\n");
  const int fileLocationLength = strlen(fileName)+strlen(agent->directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    exit(EXIT_FAIL_MALLOC);
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", agent->directory, fileName); 
  printf(" LOCATION = %s\n", fileLocation);
  FILE* file = fopen(fileLocation, "rb");
  free(fileLocation);
puts("passed\n");
  if(file == NULL){
    exit(EXIT_FAIL_FILE_OPEN);
  }
  puts("passed\n");
  agent->packet = malloc(sizeof(PACKET));
  if(agent->packet==NULL){
    fprintf(stderr,"0x%x: makePacket packet malloc error\n", EXIT_FAIL_MALLOC);
    exit(EXIT_FAIL_MALLOC);
  }
  strncpy(agent->packet->header.fileName, fileName, MAX_FILENAME_LENGTH);
  // packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  fseek(file, 0L, SEEK_END);
  agent->packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  agent->packet->content = malloc(agent->packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  if(agent->packet->content==NULL){
    fprintf(stderr,"0x%x: makePacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    free(agent->packet);
    exit(EXIT_FAIL_MALLOC);
  }
  puts("reading content...\n");
  size_t bytesRead = fread(agent->packet->content, sizeof(char), agent->packet->header.contentLength, file);
  if(bytesRead!= agent->packet->header.contentLength){
    fprintf(stderr, "0x%x: makePacket file read error\n", EXIT_FAIL_FILE_READ);
    fclose(file);
    deletePacket(agent->packet);
    exit(EXIT_FAIL_FILE_READ);
  }
  // packet->content[packet->header.contentLength] = '\0';
  fclose(file);
  printPacket(agent->packet);
  checkHex(agent->packet->header.contentLength);
  agent->packet->header.contentLength = bswap_64(agent->packet->header.contentLength);
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
