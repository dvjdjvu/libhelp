#include "config.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define CONFIG_POOL_SIZE        4096
#define CONFIG_POOL_RMD_SIZE    32
#define CONFIG_KEY_MAX_SIZE     64

static int config_load(config_t *config, char *file);
static int config_load2(config_t *config, char *file);

config_t *
config_new(log_t *log, char *file, int *line_error) {
    config_t *config;
    pool_t *pool;
    int ret;
    
    pool = pool_new(CONFIG_POOL_SIZE, CONFIG_POOL_RMD_SIZE);
    if (!pool) {
        log_error(log, "log(): create pool error");
        return NULL;
    }
    config = pool_calloc(pool, sizeof (config_t));
    if (!config) {
        log_error(log, "log(): pool calloc error");
        pool_delete(pool);
        return NULL;
    }
    config->pool = pool;
    config->log = log;
    config->line_error = 0;
    
    ret = config_load(config, file);
    *line_error = config->line_error;
    
    if (ret != OK) {
        pool_delete(pool);
        return NULL;
    }

    return config;
}

config_t *
config_new2(log_t *log, char *str, int *line_error) {
    config_t *config;
    pool_t *pool;
    int ret;
    
    pool = pool_new(CONFIG_POOL_SIZE, CONFIG_POOL_RMD_SIZE);
    if (!pool) {
        log_error(log, "log(): create pool error");
        return NULL;
    }
    config = pool_calloc(pool, sizeof (config_t));
    if (!config) {
        log_error(log, "log(): pool calloc error");
        pool_delete(pool);
        return NULL;
    }
    config->pool = pool;
    config->log = log;
    config->line_error = 0;
    
    ret = config_load2(config, str);
    *line_error = config->line_error;
    
    if (ret != OK) {
        pool_delete(pool);
        return NULL;
    }

    return config;
}

char *
config_get_str(config_t *config, char *key, char *def) {
    PWord_t pw;

    JSLG(pw, config->data, (u_char *) key);

    return pw ? (char *) *pw : def;
}

int
config_get_int(config_t *config, char *key, int def) {
    int rc;
    char *k;
    PWord_t pw;

    JSLG(pw, config->data, (u_char *) key);
    if (pw) {
        k = (char *) *pw;
        if (k[0] == '-') {
            k++;
            atoui(rc, k);
            return -rc;
        } else {
            atoui(rc, k);
            return rc;
        }
    }

    return def;
}

int
config_get_hex(config_t *config, char *key, int def) {
    int rc;
    char *k;
    PWord_t pw;

    JSLG(pw, config->data, (u_char *) key);

    if (pw) {
        k = (char *) *pw;

        return strtol(k, (char **) NULL, 16);
    }

    return def;
}

unsigned int
config_get_uint(config_t *config, char *key, unsigned int def) {
    unsigned int rc;
    char *k;
    PWord_t pw;

    JSLG(pw, config->data, (u_char *) key);
    if (pw) {
        k = (char *) *pw;
        atoui(rc, k);
        return rc;
    }

    return def;
}

int
config_get_bool(config_t *config, char *key, int def) {
    char *k;
    PWord_t pw;

    JSLG(pw, config->data, (u_char *) key);
    if (pw) {
        k = (char *) *pw;
        return k[0] == 'y' && k[1] == 'e' && k[2] == 's' && k[3] == '\0';
    }

    return def;
}

void
config_delete(config_t *config) {
    int rc;

    if (config) {
        if (config->data) {
            JSLFA(rc, config->data);
        }
        if (config->pool) {
            pool_delete(config->pool);
        }
    }
}

static int
config_load(config_t *config, char *file) {
    int fd, line, n;
    char c, *p, *k, *end, *pkey, *pvalue;
    char index[CONFIG_KEY_MAX_SIZE + 1];
    char key[CONFIG_KEY_MAX_SIZE + 1];
    ssize_t size;
    struct stat sb;
    static const u_char achar[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0.\0" "0123456789\0\0\0\0\0\0" "\0ABCDEFGHIJKLMNOPQRSTUVWXYZ\0\0\0\0_\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    PWord_t pw;

    config->line_error = 0;
    
    //  открываем файл  
    fd = open(file, O_RDONLY);
    if (fd < 0) {
        log_error(config->log, "log(): can't open %s", file);
        return ERROR;
    }
    //  получаем статистику файла
    if (fstat(fd, &sb) < 0) {
        log_error(config->log, "log(): can't stat %s", file);
        close(fd);
        return ERROR;
    }
    // удаляем предыдущий конфиг
    if (config->conf != NULL) {
        free(config->conf);
        config->conf = NULL;
    }
    //  читаем файл полностью в память 
    config->src = (char *) malloc(sb.st_size + 1);
    if (!config->src) {
        log_error(config->log, "log(): can't malloc");
        close(fd);
        return ERROR;
    }
    config->conf = (char *) malloc(sb.st_size + 1);
    if (!config->conf) {
        log_error(config->log, "log(): can't malloc");
        close(fd);
        return ERROR;
    }
    size = read(fd, config->src, sb.st_size);
    if (size != sb.st_size) {
        log_error(config->log, "log(): can't read file %s", file);
        free(config->src);
        close(fd);
        return ERROR;
    }
    close(fd);

    memcpy(config->conf, config->src, sb.st_size);
    
    enum {
        space_s,
        comment_s,
        param_key_s,
        param_key_sp_s,
        param_value_s,
        param_qt_value_s,
        param_value_skip_s,
        param_copy_value_s
    } state;

    state = space_s;
    line = 1;
    p = config->src;
    pkey = pvalue = NULL;
    end = config->src + sb.st_size;

    while (p < end) {
        c = p[0];
        switch (state) {
            case space_s:
                if (achar[(u_char) c]) {
                    pkey = p;
                    state = param_key_s;
                } else if (c == ' ' || c == '\t' || c == '\r') {
                    break;
                } else if (c == '\n') {
                    line++;
                } else if (c == '#') {
                    state = comment_s;
                } else {
                    log_error(config->log, "log(): unexpected symbol %c (%d) in %d", c, c, line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case comment_s:
                if (c == '\n') {
                    line++;
                    state = space_s;
                }
                break;
            case param_key_s:
                if (achar[(u_char) c]) {
                    break;
                } else if (c == ' ' || c == '\t') {
                    *p = '\0';
                    if (p - pkey > CONFIG_KEY_MAX_SIZE) {
                        log_error(config->log, "log(): key %s to long (%d > %d) in %d", pkey, (int) (p - pkey), CONFIG_KEY_MAX_SIZE, line);
                        free(config->src);
                        
                        config->line_error = line;
                        return ERROR;
                    }
                    state = param_key_sp_s;
                    break;
                } else {
                    log_error(config->log, "log(): unexpected symbol %c (%d) in %d", c, c, line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_key_sp_s:
                if (c == ' ' || c == '\t') {
                    break;
                } else if (c == '"') {
                    pvalue = p + 1;
                    state = param_qt_value_s;
                } else if (c == '@') {
                    pvalue = p + 1;
                    state = param_copy_value_s;
                } else if (c == '\r' || c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                } else {
                    pvalue = p;
                    state = param_value_s;
                }
                break;
            case param_value_s:
                if (c == ';') {
                    *p = '\0';
                    /*  insert key->value  */
                    JSLI(pw, config->data, (u_char *) pkey);
                    if (pw == PJERR) {
                        log_error(config->log, "log(): Judy error");
                        free(config->src);
                        return ERROR;
                    }
                    *pw = (Word_t) pool_cpy(config->pool, pvalue, p - pvalue + 1);
                    if (!*pw) {
                        log_error(config->log, "log(): pool_cpy error");
                        free(config->src);
                        return ERROR;
                    }
                    state = space_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_qt_value_s:
                if (c == '\\' && p[1] == '"') {
                    p++;
                } else if (c == '"') {
                    *p = '\0';
                    //  insert key->value  
                    JSLI(pw, config->data, (u_char *) pkey);
                    if (pw == PJERR) {
                        log_error(config->log, "log(): Judy error");
                        free(config->src);
                        return ERROR;
                    }
                    *pw = (Word_t) pool_cpy(config->pool, pvalue, p - pvalue + 1);
                    if (!*pw) {
                        log_error(config->log, "log(): pool_cpy error");
                        free(config->src);
                        return ERROR;
                    }
                    state = param_value_skip_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_value_skip_s:
                if (c == ';') {
                    state = space_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_copy_value_s:
                if (c == ';') {
                    *p = '\0';
                    n = p - pvalue;
                    if (n > CONFIG_KEY_MAX_SIZE) {
                        log_error(config->log, "log(): copy key %s to long (%d > %d) in %d", pvalue, n, CONFIG_KEY_MAX_SIZE, line);
                        free(config->src);
                        
                        config->line_error = line;
                        return ERROR;
                    }
                    memcpy(index, pvalue, n + 1);
                    //  copy values 
                    JSLF(pw, config->data, (u_char *) index);
                    while (pw) {
                        if (strncmp(index, pvalue, n) != 0) {
                            break;
                        }
                        k = (char *) *pw;
                        key[0] = '\0';
                        strcat(key, pkey);
                        strcat(key, index + n);
                        /*  insert key->value  */
                        JSLI(pw, config->data, (u_char *) key);
                        if (pw == PJERR) {
                            log_error(config->log, "log(): Judy error");
                            free(config->src);
                            return ERROR;
                        }
                        *pw = (Word_t) pool_cpy(config->pool, k, strlen(k) + 1);
                        if (!*pw) {
                            log_error(config->log, "log(): pool_cpy error");
                            free(config->src);
                            return ERROR;
                        }
                        JSLN(pw, config->data, (u_char *) index);
                    }
                    state = space_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
        }
        p++;
    }
    
    free(config->src);
    
    return OK;
}

static int
config_load2(config_t *config, char *str) {
    int fd, line, n;
    char c, *p, *k, *end, *pkey, *pvalue;
    char index[CONFIG_KEY_MAX_SIZE + 1];
    char key[CONFIG_KEY_MAX_SIZE + 1];
    static const u_char achar[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0.\0" "0123456789\0\0\0\0\0\0" "\0ABCDEFGHIJKLMNOPQRSTUVWXYZ\0\0\0\0_\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    PWord_t pw;

    int qqqq = 0;
    
    config->line_error = 0;
    
    // удаляем предыдущий конфиг
    if (config->conf != NULL) {
        free(config->conf);
        config->conf = NULL;
    }
    //  читаем файл полностью в память 
    config->src = (char *) calloc(strlen(str) + 1, sizeof(char));
    if (!config->src) {
        log_error(config->log, "log(): can't malloc");
        return ERROR;
    }
    config->conf = (char *) calloc(strlen(str) + 1, sizeof(char));
    if (!config->conf) {
        log_error(config->log, "log(): can't malloc");
        return ERROR;
    }
    
    memcpy(config->src,  str, strlen(str));
    memcpy(config->conf, config->src, strlen(str));
    
    enum {
        space_s,
        comment_s,
        param_key_s,
        param_key_sp_s,
        param_value_s,
        param_qt_value_s,
        param_value_skip_s,
        param_copy_value_s
    } state;

    state = space_s;
    line = 1;
    p = config->src;
    pkey = pvalue = NULL;
    end = config->src + strlen(str);
    
    while (p < end) {
        c = p[0];
        switch (state) {
            case space_s:
                if (achar[(u_char) c]) {
                    pkey = p;
                    state = param_key_s;
                } else if (c == ' ' || c == '\t' || c == '\r') {
                    break;
                } else if (c == '\n') {
                    line++;
                } else if (c == '#') {
                    state = comment_s;
                } else {
                    log_error(config->log, "log(): unexpected symbol %c (%d) in %d", c, c, line);
                    
                    config->line_error = line;
                    printf("qqqq1@@ = %d\n", qqqq++);
                    printf("log(): unexpected symbol %c (%d) in %d %s\n", c, c, line, p);
                    
                    free(config->src);
                    return ERROR;
                }
                break;
            case comment_s:
                if (c == '\n') {
                    line++;
                    state = space_s;
                }
                break;
            case param_key_s:
                if (achar[(u_char) c]) {
                    break;
                } else if (c == ' ' || c == '\t') {
                    *p = '\0';
                    if (p - pkey > CONFIG_KEY_MAX_SIZE) {
                        log_error(config->log, "log(): key %s to long (%d > %d) in %d", pkey, (int) (p - pkey), CONFIG_KEY_MAX_SIZE, line);
                        free(config->src);
                        
                        config->line_error = line;
                        return ERROR;
                    }
                    state = param_key_sp_s;
                    break;
                } else {
                    log_error(config->log, "log(): unexpected symbol %c (%d) in %d", c, c, line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_key_sp_s:
                if (c == ' ' || c == '\t') {
                    break;
                } else if (c == '"') {
                    pvalue = p + 1;
                    state = param_qt_value_s;
                } else if (c == '@') {
                    pvalue = p + 1;
                    state = param_copy_value_s;
                } else if (c == '\r' || c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                } else {
                    pvalue = p;
                    state = param_value_s;
                }
                break;
            case param_value_s:
                if (c == ';') {
                    *p = '\0';
                    /*  insert key->value  */
                    JSLI(pw, config->data, (u_char *) pkey);
                    if (pw == PJERR) {
                        log_error(config->log, "log(): Judy error");
                        free(config->src);
                        return ERROR;
                    }
                    *pw = (Word_t) pool_cpy(config->pool, pvalue, p - pvalue + 1);
                    if (!*pw) {
                        log_error(config->log, "log(): pool_cpy error");
                        free(config->src);
                        return ERROR;
                    }
                    state = space_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_qt_value_s:
                if (c == '\\' && p[1] == '"') {
                    p++;
                } else if (c == '"') {
                    *p = '\0';
                    //  insert key->value  
                    JSLI(pw, config->data, (u_char *) pkey);
                    if (pw == PJERR) {
                        log_error(config->log, "log(): Judy error");
                        free(config->src);
                        return ERROR;
                    }
                    *pw = (Word_t) pool_cpy(config->pool, pvalue, p - pvalue + 1);
                    if (!*pw) {
                        log_error(config->log, "log(): pool_cpy error");
                        free(config->src);
                        return ERROR;
                    }
                    state = param_value_skip_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_value_skip_s:
                if (c == ';') {
                    state = space_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
            case param_copy_value_s:
                if (c == ';') {
                    *p = '\0';
                    n = p - pvalue;
                    if (n > CONFIG_KEY_MAX_SIZE) {
                        log_error(config->log, "log(): copy key %s to long (%d > %d) in %d", pvalue, n, CONFIG_KEY_MAX_SIZE, line);
                        free(config->src);
                        
                        config->line_error = line;
                        return ERROR;
                    }
                    memcpy(index, pvalue, n + 1);
                    //  copy values 
                    JSLF(pw, config->data, (u_char *) index);
                    while (pw) {
                        if (strncmp(index, pvalue, n) != 0) {
                            break;
                        }
                        k = (char *) *pw;
                        key[0] = '\0';
                        strcat(key, pkey);
                        strcat(key, index + n);
                        /*  insert key->value  */
                        JSLI(pw, config->data, (u_char *) key);
                        if (pw == PJERR) {
                            log_error(config->log, "log(): Judy error");
                            free(config->src);
                            return ERROR;
                        }
                        *pw = (Word_t) pool_cpy(config->pool, k, strlen(k) + 1);
                        if (!*pw) {
                            log_error(config->log, "log(): pool_cpy error");
                            free(config->src);
                            return ERROR;
                        }
                        JSLN(pw, config->data, (u_char *) index);
                    }
                    state = space_s;
                } else if (c == '\n') {
                    log_error(config->log, "log(): unexpected end of line in %d", line);
                    free(config->src);
                    
                    config->line_error = line;
                    return ERROR;
                }
                break;
        }
        p++;
    }
    
    free(config->src);
    
    return OK;
}


inline char *
config_get_conf(config_t *config) {
    return config->conf;
}
