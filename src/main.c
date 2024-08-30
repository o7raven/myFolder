#include <string.h>
#include <unistd.h>

//windows defined for future
#if defined (_WIN32) || defined (_WIN64)
 #define WINDOWS 1
#elif defined (__unix__)
 #define UNIX 1
#endif

#ifdef WINDOWS
 #define HOMEENV "USERPROFILE"
#elif UNIX
 #define HOMEENV "HOME"
#endif


//#include <intin.h> for Windows
#define __STDC_FORMAT_MACROS
#define MAX_FILENAME_LENGTH 126

//for development purposes later gon delete
static const char* fileToSend = "audio_testing.mp3";



#include "misc/structures.h"
#include "network/server.h"

int client(struct FLAGS* flags);
int printFlags(const struct FLAGS* flags);
int notify(char* message);

int main(int argc, char** argv){
  if(argc < 9){
    printf("Usage: %s  --directory [what directory to watch] --type [server/client] --address --port\n", argv[0]);
    fprintf(stderr, "0x%x: not enough arguments\n", EXIT_NOT_ENOUGH_ARGS);
    exit(EXIT_NOT_ENOUGH_ARGS);
  }
  struct FLAGS flags = {0};
  for(int i = 1; i < 9; i+=2){
    const char* flag = argv[i];
    char* value = argv[i+1];
    printf("Flag: %s\n", flag);
    if(!strcmp("--directory", argv[i]) || !strcmp("-d", argv[i])){
      printf("[%d] Directory flag reached\n", i); 
      flags.dir = value;
    }
    if(!strcmp("--type", argv[i]) || !strcmp("-t", argv[i])){
      printf("[%d] Type flag reached\n", i); 
      if(!strcmp("server", value)){
        flags.type = 0;
        continue;
      }
      if(!strcmp("client", value)){
        flags.type = 1;
        continue;
      }
      puts("\nAborting, wrong \"--type\" flag!\n");
      fprintf(stderr, "0x%x: wrong --type flag\n", EXIT_FAIL_TYPE);
      exit(EXIT_FAIL_TYPE);
    }
    if(!strcmp("--address", argv[i]) || !strcmp("-a", argv[i])){
      printf("[%d] Address flag reached\n", i); 
      flags.addr = value;
    }
    if(!strcmp("--port", argv[i]) || !strcmp("-p", argv[i])){
      printf("[%d] Port flag reached\n", i); 
      flags.port = atoi(value);
      if(flags.port <= 0){
        fprintf(stderr, "0x%x: wrong port input\n", EXIT_FAIL_PORT);
        exit(EXIT_FAIL_PORT);
      }
    }
  }
  if(flags.type == 0)
    makeServer(&flags);
  else
    exit(client(&flags));
  
}



int printFlags(const struct FLAGS* flags){
  printf("PORT: %d\nADDR: %s\nDIR: %s\n", flags->port, flags->addr, flags->dir);
  if(flags->type == 0)
    puts("TYPE: server\n");
  else
    puts("TYPE: client\n");
  return EXIT_SUCCESS;
}

