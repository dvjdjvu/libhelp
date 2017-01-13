#ifndef LIB_LOG_H
#define LIB_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "core.h"

#ifdef	__cplusplus
extern "C" {
#endif

enum log_level_e
{
    log_level_error = 0,
    log_level_warn,
    log_level_notice,
    log_level_debug
};

#define LOG_TIME_STR_SIZE   sizeof("[YYYY.MM.DD HH:MM:SS] ")
#define LOG_PID_STR_SIZE    sizeof("-18446744073709551615")
#define LOG_STR_SIZE        2048

#define log_error(log, args...)         log_put(log, log_level_error,  __FILE__, __LINE__, __func__,  args)
#define log_warn(log, args...)          log_put(log, log_level_warn,   __FILE__, __LINE__, __func__, args)
#define log_notice(log, args...)        log_put(log, log_level_notice, __FILE__, __LINE__, __func__, args)

#ifdef DEBUG_ON
    #define log_debug(log, args...)     log_put(log, log_level_debug, __FILE__, __LINE__, args)
#else
    #define log_debug(log, args...)
#endif

typedef struct
{
    int         level;
    time_t      ts;
    char        ts_str[LOG_TIME_STR_SIZE];
    char        pid_str[LOG_PID_STR_SIZE];
    const char  *file;
    FILE        *fd;
} log_t;

log_t      *log_new(const char *file, int level);
void        log_rotate(log_t *log);
void        log_set_pid(log_t *log, pid_t pid);
void        log_set_str_level(log_t *log, char *level);
void        log_put(log_t *log, enum log_level_e level, const char *file, int line, const char *format, ...);
void        log_delete(log_t *log);

int         setproctitle_init(log_t *log, char **argv);
int         setproctitle(log_t *log, char *title, char *PROGRAM_NAME);

#ifdef	__cplusplus
}
#endif

#endif
