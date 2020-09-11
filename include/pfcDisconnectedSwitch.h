#ifndef pfcDisconnectedSwitch_h
#define pfcDisconnectedSwitch_h

#define get_argv(pfcId) PFC##pfcId##_argv

void childHandler(int signal);

//int getErrorInfo(char *error);
void getErrorInfo(char *error, char *sign, int *pfcNumber); 

#endif
