//
//  pfc.h
//  sistemiOperativi
//
//  Created by Alessandro Filino on 26/05/2020.
//  Copyright © 2020 Alessandro Filino. All rights reserved.
//

#ifndef pfc_h
#define pfc_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> /* For AF_UNIX sockets */
#include "utility.h"

#define TEMPO 1 //1 secondo
#define DEFAULT_PROTOCOL 0
#define RAGGIO_TERRA_METRI 6367863
#define NMEA_FORMAT "$GPGLL"

double calcoloDistanza(double latitudine, double longitudine, double latitudine_prec, double longitudine_prec);
double gradiToRadianti(double gradi);
void acquisisciCoordinate(char* line, double* latitudine, char* direzioneLatitudine, double* longitudine, char* direzioneLongitudine);
//long conversioneTempo(char* tempo);
double calcoloVelocita(double spazio, int tempo);
char *velocitaCalcolata(char *line, double *latitudine_prec, double *longitudine_prec);
ssize_t readCorrectLine(char *buffer, size_t *bufferLength, FILE *fp);

#endif
