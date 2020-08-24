#ifndef pfc_h
#define pfc_h

#define TEMPO 1 //1 secondo
#define DEFAULT_PROTOCOL 0
#define RAGGIO_TERRA_METRI 6367863
#define NMEA_FORMAT "$GPGLL"
#define MAX_SPEED_DIGITS 5
#define MAX_LINE_LENGTH 150
#define MAX_G18_FILE_LENGTH_DIGITS 6 /*Massimo numero di cifre della lunghezza in bytes del file g18.txt*/

#include "utility.h"

void changeSigusr(enum boolean *sigusr);
void addLastRead(int fd_last_read, long current_position);
double changeSpeed(double speed);
ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp);
void acquisisciCoordinate(char* line, double* latitudine, double* longitudine);
double calcoloDistanza(double latitudine, double longitudine, double latitudine_prec, double longitudine_prec);
double gradiToRadianti(double gradi);
double calcoloVelocita(double spazio, int tempo);

int exe(int fd_pfc_to_transducers, FILE *fp_g18, int last_read, double *latitudine_prec, double *longitudine_prec, enum boolean *sigusr);

#endif
