#ifndef THREAD_H
#define THREAD_H

#include "../include/shurix.h"

#define MAX_THREADS 256

typedef struct {
    uint32_t tid;
    uint32_t pid;
    char name[32];
    uint32_t state;
    uint32_t stack_ptr;
    uint32_t entry_point;
} thread_t;

int thread_create(uint32_t pid, const char *name, uint32_t entry);
int thread_kill(uint32_t tid);
int thread_list(uint32_t pid);

#endif