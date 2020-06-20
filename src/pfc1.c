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

    ssize_t read = 0;
    size_t bufferLength = 100;

    //char *filename_g18 = argv[1];
    char *filename_g18 = "../sistemioperativi/doc/G18.txt";
    FILE *fp = open_file(filename_g18, "r");

    while(read != -1) {
        sleep(1);

        /*
         * Il buffer viene ricreato ad ogni ciclo
         * per evitare di dover cancellare il
         * contenuto e fare spazio alla successiva
         * riga letta.
         */
        char *buffer = malloc(sizeof(char) * bufferLength);
        read = readCorrectLine(buffer, &bufferLength, fp);

        if(read != -1) {
            velocita = velocitaCalcolata(buffer, &latitudine_prec, &longitudine_prec);

            /*do {
                fd = open("pfc1Pipe", O_WRONLY);
                if (fd == -1) {
                    sleep(1);
                }
            } while (fd == -1);

            write(fd, velocita, sizeof(velocita));*/
            printf("%s\n", velocita);

            free(velocita);
        }

        free(buffer);
    }

    fclose(fp);
    close(fd);

    exit(EXIT_SUCCESS);
}



