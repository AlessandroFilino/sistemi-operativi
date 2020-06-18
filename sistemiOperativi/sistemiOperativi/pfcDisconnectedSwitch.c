//
//  pfcDisconnectedSwitch.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 26/05/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//
#include <stdio.h>
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
    int fd[2];
    pipe(fd);
    close(fd[READ]);
    write(fd[WRITE], pfcProcess, sizeof(pfcProcess));
    
    pidFiglio = fork();
    if (pidFiglio < 0) {
        
        //errore
    } else if (pidFiglio == 0) {
        //processo figlio
        
        generatoreFallimentiProcess = getpid();
        // INVIARE FD;
        execl("generatoreFallimenti.c", fd[0], fd[1], NULL);
    }
    
   //processo padre
   
    
    
       //execlp(...);
    
    
    return 0;
}

char** intToString(int pid){
    char converted[65535];
    sprintf(converted, "%d", pid);
    
    return converted;
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

