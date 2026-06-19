#ifndef PROCESS_H
#define PROCESS_H

#include "../include/shurix.h"

#define MAX_PROCESSES 64
#define PROCESS_NAME_LEN 32

// Состояния процесса
#define PROC_RUNNING    0
#define PROC_SLEEPING   1
#define PROC_ZOMBIE     2
#define PROC_STOPPED    3

typedef struct {
    uint32_t pid;
    char name[PROCESS_NAME_LEN];
    uint32_t state;
    uint32_t priority;
    uint32_t parent_pid;
    uint32_t stack_ptr;
    uint32_t entry_point;
    uint8_t has_threads;
} process_t;

int process_create(const char *name, uint32_t entry, uint32_t priority);
int process_kill(uint32_t pid);
int process_list(void);
int process_status(uint32_t pid);
int process_sleep(uint32_t pid);
int process_wake(uint32_t pid);
void process_scheduler(void);

#endif