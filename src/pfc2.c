#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */
#include "utility.h"
#include "pfc.h"

int main(int argc, const char * argv[]) {
    double latitudine_prec = 0;
    double longitudine_prec = 0;

    //TODO char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";
    FILE *fp = open_file(filename_g18, "r");

    //TODO unlink va rimosso
    unlink ("transducers_server");
    int clientFd, serverLen, result;
    struct sockaddr_un serverUNIXAddress; /*Server address */
    struct sockaddr* serverSockAddrPtr; /*Ptr to server address*/
    
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; /* server domain type */
    strcpy (serverUNIXAddress.sun_path, "transducers_server"); /* server name */

    //TODO result mi serve? o posso cambiare connectSocket() in void?
    result = connectSocket(clientFd, serverSockAddrPtr, serverLen);

    ssize_t read = 0;
    size_t bufferLength = MAX_LINE_LENGTH + 1;
    char *buffer = malloc(sizeof(char) * bufferLength);
    read = readCorrectLine(buffer, bufferLength, fp);
    acquisisciCoordinate(buffer, &latitudine_prec, &longitudine_prec);
    free(buffer);

    while(read != -1) {
        //TODO usare sleep(1)
        usleep((1 * 1000) * 1000); //1000 millisecondi

        read = exe(clientFd, fp, &latitudine_prec, &longitudine_prec);
    }

    fclose(fp);
    close(clientFd);
}


