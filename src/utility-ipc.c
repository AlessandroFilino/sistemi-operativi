#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/utility-ipc.h"

void createSpecialFile(char *filename, mode_t mode, mode_t permissions) {
    unlink(filename);
    mknod(filename, mode, 0);
    chmod(filename, permissions);
}

void createPipe(char *pipename, mode_t permissions) {
    createSpecialFile(pipename, S_IFIFO, permissions);
}

void setFileFlags(int fd, unsigned int newFlags) {
    int unsigned oldFlags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oldFlags | newFlags);
}


int connectPipe(char *pipename, int mode) {
    int fd;

    do {
        usleep((1 * 1000) * 500); //500 millisecondi
        printf("trying to connect...\n");

        fd = open(pipename, mode);
    } while (fd == -1);
    printf("connected!\n");

    return fd;
}

int createServerAF_UNIXSocket(char *socketname, int maximumConnections, struct sockaddr **clientSockAddrPtr, int unsigned *clientLen) {
    int serverFd, serverLen;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    struct sockaddr_un clientUNIXAddress;

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    *clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    *clientLen = sizeof (clientUNIXAddress);

    serverFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy(serverUNIXAddress.sun_path, socketname);
    unlink(socketname);
    bind(serverFd, serverSockAddrPtr, serverLen);
    listen(serverFd, maximumConnections);

    return serverFd;
}

int createClientAF_UNIXSocket(char *socketname, struct sockaddr_un* serverUNIXAddress, struct sockaddr **serverSockAddrPtr, int unsigned *serverLen) {
    int clientFd;

    *serverSockAddrPtr = (struct sockaddr*) serverUNIXAddress;
    *serverLen = sizeof(*serverUNIXAddress);

    clientFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    (*serverUNIXAddress).sun_family = AF_UNIX; //server domain type
    strcpy((*serverUNIXAddress).sun_path, socketname); //server name

    return clientFd;
}

void connectSocket(int clientFd, const struct sockaddr* serverSockAddrPtr, socklen_t serverLen) {
    int result;

    do {
        usleep((1 * 1000) * 500); //500 millisecondi
        printf("trying to connect...\n");

        result = connect(clientFd, serverSockAddrPtr, serverLen);
    } while(result == -1);
    printf("connected!\n");
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

