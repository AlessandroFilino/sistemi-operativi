#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/config.h"
#include "../include/messages.h"

int main(int argc, const char * argv[]) {
    int numberOfCharsRead = 0;

    //transducers-socket e pfc1
    char *transducers_socket_argv[] = {"transducers-socket", NULL};
    createChild(&execv, "transducers-socket", transducers_socket_argv);

    //pfc2
    createPipe(FILENAME_PFC2_PIPE, DEFAULT_PERMISSIONS);
    int fd_PFC2pipe = open(FILENAME_PFC2_PIPE, O_RDONLY | O_NONBLOCK);
    FILE *speedPFC2Log = openFile(FILENAME_SPEEDPFC2_LOG, "w");
    char velocita_pfc2[PFC_MESSAGE_MAX_LENGTH] = {0};
    enum boolean PFC2terminated = FALSE;

    //pfc3
    int fd_PFC3File = open(FILENAME_PFC3_FILE, O_RDONLY | O_CREAT, 0660);
    FILE *speedPFC3Log = openFile(FILENAME_SPEEDPFC3_LOG, "w");
    char velocita_pfc3[PFC_MESSAGE_MAX_LENGTH] = {0};
    enum boolean PFC3terminated = FALSE;

    /*
     * I messaggi ricevuti da pfc1/2/3 contengono un carattere '\n'
     * che permette di distinguere un messaggio dall'altro.
     * Dovendo scrivere i valori delle velocità nei file di log,
     * è opportuno utilizzare anche in questo caso un carattere
     * che separi tali valori per rendere i file di log leggibili.
     */

	while(PFC2terminated == FALSE || PFC3terminated == FALSE) {
        usleep((1 * 1000) * 5); //5 millisecondi

        //pfc2
        numberOfCharsRead = readLine(fd_PFC2pipe, velocita_pfc2, '\n');
        if(PFC2terminated == FALSE && numberOfCharsRead > 0) {
            fprintf(speedPFC2Log, "%s", velocita_pfc2);
            fflush(speedPFC2Log);

            removeLastChar(velocita_pfc2);

            if(strcmp(velocita_pfc2, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC2terminated = TRUE;
            }

            memset(velocita_pfc2, '\0', sizeof(char) * PFC_MESSAGE_MAX_LENGTH);
        }

        //pfc3
        numberOfCharsRead = readLine(fd_PFC3File, velocita_pfc3, '\n');
        if(PFC3terminated == FALSE && numberOfCharsRead > 0) {
            fprintf(speedPFC3Log, "%s", velocita_pfc3);
            fflush(speedPFC3Log);

            removeLastChar(velocita_pfc3);

            if(strcmp(velocita_pfc3, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC3terminated = TRUE;
            }

            memset(velocita_pfc3, '\0', sizeof(char) * PFC_MESSAGE_MAX_LENGTH);
        }
    }

	int status = 0;
	int pid = wait(&status);


	if (!WIFEXITED(status)) {
	    printf("transducers: Child process %d exited with %d status\n", pid, WEXITSTATUS(status));
	}

    //pfc2
    close(fd_PFC2pipe);
    fclose(speedPFC2Log);

    //pfc3
    close(fd_PFC3File);
    fclose(speedPFC3Log);

    return 0;
}

