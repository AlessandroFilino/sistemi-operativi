#ifndef main_h
#define main_h

#define PATHNAME_TEMP "../sistemioperativi/temp/"
#define PATHNAME_LOG "../sistemioperativi/log/"
#define COMMAND_REMOVE_FILES "[ $(ls %s | wc -l) -gt 0 ] && rm %s* || :"

#endif
