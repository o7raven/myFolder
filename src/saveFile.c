#include "saveFile.h"

int saveFile(CLIENT* client, const char* directory){
  puts("fopen section...\n");
  const int fileLocationLength = strlen(client->packet->header.fileName)+strlen(directory)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    return EXIT_FAIL_MALLOC;
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", directory, client->packet->header.fileName); 
  // FILE* newFile = fopen(receivedPacket->header.fileName, "wb");
  FILE* newFile = fopen(fileLocation, "wb");
  if(newFile == NULL){
    close(client->clientSocket);
    deletePacket(client->packet);
    fprintf(stderr, "0x%x: client fail file open\nfilename:%s\n", EXIT_FAIL_FILE_OPEN, client->packet->header.fileName);
    return EXIT_FAIL_FILE_OPEN;
  }
  puts("passed\n");
  puts("fwrite section...\n");
  uint64_t bytesWritten = fwrite(client->packet->content, sizeof(char), client->packet->header.contentLength, newFile);
  if(bytesWritten != client->packet->header.contentLength){
    close(client->clientSocket);
    deletePacket(client->packet);
    fprintf(stderr, "0x%x: client file written characters:%"PRIu64"d\ncharacters to be written:%zu\n", EXIT_FAIL_FWRITE, bytesWritten, client->packet->header.contentLength);
    return EXIT_FAIL_FWRITE;
  }
  puts("passed\n");
  fclose(newFile);
  puts("mallocing message section...\n");
  puts("passed\n");
  notify(client->packet->header.fileName);
  return EXIT_SUCCESS;
}


