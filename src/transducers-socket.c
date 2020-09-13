#include <stdlib.h>
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
	//pfc1
    int serverFd, clientFd;
    unsigned int clientLen;
    struct sockaddr* clientSockAddrPtr;

    serverFd = createServerAF_UNIXSocket(FILENAME_PFC1_SOCKET, MAXIMUM_CONNECTIONS, &clientSockAddrPtr, &clientLen);
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    setFileFlags(clientFd, O_NONBLOCK);

    FILE *speedPFC1Log = openFile(FILENAME_SPEEDPFC1_LOG, "w");
    char velocita_pfc1[15] = {0};
    enum boolean PFC1terminated = FALSE;

    int numberOfCharsRead = 0;

    while(!PFC1terminated) {
	usleep((1 * 1000) * 5); //5 millisecondi

	 //pfc1
        numberOfCharsRead = readLine(clientFd, velocita_pfc1, MESSAGES_SEPARATOR);
        //printf("pfc2 - transducer: %s (%d)\n", velocita_pfc1, tempo);

        if(!PFC1terminated && numberOfCharsRead > 0) {
            fprintf(speedPFC1Log, "%s", velocita_pfc1);
            fflush(speedPFC1Log);

            removeLastChar(velocita_pfc1);

            //printf("pfc2 - transducer: (%d) %s\n", tempo, velocita_pfc1);
            //fflush(stdout);

            if(strcmp(velocita_pfc1, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC1terminated = TRUE;
            }

            memset(velocita_pfc1, '\0', sizeof(char) * 15);
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
