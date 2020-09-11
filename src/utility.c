#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <math.h>
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

        fprintf(stderr, "File non trovato: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    return fp;
}

int readLine(int fd, char *buffer, char delimiter) {
    int n = 0;
    int count = 0;

    do { /* Read characters until ’\0’ or end-of-input */
        n = read(fd, buffer, 1); /* Read one character */
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

double roundAndShiftLeft(double value, int shift) {
    return (double) (((int) round(value)) << shift);
}

void createEmptyFile(char *filename, char *mode) {
    fclose(openFile(filename, mode));
}

void createSpecialFile(char *filename, mode_t mode, mode_t permissions) {
    unlink(filename);
    mknod(filename, mode, 0);
    chmod(filename, permissions);
}

void createPipe(char *pipename, mode_t permissions) {
    createSpecialFile(pipename, S_IFIFO, permissions);
}

int connectPipe(char *pipename, int mode) {
    int fd;

    do {
        usleep((1 * 1000) * 400); //400 millisecondi
        printf("trying to connect...\n");

        fd = open(pipename, mode);
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

    serverFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Set domain type
    strcpy(serverUNIXAddress.sun_path, socketname); // Set name
    unlink(socketname); // Remove file if it already exists
    bind(serverFd, serverSockAddrPtr, serverLen);// Create file
    listen(serverFd, maximumConnections); // Maximum pending connection length

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

int createClientAF_UNIXSocket(char *socketname, struct sockaddr_un* serverUNIXAddress, struct sockaddr **serverSockAddrPtr, int unsigned *serverLen) {
    int clientFd;

    *serverSockAddrPtr = (struct sockaddr*) serverUNIXAddress;
    *serverLen = sizeof(*serverUNIXAddress);

    clientFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    (*serverUNIXAddress).sun_family = AF_UNIX; //server domain type
    strcpy((*serverUNIXAddress).sun_path, socketname); //server name

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
        usleep((1 * 1000) * 400); //400 millisecondi
        printf("trying to connect...\n");

        result = connect(clientFd, serverSockAddrPtr, serverLen);
    } while(result == -1);
    printf("connected!\n");
}

void setFileFlags(int fd, unsigned int newFlags) {
    int unsigned oldFlags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oldFlags | newFlags);
}

enum boolean socketIsNotConnected(int socketFd) {
    unsigned char buf;
    size_t len = 1;
    int err = recv(socketFd, &buf, len, MSG_PEEK | MSG_DONTWAIT);

    /*
     * MSG_PEEK
              This flag causes the receive operation to return data from the
              beginning of the receive queue without removing that data from
              the queue.  Thus, a subsequent receive call will return the
              same data.
       MSG_DONTWAIT
              Enables nonblocking operation;
     */
    return err == -1
            ? FALSE
            : TRUE;
}

double degreeToRadian(double degree) {
    return degree * ((double) M_PI / 180);
}

double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude){
    //TODO rinominare "dLat" in "differenceLatitude" e "dLon" in "differenceLongitude"?

    double dLat = degreeToRadian(latitude - previousLatitude);
    double dLon = degreeToRadian(longitude - previousLongitude);
    latitude = degreeToRadian(latitude);
    previousLatitude = degreeToRadian(previousLatitude);

    double a = sin(dLat/2) * sin(dLat/2) +
               sin(dLon/2) * sin(dLon/2) * cos(previousLatitude) * cos(latitude);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return RAGGIO_TERRA_METRI * c;
}

double getVelocity(double space, int time){
    /* Spazio in metri, time in secondi m/s */
    return space / time;
}


int numberOfDigits(int value) {
    int digits = 0;

    do {
        value /= 10;
        digits++;
    } while (value != 0);

    return digits;
}

void removeLastChar(char *string) {
    unsigned long length = strlen(string);
    *(string + (length-1)) = 0;
}

void tokenize(char *string, char *separator, int tokenNumber, ...) {
    char *argument;
    char *temp = string;

    va_list listOfArgs;
    va_start(listOfArgs, tokenNumber);

    for(int i=0; i<tokenNumber; i++) {
        argument = va_arg(listOfArgs, char *);

        if(argument != NULL) {
            strcpy(argument, strtok(temp, separator));
        }

        if(i == 0) {
            temp = NULL;
        }
    }

    va_end(listOfArgs);
}


