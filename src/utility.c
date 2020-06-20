//
// Created by Gioele Dimilta on 20/06/20.
//

#include "utility.h"

FILE *open_file(const char* filename, const char* mode) {
    FILE *fp = fopen(filename, mode);

    if (fp == NULL){
        //TODO come gestire gli errori?

        fprintf(stderr, "File non trovato\n");
        exit(EXIT_FAILURE);
    }

    return fp;
}


