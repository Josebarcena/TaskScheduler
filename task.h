#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <stdbool.h>

#define NUM_QUEUES 3

typedef struct task_t {
    void (*function)(void *);
    void *arg;
    int priority;
    int task_id;
} task_t;

typedef struct Node {
    task_t task;
    struct Node *next;
} Node;

typedef Node *TaskList;

typedef struct task_queue_t {
    TaskList tasks;
    int idx;
    int capacity;
    int size;

    pthread_mutex_t mutex;
} task_queue_t;

typedef struct scheduler_t {
    task_queue_t queues[NUM_QUEUES];
    pthread_t *workers;
    int num_workers;

    bool shutdown;

    pthread_mutex_t mutex;
    pthread_cond_t tasks_available;
    pthread_cond_t space_available;
} scheduler_t;

/* queue.c */
int queue_init(task_queue_t *q, int capacity, int idx);
int queue_add(task_queue_t *q, task_t task);
int queue_pop(task_queue_t *q, task_t *task);
void queue_destroy(task_queue_t *q);

/* scheduler.c */
int scheduler_init(scheduler_t *sched, int num_workers, int queue_capacity);
int scheduler_submit(scheduler_t *sched, void (*function)(void *), void *arg, int priority, int task_id);
void *worker_routine(void *arg);
void scheduler_destroy(scheduler_t *sched);

/* test.c */
void cpu_task(void *arg);
void sleep_task(void *arg);
#endif