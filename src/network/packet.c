#include "packet.h"
#include <string.h>
PACKET* makePacket(const char* fileName, const char* directory, int* errorCode){
  puts("\n---makePacket---\n");
  const int fileLocationLength = strlen(fileName)+strlen(directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    free(fileLocation);
    return NULL;
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", directory, fileName); 
  printf(" LOCATION = %s\n", fileLocation);
  FILE* file = fopen(fileLocation, "rb");
  free(fileLocation);
  puts("passed\n");
  if(file == NULL){
    *errorCode = EXIT_FAIL_FILE_OPEN;
  }
  puts("passed\n");
  PACKET* packet = malloc(sizeof(PACKET));
  if(packet==NULL){
    fprintf(stderr,"0x%x: makePacket packet malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    return NULL;
  }
  strncpy(packet->header.fileName, fileName, MAX_FILENAME_LENGTH);
  // packet->header.fileName[MAX_FILENAME_LENGTH-1] = '\0';
  fseek(file, 0L, SEEK_END);
  packet->header.contentLength = ftell(file);
  fseek(file, 0L, SEEK_SET);
  packet->content = malloc(packet->header.contentLength);
  // packet->content = malloc(packet->header.contentLength+1);
  if(packet->content==NULL){
    fprintf(stderr,"0x%x: makePacket packet->content malloc error\n", EXIT_FAIL_MALLOC);
    *errorCode = EXIT_FAIL_MALLOC;
    free(packet);
    return NULL;
  }
  puts("reading content...\n");
  size_t bytesRead = fread(packet->content, sizeof(char), packet->header.contentLength, file);
  if(bytesRead!= packet->header.contentLength){
    fprintf(stderr, "0x%x: makePacket file read error\n", EXIT_FAIL_FILE_READ);
    *errorCode = EXIT_FAIL_FILE_READ;
    fclose(file);
    deletePacket(packet);
    return NULL;
  }
  // packet->content[packet->header.contentLength] = '\0';
  fclose(file);
  printPacket(packet);
  checkHex(packet->header.contentLength);
  packet->header.contentLength = bswap_64(packet->header.contentLength);
  return packet;
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
