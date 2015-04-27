#include "pool.h"

pool_t *
pool_new(size_t size, size_t rmd)
{
    pool_t  *pool;

    if (size < 0) {
        return NULL;
    }
    pool = (pool_t *) malloc(size);
    if (!pool) {
        return NULL;
    }
    pool->rmd = rmd < ALIGNMENT ? ALIGNMENT : rmd;
    pool->size = size;
    pool->last = (u_char *) pool + sizeof(pool_t);
    pool->end = (u_char *) pool + size;
    pool->cur = pool;
    pool->next = NULL;
    pool->large = NULL;

    return pool;
}

void *
pool_alloc(pool_t *pool, size_t size)
{
    u_char              *m;
    pool_t          *p, *n, *c;
    pool_large_t    *l;

    if (size <= POOL_ALLOC_MAX && size <= (pool->end - (u_char *) pool - (size_t) align_ptr(sizeof(pool_t)))) {
        p = c = pool->cur;
        for (;;) {
            m = (u_char *) align_ptr(p->last);
            if ((size_t) (p->end - m) >= size) {
                p->last = m + size;
                return m;
            }
            if ((size_t) (p->end - m) < p->rmd) {
                c = p->next;
            }
            if (p->next == NULL) {
                break;
            }
            p = p->next;
            pool->cur = c;
        }
        n = pool_new((size_t) (p->end - (u_char *) p), p->rmd);
        if (n == NULL) {
            return NULL;
        }
        pool->cur = c ? c : n;
        pool->size += n->size;
        p->next = n;
        m = (u_char *) align_ptr(n->last);
        n->last = m + size;
        return m;
    }
    m = (u_char *) malloc(size);
    if (!m) {
        return NULL;
    }
    l = (pool_large_t *) pool_alloc(pool, sizeof(pool_large_t));
    if (!l) {
        free(m);
        return NULL;
    }
    l->data = m;
    l->next = pool->large;
    pool->large = l;
    pool->size += size;

    return m;
}

void *
pool_calloc(pool_t *pool, size_t size)
{
    void    *p;

    p = pool_alloc(pool, size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}

void *
pool_cpy(pool_t *pool, void *p, size_t size)
{
    u_char  *ret;

    ret = pool_alloc(pool, size);
    if (!ret) {
        return NULL;
    }
    memcpy(ret, p, size);

    return ret;
}

void
pool_free(pool_t *pool, void *p, size_t size)
{
    if (pool->cur->last == ((u_char *) p + size)) {
        pool->cur->last = (u_char *) p;
    }
}

void
pool_delete(pool_t *pool)
{
    pool_t          *p, *n;
    pool_large_t    *l;

    if (pool) {
        for (l = pool->large; l; l = l->next) {
            if (l->data) {
                free(l->data);
            }
        }
        for (p = pool, n = pool->next; ; p = n, n = n->next) {
            free(p);
            if (n == NULL) {
                break;
            }
        }
    }
}
