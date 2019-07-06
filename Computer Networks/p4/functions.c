#include "functions.h"

/*
 * Free dynamicly allocated memory of httpRequest structure
 */
void freeHttpRequestMemory(struct httpRequest *httpRequest) {
	free(httpRequest->url);
	free(httpRequest->connection);
	free(httpRequest->host);
	free(httpRequest);
}

/*
 * Creates header, filling the headerBuffer
 */
void prepareHeader(char headerBuffer[], int statusCode, char path[], long int fileSize) {
	/* Add first line */
	strcat(headerBuffer, "HTTP/1.1 ");
	if (statusCode == 200) {
		strcat(headerBuffer, "200 OK\r\n");
	} else if (statusCode == 301) {
		strcat(headerBuffer, "301 Moved Permanently\r\n");
	} else if (statusCode == 403) {
		strcat(headerBuffer, "403 Forbidden\r\n");
	} else if (statusCode == 404) {
		strcat(headerBuffer, "404 Not Found\r\n");
	} else if (statusCode == 414) {
		strcat(headerBuffer, "414 Request-URI Too Long\r\n");
	} else if (statusCode == 501) {
		strcat(headerBuffer, "501 Not Implemented\r\n");
	}
	int pathLen = strlen(path);
	int lastDotIndex = 0;
	for(int i = 0; i<pathLen; i++) {
		if(path[i] == '.') {
			lastDotIndex = i;
		}
	}
	/* Add Content-Type */
	char * fileFormat = malloc(pathLen-lastDotIndex);
	memcpy(fileFormat, path+lastDotIndex+1, pathLen-lastDotIndex);
	strcat(headerBuffer, "Content-Type: ");
	if (strcmp(fileFormat, "txt") == 0) {
		strcat(headerBuffer, "text/plain");
	} else if (strcmp(fileFormat, "html") == 0) {
		strcat(headerBuffer, "text/html");
	} else if (strcmp(fileFormat, "css") == 0) {
		strcat(headerBuffer, "text/css");
	} else if ((strcmp(fileFormat, "jpg") == 0) || (strcmp(fileFormat, "jpeg") == 0)) {
		strcat(headerBuffer, "image/jpeg");
	} else if (strcmp(fileFormat, "png") == 0) {
		strcat(headerBuffer, "image/png");
	} else if (strcmp(fileFormat, "pdf") == 0) {
		strcat(headerBuffer, "application/pdf");
	} else {
		strcat(headerBuffer, "application/octet-stream");
	}
	free(fileFormat);
	strcat(headerBuffer, "\r\n");
	/* Add Content-Length */
	strcat(headerBuffer, "Content-Length: ");
	char fileSizeStr[20];
	sprintf(fileSizeStr, "%ld", fileSize);
	strcat(headerBuffer, fileSizeStr);
	strcat(headerBuffer, "\r\n\r\n");
}


/*
 * Sends header and body depending on statusCode and path to socket
 */
void sendResponse(int socket, int statusCode, char path[]) {
	char doFree = 0;
	/* set path to particular error page in case of error code */
	if (!path) {
		char * addressPrefix = "errorpages/";
		int statusCodeLen = 3;
		char fileName[statusCodeLen];
		sprintf(fileName, "%d", statusCode);
		char * addressSufix = ".html";
		path = calloc(strlen(addressPrefix) + statusCodeLen + strlen(addressSufix), (sizeof(char)));
		strcat(path, addressPrefix);
		strcat(path, fileName);
		strcat(path, addressSufix);
		doFree = 1;
	}
	/* open file from path and find its length */
	int read_fd = open(path, O_RDONLY);
	struct stat st;
	stat(path, &st);

	/* prepare response header */
	char headerBuffer[1000] = {0};
	prepareHeader(headerBuffer, statusCode, path, st.st_size);
	int headerLen = strlen(headerBuffer);
	
	write(socket, headerBuffer, headerLen);
	sendfile(socket, read_fd, 0, st.st_size);
	/* prints header of response that was sent */
	printf("%s", headerBuffer);
	if (doFree) free(path);
}

/*
 * Parse raw Http request line, detects, if its first request line or header field, finds
 * url address and "Connection" and "Host" header fields and assigns it to the according fields of
 * httpRequest struct. Returns 1 in case of success, else returns http error status code
 */
int parseHttpRequestLine(char rawHttpRequest[], struct httpRequest *httpRequest, size_t lineLen) {
    if (memcmp(rawHttpRequest, "GET ", 4 /* length of "GET " */) == 0) /* first line */ {
		rawHttpRequest += 4;
		if(rawHttpRequest[0]!='/') return 501; /* url should at least consist '/' character at the beggining */

		size_t urlSize = strcspn(rawHttpRequest, " ");
		if (urlSize > MAX_URL_LENGTH) {
			return 414;
		} else if (urlSize + 4 == lineLen) /* if line ends on url and does not have version */ {
			return 501;
		}
		httpRequest->url = malloc(urlSize + 1);
		if (!httpRequest->url) {
			freeHttpRequestMemory(httpRequest);
			ERROR("url error");
		}
		memcpy(httpRequest->url, rawHttpRequest, urlSize);
		httpRequest->url[urlSize] = '\0';
	} else /* header field */ {
		if (memcmp(rawHttpRequest, "\r\n", 2) != 0) {
			size_t nameLen = strcspn(rawHttpRequest, ": ");
			if(nameLen == lineLen) return 501;
			int contentLen = strcspn(rawHttpRequest+nameLen+2, "\r\n"); /* count content length after ": " */
			if(contentLen == 0) return 501;
			if (memcmp(rawHttpRequest, "Connection", nameLen) == 0) {
				httpRequest->connection = malloc(contentLen + 1);
				if (!httpRequest->connection) {
					freeHttpRequestMemory(httpRequest);
					ERROR("connection error");
				}
				memcpy(httpRequest->connection, rawHttpRequest+nameLen+2, contentLen);
				httpRequest->connection[contentLen] = '\0';
			} else if (memcmp(rawHttpRequest, "Host", nameLen) == 0) {
				size_t hostWithoutPortLen = strcspn(rawHttpRequest+nameLen+2, ":");
				if (hostWithoutPortLen > MAX_URL_LENGTH) {
					return 414;
				}
				httpRequest->host = malloc(hostWithoutPortLen + 1);
				if (!httpRequest->host) {
					freeHttpRequestMemory(httpRequest);
					ERROR("host error");
				}
				memcpy(httpRequest->host, rawHttpRequest+nameLen+2, hostWithoutPortLen);
				httpRequest->host[hostWithoutPortLen] = '\0';
			}
		}
	}
	return 1;
}

/*
 * Processes the request basing on httpRequest and address and calls sendResponse with following codes:
 * 200 OK, if file exists
 * 301 Moved Permanently if client wants to fetch folder. In that case, index.html of that folder is returned 
 * 		(if it exists, elsewhere 404 is returned)
 * 403 Forbidden, if client wants to fetch address beyond the current domain
 * 404 Not Found, if client wants to fetch address that does not exist
 * 414 Request-URI, Too Long if request uri is too long
 */
void processRequest(int socket, struct httpRequest *httpRequest, char address[], size_t addressLen) {
	if (strlen(httpRequest->host) + strlen(httpRequest->url) > MAX_URL_LENGTH) {
		sendResponse(socket, 414, NULL);
		return;
	} else if (strstr(httpRequest->url, "/../") || strstr(httpRequest->url, "/..")) {
		sendResponse(socket, 403, NULL);
		return;
	} else {
		char currentAddress[MAX_FILEPATH_LENGTH + MAX_URL_LENGTH] = {0};
		strncpy(currentAddress, address, addressLen);
		strcat(currentAddress, httpRequest->host);
		strcat(currentAddress, httpRequest->url);
		struct stat st;
		stat(currentAddress, &st);
		if(S_ISREG(st.st_mode)) {
			sendResponse(socket, 200, currentAddress);
			return;
		} else if (S_ISDIR(st.st_mode)){
			strcat(currentAddress, "index.html");
			stat(currentAddress, &st);
			if(S_ISREG(st.st_mode)) {
				sendResponse(socket, 301, currentAddress);
				return;
			}
		}
		sendResponse(socket, 404, NULL);
		return;
	}
}

/*
 * Processes requests for single connection in a loop, parsing them line by line
 */
int handleRequestsForSingleConnection(int socket, char recvBuffer[], struct httpRequest *httpRequest, char address[], size_t addressLen) {
	struct timeval tv; tv.tv_sec = TIMEOUT_SEC; tv.tv_usec = 0;		// pozostały czas
	char * buffer = recvBuffer;
	int totalBytesRead = 0;
	int bytesProcessed = 0;

	for(;;) {
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (socket, &descriptors);
		
		/* select on socket file descriptor */
		int ready = select(socket+1, &descriptors, NULL, NULL, &tv);
		if (ready < 0)
			ERROR("select error");
		if (ready == 0) return 0; /* timeout is reached */

		/* receive data to specific place in buffer */
		int bytesRead = recv(socket, recvBuffer + totalBytesRead, BUFFER_SIZE - totalBytesRead, 0);
		totalBytesRead += bytesRead;
		if (bytesRead <= 0) return bytesRead; /* error occured */
		
		/* process buffer line by line */
		while(bytesProcessed < totalBytesRead) {
			size_t fullLength = strlen(buffer);
			size_t lineLength = strcspn(buffer, "\r\n");

			/* newline is missing in the last line, breaking the loop and waiting 
			for the missing part of message until the end of timeout */
			if(fullLength == lineLength) break;

			int parsed = parseHttpRequestLine(buffer, httpRequest, lineLength);
			/* error occured, parsed variable consists error code */
			if(parsed != 1) sendResponse(socket, parsed, NULL);
			/* all needed fields of request are processed and response can be sent */
			if(httpRequest->url && httpRequest->host && httpRequest->connection) {
				processRequest(socket, httpRequest, address, addressLen);
				httpRequest->url = NULL;
				httpRequest->host = NULL;
				/* break the connection if "Connection" field is "close" */ 
				if (strcmp(httpRequest->connection, "close") == 0) {
					httpRequest->connection = NULL;
					return 0;
				}
				httpRequest->connection = NULL;
			}
			buffer += lineLength + 2;
			bytesProcessed += lineLength + 2;
		}
	}
	return 1;
}

/*
 * Main loop of the server
 */
int runServer(int port, char address[], size_t addressLen) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ERROR("socket error");

	struct sockaddr_in server_address;
	memset (&server_address, 0, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	int reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)))
		ERROR("SO_REUSEADDR error");
	if (bind (sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) 
		ERROR("bind error");

	if (listen (sockfd, 64) < 0)
		ERROR("listen error");
    
	printf("Waiting for incoming requests:\n\n");
    for (;;) {
		/* create new socket address and socket for incoming messages */
		struct sockaddr_in client_address;
		memset (&client_address, 0, sizeof(client_address));
		socklen_t len = sizeof(client_address);
		int conn_sockfd = accept (sockfd, (struct sockaddr*) &client_address, &len);
		if (conn_sockfd < 0)
			ERROR("accept error");

		char recvBuffer[BUFFER_SIZE] = {0};
		/* creating httpRequest struct for single connection */
		struct httpRequest *httpRequest = calloc(sizeof *httpRequest, sizeof *httpRequest);
		if (!httpRequest) ERROR("httpRequest allocation error");
		/* processing requests in one connection */
		handleRequestsForSingleConnection(conn_sockfd, recvBuffer, httpRequest, address, addressLen);
		/* free httpRequest struct */
		freeHttpRequestMemory(httpRequest);
        
		if (close (conn_sockfd) < 0)
			ERROR("close error");
    }
    return 0;
}