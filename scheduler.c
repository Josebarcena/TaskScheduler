
#include "task.h"
#include <stdlib.h>
#include <stdio.h>

static int total_tasks_in_scheduler(scheduler_t *sched) {
    int total = 0;

    for (int i = 0; i < NUM_QUEUES; i++) {
        pthread_mutex_lock(&sched->queues[i].mutex);
        total += sched->queues[i].size;
        pthread_mutex_unlock(&sched->queues[i].mutex);
    }

    return total;
}

static int map_priority_to_queue(int priority) {
    if (priority <= 0) return 0;
    if (priority >= NUM_QUEUES) return NUM_QUEUES - 1;
    return priority;
}

int scheduler_init(scheduler_t *sched, int num_workers, int queue_capacity) {
    sched->num_workers = num_workers;
    sched->shutdown = false;
    sched->workers = malloc(sizeof(pthread_t) * num_workers);

    if (!sched->workers) {
        return -1;
    }

    if (pthread_mutex_init(&sched->mutex, NULL) != 0) {
        free(sched->workers);
        return -1;
    }

    if (pthread_cond_init(&sched->tasks_available, NULL) != 0) {
        pthread_mutex_destroy(&sched->mutex);
        free(sched->workers);
        return -1;
    }

    if (pthread_cond_init(&sched->space_available, NULL) != 0) {
        pthread_cond_destroy(&sched->tasks_available);
        pthread_mutex_destroy(&sched->mutex);
        free(sched->workers);
        return -1;
    }

    for (int i = 0; i < NUM_QUEUES; i++) {
        if (queue_init(&sched->queues[i], queue_capacity, i) != 0) {
            for (int j = 0; j < i; j++) {
                queue_destroy(&sched->queues[j]);
            }
            pthread_cond_destroy(&sched->space_available);
            pthread_cond_destroy(&sched->tasks_available);
            pthread_mutex_destroy(&sched->mutex);
            free(sched->workers);
            return -1;
        }
    }

    for (int i = 0; i < num_workers; i++) {
        if (pthread_create(&sched->workers[i], NULL, worker_routine, sched) != 0) {
            pthread_mutex_lock(&sched->mutex);
            sched->shutdown = true;
            pthread_cond_broadcast(&sched->tasks_available);
            pthread_mutex_unlock(&sched->mutex);

            for (int j = 0; j < i; j++) {
                pthread_join(sched->workers[j], NULL);
            }

            for (int j = 0; j < NUM_QUEUES; j++) {
                queue_destroy(&sched->queues[j]);
            }

            pthread_cond_destroy(&sched->space_available);
            pthread_cond_destroy(&sched->tasks_available);
            pthread_mutex_destroy(&sched->mutex);
            free(sched->workers);
            return -1;
        }
    }

    return 0;
}

int scheduler_submit(scheduler_t *sched, void (*function)(void *), void *arg, int priority, int task_id) {
    task_t task;
    task.function = function;
    task.arg = arg;
    task.priority = priority;
    task.task_id = task_id;

    int qidx = map_priority_to_queue(priority);

    pthread_mutex_lock(&sched->mutex);

    while (!sched->shutdown) {
        pthread_mutex_lock(&sched->queues[qidx].mutex);
        int has_space = (sched->queues[qidx].size < sched->queues[qidx].capacity);
        pthread_mutex_unlock(&sched->queues[qidx].mutex);

        if (has_space) {
            break;
        }

        pthread_cond_wait(&sched->space_available, &sched->mutex);
    }

    if (sched->shutdown) {
        pthread_mutex_unlock(&sched->mutex);
        return -1;
    }

    if (queue_add(&sched->queues[qidx], task) != 0) {
        pthread_mutex_unlock(&sched->mutex);
        return -1;
    }

    pthread_cond_signal(&sched->tasks_available);
    pthread_mutex_unlock(&sched->mutex);

    return 0;
}

void *worker_routine(void *arg) {
    scheduler_t *sched = (scheduler_t *)arg;
    task_t task;

    while (1) {
        int found = 0;

        pthread_mutex_lock(&sched->mutex);

        while (!sched->shutdown && total_tasks_in_scheduler(sched) == 0) {
            pthread_cond_wait(&sched->tasks_available, &sched->mutex);
        }

        if (sched->shutdown && total_tasks_in_scheduler(sched) == 0) {
            pthread_mutex_unlock(&sched->mutex);
            break;
        }

        for (int i = NUM_QUEUES - 1; i >= 0; i--) {
            if (queue_pop(&sched->queues[i], &task) == 0) {
                found = 1;
                pthread_cond_signal(&sched->space_available);
                break;
            }
        }

        pthread_mutex_unlock(&sched->mutex);

        if (found) {
            task.function(task.arg);
        }
    }

    return NULL;
}

void scheduler_destroy(scheduler_t *sched) {
    pthread_mutex_lock(&sched->mutex);
    sched->shutdown = true;
    pthread_cond_broadcast(&sched->tasks_available);
    pthread_cond_broadcast(&sched->space_available);
    pthread_mutex_unlock(&sched->mutex);

    for (int i = 0; i < sched->num_workers; i++) {
        pthread_join(sched->workers[i], NULL);
    }

    for (int i = 0; i < NUM_QUEUES; i++) {
        queue_destroy(&sched->queues[i]);
    }

    free(sched->workers);
    pthread_cond_destroy(&sched->space_available);
    pthread_cond_destroy(&sched->tasks_available);
    pthread_mutex_destroy(&sched->mutex);
}