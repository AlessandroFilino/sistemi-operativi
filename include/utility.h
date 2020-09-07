#ifndef utility_h
#define utility_h

#include <stdio.h> /* Per il tipo "FILE"*/
#include <sys/socket.h>
#include <sys/un.h>

#define inverse(NUM) ((double) 1/NUM)

/*
 * "[ $(ls __PATHDIR__ | wc -l) -gt 0 ] && rm __PATHDIR__* || :"
 * TODO: controllare se l'ultimo carattere di PATHDIR è '/' e, in caso mancasse, inserirlo?
 */
#define remove_files_from(PATHDIR) "[ $(ls " PATHDIR " | wc -l) -gt 0 ] && rm " PATHDIR "* || :"
#define concat(DEST, S)            DEST S

/*
 * array_length considera anche il carattere di fine stringa
 * perciò dobbiamo sottrarre 1 per il calcolo della lunghezza
 * effettiva
 */
#define array_length(ARRAY)        (sizeof(ARRAY)/sizeof(ARRAY[0]))
#define string_length(S)           (array_length(S)-1)

#define DEFAULT_PERMISSIONS 0660
#define DEFAULT_PROTOCOL    0

enum boolean {FALSE, TRUE};

FILE *openFile(const char* filename, const char* mode);
int readLine(int fd, char *buffer, char delimiter);
int createChild(int (*execv_function)(const char*, char* const*), char *filename, char **argv);

void createEmptyFile(char *filename, char *mode);
void createSpecialFile(char *filename, mode_t mode, mode_t permissions);
void createPipe(char *pipename, mode_t permissions);

int connectPipe(char *pipename, int mode);
int createServerAF_UNIXSocket(char *socketname, int maximumConnections, struct sockaddr **clientSockAddrPtr, int unsigned *clientLen);
int createClientAF_UNIXSocket(char *socketname, struct sockaddr_un* serverUNIXAddress, struct sockaddr **serverSockAddrPtr, int unsigned *serverLen);
void connectSocket(int clientFd, const struct sockaddr* serverSockAddrPtr, socklen_t serverLen);
void setFileFlags(int fd, unsigned int newFlags);

int numberOfDigits(int value);
void removeLastChar(char *string);
void tokenize(char *string, char *separator, int tokenNumber, ...);

#endif
