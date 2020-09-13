#ifndef config_h
#define config_h

/* PROCESS */
#define PID_MAX_LENGTH      10 /*Numero massimo di cifre di cui possono esser composti i pid dei PFC*/
#define PFC_ID_MAX_DIGITS   5 /*Numero massimo di cifre di cui sono composti gli ID dei PFC (sono compresi tra 1 e 3)*/

/* G18 */
#define TEMPO                       1 /*1 secondo*/
#define NMEA_FORMAT                 "$GPGLL"
#define NMEA_FORMAT_SEPARATOR       ","
#define MAX_SPEED_DIGITS            10
#define MAX_LINE_LENGTH             150
#define MAX_G18_FILE_LENGTH_DIGITS  10 /*Massimo numero di cifre della lunghezza in bytes del file g18.txt*/

/* FILES CONFIGURATIONS */
#define DEFAULT_PERMISSIONS 0660

/* PATH */
#define PATHNAME_BIN "../bin/"
#define PATHNAME_LOG "../log/"
#define PATHNAME_TEMP "../temp/"

#define FILENAME_WES_PIPE                   PATHNAME_TEMP "wesPipe"
#define FILENAME_PFC1_SOCKET                PATHNAME_TEMP "transducers_server"
#define FILENAME_PFC2_PIPE                  PATHNAME_TEMP "pfc2Pipe"
#define FILENAME_GENERATOREFALLIMENTI_PIPE  PATHNAME_TEMP "generatoreFallimentiPipe"

#define FILENAME_LAST_READ   PATHNAME_TEMP "last_read.txt"
#define FILENAME_PFC3_FILE   PATHNAME_TEMP "pfcTransducers.txt"

#define FILENAME_SWITCH_LOG    PATHNAME_LOG "switch.log"
#define FILENAME_STATUS_LOG    PATHNAME_LOG "status.log"
#define FILENAME_FAILURES_LOG  PATHNAME_LOG "failures.log"
#define FILENAME_SPEEDPFC1_LOG PATHNAME_LOG "speedPFC1.log"
#define FILENAME_SPEEDPFC2_LOG PATHNAME_LOG "speedPFC2.log"
#define FILENAME_SPEEDPFC3_LOG PATHNAME_LOG "speedPFC3.log"

#endif
