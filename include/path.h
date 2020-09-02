#ifndef path_h
#define path_h

#define PATHNAME_BIN "../bin/"
#define PATHNAME_LOG "../log/"
#define PATHNAME_TEMP "../temp/"

#define FILENAME_WES_PIPE        PATHNAME_TEMP "wesPipe"
#define FILENAME_PFC1_PIPE       PATHNAME_TEMP "pfc1Pipe"
#define FILENAME_PFC2_SOCKET     PATHNAME_TEMP "transducers_server"
#define FILENAME_PFCSWITCH_PIPE  PATHNAME_TEMP "pfcSwitch_pipe"

#define FILENAME_LAST_READ   PATHNAME_TEMP "last_read.txt"
#define FILENAME_PFC3_FILE   PATHNAME_TEMP "pfcTransducers.txt"

#define FILENAME_SWITCH_LOG    PATHNAME_LOG "switch.log"
#define FILENAME_STATUS_LOG    PATHNAME_LOG "status.log"
#define FILENAME_FAILURES_LOG  PATHNAME_LOG "failures.log"
#define FILENAME_SPEEDPFC1_LOG PATHNAME_LOG "speedPFC1.log"
#define FILENAME_SPEEDPFC2_LOG PATHNAME_LOG "speedPFC2.log"
#define FILENAME_SPEEDPFC3_LOG PATHNAME_LOG "speedPFC3.log"

#endif
