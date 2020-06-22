#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "wes.h"
#include "utility.h"

int main(int argc, const char *argv[]) {
    int speedPFC1Log = open("../sistemioperativi/log/speedPFC1.log", O_RDONLY);
    int speedPFC2Log = open("../sistemioperativi/log/speedPFC2.log", O_RDONLY);
    int speedPFC3Log = open("../sistemioperativi/log/speedPFC3.log", O_RDONLY);
    FILE *status = open_file("../sistemioperativi/log/status.log", "w+");

    double velocita_pfc1;
    double velocita_pfc2;
    double velocita_pfc3;

    for(int i=0; i<50; i++) {
        sleep(1);

        velocita_pfc1 = readSpeed(speedPFC1Log);
        velocita_pfc2 = readSpeed(speedPFC2Log);
        velocita_pfc3 = readSpeed(speedPFC3Log);

        /*
         * TODO: controllare che velocita_pfc1, velocita_pfc2
         *       e velocita_pfc3 abbiano valori diversi da '\0'
         */

        if(velocita_pfc1 == velocita_pfc2) {
            if(velocita_pfc1 == velocita_pfc3) {
                printf("OK\n");
                fwrite("OK\n", sizeof(char), 3, status);
            } else {
                printf("ERRORE - PFC3\n");
                fwrite("ERRORE - PFC3\n", sizeof(char), 14, status);
                //TODO invio messaggio a pfc disconnect switch
            }
        } else {
            if(velocita_pfc1 == velocita_pfc3) {
                printf("ERRORE - PFC2\n");
                fwrite("ERRORE - PFC2\n", sizeof(char), 14, status);
                //TODO invio messaggio a pfc disconnected switch
            } else if(velocita_pfc2 == velocita_pfc3) {
                printf("ERRORE - PFC1\n");
                fwrite("ERRORE - PFC1\n", sizeof(char), 14, status);
                //TODO invio messaggio a pfc disconnected switch
            } else {
                printf("EMERGENZA\n");
                fwrite("EMERGENZA\n", sizeof(char), 10, status);
                //TODO invio messaggio a pfc disconnected switch
            }
        }
    }

    close(speedPFC1Log);
    close(speedPFC2Log);
    close(speedPFC3Log);
    fclose(status);

    return 0;
}

double readSpeed(int fd) {
    double speed;
    char *buffer = calloc(10, sizeof(char));

    readLine(fd, buffer);
    speed = strtod(buffer, NULL);
    free(buffer);

    return speed;
}
