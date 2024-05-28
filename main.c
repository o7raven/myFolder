#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

#define EXIT_SUCCESS 0
#define EXIT_FAIL 1
#define EXIT_NOT_ENOUGH_ARGS 2

int main(int argc, char** argv){ 
  if(argc < 7){
    printf("Usage: %s -d [directory] -p [port] -t [type]\n", argv[0]);
    return EXIT_NOT_ENOUGH_ARGS;
  }
  int port;
  for(int i = 1; i < 7; i+=2){
    char* flag = argv[i];
    char* value = argv[i+1];
    printf("flag %s and value %s \n", flag, value);
    if(strcmp(flag, "-p")){
      puts("compared");
      port = atoi(value);
    }
  }
  printf("The port is %d\n", port);


  return EXIT_SUCCESS;
} 
