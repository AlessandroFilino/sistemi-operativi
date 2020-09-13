#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/transducers.h"
#include "../include/config.h"
#include "../include/messages.h"

int main(int argc, const char * argv[]) {
    int tempo = 0; //microsecondi trascorsi
    int numberOfCharsRead = 0;

    //transducers-socket e pfc1
    char *transducers_socket_argv[] = {"transducers-socket", NULL};
    int transducers_socket_pid = createChild(&execv, "transducers-socket", transducers_socket_argv);

    //pfc2
    createPipe(FILENAME_PFC2_PIPE, DEFAULT_PERMISSIONS);
    int fd_PFC2pipe = open(FILENAME_PFC2_PIPE, O_RDONLY | O_NONBLOCK);
    FILE *speedPFC2Log = openFile(FILENAME_SPEEDPFC2_LOG, "w");
    char velocita_pfc2[15] = {0};
    enum boolean PFC2terminated = FALSE;

    //pfc3
    int fd_PFC3File = open(FILENAME_PFC3_FILE, O_RDONLY | O_CREAT, 0660);
    FILE *speedPFC3Log = openFile(FILENAME_SPEEDPFC3_LOG, "w");
    char velocita_pfc3[15] = {0};
    enum boolean PFC3terminated = FALSE;

    /*
     * I messaggi ricevuti da pfc1/2/3 contengono un carattere '\n'
     * che permette di distinguere un messaggio dall'altro.
     * Dovendo scrivere i valori delle velocità nei file di log,
     * è opportuno utilizzare anche in questo caso un carattere
     * che separi tali valori per rendere i file di log leggibili.
     * Abbiamo deciso di usare il medesimo carattere, ovvero MESSAGES_SEPARATOR.
     * Successivamente, il while deve controllare se velocita_pfc2/2/3 contengono
     * la stringa di terminazione, perciò è necessario rimuovere il carattere
     * MESSAGES_SEPARATOR
     */


    //while(PFC2terminated == FALSE || PFC2terminated == FALSE || PFC3terminated == FALSE) {
	while(PFC2terminated == FALSE || PFC3terminated == FALSE) {

        //TODO usare usleep(5000) --> sleep di 5 millisecondi
        usleep((1 * 1000) * 5); //5 millisecondi

        /*
         * TODO: Visto che il transducers va più veloce
         *       dei pfc, non c'è bisogno di settare a zero
         *       le stringhe velocita_pfc2, velocita_pfc2,
         *       velocita_pfc3 perchè, leggendo da un buffer vuoto,
         *       si setteranno a zero da sole (?)
         *
         * TODO: però forse leggendo da un buffer vuoto, velocita_pfc2,
         *       velocita_pfc2, velocita_pfc3 vengono sporcate con valori
         *       rimasti in memoria? Se si, ha senso lasciare il memset fuori
         *       dall'if, altrimenti va inserito dentro l'if
         */

        //pfc2
        numberOfCharsRead = readLine(fd_PFC2pipe, velocita_pfc2, MESSAGES_SEPARATOR);
        //printf("pfc1 - transducer: %s (%d)\n", velocita_pfc2, tempo);
        if(PFC2terminated == FALSE && numberOfCharsRead > 0) {
            fprintf(speedPFC2Log, "%s", velocita_pfc2);
            fflush(speedPFC2Log);

            removeLastChar(velocita_pfc2);

            //printf("pfc1 - transducer: (%d) %s\n", tempo, velocita_pfc2);
            //fflush(stdout);

            if(strcmp(velocita_pfc2, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC2terminated = TRUE;
            }
            memset(velocita_pfc2, '\0', sizeof(char) * 15);
        }

        //pfc3
        numberOfCharsRead = readLine(fd_PFC3File, velocita_pfc3, MESSAGES_SEPARATOR);
        //printf("pfc3 - transducer: %s (%d)\n", velocita_pfc3, tempo);
        if(PFC3terminated == FALSE && numberOfCharsRead > 0) {
            fprintf(speedPFC3Log, "%s", velocita_pfc3);
            fflush(speedPFC3Log);

            removeLastChar(velocita_pfc3);

            if(strcmp(velocita_pfc3, APPLICATION_ENDED_MESSAGE) == 0) {
                PFC3terminated = TRUE;
            }

            memset(velocita_pfc3, '\0', sizeof(char) * 15);
        }

        tempo++;
    }

	int status;
	int pid = wait(&status);

        //Child process exited normally, through `return` or `exit`
        if (WIFEXITED(status)) {
            char name[50] = {0};

            if(pid == transducers_socket_pid) {
                strcpy(name, "transducers-socket"); 
		    } else {
                snprintf(name, sizeof(char) * 10, "%d", pid);
            }
            
            printf("transducers: Child process '%s' exited with %d status\n", name, WEXITSTATUS(status));
        }

    //pfc2
    close(fd_PFC2pipe);
    fclose(speedPFC2Log);

    //pfc3
    close(fd_PFC3File);
    fclose(speedPFC3Log);

    return 0;
}

