#include "saveFile.h"

int saveFIle(const PACKET*){
  puts("fopen section...\n");
  const int fileLocationLength = strlen(fileToSend)+strlen((*flags).dir)+2; 
  char* fileLocation = malloc(fileLocationLength);
  if(fileLocation == NULL){
    fprintf(stderr,"0x%x: makePacket filelocation malloc error\n", EXIT_FAIL_MALLOC);
    free(fileLocation);
    return EXIT_FAIL_MALLOC;
  }
  snprintf(fileLocation, fileLocationLength,"%s/%s", (*flags).dir, fileToSend); 
  // FILE* newFile = fopen(receivedPacket->header.fileName, "wb");
  FILE* newFile = fopen(fileLocation, "wb");
  if(newFile == NULL){
    close(clientSocket);
    deletePacket(receivedPacket);
    fprintf(stderr, "0x%x: client fail file open\nfilename:%s\n", EXIT_FAIL_FILE_OPEN, receivedPacket->header.fileName);
    return EXIT_FAIL_FILE_OPEN;
  }
  puts("passed\n");
  puts("fwrite section...\n");
  uint64_t bytesWritten = fwrite(receivedPacket->content, sizeof(char), receivedPacket->header.contentLength, newFile);
  if(bytesWritten != receivedPacket->header.contentLength){
    close(clientSocket);
    deletePacket(receivedPacket);
    fprintf(stderr, "0x%x: client file written characters:%"PRIu64"d\ncharacters to be written:%zu\n", EXIT_FAIL_FWRITE, bytesWritten, receivedPacket->header.contentLength);
    return EXIT_FAIL_FWRITE;
  }
  puts("passed\n");
  fclose(newFile);
  puts("mallocing message section...\n");
  puts("passed\n");
  notify(receivedPacket->header.fileName);
}

