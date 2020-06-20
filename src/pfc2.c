//
//  pfc2.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 18/06/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#include "pfc.h"

int main(int argc, const char * argv[]) {
    char* velocita;
    double latitudine_prec = 0;
    double longitudine_prec = 0;

    //open g18.txt
    FILE *fp = open_file(argv[1], "r");
    
    int clientFd, serverLen, result;
    struct sockaddr_un serverUNIXAddress; /*Server address */
    struct sockaddr* serverSockAddrPtr; /*Ptr to server address*/
    
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; /* server domain type */
    strcpy (serverUNIXAddress.sun_path, "pfc2"); /* server name */
    
    for(;;) {
        sleep(1);
        velocita = velocitaCalcolata(fp, &latitudine_prec, &longitudine_prec);

        do {
            result = connect (clientFd, serverSockAddrPtr, serverLen);
        } while(result == -1);

        write (clientFd, velocita, sizeof(velocita));
    }

    close(fp);
    close(clientFd);
}


