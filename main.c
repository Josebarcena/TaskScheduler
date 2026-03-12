#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int test(int workers, int queue_capacity, int num_tasks, int workload) {
    srand(time(NULL));

    scheduler_t sched;
    int num_workers = workers;
    int capacity = queue_capacity;
    int tasks = num_tasks;
    int load = workload;

    if (scheduler_init(&sched, num_workers, capacity) != 0) {
        fprintf(stderr, "Error inicializando scheduler\n");
        return 1;
    }

    double start = now_sec();

    for (int i = 0; i < tasks; i++) {
        int priority = rand() % NUM_QUEUES;  /* 0, 1, 2 */

        if (scheduler_submit(&sched, cpu_task, &load, priority, i) != 0) {
            fprintf(stderr, "Error enviando tarea %d\n", i);
        }
    }

    scheduler_destroy(&sched);

    double end = now_sec();

    printf("Workers: %d\n", workers);
    printf("Queues: %d\n", NUM_QUEUES);
    printf("Tasks: %d\n", tasks);
    printf("Execution time: %.4f sec\n", end - start);
    printf("Throughput: %.2f tasks/sec\n", tasks / (end - start));

    return 0;
}

int main() {
    int workers_max = 16;
    for(int i = 1; i < workers_max; i++) {

        int workers = i;
        int queue_capacity = 10;
        int num_tasks = 100;
        int workload = 100;

        test(workers, queue_capacity, num_tasks, workload);
    }
    return 0;
}