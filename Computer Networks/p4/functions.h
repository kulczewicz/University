#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(EXIT_FAILURE); }
#define MAX_URL_LENGTH 2001
#define MAX_FILEPATH_LENGTH 4096
#define BUFFER_SIZE 30000
#define TIMEOUT_SEC 1

struct httpRequest {
    char* url;
    char* connection;
    char* host;
};

struct args {
    int socket;
    char * recvBuffer;
    int * totalBytesRead;
};

struct httpResponse {
    int statusCode;
    char* statusMessage;
};

int runServer(int port, char address[], size_t addressLen);