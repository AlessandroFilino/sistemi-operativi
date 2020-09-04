#ifndef pfcDisconnectedSwitch_h
#define pfcDisconnectedSwitch_h

#define get_argv(pfcId) PFC##pfcId##_argv

void childHandler(int);
int getErrorInfo(char *error);

#endif
