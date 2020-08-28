#ifndef main_h
#define main_h

#include "path.h"

#define COMMAND_LS_TEMP_DIR             "ls " PATHNAME_TEMP
#define COMMAND_RM_FILES_TEMP_DIR       "rm " PATHNAME_TEMP "*"
#define COMMAND_TEMP_DIR_NOT_EMPTY      "[ $(" COMMAND_LS_TEMP_DIR " | wc -l) -gt 0 ]"
#define COMMAND_REMOVE_FILES_TEMP_DIR   COMMAND_TEMP_DIR_NOT_EMPTY " && " COMMAND_RM_FILES_TEMP_DIR " || :"

#define COMMAND_LS_LOG_DIR             "ls " PATHNAME_LOG
#define COMMAND_RM_FILES_LOG_DIR       "rm " PATHNAME_LOG "*"
#define COMMAND_LOG_DIR_NOT_EMPTY      "[ $(" COMMAND_LS_LOG_DIR " | wc -l) -gt 0 ]"
#define COMMAND_REMOVE_FILES_LOG_DIR   COMMAND_LOG_DIR_NOT_EMPTY " && " COMMAND_RM_FILES_LOG_DIR " || :"

/*
 * "[ $(ls __PATHDIR__ | wc -l) -gt 0 ] && rm __PATHDIR__* || :"
 */

#endif
