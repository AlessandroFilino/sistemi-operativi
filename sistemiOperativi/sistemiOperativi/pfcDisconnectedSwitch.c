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
    int pid = creaFiglio();
    
     if (pid == 0) {
         pfcProcess[0] = getpid();
       // execlp("/bin/ls", "ls", NULL);
    }
    else {
        pid = creaFiglio();
        wait (NULL);
        printf ("Child Complete\n");
        exit(0);
    }

    return 0;
}

int creaFiglio(){
    
    int pidFiglio = fork();
    if (pidFiglio < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        exit(-1);
    }

    return pidFiglio;
}

