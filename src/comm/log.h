#ifndef __COMM_LOG_H__
#define __COMM_LOG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <pthread.h>

struct _log_s {
    FILE *fp;
    pthread_mutex_t lock;
};

typedef struct _log_s log_t;

log_t *create_log(const char *filename);
void destory_log(log_t *log);
void info(log_t *log, const char *format, ...);
void error(log_t *log, const char *format, ...);
void debug(log_t *log, const char *format, ...);

#ifdef __cplusplus
}
#endif //? extern "C"
#endif //? __COMM_LOG_H
