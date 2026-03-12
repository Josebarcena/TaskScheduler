
# Concurrent Task Scheduler (C / pthreads)

A multi-threaded task scheduler implemented in **C** using **POSIX threads (`pthreads`)**.  
The system follows a **producer–consumer architecture** with multiple priority queues and a pool of worker threads that execute tasks concurrently.

This project demonstrates key concepts of **concurrent systems programming**, including thread synchronization, task scheduling, and scalability analysis.

---

# Overview

The scheduler manages a set of worker threads responsible for executing tasks submitted by producers.

Tasks are stored in **bounded queues** according to their **priority level**. Workers continuously monitor these queues and retrieve tasks to execute as soon as they become available.

The system ensures **thread-safe access to shared resources** through mutexes and condition variables.

---

# Features

- Thread pool with configurable number of workers
- Multiple priority queues
- Producer–consumer architecture
- Thread-safe task submission
- Concurrent task execution
- Synchronization using `pthread_mutex` and `pthread_cond`
- Bounded queues with configurable capacity
- Performance benchmarking (execution time and throughput)

---

# Architecture

The system is composed of three main components.

## Task

A task represents a unit of work and contains:

- a function pointer to execute
- a generic argument (`void *`)
- a priority level
- a task identifier

Example structure:

```c
typedef struct task_t {
    void (*function)(void *);
    void *arg;
    int priority;
    int task_id;
} task_t;
```

---

## Task Queue

Each queue stores tasks using a **linked list**.  
Queues are protected by a **mutex** to guarantee safe concurrent access.

Properties:

- bounded capacity
- FIFO ordering inside each queue
- thread-safe insertion and removal

Each queue is associated with a specific **priority level**.

---

## Scheduler

The scheduler manages:

- multiple priority queues
- a pool of worker threads
- synchronization mechanisms
- task submission
- system shutdown

Workers always attempt to retrieve tasks from the **highest priority queue available**.

---

# Execution Flow

1. The scheduler initializes worker threads and task queues.
2. Producers submit tasks with a given priority.
3. The task is inserted into the corresponding queue.
4. Worker threads wait for tasks to become available.
5. A worker retrieves a task from the highest-priority non-empty queue.
6. The worker executes the task function.
7. The process repeats until the scheduler is shut down.

---

# Example Output

Example run:

Workers: 15
Queues: 3
Tasks: 100
Execution time: 0.0814 sec
Throughput: 1228.28 tasks/sec

---

# Performance

The scheduler demonstrates good scalability as the number of worker threads increases.

Example benchmark results:

| Workers | Execution Time (s) | Throughput (tasks/sec) |
|--------:|-------------------:|-----------------------:|
| 1 | 0.7929 | 126.12 |
| 4 | 0.2035 | 491.34 |
| 8 | 0.1212 | 824.80 |
| 10 | 0.0951 | 1051.26 |
| 15 | 0.0814 | 1228.28 |

Increasing the number of workers reduces execution time and improves throughput, although improvements become sublinear due to synchronization overhead and contention.

---

# Technologies

- **C (C11)**
- **POSIX Threads (`pthread`)**
- Mutexes and condition variables
- Concurrent data structures

---

# Build

Compile the project with:

make

---

# Run

Execute the scheduler:

./scheduler

---

# Project Goals

This project explores fundamental concepts in concurrent systems programming:

- thread synchronization
- producer–consumer patterns
- concurrent queue design
- task scheduling
- scalability analysis
- performance benchmarking

---

# License

This project is released for educational purposes.
