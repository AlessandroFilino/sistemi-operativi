#ifndef utility_textfile_h
#define utility_textfile_h

#include <stdio.h> /* Per il tipo "FILE"*/

void createEmptyFile(char *filename, char *mode);
FILE *openFile(const char* filename, const char* mode);
int readLine(int fd, char *buffer, char delimiter);

#endif
