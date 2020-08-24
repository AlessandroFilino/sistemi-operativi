#ifndef main_h
#define main_h

#define COMMAND_REMOVE_FILES "[ $(ls %s | wc -l) -gt 0 ] && rm %s* || :"

#endif
