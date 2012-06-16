#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

void test_log(log_t *log) {
    int j;
    for (j = 0; j < 10; ++j) {
        info(log, "hello %d", j);
    }
}
void *test_log2(void *log) {
    int j;
    log = (log_t *)log;
    for (j = 0; j < 10; ++j) {
        error(log, "hello error %d", j);
        debug(log, "hello debug %d", j);
    }
    return NULL;
}

void do_multi_thread(log_t *log) {
    pthread_t pid[10];
    int i;
    for (i = 0; i < 10; ++i) {
        pthread_create(&pid[i], NULL, test_log2, log);
    }
    for (i = 0; i < 10; ++i) {
        pthread_join(pid[i], NULL);
    }
}

void do_multi_process(log_t *log) {
    pid_t pid;
    int i;
    signal(SIGCHLD, SIG_IGN);
    for (i = 0; i < 10; ++i) {
        if ((pid = fork()) < 0) {
        } else if (pid == 0) {
            test_log(log);
            exit(0);
        } else {}
    }
}

void main() {
    log_t *log = create_log("test");
    do_multi_process(log);
    do_multi_thread(log);
    destory_log(log);
}
