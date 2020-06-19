//
// Created by Gioele Dimilta on 18/06/20.
//

#include "wes.h"

int main(int argc, const char *argv[]) {
    FILE *speedPFC1Log = fopen("speedPFC1.log", "r");
    FILE *speedPFC1Log = fopen("speedPFC2.log", "r");
    FILE *speedPFC1Log = fopen("speedPFC3.log", "r");
    FILE *status = fopen("status.log", "w+");

    int velocita_pfc1;
    int velocita_pfc2;
    int velocita_pfc3;

    for(;;) {
        sleep(1);

        fread(&velocita_pfc1, sizeof(int), 1, speedPFC1Log);
        fread(&velocita_pfc2, sizeof(int), 1, speedPFC2Log);
        fread(&velocita_pfc3, sizeof(int), 1, speedPFC3Log);

        if(velocita_pfc1 == velocita_pfc2) {
            if(velocita_pfc1 == velocita_pfc3) {
                printf("OK");
                fwrite("OK", sizeof(char), 2, status);
            } else {
                printf("ERRORE - PFC3");
                fwrite("ERRORE - PFC3", sizeof(char), 13, status);
                //invio messaggio a pfc disconnect switch
            }
        } else {
            if(velocita_pfc1 == velocita_pfc3) {
                printf("ERRORE - PFC2");
                fwrite("ERRORE - PFC2", sizeof(char), 13, status);
                //invio messaggio a pfc disconnect switch
            } else if(velocita_pfc2 == velocita_pfc3) {
                printf("ERRORE - PFC1");
                fwrite("ERRORE - PFC1", sizeof(char), 13, status);
                //invio messaggio a pfc disconnect switch
            } else {
                printf("EMERGENZA");
                fwrite("EMERGENZA", sizeof(char), 9, status);
                //invio messaggio a pfc disconnect switch
            }
        }
    }

    return 0;
}
