//
//  pfc2.c
//  sistemiOperativi
//
//  Created by Alessandro Filino on 18/06/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#include "pfc.h"
#include <sys/socket.h>
#include <sys/un.h> /* For AFUNIX sockets */
#define DEFAULT_PROTOCOL 0

int main(int argc, const char * argv[]) {
    
    char *riga = NULL;
    size_t lunghezzaRiga = 0;
    long caratteriLetti;
    
    double latitudine;
    char direzioneLatitudine ;
    double longitudine;
    char direzioneLongitudine;
    long tempo;
    char comandoControllo[5];
    
    double latitudine_prec = 0;
    char direzioneLatitudine_prec = 0;
    double longitudine_prec = 0;
    char direzioneLongitudine_prec = 0;
    long tempo_prec = 0;
    char comandoControllo_prec[5] = {0};
    
    
    FILE *fp;
    fp = fopen("G18.txt", "r");  //PATH FILE G18
    
    if (fp == NULL){
        fprintf(stderr, "File non trovato\n");
        exit (EXIT_FAILURE);
    }
    
    int clientFd, serverLen, result;
    struct sockaddr_un serverUNIXAddress; /*Server address */
    struct sockaddr* serverSockAddrPtr; /*Ptr to server address*/
    
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; /* server domain type */
    strcpy (serverUNIXAddress.sun_path, "pfc2"); /* server name */
    
    for(;;) {
        do {
            result = connect (clientFd, serverSockAddrPtr, serverLen);
        } while(result == -1);

        sleep(1);

        caratteriLetti = getline(&riga, &lunghezzaRiga, fp);
            
        if (caratteriLetti == -1 || (strstr(riga,"$GPGLL") == NULL)) {
            fprintf(stderr, "Errore di lettura\n");
            exit (EXIT_FAILURE);
        }
            
        acquisisciCoordinate(riga, &latitudine, &direzioneLatitudine, &longitudine, &direzioneLongitudine, &tempo, comandoControllo);
        double distanza = calcoloDistanza(latitudine, longitudine, latitudine_prec, longitudine_prec);
        char velocita[5];
        snprintf(velocita, sizeof(velocita), "%f", (calcoloVelocita(distanza, tempo)));
            
        write (clientFd, velocita, sizeof(velocita));
            
        latitudine_prec = latitudine;
        direzioneLatitudine = direzioneLatitudine_prec;
        longitudine_prec = longitudine;
        direzioneLongitudine = direzioneLongitudine_prec;
        tempo_prec = tempo;
        strcpy(comandoControllo_prec,comandoControllo);
    }
    
    close(clientFd);
}

void acquisisciCoordinate(char* riga, double* latitudine, char* direzioneLatitudine, double* longitudine, char* direzioneLongitudine, long* tempo, char* comandoControllo){
    
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
    *tempo = conversioneTempo(strtok(NULL, ","));
    strcpy(comandoControllo, strtok(NULL, ","));
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

double calcoloVelocita(int spazio, long tempo){
    /* Spazio in metri, tempo in secondi M/s */
    return spazio/tempo;
}

long conversioneTempo(char* tempo){
    
    int ore = atoi(&tempo[0])*10 + atoi(&tempo[1]);
    int minuti = atoi(&tempo[2])*10 + atoi(&tempo[3]);
    int secondi = atoi(&tempo[4])*10 + atoi(&tempo[5]);
    
    return ore/3600 + minuti/60 + secondi;
}

