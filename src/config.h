#ifndef LIB_CONFIG_H
#define LIB_CONFIG_H

#include "core.h"
#include "logger.h"
#include "pool.h"
#include <Judy.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct config_s     config_t;

struct config_s
{
    Pvoid_t     data;
    pool_t  *pool;
    log_t   *log;
    
    char *src;
    int line_error;
    
    char *conf;
};

config_t       *config_new(log_t *log, char *file, int *line_error);
config_t       *config_new2(log_t *log, char *str, int *line_error);
int            *config_test(log_t *log, char *str, int *size);

char           *config_get_str(config_t *config, char *key, char *def);
int             config_get_int(config_t *config, char *key, int def);
int             config_get_hex(config_t *config, char *key, int def);
unsigned int    config_get_uint(config_t *config, char *key, unsigned int def);
int             config_get_bool(config_t *config, char *key, int def);
void            config_delete(config_t *config);
char           *config_get_conf(config_t *config);

#ifdef	__cplusplus
}
#endif

#endif
