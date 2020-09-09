#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/messages.h"
#include "../include/wes.h"
#include "../include/utility.h"
#include "../include/path.h"

int main(int argc, const char *argv[]) {
    int wesPipe = connectPipe(FILENAME_WES_PIPE, O_WRONLY);
    FILE *status = openFile(FILENAME_STATUS_LOG, "w");

    int speedPFC1Log = open(FILENAME_SPEEDPFC1_LOG, O_RDONLY | O_CREAT, DEFAULT_PERMISSIONS);
    int speedPFC2Log = open(FILENAME_SPEEDPFC2_LOG, O_RDONLY | O_CREAT, DEFAULT_PERMISSIONS);
    int speedPFC3Log = open(FILENAME_SPEEDPFC3_LOG, O_RDONLY | O_CREAT, DEFAULT_PERMISSIONS);

    double speedPFC1 = 0;
    double speedPFC2 = 0;
    double speedPFC3 = 0;

    char bufferPFC1[string_length(APPLICATION_ENDED_MESSAGE) + 1 + 1] = {0};
    char bufferPFC2[string_length(APPLICATION_ENDED_MESSAGE) + 1 + 1] = {0};
    char bufferPFC3[string_length(APPLICATION_ENDED_MESSAGE) + 1 + 1] = {0};

    int numberOfCharsRead = 0;
    enum boolean terminated = FALSE;


    while(!terminated) {
        usleep((1 * 1000) * 100); //100 millisecondi

        /*
         * TODO: Come potrebbe succedere nel transducer, leggendo da un
         *       file vuoto, bufferPFC1/2/3 vengono sporcati con valori
         *       rimasti in memoria? Se si, ha senso usare il memset, altrimenti
         *       non ce n'è bisogno
         */

        memset(bufferPFC1, '\0', sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));
        numberOfCharsRead = readLine(speedPFC1Log, bufferPFC1, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(bufferPFC1);
        } else {
            snprintf(bufferPFC1, sizeof(char) * 3, "-1");
        }

        memset(bufferPFC2, '\0', sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));
        numberOfCharsRead = readLine(speedPFC2Log, bufferPFC2, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(bufferPFC2);
        } else {
            snprintf(bufferPFC2, sizeof(char) * 3, "-1");
        }

        memset(bufferPFC3, '\0', sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));
        numberOfCharsRead = readLine(speedPFC3Log, bufferPFC3, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(bufferPFC3);
        } else {
            snprintf(bufferPFC3, sizeof(char) * 3, "-1");
        }

        if((strcmp(bufferPFC1, APPLICATION_ENDED_MESSAGE) == 0) ||
           (strcmp(bufferPFC2, APPLICATION_ENDED_MESSAGE) == 0) ||
           (strcmp(bufferPFC2, APPLICATION_ENDED_MESSAGE)) == 0) {
            terminated = TRUE;
        } else {
            speedPFC1 = strtod(bufferPFC1, NULL);
            speedPFC2 = strtod(bufferPFC2, NULL);
            speedPFC3 = strtod(bufferPFC3, NULL);

            if (speedPFC1 == speedPFC2) {
                if (speedPFC1 == speedPFC3) {
                    char message[] = concat(WES_MESSAGE_SUCCESS, "\n");
                    int messageLength = string_length(WES_MESSAGE_SUCCESS) + 1;

                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("(%.2f, %.2f, %.2f) - %s", speedPFC1, speedPFC2, speedPFC3, message);
                } else {
                    char message[] = concat(WES_MESSAGE_PFC3_ERROR, "\n");
                    int messageLength = string_length(WES_MESSAGE_PFC3_ERROR) + 1;

                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("(%.2f, %.2f, %.2f) - %s", speedPFC1, speedPFC2, speedPFC3, message);
                }
            } else {
                if (speedPFC1 == speedPFC3) {
                    char message[] = concat(WES_MESSAGE_PFC2_ERROR, "\n");
                    int messageLength = string_length(WES_MESSAGE_PFC2_ERROR) + 1;

                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);

                    //printf("(%.2f, %.2f, %.2f) - %s", speedPFC1, speedPFC2, speedPFC3, message);
                } else if (speedPFC2 == speedPFC3) {
                    char message[] = concat(WES_MESSAGE_PFC1_ERROR, "\n");
                    int messageLength = string_length(WES_MESSAGE_PFC1_ERROR) + 1;

                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("(%.2f, %.2f, %.2f) - %s", speedPFC1, speedPFC2, speedPFC3, message);
                } else {
                    char message[] = concat(WES_MESSAGE_EMERGENCY, "\n");
                    int messageLength = string_length(WES_MESSAGE_EMERGENCY) + 1;

                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);

                    printf("(%.2f, %.2f, %.2f) - %s", speedPFC1, speedPFC2, speedPFC3, message);
                }
            }
        }
    }

    char message[] = concat(APPLICATION_ENDED_MESSAGE, "\n");
    int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    fprintf(status, "%s", message);
    write(wesPipe, message, sizeof(char) * messageLength);

    printf("(/, /, /) - %s", message);

    close(speedPFC1Log);
    close(speedPFC2Log);
    close(speedPFC3Log);
    close(wesPipe);
    fclose(status);

    return 0;
}
