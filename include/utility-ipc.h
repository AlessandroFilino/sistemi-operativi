#ifndef utility_ipc_h
#define utility_ipc_h

#include <sys/socket.h>
#include <sys/un.h>

#define DEFAULT_PROTOCOL 0

void createSpecialFile(char *filename, mode_t mode, mode_t permissions);
void createPipe(char *pipename, mode_t permissions);
void setFileFlags(int fd, unsigned int newFlags);

int connectPipe(char *pipename, int mode);
int createServerAF_UNIXSocket(char *socketname, int maximumConnections, struct sockaddr **clientSockAddrPtr, int unsigned *clientLen);
int createClientAF_UNIXSocket(char *socketname, struct sockaddr_un* serverUNIXAddress, struct sockaddr **serverSockAddrPtr, int unsigned *serverLen);
void connectSocket(int clientFd, const struct sockaddr* serverSockAddrPtr, socklen_t serverLen);
enum boolean socketIsNotConnected(int socketFd);

#endif
