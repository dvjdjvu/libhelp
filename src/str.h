#ifndef COR_STRING_H
#define COR_STRING_H

#include "core.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define ITOA_BUF_SIZE   sizeof("-18446744073709551615")

#define _string(_s)      { sizeof(_s) - 1, _s }
#define _string_null     { 0, NULL }
#define _tolower(_c)     (_c >= 'A' && _c <= 'Z') ? (_c | 0x20) : _c
#define _toupper(_c)     (_c >= 'a' && _c <= 'z') ? (_c & ~0x20) : _c

#define strcmp2(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1])
#define strcmp3(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2])
#define strcmp4(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3])
#define strcmp5(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4])
#define strcmp6(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4] && _s[_off + 5] == _d[5])
#define strcmp7(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4] && _s[_off + 5] == _d[5] && _s[_off + 6] == _d[6])
#define strcmp8(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4] && _s[_off + 5] == _d[5] && _s[_off + 6] == _d[6] && _s[_off + 7] == _d[7])
#define strcmp9(_off, _s, _d)   (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4] && _s[_off + 5] == _d[5] && _s[_off + 6] == _d[6] && _s[_off + 7] == _d[7] && _s[_off + 8] == _d[8])
#define strcmp10(_off, _s, _d)  (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4] && _s[_off + 5] == _d[5] && _s[_off + 6] == _d[6] && _s[_off + 7] == _d[7] && _s[_off + 8] == _d[8] && _s[_off + 9] == _d[9])
#define strcmp11(_off, _s, _d)  (_s[_off] == _d[0] && _s[_off + 1] == _d[1] && _s[_off + 2] == _d[2] && _s[_off + 3] == _d[3] && _s[_off + 4] == _d[4] && _s[_off + 5] == _d[5] && _s[_off + 6] == _d[6] && _s[_off + 7] == _d[7] && _s[_off + 8] == _d[8] && _s[_off + 9] == _d[9] && _s[_off + 10] == _d[10])

#define strcatps(_p, _s) \
    memcpy(_p, _s->data, _s->size); \
    _p += _s->size;

#define strcatps2(_p, _s) \
    memcpy(_p, _s.data, _s.size); \
    _p += _s.size;

#define strcatpd(_p, _s) \
    memcpy(_p, _s, sizeof(_s) - 1); \
    _p += sizeof(_s) - 1;

#define strcatpc(_p, _s, _z) \
    memcpy(_p, _s, _z); \
    _p += _z;

#define atoui(_u, _c) \
    _u = 0; \
    while (*_c) { \
        if (*_c < '0' || *_c > '9') { \
            break; \
        } \
        _u = _u * 10 + (*_c - '0'); \
        _c++; \
    }

#define itoa(_p, _v, _n, _k, _b) \
    _n = _v; \
    _k = _b; \
    do { \
        *_k++ = _n % 10 + '0'; \
    } while (_n /= 10); \
    while (_k > _b) { \
        *_p++ = *(--_k); \
    }

#define itoa64(_p, _v, _n, _k, _b, _ch) \
    _n = _v; \
    _k = _b; \
    do { \
        *_k++ = _ch[_n & 0x3f]; \
    } while (_n >>= 6); \
    while (_k > _b) { \
        *_p++ = *(--_k); \
    }

#define pstr_new(_s, _size, _p) \
    _s = pool_alloc(_p, sizeof(str_t) + _size + 1); \
    if (_s) { \
        _s->size = _size; \
        _s->data = (char *) _s + sizeof(str_t); \
        _s->data[_size] = '\0'; \
    }

#define pstr_dup(_s, _s2, _p) \
    _s = pool_alloc(_p, sizeof(str_t) + _s2->size + 1); \
    if (_s) { \
        _s->size = _s2->size; \
        _s->data = (char *) _s + sizeof(str_t); \
        memcpy(_s->data, _s2->data, _s->size + 1); \
    }

#define pstr_cpy(_s, _c, _p) \
    _s = pool_alloc(_p, sizeof(str_t) + strlen(_c) + 1); \
    if (_s) { \
        _s->size = strlen(_c); \
        _s->data = (char *) _s + sizeof(str_t); \
        memcpy(_s->data, _c, _s->size + 1); \
    }

#define pstr_ncpy(_s, _c, _n, _p) \
    _s = pool_alloc(_p, sizeof(str_t) + (_n) + 1); \
    if (_s) { \
        _s->size = (_n); \
        _s->data = (char *) _s + sizeof(str_t); \
        memcpy(_s->data, _c, _s->size + 1); \
    }

#ifdef	__cplusplus
}
#endif

#endif

