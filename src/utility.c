#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "../include/utility.h"

/*
 * TODO: in queste funzioni sarebbe corretto restituire ogni errore
 *       perchè si tratta di funzioni generiche slegate dalla specifica
 *       implementazione del programma. Magari per semplicità possiamo
 *       ignorare gli errori quando richiamiamo le funzioni.
 */

FILE *openFile(const char* filename, const char* mode) {
    FILE *fp = fopen(filename, mode);

    if (fp == NULL) {
        //TODO come gestire gli errori?

        fprintf(stderr, "File non trovato\n");
        exit(EXIT_FAILURE);
    }

    return fp;
}

int readLine(int fd, char *buffer, char delimiter) {
    int n;
    int count = 0;

    do { /* Read characters until ’\0’ or end-of-input */
        n = read (fd, buffer, 1); /* Read one character */
        count += n;
    } while (n > 0 && *buffer++ != delimiter);

    return count;
}

//TODO implementare scrittura con fprintf e dprintf

int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv) {
    int pid = fork();

    if (pid < 0) {
        //TODO usare perror

        fprintf(stderr, "Fork Failed\n");
        return -1;
    } else if(pid == 0) {
        if(execv_function(filename, argv) == -1) {
            return -1;
        }
    }

    return pid;
}

void createPipe(char *pipename) {
    unlink(pipename);
    mknod(pipename, S_IFIFO, 0);
    chmod(pipename, 0660);
}

int connectPipe(char *pipename, int mode) {
    int fd;

    do {
        fd = open(pipename, mode);
        if (fd == -1) {
            printf("trying to connect...\n");
            usleep((1 * 1000) * 400); //400 millisecondi
        }
    } while (fd == -1);
    printf("connected!\n");

    return fd;
}

int createServerAF_UNIXSocket(char *socketname, int maximumConnections, struct sockaddr **clientSockAddrPtr, int unsigned *clientLen) {
    int serverFd, serverLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    struct sockaddr_un clientUNIXAddress; //Client address

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    *clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    *clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Set domain type
    strcpy (serverUNIXAddress.sun_path, socketname); // Set name
    unlink (socketname); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen);// Create file
    listen (serverFd, maximumConnections); // Maximum pending connection length

    return serverFd;
    /*
    int serverFd, clientFd, serverLen, result;
    int unsigned clientLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    struct sockaddr_un clientUNIXAddress; //Client address
    struct sockaddr* clientSockAddrPtr;//Ptr to client address

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Set domain type
    strcpy (serverUNIXAddress.sun_path, FILENAME_PFC2_SOCKET); // Set name
    unlink (FILENAME_PFC2_SOCKET); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen);// Create file
    listen (serverFd, 1); // Maximum pending connection length
     */
}

int createClientAF_UNIXSocket(char *socketname, struct sockaddr **serverSockAddrPtr, int unsigned *serverLen) {
    int clientFd;
    struct sockaddr_un serverUNIXAddress; //Server address

    *serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    *serverLen = sizeof (serverUNIXAddress);

    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; //server domain type
    strcpy (serverUNIXAddress.sun_path, socketname); //server name

    return clientFd;

    /*
     int clientFd, serverLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);

    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; //server domain type
    strcpy (serverUNIXAddress.sun_path, FILENAME_PFC2_SOCKET); //server name
     */
}

void connectSocket(int clientFd, const struct sockaddr* serverSockAddrPtr, socklen_t serverLen) {
    int result;

    do {
        result = connect(clientFd, serverSockAddrPtr, serverLen);
        if (result == -1) {
            printf("trying to connect...\n");
            usleep((1 * 1000) * 400); //400 millisecondi
        }
    } while(result == -1);
    printf("connected!\n");
}

void setFileFlags(int fd, unsigned int newFlags) {
    int unsigned oldFlags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oldFlags | newFlags);
}

int numberOfDigits(int value) {
    int digits = 0;

    do {
        value /= 10;
        digits++;
    } while (value != 0);

    return digits;
}

enum boolean tokenize(char *string, char *separator, int tokenNumber, ...) {
    char (*token)[];
    char *temp = string;

    va_list listOftokens;
    va_start(listOftokens, tokenNumber);

    for(int i=0; i<tokenNumber; i++) {
        token = va_arg(listOftokens, char (*)[]);
        strcpy(*token, strtok(temp, separator));

        if(token == NULL) {
            return FALSE;
        }

        if(i == 0) {
            temp = NULL;
        }
    }

    va_end(listOftokens);

    return TRUE;
}


