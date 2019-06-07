/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   semaphore.h
 * Author: root
 *
 * Created on 7 июня 2019 г., 14:19
 */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct semaphore semaphore_t;

/**
  * @brief Создать семафор. По умолчанию он закрыт.
  */
semaphore_t *semaphore_create(char *semaphore_name);

/**
  * @brief Открыть семафор.
  */
semaphore_t *semaphore_open(char *semaphore_name);

/**
  * @brief Освободить семафор.
  */
void semaphore_unlock(semaphore_t *semap);

/**
  * @brief Захватить семафор.
  */
void semaphore_lock(semaphore_t *semap);

/**
  * @brief Закрыть семафор.
  */
void semaphore_close(semaphore_t *semap);


struct semaphore {
    pthread_mutex_t lock;
    pthread_cond_t nonzero;
    unsigned count;
};

#ifdef __cplusplus
}
#endif

#endif /* SEMAPHORE_H */

