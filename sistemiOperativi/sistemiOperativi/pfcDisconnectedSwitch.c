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
    
    int pidFiglio = fork();
    if (pidFiglio < 0) {
        //errore
    } else if (pidFiglio == 0) {
        //processo figlio
        pfcProcess[0] = getpid();
        //execlp(...);
    }


    //processo padre
    pidFiglio = fork();
    if (pidFiglio < 0) {
        //errore
    } else if (pidFiglio == 0) {
        //processo figlio
        pfcProcess[1] = getpid();
        // execlp(...);
    }


    //processo padre
    pidFiglio = fork();
    if (pidFiglio < 0) {
        //errore
    } else if (pidFiglio == 0) {
        //processo figlio
        pfcProcess[2] = getpid();
        // execlp(...);
    }


    //processo padre
    pidFiglio = fork();
    if (pidFiglio < 0) {
        //errore
    } else if (pidFiglio == 0) {
        //processo figlio
        
        generatoreFallimentiProcess = getpid();
        execl("generatoreFallimenti.c", intToString(pfcProcess[0]), intToString(pfcProcess[1]), intToString(pfcProcess[2]), NULL);
    }

    //execlp(...);
    
    
    return 0;
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

int creaFiglio(int* pfcProcess) {
    int pidFiglio = fork();
    if (pidFiglio < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        exit(-1);
    } else if (pidFiglio == 0) {
        *pfcProcess = getpid();
    }
    
    return pidFiglio;
}

