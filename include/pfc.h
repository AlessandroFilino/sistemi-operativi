#ifndef pfc_h
#define pfc_h

#include "utility.h"

void setSignalStatus(int signalReceived, enum boolean *PFC_sigUsr, enum boolean *PFC_sigRestart);
void addLastRead(long current_position, FILE *lastRead);
void changePointerPosition(FILE *fp_g18, FILE *lastRead);
int setPreviousGeographicCoordinates(FILE *fp_g18, double *previousLatitude, double *previousLongitude);
enum boolean checkCorrectPosition(FILE *fp_g18, FILE *lastRead);
 
double changeSpeed(double speed);
ssize_t readCorrectLine(char *buffer, size_t bufferLength, FILE *fp);
void getGeographicCoordinates(char* line, double* latitude, double* longitude);

int exe(int fd_pfcToTransducers, FILE *fp_g18, FILE *lastRead, double *previousLatitude, double *previousLongitude, enum boolean *sigUsr, enum boolean *sigRestart);

#endif
