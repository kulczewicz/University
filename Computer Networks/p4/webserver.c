#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "functions.h"

int main(int argc, char* argv[])
{
	if (argc < 3) {
		fprintf (stderr, "Please provide two arguments: port and directory\n");
		return EXIT_FAILURE;
	}

	int port = atoi(argv[1]);

	if (port < 1024 || port > 49151) {
        fprintf(stderr, "port is invalid (must be a value between 1024 and 49151)\n");
        return EXIT_FAILURE;
    }

    if(strlen(argv[2]) > MAX_FILEPATH_LENGTH) {
        fprintf(stderr, "file path is too long (4096 maximum))\n");
        return EXIT_FAILURE;
    }

    char dirAddress[MAX_FILEPATH_LENGTH + MAX_URL_LENGTH];
	strcat(dirAddress, argv[2]);
    /* add backslash to directory address, if one is lacking */
    size_t addressLen = strlen(dirAddress);
    if (dirAddress[addressLen - 1] != '/') {
        dirAddress[addressLen] = '/';
        addressLen++;
    }
    DIR* dir = opendir(dirAddress);

    if (!dir) {
        if (ENOENT == errno) {
            fprintf(stderr, "Directory error: %s\n", strerror(errno)); 
            return EXIT_FAILURE;
        } else {
            fprintf(stderr, "Unknown error\n"); 
            return EXIT_FAILURE;
        }
    }

    return runServer(port, dirAddress, addressLen);
}
