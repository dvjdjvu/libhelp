#ifndef LIB_POOL_H_
#define LIB_POOL_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>

#include "str.h"
#include "core.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define ALIGNMENT           sizeof(unsigned long)
#define POOL_ALLOC_MAX      4095

#define align_ptr(p)        (((uintptr_t) p + ((uintptr_t) ALIGNMENT - 1)) & ~((uintptr_t) ALIGNMENT - 1))

typedef struct pool_s           pool_t;
typedef struct pool_large_s     pool_large_t;

struct pool_s
{
    size_t              rmd;
    size_t              size;
    u_char              *last;
    u_char              *end;
    pool_t          *cur;
    pool_t          *next;
    pool_large_t    *large;
};

struct pool_large_s
{
    pool_large_t    *next;
    void                *data;
};

pool_t      *pool_new(size_t size, size_t rmd);
void            *pool_alloc(pool_t *pool, size_t size);
void            *pool_calloc(pool_t *pool, size_t size);
void            *pool_cpy(pool_t *pool, void *p, size_t size);
void            pool_free(pool_t *pool, void *p, size_t size);
void            pool_delete(pool_t *pool);

#ifdef	__cplusplus
}
#endif

#endif
