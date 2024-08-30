#include "saveFile.h"

int saveFile(AGENT* agent){
  puts("fopen section...\n");
  const int fileLocationLength = strlen(agent->packet->header.fileName)+strlen((*agent).directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    return EXIT_FAIL_MALLOC;
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", (*agent).directory, agent->packet->header.fileName); 
  // FILE* newFile = fopen(receivedPacket->header.fileName, "wb");
  FILE* newFile = fopen(fileLocation, "wb");
  if(newFile == NULL){
    close(agent->socket);
    deletePacket(agent->packet);
    fprintf(stderr, "0x%x: agent fail file open\nfilename:%s\n", EXIT_FAIL_FILE_OPEN, agent->packet->header.fileName);
    return EXIT_FAIL_FILE_OPEN;
  }
  puts("passed\n");
  puts("fwrite section...\n");
  uint64_t bytesWritten = fwrite(agent->packet->content, sizeof(char), agent->packet->header.contentLength, newFile);
  if(bytesWritten != agent->packet->header.contentLength){
    close(agent->socket);
    deletePacket(agent->packet);
    fprintf(stderr, "0x%x: agent file written characters:%"PRIu64"d\ncharacters to be written:%zu\n", EXIT_FAIL_FWRITE, bytesWritten, agent->packet->header.contentLength);
    return EXIT_FAIL_FWRITE;
  }
  puts("passed\n");
  fclose(newFile);
  puts("mallocing message section...\n");
  puts("passed\n");
  notify(agent->packet->header.fileName);
  return EXIT_SUCCESS;
}


