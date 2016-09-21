#ifndef LIB_FILE_H
#define LIB_FILE_H

#include <stdio.h>
#include <dirent.h>

#include <ustat.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <time.h>
#include <utime.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define KBYTE (1024)
#define MBYTE (KBYTE * KBYTE)
    
#define BUFSIZE 512
#define PERM_FILE (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
    
double memory_free(const char *dir);
double memory_all(const char *dir);
    
#ifdef	__cplusplus
}
#endif

#endif