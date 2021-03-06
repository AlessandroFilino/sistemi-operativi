#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/transducers-socket.h"
#include "../include/config.h"
#include "../include/messages.h"

int main(int argc, const char *argv[]) {
    int serverFd, clientFd;
    unsigned int clientLen;
    struct sockaddr* clientSockAddrPtr;

    serverFd = createServerAF_UNIXSocket(FILENAME_PFC1_SOCKET, MAXIMUM_CONNECTIONS, &clientSockAddrPtr, &clientLen);
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    setFileFlags(clientFd, O_NONBLOCK);

    FILE *speedPFC1Log = openFile(FILENAME_SPEEDPFC1_LOG, "w");
    char velocita_pfc1[PFC_MESSAGE_MAX_LENGTH] = {0};
    enum boolean PFC1terminated = FALSE;

    int numberOfCharsRead = 0;

    while(!PFC1terminated) {
	    usleep((1 * 1000) * 5); //5 millisecondi

        numberOfCharsRead = readLine(clientFd, velocita_pfc1, '\n');

        if(!PFC1terminated && numberOfCharsRead > 0) {
            fprintf(speedPFC1Log, "%s", velocita_pfc1);
            fflush(speedPFC1Log);

            removeLastChar(velocita_pfc1);

            if(strcmp(velocita_pfc1, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC1terminated = TRUE;
            }

            memset(velocita_pfc1, '\0', sizeof(char) * PFC_MESSAGE_MAX_LENGTH);
        } else if(socketIsNotConnected(clientFd) && !PFC1terminated) {
		    close(clientFd);
	
		    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    		setFileFlags(clientFd, O_NONBLOCK);
        }
    }

    close(clientFd);
    close(serverFd);
    fclose(speedPFC1Log);

    return 0;
}
