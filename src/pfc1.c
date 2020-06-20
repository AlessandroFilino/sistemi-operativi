//
//  pfc.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 26/05/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#include "pfc.h"

int main(int argc, const char * argv[]) {
    int fd;
    char* velocita;
    double latitudine_prec = 0;
    double longitudine_prec = 0;

    //open g18.txt
    FILE *fp = open_file(argv[1], "r");
    
    for(;;) {
        sleep(1);
        velocita = velocitaCalcolata(fp, &latitudine_prec, &longitudine_prec);

        do {
            fd = open("pfc1Pipe", O_WRONLY);
            if (fd == -1){
                sleep (1);
            }
        } while (fd == -1);

        write (fd, velocita, sizeof(velocita));
    }

    close(fp);
    close(fd);
}



