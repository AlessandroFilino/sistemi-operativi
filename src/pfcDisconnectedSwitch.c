#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "../include/pfcDisconnectedSwitch.h"
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/config.h"
#include "../include/messages.h"

int main(int argc, const char *argv[]) {
    unsigned int pathnameLength = strlen(argv[1]) + 1;
    char *filename_g18 = malloc(sizeof(char) * pathnameLength);
    strcpy(filename_g18, argv[1]);

    int numberOfCharsRead;
    int pid_pfc;
    int pfcProcessPid[3] = {0};
    enum boolean terminated = FALSE;
    char error[WES_MESSAGE_MAX_LENGTH] = {0};

    char *PFC1_argv[] = {"pfc1", filename_g18, NULL};
    pfcProcessPid[0] = createChild(&execv, "pfc1", PFC1_argv);

    char *PFC2_argv[] = {"pfc2", filename_g18, NULL};
    pfcProcessPid[1] = createChild(&execv, "pfc2", PFC2_argv);

    char *PFC3_argv[] = {"pfc3", filename_g18, NULL};
    pfcProcessPid[2] = createChild(&execv, "pfc3", PFC3_argv);

    /*
     * alloco 3 buffer per contenere i PID dei processi PFC
     * in forma di stringa. snprintf mi permette appunto di
     * scrivere ogni PID all'interno del corrispondente buffer
     * sottoforma di stringa. I PID devono essere inviati come
     * parametri al processo generatoreFallimenti.
     */
    char pfc1Process[PID_MAX_LENGTH + 1] = {0};
    char pfc2Process[PID_MAX_LENGTH + 1] = {0};
    char pfc3Process[PID_MAX_LENGTH + 1] = {0};
    snprintf(pfc1Process, sizeof(char) * PID_MAX_LENGTH, "%d", pfcProcessPid[0]);
    snprintf(pfc2Process, sizeof(char) * PID_MAX_LENGTH, "%d", pfcProcessPid[1]);
    snprintf(pfc3Process, sizeof(char) * PID_MAX_LENGTH, "%d", pfcProcessPid[2]);

    char *generatoreFallimenti_argv[] = {"generatoreFallimenti", pfc1Process, pfc2Process, pfc3Process, NULL};
    createChild(&execv, "generatoreFallimenti", generatoreFallimenti_argv);

    /*
     * pfcArgv è un array di puntatori e memorizza gli indirizzi
     * degli array contenenti gli argomenti di cui ogni processo PFC
     * necessita per l'esecuzione
     */
    char **pfcArgv[] = {PFC1_argv, PFC2_argv, PFC3_argv};

    createPipe(FILENAME_WES_PIPE, DEFAULT_PERMISSIONS);
    int wesPipe = open(FILENAME_WES_PIPE, O_RDONLY);

    int generatoreFallimentiPipe = connectPipe(FILENAME_GENERATOREFALLIMENTI_PIPE, O_WRONLY | O_NONBLOCK);
    FILE *switchLog = openFile(FILENAME_SWITCH_LOG, "w");

    while(!terminated) {
        sleep(1);

        numberOfCharsRead = readLine(wesPipe, error, '\n');

        if(numberOfCharsRead > 0) {
            removeLastChar(error);

            if(strcmp(error, APPLICATION_ENDED_MESSAGE) == 0) {
                int status = 0;
                int pid = 0;

                //invia "Terminated" a generatoreFallimenti
                char message[] = concat(APPLICATION_ENDED_MESSAGE, "\n");
                int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;
                write(generatoreFallimentiPipe, message, sizeof(char) * messageLength);

                /*
                 * Scrive sul file switch.log che il sistema
                 * sta terminando l'esecuzione
                 */
                fprintf(switchLog, "%s", message);
                fflush(switchLog);

                /*
                 * Non abbiamo usato signal(SIGCHLD, SIG_IGN); per ignorare la morte
                 * dei processi figli per prevenire zombie perchè ciò crea figli orfani.
                 * Abbiamo preferito usare la wait ripetuta 4 volte (pfc1/2/3 e generatoreFallimenti)
                 */

                for(int i=0; i<4; i++) {
                    pid = wait(&status);

                    /*
                     * Se WIFEXITED(status) != 0, allora un processo figlio
                     * è terminato con un errore
                     */
                    if (!WIFEXITED(status)) {
                        printf("PfcDiscSwitch: Child process %d exited with %d status\n", pid, WEXITSTATUS(status));
                    }
                }

                terminated = TRUE;

            } else if(strcmp(error, WES_MESSAGE_EMERGENCY) == 0) {
                char message[] = concat(PFCDISCONNECTEDSWITCH_MESSAGE_EMERGENCY, "\n");

                kill(pfcProcessPid[0], SIGCONT);
                kill(pfcProcessPid[1], SIGCONT);
                kill(pfcProcessPid[2], SIGCONT);

                fprintf(switchLog, "%s", message);
		        fflush(switchLog);

            } else if (strcmp(error, WES_MESSAGE_SUCCESS) != 0) {
                char sign = 0;
                int pfcNumber = 0;

                getErrorInfo(error, &sign, &pfcNumber);
                pid_pfc = pfcProcessPid[pfcNumber - 1];

                char status = getProcessStatus(pid_pfc);

                if(status == 'Z') {
                    //Il processo e' uno zombie
			        wait(NULL);

			        /*
			         * filename deve contenere il nome del file pfc
			         * interrotto, ovvero una stringa formata da "pfc"
			         * di 3 caratteri e da un id identificativo di lunghezza
			         * massima PFC_ID_MAX_DIGITS. +1 per il carattere di fime stringa.
			         */
                    char filename[PFC_ID_MAX_DIGITS + 3 + 1] = {0};
                    snprintf(filename, sizeof(char) * (PFC_ID_MAX_DIGITS + 3), "pfc%d", pfcNumber);

                    /*
                     * Viene creato il nuovo processo ed il nuovo pid viene
                     * salvato nell'array contenente tutti i pid dei processi
                     * pfc
                     */
                    int newPid = createChild(&execv, filename, pfcArgv[pfcNumber - 1]);
                    pfcProcessPid[pfcNumber - 1] = newPid;

                    /*
                     * Scriviamo sul file switch.log che il processo PFC
                     * che era stati interroto è stato appena ricreato
                     */
                    char messagePfcCreated[] = concat(PFCDISCONNECTEDSWITCH_MESSAGE_PFC_CREATED, "\n");
                    fprintf(switchLog, messagePfcCreated, pfcNumber);
                    fflush(switchLog);

                    /*
                     * invio il pid del nuovo processo pfc creato a
                     * generatoreFallimenti
                     */
                    char messageNewPid[PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH + 1] = {0};
                    snprintf(messageNewPid, sizeof(char) * PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH, "%d%s%d\n", pfcNumber, PFCDISCONNECTEDSWITCH_SEPARATOR, newPid);
                    write(generatoreFallimentiPipe, messageNewPid, sizeof(char) * strlen(messageNewPid));

                    /*
                     * Scriviamo sul file switch.log che è stato inviato
                     * il nuovo pid a generatoreFallimenti
                     */
                    char messageGeneratoreFallimenti[] = concat(PFCDISCONNECTEDSWITCH_MESSAGE_GENERATORE_FALLIMENTI, "\n");
                    fprintf(switchLog, messageGeneratoreFallimenti, pfcNumber);
			        fflush(switchLog);
                } else if(status == 'T' || sign == 'P') {
                    /*
                        il processo potrebbe essere:
                            - bloccato
                            - in esecuzione ma con il valore della
                              velocita modificato da SIGURS1
                            - in esecuzione ma non sincronizzato con
                              il valore di last_read degli altri due
                              pfc
                    */
                    char message[] = concat(PFCDISCONNECTEDSWITCH_MESSAGE_SIGCONT, "\n");

                    kill(pid_pfc, SIGCONT);
                    fprintf(switchLog, message, pfcNumber);
                    fflush(switchLog);
			    }
            }

            memset(error, '\0', sizeof(char) * WES_MESSAGE_MAX_LENGTH);
        }
    }

    close(wesPipe);
    close(generatoreFallimentiPipe);
    fclose(switchLog);

    return 0;
}

void getErrorInfo(char *error, char *sign, int *pfcNumber) {
    char temp_pfc[PFC_ID_MAX_DIGITS + 3 + 1] = {0};
    char temp_sign[WES_MESSAGE_SIGN_MAX_LENGTH + 1] = {0};
    char temp_error[15] = {0};

    tokenize(error, "-", 3, temp_error, temp_pfc, temp_sign);
    *sign = temp_sign[0];

    unsigned long size = strlen(temp_pfc);
    char *endPointer = &temp_pfc[size-1];	

    while(*endPointer >= '0' && *endPointer <= '9') {
        *pfcNumber = (*pfcNumber) * 10 + (*endPointer - '0');
        endPointer--;
    }
}

char getProcessStatus(int pid) {
    char status = 0;
    char path[25] = {0};
    int lengthPath = 15 + PID_MAX_LENGTH;

    snprintf(path, sizeof(char) * lengthPath, "/proc/%d/stat", pid);

    FILE *proc = openFile(path, "r");
    fscanf(proc, "%*d %*s %c", &status);

    fclose(proc);

    return status;
}

