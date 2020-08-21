#ifndef pfc_h
#define pfc_h

#define TEMPO 1 //1 secondo
#define DEFAULT_PROTOCOL 0
#define RAGGIO_TERRA_METRI 6367863
#define NMEA_FORMAT "$GPGLL"
#define MAX_SPEED_DIGITS 5
#define MAX_LINE_LENGTH 150
#define FILENAME_LAST_READ "../sistemioperativi/temp/last_read.txt"

#include "utility.h"

void changeSigusr(enum boolean *sigusr);
int addLastRead(FILE *last_read, long position);
double changeSpeed(double speed);
ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp);
void acquisisciCoordinate(char* line, double* latitudine, double* longitudine);
double calcoloDistanza(double latitudine, double longitudine, double latitudine_prec, double longitudine_prec);
double gradiToRadianti(double gradi);
double calcoloVelocita(double spazio, int tempo);

int exe(int fd, FILE *fp, FILE *last_read, double *latitudine_prec, double *longitudine_prec, enum boolean *sigusr);

#endif