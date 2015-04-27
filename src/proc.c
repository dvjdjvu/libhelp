#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <proc.h>

static char     **proc_argv;
static char     *proc_last;

int
proc_init(char **argv, log_t *log)
{
    int         i;
    char        *p;
    size_t      size;
    extern char **environ;

    proc_argv = argv;
    size = 0;
    for (i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }
    p = (char *) malloc(size);
    if (!p) {
        log_error(log, "can't malloc");
        return ERROR;
    }
    proc_last = argv[0];
    for (i = 0; argv[i]; i++) {
        if (proc_last == argv[i]) {
            proc_last = argv[i] + strlen(argv[i]) + 1;
        }
    }
    for (i = 0; environ[i]; i++) {
        if (proc_last == environ[i]) {
            size = strlen(environ[i]) + 1;
            proc_last = environ[i] + size;
            memcpy(p, environ[i], size);
            environ[i] = p;
            p += size;
        }
    }
    proc_last--;

    return OK;
}

void
proc_set_title(char *title)
{
    size_t  size;

    proc_argv[1] = NULL;
    size = strlen(title);
    if (size > proc_last - proc_argv[0]) {
        size = proc_last - proc_argv[0] - 1;
    }
    memcpy(proc_argv[0], title, size);
    memset(proc_argv[0] + size, 0, proc_last - proc_argv[0] - size);
}

int
proc_daemonize(log_t *log)
{
    int     fd;
    pid_t   pid;

    pid = fork();
    if (pid == -1) {
        log_error(log, "can't fork");
        return ERROR;
    } else if (pid) {
        exit(0);
    }
    if (setsid() == -1) {
        log_error(log, "can't setsid");
        return ERROR;
    }
    umask(0);
    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        log_error(log, "can't open /dev/null");
        return ERROR;
    }
    if (dup2(fd, STDIN_FILENO) == -1) {
        log_error(log, "can't dup for stdin");
        return ERROR;
    }
    if (dup2(fd, STDOUT_FILENO) == -1) {
        log_error(log, "can't dup for stdout");
        return ERROR;
    }
    if (dup2(fd, STDERR_FILENO) == -1) {
        log_error(log, "can't dup for stderr");
        return ERROR;
    }
    close(fd);

    return OK;
}

int
proc_pid_save(char *file, log_t *log)
{
    pid_t   pid;
    FILE    *fd;

    fd = fopen(file, "w");
    if (!fd) {
        log_error(log, "can't fopen %s for write", file);
        return ERROR;
    }
    pid = getpid();
    fprintf(fd, "%ld", (long) pid);
    if (fclose(fd) == -1) {
        log_error(log, "can't fclose %s", file);
        return ERROR;
    }

    return OK;
}

int
proc_pid_delete(char *file, log_t *log)
{
    if (unlink(file)) {
        log_error(log, "can't unlink %s", file);
        return ERROR;
    }

    return OK;
}
