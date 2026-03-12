#include "task.h"
#include <stdlib.h>
#include <stdio.h>

int queue_init(task_queue_t *q, int capacity, int idx) {
    q->tasks = NULL;
    q->idx = idx;
    q->capacity = capacity;
    q->size = 0;

    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        return -1;
    }

    return 0;
}

int queue_add(task_queue_t *q, task_t task) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        return -1;
    }

    new_node->task = task;
    new_node->next = NULL;

    pthread_mutex_lock(&q->mutex);

    if (q->size >= q->capacity) {
        pthread_mutex_unlock(&q->mutex);
        free(new_node);
        return -1;
    }

    if (q->tasks == NULL) {
        q->tasks = new_node;
    } else {
        Node *curr = q->tasks;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new_node;
    }

    q->size++;

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int queue_pop(task_queue_t *q, task_t *task) {
    pthread_mutex_lock(&q->mutex);

    if (q->size == 0 || q->tasks == NULL) {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }

    Node *first = q->tasks;
    *task = first->task;
    q->tasks = first->next;
    q->size--;

    pthread_mutex_unlock(&q->mutex);

    free(first);
    return 0;
}

void queue_destroy(task_queue_t *q) {
    pthread_mutex_lock(&q->mutex);

    Node *curr = q->tasks;
    while (curr != NULL) {
        Node *tmp = curr;
        curr = curr->next;
        free(tmp);
    }

    q->tasks = NULL;
    q->size = 0;

    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
}