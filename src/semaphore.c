#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include "semaphore.h"

semaphore_t *
semaphore_create(char *semaphore_name) {
    int fd;
    semaphore_t *semap;
    pthread_mutexattr_t psharedm;
    pthread_condattr_t psharedc;

    fd = open(semaphore_name, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (fd < 0) {
        return NULL;
    }
    
    (void) ftruncate(fd, sizeof(semaphore_t));
    (void) pthread_mutexattr_init(&psharedm);
    (void) pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);
    (void) pthread_condattr_init(&psharedc);
    (void) pthread_condattr_setpshared(&psharedc, PTHREAD_PROCESS_SHARED);
    
    semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    (void) pthread_mutex_init(&semap->lock, &psharedm);
    (void) pthread_cond_init(&semap->nonzero, &psharedc);
    semap->count = 0;
    
    return semap;
}

semaphore_t *
semaphore_open(char *semaphore_name) {
    int fd;
    semaphore_t *semap;

    fd = open(semaphore_name, O_RDWR, 0666);
    if (fd < 0) {
        return (NULL);
    }
    
    semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    return semap;
}

void
semaphore_post(semaphore_t *semap) {
    pthread_mutex_lock(&semap->lock);
    
    if (semap->count == 0) {
        pthread_cond_signal(&semap->nonzero);
    }
    
    semap->count++;
    pthread_mutex_unlock(&semap->lock);
}

void
semaphore_wait(semaphore_t *semap) {
    pthread_mutex_lock(&semap->lock);
    
    while (semap->count == 0) {
        pthread_cond_wait(&semap->nonzero, &semap->lock);
    }
    
    semap->count--;
    pthread_mutex_unlock(&semap->lock);
}

void
semaphore_close(semaphore_t *semap) {
    munmap((void *) semap, sizeof(semaphore_t));
}

/*
 * Example:

//create.c
#include "pthread.h"
#include "semaphore.h"

int
main()
{
    semaphore_t *semap;

    semap = semaphore_create("/tmp/semaphore");
    if (semap == NULL)
        exit(1);
    semaphore_close(semap);
    return (0);
}

// post
#include "pthread.h"
#include "semaphore.h"

int
main()
{
    semaphore_t *semap;

    semap = semaphore_open("/tmp/semaphore");
    if (semap == NULL)
        exit(1);
    semaphore_post(semap);
    semaphore_close(semap);
    return (0);
}

// wait
#include "pthread.h"
#include "semaphore.h"

int
main()
{
    semaphore_t *semap;

    semap = semaphore_open("/tmp/semaphore");
    if (semap == NULL)
        exit(1);
    semaphore_wait(semap);
    semaphore_close(semap);
    return (0);
} 


#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>

#define SEMAPHORE_NAME "/my_named_semaphore"

int main(int argc, char ** argv) {
    sem_t *sem;

    if ( argc == 2 ) {
        printf("Dropping semaphore...\n");
        if ( (sem = sem_open(SEMAPHORE_NAME, 0)) == SEM_FAILED ) {
            perror("sem_open");
            return 1;
        }
        sem_post(sem);
        perror("sem_post");
        printf("Semaphore dropped.\n");
        return 0;
    }

    if ( (sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0777, 0)) == SEM_FAILED ) {
        perror("sem_open");
        return 1;
    }

    printf("Semaphore is taken.\nWaiting for it to be dropped.\n");
    if (sem_wait(sem) < 0 )
        perror("sem_wait");
    if ( sem_close(sem) < 0 )
        perror("sem_close");

    return 0;
}
 */
