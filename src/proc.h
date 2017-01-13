#ifndef LIB_COR_PROC_H
#define LIB_COR_PROC_H

#include "core.h"
#include "logger.h"

#ifdef	__cplusplus
extern "C" {
#endif

int     proc_init(char **argv, log_t *log);
void    proc_set_title(char *title);
int     proc_daemonize(log_t *log);
int     proc_pid_save(char *file, log_t *log);
int     proc_pid_delete(char *file, log_t *log);

#ifdef	__cplusplus
}
#endif

#endif
