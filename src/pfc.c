//
// Created by Gioele Dimilta on 20/06/20.
//

#include "pfc.h"

void acquisisciCoordinate(char* riga, double* latitudine, char* direzioneLatitudine, double* longitudine, char* direzioneLongitudine){
    char* tok;
    tok = strtok(riga, ",");

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

double calcoloVelocita(int spazio, int tempo){
    /* Spazio in metri, tempo in secondi M/s */
    return spazio/tempo;
}

char *velocitaCalcolata(FILE *fp, double *latitudine_prec, double *longitudine_prec) {
    char *velocita = malloc(sizeof(char) * 5);
    char *riga = NULL;
    size_t lunghezzaRiga = 30;

    double latitudine;
    char direzioneLatitudine ;
    double longitudine;
    char direzioneLongitudine;

    long caratteriLetti = getline(&riga, &lunghezzaRiga, fp);

    if (caratteriLetti == -1 || (strstr(riga,"$GPGLL") == NULL)) {
        fprintf(stderr, "Errore di lettura\n");
        exit(EXIT_FAILURE);
    }

    acquisisciCoordinate(riga, &latitudine, &direzioneLatitudine, &longitudine, &direzioneLongitudine);
    distanza = calcoloDistanza(latitudine, longitudine, latitudine_prec, longitudine_prec);

    snprintf(velocita, sizeof(velocita), "%f", (calcoloVelocita(distanza, TEMPO)));

    *latitudine_prec = latitudine;
    *longitudine_prec = longitudine;

    return velocita;
}

/*long conversioneTempo(char* tempo){

    int ore = atoi(&tempo[0])*10 + atoi(&tempo[1]);
    int minuti = atoi(&tempo[2])*10 + atoi(&tempo[3]);
    int secondi = atoi(&tempo[4])*10 + atoi(&tempo[5]);

    return ore/3600 + minuti/60 + secondi;
}*/

