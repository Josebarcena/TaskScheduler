#include "task.h"
#include <stdint.h>
#include <time.h>

void cpu_task(void *arg) {
    int n = *(int *)arg;
    volatile uint64_t sum = 0;

    for (int i = 0; i < n * 100000; i++) {
        sum += (uint64_t)(i % 7);
    }
}

void sleep_task(void *arg) {
    int ms = *(int *)arg;

    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;

    nanosleep(&ts, NULL);
}