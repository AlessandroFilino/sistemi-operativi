//
//  pfcDisconnectedSwitch.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 26/05/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#include "pfcDisconnectedSwitch.h"

int main(int argc, const char * argv[]) {
    
    int pfcProcess[3];
    int generatoreFallimentiProcess;

    /*Install death-of-child handler */
    signal (SIGCHLD, childHandler);

    if(creaFiglio() == 0) {
        pfcProcess[0] = getpid();
        execl("pfc1", "pfc1", argv[1], NULL);
    }

    if(creaFiglio() == 0) {
        pfcProcess[1] = getpid();
        execl("pfc2", "pfc2", argv[1], NULL);
    }

    if(creaFiglio() == 0) {
        pfcProcess[2] = getpid();
        execl("pfc3", "pfc3", argv[1], NULL);
    }

    if(creaFiglio() == 0) {
        generatoreFallimentiProcess = getpid();
        execl(
                "generatoreFallimenti",
                "generatoreFallimenti",
                intToString(pfcProcess[0]),
                intToString(pfcProcess[1]),
                intToString(pfcProcess[2]),
                NULL);
    }

    //execlp(...);
    //signal(SIGCHLD, SIG_IGN); /*ignore death-of-child signals to prevent zombies*/
    //kill(pidFiglio, SIGN_INT) per tutti i figli
    //kill(pidFiglio, 0) restituisce 0 se esiste un processo con pid uguale a pidFiglio, altrimenti -1

    return 0;
}

int creaFiglio() {
    int pid = fork();

    if (pid < 0) { /* error occurred */
        //TODO usare perror

        fprintf(stderr, "Fork Failed\n");
        exit(-1);
    }

    return pid;
}

char* intToString(int number){
    int digits;

    do {
        number /= 10;
        digits++;
    } while (number != 0);

    char* buffer = malloc(sizeof(char) * digits + 1); //+1 per avere il carattere di fine stringa
    sprintf(buffer, "%d", number);
    
    return buffer;
}

void childHandler (int sig) { /* Executed if the child dies */
    int childPid, childStatus; /* before the parent */

    /* Accept child termination code */
    childPid = wait(&childStatus);
    printf ("Child %d terminated\n", childPid);
    exit(0);
}

