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
  * @brief Создать семафор.
  */
semaphore_t *semaphore_create(char *semaphore_name);

/**
  * @brief Открыть семафор.
  */
semaphore_t *semaphore_open(char *semaphore_name);

/**
  * @brief Освобождаем семафор.
  */
void semaphore_post(semaphore_t *semap);

/**
  * @brief Захватываем семафор.
  */
void semaphore_wait(semaphore_t *semap);

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

