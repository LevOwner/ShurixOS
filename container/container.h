#ifndef CONTAINER_H
#define CONTAINER_H

#include "../include/shurix.h"

typedef struct {
    char name[CONTAINER_NAME_LEN];
    uint32_t ram_mb;
    uint8_t cpu_cores;
    uint8_t state;          // 0 = stopped, 1 = running
    uint8_t is_default;
    char image_path[64];
    void *vm_handle;        // для KVM (позже)
} container_t;

extern container_t containers[MAX_CONTAINERS];
extern int container_count;

int container_create(const char *name, uint32_t ram, uint8_t cores);
int container_delete(int index);
int container_list(void);
int container_set_default(int index);
void container_print_status(int index);
int container_start(int index);   // ← НОВОЕ
int container_stop(int index);    // ← НОВОЕ

#endif