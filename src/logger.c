#include "logger.h"

/*  variables for process title change  */
static char **argv;
static char *argv_last;

static const char   *log_levels[] = {"error  ", "warn   ", "notice ", "debug  "};

log_t *
log_new(const char *file, int level) 
{
    pid_t       pid;
    log_t   *log;

    log = (log_t *) malloc(sizeof(log_t));
    if (!log) {
        fprintf(stderr, "can't malloc in %s:%d\n", __FILE__, __LINE__);
        return NULL;
    }
    log->fd = fopen(file, "a");
    if (!log->fd) {
        fprintf(stderr, "can't fopen %s in %s:%d %s\n", file, __FILE__, __LINE__, strerror(errno));
        free(log);
        return NULL;
    }
    log->ts = 0;
    log->level = level;
    pid = getpid();
    snprintf(log->pid_str, LOG_PID_STR_SIZE, "(%lu)", (unsigned long) pid);

    return log;
}

void
log_rotate(log_t *log)
{
    if (log) {
        if (log->fd) {
            fclose(log->fd);
            log->fd = 0;
        }
        log->fd = fopen(log->file, "a");
        if (!log->fd) {
            fprintf(stderr, "can't fopen %s in %s:%d\n", log->file, __FILE__, __LINE__);
        }
    }
}

void
log_set_pid(log_t *log, pid_t pid)
{
    snprintf(log->pid_str, LOG_PID_STR_SIZE, "(%lu)", (unsigned long) pid);
}

void
log_set_str_level(log_t *log, char *level)
{
    if (strcmp(level, "debug") == 0) {
        log->level = log_level_debug;
    } else if (strcmp(level, "notice") == 0) {
        log->level = log_level_notice;
    } else if (strcmp(level, "warn") == 0) {
        log->level = log_level_warn;
    } else {
        log->level = log_level_error;
    }
}

void
log_put(log_t *log, enum log_level_e level, const char *file, const char *_func, int line, const char *format, ...) 
{
    int         ret;
    char        *p, *end;
    char        str[LOG_STR_SIZE];
    time_t      ts;
    struct tm   tm;
    va_list     vars;

    if (log == NULL) {
        return;
    }
    if (!log || log->level < level) {
        return;
    }
    /*  make time if need  */
    ts = time(NULL);
    if (ts != log->ts) {
        log->ts = ts;
        localtime_r(&ts, &tm);
        snprintf(log->ts_str, LOG_TIME_STR_SIZE, "[%04d.%02d.%02d %02d:%02d:%02d] ", 
            (tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    p = str;
    end = p + LOG_STR_SIZE;
    if (file && _func) {
        ret = snprintf(p, LOG_STR_SIZE, "%s%s%s in %s:%s:%d ",
            log->ts_str, log_levels[level], log->pid_str, file, _func, line);
        p += ret;
    } else {
        ret = snprintf(p, LOG_STR_SIZE, "%s%s%s ",
            log->ts_str, log_levels[level], log->pid_str);
        p += ret;
    }
    /*  write message  */
    va_start(vars, format);
    ret = vsnprintf(p, end - p, format, vars);
    va_end(vars);
    if (ret < 0) {
        return;
    }
    p += ret;
    *p++ = '\n';
    /*  write to file  */
    fwrite(str, 1, p - str, log->fd);
    fflush(log->fd);
}

void
log_delete(log_t *log) 
{
    if (log) {
        if (log->fd) {
            fclose(log->fd);
            log->fd = 0;
        }
        free(log);
    }
}

int
setproctitle_init(log_t *log, char **argv)
{
    int         i;
    char        *p;
    size_t      size;
    extern char **environ;
    
    size = 0;
    argv = argv;
    
    for (i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }
    
    p = (char *) malloc(size);
    if (p == NULL) {
        log_error(log, "setproctitle_init() can't allocate memory for environment");
        
        return ERROR;
    }
    
    argv_last = argv[0];
    for (i = 0; argv[i]; i++) {
        if (argv_last == argv[i]) {
            argv_last = argv[i] + strlen(argv[i]) + 1;
        }
    }
    
    for (i = 0; environ[i]; i++) {
        if (argv_last == environ[i]) {
            size = strlen(environ[i]) + 1;
            argv_last = environ[i] + size;
            
            memcpy(p, environ[i], size);
            environ[i] = p;
            p += size;
        }
    }
    
    argv_last--;
    
    return OK;
}

int
setproctitle(log_t *log, char *title, char *PROGRAM_NAME)
{
    char    *p;
    
    p = argv[0];
    argv[1] = NULL;
    
    strncpy(p, PROGRAM_NAME, argv_last - p);
    p += strlen(PROGRAM_NAME) - 1;

    if (p > argv_last) {
        log_error(log, "setproctitle() title is to big");
        
        return ERROR;
    }

    strncpy(p, title, argv_last - p);
    p += strlen(title);
    
    if (p < argv_last) {
        memset(p, 0, argv_last - p);
    }

    return OK;
}