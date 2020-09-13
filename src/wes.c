#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/utility.h"
#include "../include/utility-textfile.h"
#include "../include/utility-ipc.h"
#include "../include/messages.h"
#include "../include/wes.h"
#include "../include/config.h"

int main(int argc, const char *argv[]) {
    enum boolean terminated = FALSE;
    char message[WES_MESSAGE_MAX_LENGTH + 1] = {0};
    int messageLength = 0;

    int wesPipe = connectPipe(FILENAME_WES_PIPE, O_WRONLY);
    FILE *status = openFile(FILENAME_STATUS_LOG, "w");

    int speedPFC1Log = open(FILENAME_SPEEDPFC1_LOG, O_RDONLY | O_CREAT, DEFAULT_PERMISSIONS);
    int speedPFC2Log = open(FILENAME_SPEEDPFC2_LOG, O_RDONLY | O_CREAT, DEFAULT_PERMISSIONS);
    int speedPFC3Log = open(FILENAME_SPEEDPFC3_LOG, O_RDONLY | O_CREAT, DEFAULT_PERMISSIONS);

    double speedPFC1 = 0;
    double speedPFC2 = 0;
    double speedPFC3 = 0;

    /*
     * +1 per il carattere '\n' e +1 per il carattere '\0'
     * di fine stringa
     */
    char bufferPFC1[PFC_MESSAGE_MAX_LENGTH + 1 + 1] = {0};
    char bufferPFC2[PFC_MESSAGE_MAX_LENGTH + 1 + 1] = {0};
    char bufferPFC3[PFC_MESSAGE_MAX_LENGTH + 1 + 1] = {0};

    while(!terminated) {
        sleep(1);

        ReadLog(bufferPFC1, speedPFC1Log);
        ReadLog(bufferPFC2, speedPFC2Log);
        ReadLog(bufferPFC3, speedPFC3Log);

        if((strcmp(bufferPFC1, APPLICATION_ENDED_MESSAGE) == 0) ||
           (strcmp(bufferPFC2, APPLICATION_ENDED_MESSAGE) == 0) ||
           (strcmp(bufferPFC2, APPLICATION_ENDED_MESSAGE)) == 0) {
            terminated = TRUE;
        } else {
            speedPFC1 = strtod(bufferPFC1, NULL);
            speedPFC2 = strtod(bufferPFC2, NULL);
            speedPFC3 = strtod(bufferPFC3, NULL);

            /*
             * Se speedPFC1 == speedPFC2 == speedPFC3, non ci sono errori
             *
             * Se speedPFC2 == speedPFC1 != speedPFC3, allora ci sono degli errori:
             *     - Se speedPFC1 == -1, allora speedPFC2 = -1 quindi
             *       2 valori su 3 sono scorretti e viene mandato un messaggio
             *       di EMERGENZA
             *     - Se speedPFC3 == -1, allora c'è un solo valore scorretto
             *       perciò viene inviato un messaggio di ERROR_PFC3_N
             *     - Se speedPFC1 != -1 e speedPFC3 != -1, allora non ci sono
             *       valori uguali a -1 quindi l'unica velocità scorretta è
             *       speedPFC3 e viene inviato un errore di ERROR_PFC3_P
             *
             * Se speedPFC2 != speedPFC1 == speedPFC3, allora ci sono degli erroi:
             *     - Se speedPFC3 == -1, allora speedPFC1 = -1 quindi
             *       2 valori su 3 sono scorretti e viene mandato un messaggio
             *       di EMERGENZA
             *     - Se speedPFC2 == -1, allora c'è un solo valore scorretto
             *       perciò viene inviato un messaggio di ERROR_PFC2_N
             *     - Se speedPFC3 != -1 e speedPFC2 != -1, allora non ci sono
             *       valori uguali a -1 quindi l'unica velocità scorretta è
             *       speedPFC2 e viene inviato un errore di ERROR_PFC2_P
             *
             * Se speedPFC1 != speedPFC2 == speedPFC3, allora ci sono degli erroi:
             *     - Se speedPFC3 == -1, allora speedPFC2 = -1 quindi
             *       2 valori su 3 sono scorretti e viene mandato un messaggio
             *       di EMERGENZA
             *     - Se speedPFC1 == -1, allora c'è un solo valore scorretto
             *       perciò viene inviato un messaggio di ERROR_PFC1_N
             *     - Se speedPFC3 != -1 e speedPFC1 != -1, allora non ci sono
             *       valori uguali a -1 quindi l'unica velocità scorretta è
             *       speedPFC2 e viene inviato un errore di ERROR_PFC1_P
             *
             * Se speedPFC1 != speedPFC2 != speedPFC3, allora viene mandato
             * un errore di EMERGENZA
             */

            if (speedPFC1 == speedPFC2) {
                if (speedPFC1 == speedPFC3) {
                    strcpy(message, concat(WES_MESSAGE_SUCCESS, "\n"));
                    messageLength = string_length(WES_MESSAGE_SUCCESS) + 1;

                    fprintf(status, "%s", message);
				    fflush(status);

                   	 write(wesPipe, message, sizeof(char) * messageLength);

                   	 printf("%s", message);
                } else {
                    if(speedPFC1 == -1) {
                        strcpy(message, concat(WES_MESSAGE_EMERGENCY, "\n"));
                        messageLength = string_length(WES_MESSAGE_EMERGENCY) + 1;
                    } else if(speedPFC3 == -1) {
                        strcpy(message, concat(WES_MESSAGE_PFC_ERROR_NEGATIVE(3), "\n"));
                        messageLength = string_length(WES_MESSAGE_PFC_ERROR_NEGATIVE(3)) + 1;
                    } else {
                        strcpy(message, concat(WES_MESSAGE_PFC_ERROR_POSITIVE(3), "\n"));
                        messageLength = string_length(WES_MESSAGE_PFC_ERROR_POSITIVE(3)) + 1;
                    }

                    fprintf(status, "%s", message);
				    fflush(status);

				    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("%s", message);
                }	
            } else {
                if (speedPFC1 == speedPFC3) {
                    if(speedPFC3 == -1) {
                        strcpy(message, concat(WES_MESSAGE_EMERGENCY, "\n"));
                        messageLength = string_length(WES_MESSAGE_EMERGENCY) + 1;
                    } else if(speedPFC2 == -1) {
                        strcpy(message, concat(WES_MESSAGE_PFC_ERROR_NEGATIVE(2), "\n"));
                        messageLength = string_length(WES_MESSAGE_PFC_ERROR_NEGATIVE(2)) + 1;
                    } else {
                        strcpy(message, concat(WES_MESSAGE_PFC_ERROR_POSITIVE(2), "\n"));
                        messageLength = string_length(WES_MESSAGE_PFC_ERROR_POSITIVE(2)) + 1;
                    }

                    fprintf(status, "%s", message);
                    fflush(status);

                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("%s", message);
                } else if (speedPFC2 == speedPFC3) {
                    if(speedPFC3 == -1) {
                        strcpy(message, concat(WES_MESSAGE_EMERGENCY, "\n"));
                        messageLength = string_length(WES_MESSAGE_EMERGENCY) + 1;
                    } else if(speedPFC1 == -1) {
                        strcpy(message, concat(WES_MESSAGE_PFC_ERROR_NEGATIVE(1), "\n"));
                        messageLength = string_length(WES_MESSAGE_PFC_ERROR_NEGATIVE(1)) + 1;
                    } else {
                        strcpy(message, concat(WES_MESSAGE_PFC_ERROR_POSITIVE(1), "\n"));
                        messageLength = string_length(WES_MESSAGE_PFC_ERROR_POSITIVE(1)) + 1;
                    }

                    fprintf(status, "%s", message);
                    fflush(status);

                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("%s", message);
                } else {
                    strcpy(message, concat(WES_MESSAGE_EMERGENCY, "\n"));
                    messageLength = string_length(WES_MESSAGE_EMERGENCY) + 1;

                    fprintf(status, "%s", message);
                    fflush(status);

                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("%s", message);
                }
            }
        }

        memset(message, '\0', sizeof(char) * PFC_MESSAGE_MAX_LENGTH);
    }

    strcpy(message, concat(APPLICATION_ENDED_MESSAGE, "\n"));
    messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    fprintf(status, "%s", message);
    fflush(status);

    write(wesPipe, message, sizeof(char) * messageLength);

    printf("%s", message);

    close(speedPFC1Log);
    close(speedPFC2Log);
    close(speedPFC3Log);
    close(wesPipe);
    fclose(status);

    return 0;
}

void ReadLog(char *buffer, int logFile) {
    memset(buffer, '\0', sizeof(char) * PFC_MESSAGE_MAX_LENGTH);

    int numberOfCharsRead = readLine(logFile, buffer, '\n');
    if(numberOfCharsRead > 0) {
        removeLastChar(buffer);
    } else {
        snprintf(buffer, sizeof(char) * 3, "-1");
    }
}