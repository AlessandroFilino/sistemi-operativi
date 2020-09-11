#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../include/utility.h"
#include "../include/transducers.h"
#include "../include/path.h"
#include "../include/messages.h"

enum boolean is_connected(int sock)
{
    unsigned char buf;
    int err = recv(sock, &buf, 1, MSG_PEEK | MSG_DONTWAIT);
    return err == -1 ? FALSE : TRUE;
}

int main(int argc, const char *argv[]) {
	//pfc2
    int serverFd, clientFd;
    unsigned int clientLen;
    struct sockaddr* clientSockAddrPtr;

    serverFd = createServerAF_UNIXSocket(FILENAME_PFC2_SOCKET, MAXIMUM_CONNECTIONS, &clientSockAddrPtr, &clientLen);
    clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    setFileFlags(clientFd, O_NONBLOCK);

    FILE *speedPFC2Log = openFile(FILENAME_SPEEDPFC2_LOG, "w");
    char velocita_pfc2[15] = {0};
    enum boolean PFC2terminated = FALSE;

    int numberOfCharsRead = 0;

    while(!PFC2terminated) {
	usleep((1 * 1000) * 5); 

	 //pfc2
	/*	
		int error = 0;
		socklen_t len = sizeof(error);
		int retval = getsockopt(serverFd, SOL_SOCKET, SO_ERROR, &error, &len);

		if(error != 0) {
			printf("socket error: %s\n", strerror(error));
			fflush(stdout);
		}
	*/
        numberOfCharsRead = readLine(clientFd, velocita_pfc2, MESSAGES_SEPARATOR);
        //printf("pfc2 - transducer: %s (%d)\n", velocita_pfc2, tempo);

        if(!PFC2terminated && numberOfCharsRead > 0) {
            fprintf(speedPFC2Log, "%s", velocita_pfc2);
            fflush(speedPFC2Log);

            removeLastChar(velocita_pfc2);

            //printf("pfc2 - transducer: (%d) %s\n", tempo, velocita_pfc2);
            //fflush(stdout);

            if(strcmp(velocita_pfc2, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC2terminated = TRUE;
            }

            memset(velocita_pfc2, '\0', sizeof(char) * 15);
        } else if(is_connected(clientFd) && !PFC2terminated) {
		close(clientFd);
	
		clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
    		setFileFlags(clientFd, O_NONBLOCK);
	  }
    }

    close(clientFd);
    close(serverFd);
    fclose(speedPFC2Log);

    return 0;
}
