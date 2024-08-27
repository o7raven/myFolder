#ifndef ERROR_CODES_H_
#include <stdlib.h>
#define ERROR_CODES_H_
#define EXIT_FAIL_SOCKET_CREATE 0x01
#define EXIT_FAIL_SOCKET_BIND 0x02
#define EXIT_FAIL_SOCKET_LISTEN 0x03
#define EXIT_FAIL_SOCKET_ACCEPT 0x04
#define EXIT_FAIL_SOCKET_CONNECT 0x05
#define EXIT_NOT_ENOUGH_ARGS 0x06
#define EXIT_FAIL_PORT 0x07
#define EXIT_FAIL_TYPE 0x08
#define EXIT_FAIL_FILE_OPEN 0x09
#define EXIT_FAIL_FILE_READ 0x0A
#define EXIT_FAIL_SOCKET_RECEIVE 0x0B
#define EXIT_FAIL_SOCKET_SEND 0x0C
#define EXIT_FAIL_NOTIFY_SEND 0x0D
#define EXIT_FAIL_SOCKET_REUSE 0x0E
#define EXIT_FAIL_MALLOC 0x0F
#define EXIT_FAIL_FWRITE 0x10

#define EXIT_FAIL_FATAL 0xFF
#endif