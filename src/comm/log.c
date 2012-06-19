#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>  //struct tm
#include <sys/time.h> //gettimeofday
#include "log.h"

#define BUFSIZE 2048

static inline int get_date(char *buf) {
    struct timeval tv;
    struct tm gmt;
    gettimeofday(&tv, 0);
    localtime_r(&tv.tv_sec, &gmt);
    return sprintf(buf, "%04d%02d%02d",
                   gmt.tm_year+1900, gmt.tm_mon+1, gmt.tm_mday);
}

static inline int get_time(char *buf) {
    struct timeval tv;
    struct tm gmt;
    gettimeofday(&tv, 0);
    localtime_r(&tv.tv_sec, &gmt);
    return sprintf(buf, "%04d%02d%02d %02d:%02d:%02d.%02ld",
                   gmt.tm_year+1900, gmt.tm_mon+1, gmt.tm_mday,
                   gmt.tm_hour, gmt.tm_min, gmt.tm_sec, tv.tv_usec/1000);
}

static inline void write_log(const char *buf, log_t *log) {
    if ((log != NULL) && (log->fp != NULL) && (buf != NULL)) {
        pthread_mutex_lock(&log->lock);
        fputs(buf, log->fp);
        fflush(log->fp);
        pthread_mutex_unlock(&log->lock);
    }
}

log_t *create_log(const char *filename) {
    if (filename == NULL) {
        return NULL;
    }
    char buf[256], date_buf[9];
    log_t *log = (log_t *)malloc(sizeof(log_t));
    get_date(date_buf);
    date_buf[8] = '\0';
    snprintf(buf, 256, "%s_%s.log", filename, date_buf);

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        destory_log(log);
        return NULL;
    }
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        destory_log(log);
        return NULL;
    }
    pthread_mutex_init(&log->lock, &attr);
    log->fp = fopen(buf, "a");
    return log;
}

void destory_log(log_t *log) {
    if (log != NULL) {
        if (log->fp != NULL)
            fclose(log->fp);
        pthread_mutex_destroy(&log->lock);
        free(log);
    }
}

void info(log_t *log, const char *format, ...) {
    char buf[BUFSIZE];
    va_list arg_list;
    int ret = 0;

    ret = get_time(buf);
    ret += sprintf(buf+ret, " %s - ", "INF");

    va_start(arg_list, format);
    ret += vsnprintf(buf+ret, BUFSIZE-ret, format, arg_list);
    va_end(arg_list);
    if (ret < BUFSIZE) {
        buf[ret] = '\n';
        buf[ret+1] = '\0';
        write_log(buf, log);
    } else {
        write_log("too long line!\n", log);
    }
}

void error(log_t *log, const char *format, ...) {
    char buf[BUFSIZE];
    va_list arg_list;
    int ret = 0;

    ret = get_time(buf);
    ret += sprintf(buf+ret, " %s - ", "ERR");

    va_start(arg_list, format);
    ret += vsnprintf(buf+ret, BUFSIZE-ret, format, arg_list);
    va_end(arg_list);
    if (ret < BUFSIZE) {
        buf[ret] = '\n';
        buf[ret+1] = '\0';
        write_log(buf, log);
    } else {
        write_log("too long line!\n", log);
    }
}

void debug(log_t *log, const char *format, ...) {
    char buf[BUFSIZE];
    va_list arg_list;
    int ret = 0;

    ret = get_time(buf);
    ret += sprintf(buf+ret, " %s - ", "DEB");

    va_start(arg_list, format);
    ret += vsnprintf(buf+ret, BUFSIZE-ret, format, arg_list);
    va_end(arg_list);
    if (ret < BUFSIZE) {
        buf[ret] = '\n';
        buf[ret+1] = '\0';
        write_log(buf, log);
    } else {
        write_log("too long line!\n", log);
    }
}
