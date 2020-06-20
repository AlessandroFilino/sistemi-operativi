//
//  pfc3.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 18/06/2020.
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
    FILE *fpTransducers = open_file("pfcTransducers.txt", "w");
    
    for(;;) {
        sleep(1);
        velocita = velocitaCalcolata(fp, &latitudine_prec, &longitudine_prec);
        
        fflush(fpTransducers);
        fwrite(velocita, sizeof(int), 1, fpTransducers);
    }

    fclose(fp);
    close(fd);
    fclose(fpTransducers);
}



