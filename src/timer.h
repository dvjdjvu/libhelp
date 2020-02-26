#ifndef __TIME_H__
#define __TIME_H__
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TIMER_SINGLE_SHOT = 0, /*Periodic Timer*/
    TIMER_PERIODIC /*Single Shot Timer*/
} t_timer;

typedef void (*time_handler)(size_t timer_id, void *user_data);

int timer_initialize();
size_t timer_start(unsigned int interval, time_handler handler, t_timer type, void *user_data);
void timer_stop(size_t timer_id);
void timer_finalize();

#ifdef __cplusplus
}
#endif

#endif