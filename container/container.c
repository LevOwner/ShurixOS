#include "container.h"
#include "../ui/menu.h"
#include "../lib/string.h"

container_t containers[MAX_CONTAINERS];
int container_count = 0;

int container_create(const char *name, uint32_t ram, uint8_t cores) {
    if (container_count >= MAX_CONTAINERS) return -1;
    container_t *c = &containers[container_count];
    strcpy(c->name, name);
    c->ram_mb = ram;
    c->cpu_cores = cores;
    c->state = 0;
    c->is_default = 0;
    strcpy(c->image_path, "none");
    container_count++;
    return 0;
}

int container_delete(int index) {
    if (index < 0 || index >= container_count) return -1;
    for (int i = index; i < container_count - 1; i++) {
        containers[i] = containers[i + 1];
    }
    container_count--;
    return 0;
}

int container_list(void) {
    print("=== CONTAINERS ===\n");
    for (int i = 0; i < container_count; i++) {
        print("  ");
        print_int(i + 1);
        print(". ");
        print(containers[i].name);
        print(" [");
        print(containers[i].state ? "RUN" : "STOP");
        print("]");
        if (containers[i].is_default) print(" (DEFAULT)");
        print("\n");
    }
    return 0;
}

int container_set_default(int index) {
    if (index < 0 || index >= container_count) return -1;
    for (int i = 0; i < container_count; i++) {
        containers[i].is_default = 0;
    }
    containers[index].is_default = 1;
    return 0;
}

void container_print_status(int index) {
    if (index < 0 || index >= container_count) {
        print("ERROR: Invalid container index!\n");
        return;
    }
    container_t *c = &containers[index];
    print("=== CONTAINER STATUS ===\n");
    print("Name:   "); print(c->name); print("\n");
    print("RAM:    "); print_int(c->ram_mb); print(" MB\n");
    print("CPU:    "); print_int(c->cpu_cores); print(" core(s)\n");
    print("State:  "); print(c->state ? "RUNNING" : "STOPPED"); print("\n");
    print("Default: "); print(c->is_default ? "YES" : "NO"); print("\n");
    print("Image:  "); print(c->image_path); print("\n");
}

int container_start(int index) {
    if (index < 0 || index >= container_count) return -1;
    if (containers[index].state == 1) {
        print("Container is already running!\n");
        return -1;
    }
    print("Starting container '");
    print(containers[index].name);
    print("' (simulated, no hypervisor yet)\n");
    containers[index].state = 1;
    return 0;
}

int container_stop(int index) {
    if (index < 0 || index >= container_count) return -1;
    if (containers[index].state == 0) {
        print("Container is already stopped!\n");
        return -1;
    }
    print("Stopping container '");
    print(containers[index].name);
    print("'\n");
    containers[index].state = 0;
    return 0;
}