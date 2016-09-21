#ifndef LIB_FILE_H
#define LIB_FILE_H


#ifdef	__cplusplus
extern "C" {
#endif
    
#define KBYTE (1024)
#define MBYTE (KBYTE * KBYTE)
    
double memory_free(const char *dir);
double memory_all(const char *dir);
    
#ifdef	__cplusplus
}
#endif

#endif