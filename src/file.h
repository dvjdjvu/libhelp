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
    
/**
 * @brief Возвращает колличество свободной памяти доступоной rootу в файловой системе в которой находится каталог dir.
 * @param dir - путь к диску
 * @return Кол-во свободной памяти в мегабайтах
 */
double memory_free(const char *dir);

/**
 * @brief Возвращает колличество памяти доступоной rootу в файловой системе в которой находится каталог dir.
 * @param dir - путь к диску
 * @return Кол-во памяти в мегабайтах
 */
double memory_all(const char *dir);

/**
 * @brief Функция блочного(по BUFSIZE байт) копирования файла from в файл to.
 * @param from Копируемый файл.
 * @param to   Куда скоприовать файл.
 * @return  0 - все хорошо, -1 - ошибка копирования.
 */
int file_copy(const char *from, const char *to);

/**
 * @brief Проверяет, запущен ли Xserver.
 * @return 0 - Xserver запущен, -1 - Xserver не запущен.
 */
int xserver_is_start();

#ifdef	__cplusplus
}
#endif

#endif