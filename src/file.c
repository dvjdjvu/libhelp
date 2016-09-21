/* 
 * File:   file.c
 * Author: root
 *
 * Created on 21 сентября 2016 г., 16:20
 */

#include "file.h"

/**
 * @brief Возвращает колличество свободной памяти доступоной rootу в файловой системе в которой находится каталог dir.
 * @param dir - путь к диску
 * @return Кол-во свободной памяти в мегабайтах
 */
double 
memory_free(const char *dir) {

	int mbyte = MBYTE;
	struct statvfs fs_pnki;
	size_t size_free;
	size_t size_all;

	if(statvfs(dir, &fs_pnki) >= 0){
		size_free = (geteuid() == 0 ) ? fs_pnki.f_frsize * fs_pnki.f_bfree : fs_pnki.f_frsize * fs_pnki.f_bavail;

		return (size_free/mbyte);
	}else
		return 0;

}

/**
 * @brief Возвращает колличество памяти доступоной rootу в файловой системе в которой находится каталог dir.
 * @param dir - путь к диску
 * @return Кол-во памяти в мегабайтах
 */

double 
memory_all(const char *dir) {

	int mbyte = MBYTE;
	struct statvfs fs_pnki;
	size_t size_free;
	size_t size_all;

	if(statvfs(dir, &fs_pnki) >= 0){
		size_all  = fs_pnki.f_frsize * fs_pnki.f_blocks;

		return (size_all/mbyte);
	}else
		return 0;

}
