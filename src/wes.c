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
    FILE *status = openFile(FILENAME_STATUS_LOG, "w+");

    int speedPFC1Log = open(FILENAME_SPEEDPFC1_LOG, O_RDONLY | O_CREAT, 0660);
    int speedPFC2Log = open(FILENAME_SPEEDPFC2_LOG, O_RDONLY | O_CREAT, 0660);
    int speedPFC3Log = open(FILENAME_SPEEDPFC3_LOG, O_RDONLY | O_CREAT, 0660);

    double speedPFC1 = 0;
    double speedPFC2 = 0;
    double speedPFC3 = 0;

    char bufferPFC1[string_length(APPLICATION_ENDED_MESSAGE) + 1] = {0};
    char bufferPFC2[string_length(APPLICATION_ENDED_MESSAGE) + 1] = {0};
    char bufferPFC3[string_length(APPLICATION_ENDED_MESSAGE) + 1] = {0};

    int numberOfCharsRead = 0;
    enum boolean terminated = FALSE;

    while(!terminated) {
        sleep(1);

        /*
         * TODO: Come potrebbe succedere nel transducer, leggendo da un
         *       file vuoto, bufferPFC1/2/3 vengono sporcati con valori
         *       rimasti in memoria? Se si, ha senso usare il memset, altrimenti
         *       non ce n'è bisogno
         */

        numberOfCharsRead = readLine(speedPFC1Log, bufferPFC1, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(bufferPFC1);
        } else {
            memset(bufferPFC1, '\0', sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));
        }

        numberOfCharsRead = readLine(speedPFC2Log, bufferPFC2, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(bufferPFC2);
        } else {
            memset(bufferPFC2, '\0', sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));
        }

        numberOfCharsRead = readLine(speedPFC3Log, bufferPFC3, MESSAGES_SEPARATOR);
        if(numberOfCharsRead > 0) {
            removeLastChar(bufferPFC3);
        } else {
            memset(bufferPFC3, '\0', sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));
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

                    printf("(%.2f, %.2f, %.2f) - %s", speedPFC1, speedPFC2, speedPFC3, message);
                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);
                } else {
                    char *message = concat(WES_MESSAGE_PFC3_ERROR, "\n");
                    int messageLength = string_length(WES_MESSAGE_PFC3_ERROR) + 1;

                    printf("%s", message);
                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);
                }
            } else {
                if (speedPFC1 == speedPFC3) {
                    char *message = concat(WES_MESSAGE_PFC2_ERROR, "\n");
                    int messageLength = string_length(WES_MESSAGE_PFC2_ERROR) + 1;

                    printf("%s", message);
                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);
                } else if (speedPFC2 == speedPFC3) {
                    char *message = concat(WES_MESSAGE_PFC1_ERROR, "\n");
                    int messageLength = string_length(WES_MESSAGE_PFC1_ERROR) + 1;

                    printf("%s", message);
                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);
                } else {
                    char *message = concat(WES_MESSAGE_EMERGENCY, "\n");
                    int messageLength = string_length(WES_MESSAGE_EMERGENCY) + 1;

                    printf("%s", message);
                    fprintf(status, "%s", message);
                    write(wesPipe, message, sizeof(char) * messageLength);
                }
            }
        }
    }

    char *message = concat(APPLICATION_ENDED_MESSAGE, "\n");
    int messageLength = string_length(APPLICATION_ENDED_MESSAGE) + 1;

    printf("%s", message);
    fwrite(message, sizeof(char), messageLength, status);
    write(wesPipe, APPLICATION_ENDED_MESSAGE, sizeof(char) * string_length(APPLICATION_ENDED_MESSAGE));

    close(speedPFC1Log);
    close(speedPFC2Log);
    close(speedPFC3Log);
    close(wesPipe);
    fclose(status);

    return 0;
}
