#ifndef errors_h
#define errors_h

/*
 * TODO cambiare '\n' in "\n" e modificare readLine
 *      (readLine dovrebbe accettare una stringa come delimiter
 *      anzichè un carattere perchè così, se in un futuro volessimo
 *      cambiare il MESSAGES_SEPARATOR in "\n\r" potremo farlo)
 */
#define MESSAGES_SEPARATOR     '\n'

#define WES_MESSAGE_MAX_LENGTH 			        30
#define WES_MESSAGE_SIGN_MAX_LENGTH             5
#define WES_MESSAGE_SUCCESS    			        "OK"
#define WES_MESSAGE_PFC1_ERROR 			        "ERROR-PFC1"
#define WES_MESSAGE_PFC2_ERROR 			        "ERROR-PFC2"
#define WES_MESSAGE_PFC3_ERROR 			        "ERROR-PFC3"
#define WES_MESSAGE_EMERGENCY  			        "EMERGENCY"
#define WES_MESSAGE_PFC_ERROR_NEGATIVE(pfc)	    WES_MESSAGE_PFC##pfc##_ERROR "-N"
#define WES_MESSAGE_PFC_ERROR_POSITIVE(pfc)	    WES_MESSAGE_PFC##pfc##_ERROR "-P"

#define GENERATOREFALLIMENTI_MESSAGE_MAX_LENGTH 50
#define GENERATOREFALLIMENTI_SIGSTOP "SIGSTOP signal sent to PFC%d"
#define GENERATOREFALLIMENTI_SIGINT  "SIGINT signal sent to PFC%d"
#define GENERATOREFALLIMENTI_SIGCONT "SIGCONT signal sent to PFC%d"
#define GENERATOREFALLIMENTI_SIGUSR1 "SIGUSR1 signal sent to PFC%d"

#define INPUT_FNF_MESSAGE       "Input file not found"
#define PATH_ERROR_MESSAGE      "path error"
#define MAIN_PATH_ERROR_MESSAGE "main: " PATH_ERROR_MESSAGE
#define MAIN_INPUT_FNF_MESSAGE  "main: " INPUT_FNF_MESSAGE

#define PFCDISCONNECTEDSWITCH_MESSAGE_MAX_LENGTH            50
#define PFCDISCONNECTEDSWITCH_SEPARATOR                     ":"
#define PFCDISCONNECTEDSWITCH_MESSAGE_SIGCONT               "SIGCONT sent to PFC%d"
#define PFCDISCONNECTEDSWITCH_MESSAGE_PFC_CREATED           "PFC%d is created again"
#define PFCDISCONNECTEDSWITCH_MESSAGE_GENERATORE_FALLIMENTI "New Pid sent to Generatore Fallimenti"
#define PFCDISCONNECTEDSWITCH_MESSAGE_EMERGENCY             "SIGCONT signal sent to PFC1, PFC2, PFC3"

#define APPLICATION_ENDED_MESSAGE "Terminated"

#endif
