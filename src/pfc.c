//
// Created by Gioele Dimilta on 20/06/20.
//

#include "pfc.h"

void acquisisciCoordinate(char* line, double* latitudine, char* direzioneLatitudine, double* longitudine, char* direzioneLongitudine){
    char* tok;
    tok = strtok(line, ",");

    if(tok == NULL){
        fprintf(stderr, "Errore\n");
        exit (EXIT_FAILURE);
    }

    *latitudine = atof(strtok(NULL, ","));
    direzioneLatitudine = strtok(NULL, ",");
    *longitudine = atof(strtok(NULL, ","));
    direzioneLongitudine = strtok(NULL, ",");
}

double gradiToRadianti(double gradi) {
    return gradi * M_PI / 180;
}

double calcoloDistanza(double latitudine, double longitudine, double latitudine_prec, double longitudine_prec){
    double dLat = gradiToRadianti(latitudine - latitudine_prec);
    double dLon = gradiToRadianti(longitudine - longitudine_prec);
    latitudine = gradiToRadianti(latitudine);
    latitudine_prec = gradiToRadianti(latitudine_prec);

    double a = sin(dLat/2) * sin(dLat/2) +
               sin(dLon/2) * sin(dLon/2) * cos(latitudine_prec) * cos(latitudine);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return RAGGIO_TERRA_METRI * c;
}

double calcoloVelocita(double spazio, int tempo){
    /* Spazio in metri, tempo in secondi M/s */
    return spazio/tempo;
}

char *velocitaCalcolata(char *line, double *latitudine_prec, double *longitudine_prec) {
    double latitudine;
    char direzioneLatitudine ;
    double longitudine;
    char direzioneLongitudine;
    double distanza;

    char *velocita = malloc(sizeof(char) * 5);

    acquisisciCoordinate(line, &latitudine, &direzioneLatitudine, &longitudine, &direzioneLongitudine);
    distanza = calcoloDistanza(latitudine, longitudine, *latitudine_prec, *longitudine_prec);

    snprintf(velocita, sizeof(velocita), "%f", (calcoloVelocita(distanza, TEMPO)));

    *latitudine_prec = latitudine;
    *longitudine_prec = longitudine;

    return velocita;
}

ssize_t readCorrectLine(char *buffer, size_t *bufferLength, FILE *fp) {
    /*
     * char *strstr(const char *haystack, const char *needle)
     *
     * finds the first occurrence of the substring needle in the string haystack.
     * This function returns a pointer to the first occurrence in haystack of any
     * of the entire sequence of characters specified in needle, or a null pointer
     * if the sequence is not present in haystack
     */
    ssize_t read;

    do {
        read = getline(&buffer, bufferLength, fp);
    } while(read != -1 && strstr(buffer, NMEA_FORMAT) == NULL);

    return read;
}

/*long conversioneTempo(char* tempo){

    int ore = atoi(&tempo[0])*10 + atoi(&tempo[1]);
    int minuti = atoi(&tempo[2])*10 + atoi(&tempo[3]);
    int secondi = atoi(&tempo[4])*10 + atoi(&tempo[5]);

    return ore/3600 + minuti/60 + secondi;
}*/

