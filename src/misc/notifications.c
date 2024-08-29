#include "notifications.h"
int notify(char* fileName){
  char* command = "notify-send  \"has been edited\""; 
  puts("whats it gon be\n");
  command = malloc(strlen(command)+strlen(fileName));
  puts("whats it gon be\n");
  if(command==NULL){
    fprintf(stderr, "0x%x: notify command malloc error\n", EXIT_FAIL_MALLOC);
    return EXIT_FAIL_MALLOC;
  }
  puts("worrked\n");
  snprintf(command, strlen(command), "notify-send %s \"has been edited\"", fileName);
  printf("%s\n", command);
  if(system(command) == -1){
    fprintf(stderr, "0x%x: notify system command error\n", EXIT_FAIL_NOTIFY_SEND);
    free(command);
    return EXIT_FAIL_NOTIFY_SEND;
  }
  free(command);
  return EXIT_SUCCESS;
}
