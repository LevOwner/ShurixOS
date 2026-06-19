#include "process.h"
#include "../ui/menu.h"
#include "../lib/string.h"

static process_t processes[MAX_PROCESSES];
static uint32_t process_count = 0;
static uint32_t next_pid = 1;
static uint32_t current_pid = 0;

// ============================================================
// СОЗДАНИЕ ПРОЦЕССА
// ============================================================

int process_create(const char *name, uint32_t entry, uint32_t priority) {
    if (process_count >= MAX_PROCESSES) {
        print_color_full("Process: max processes reached!\n", 0x0C, 0x01);
        return -1;
    }

    process_t *p = &processes[process_count];
    p->pid = next_pid++;
    strcpy(p->name, name);
    p->state = PROC_RUNNING;
    p->priority = priority;
    p->parent_pid = current_pid;
    p->entry_point = entry;
    p->stack_ptr = 0;
    p->has_threads = 0;
    process_count++;

    print_color_full("Process: created ", 0x0A, 0x01);
    print_color_full(name, 0x0F, 0x01);
    print_color_full(" (PID: ", 0x0F, 0x01);
    print_int(p->pid);
    print_color_full(")\n", 0x0F, 0x01);

    return p->pid;
}

// ============================================================
// УДАЛЕНИЕ ПРОЦЕССА
// ============================================================

int process_kill(uint32_t pid) {
    for (uint32_t i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROC_ZOMBIE;
            print_color_full("Process: killed PID ", 0x0C, 0x01);
            print_int(pid);
            print_color_full("\n", 0x0C, 0x01);
            return 0;
        }
    }
    print_color_full("Process: PID not found\n", 0x0C, 0x01);
    return -1;
}

// ============================================================
// СПИСОК ПРОЦЕССОВ
// ============================================================

int process_list(void) {
    print_color_full("=== PROCESS LIST ===\n", 0x0E, 0x01);
    print_color_full("PID  STATE    PRIORITY  NAME\n", 0x0F, 0x01);

    for (uint32_t i = 0; i < process_count; i++) {
        process_t *p = &processes[i];
        if (p->state == PROC_ZOMBIE) continue;

        print_int(p->pid);
        print_color_full("   ", 0x0F, 0x01);

        switch (p->state) {
            case PROC_RUNNING: print_color_full("RUNNING ", 0x0A, 0x01); break;
            case PROC_SLEEPING: print_color_full("SLEEP   ", 0x0E, 0x01); break;
            case PROC_STOPPED: print_color_full("STOPPED ", 0x08, 0x01); break;
            default: print_color_full("UNKNOWN ", 0x0C, 0x01);
        }

        print_color_full("  ", 0x0F, 0x01);
        print_int(p->priority);
        print_color_full("      ", 0x0F, 0x01);
        print_color_full(p->name, 0x0F, 0x01);
        print_color_full("\n", 0x0F, 0x01);
    }
    return 0;
}

// ============================================================
// СТАТУС ПРОЦЕССА
// ============================================================

int process_status(uint32_t pid) {
    for (uint32_t i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            process_t *p = &processes[i];
            print_color_full("=== PROCESS STATUS ===\n", 0x0E, 0x01);
            print_color_full("Name:  ", 0x0F, 0x01);
            print_color_full(p->name, 0x0F, 0x01);
            print_color_full("\n", 0x0F, 0x01);
            print_color_full("PID:   ", 0x0F, 0x01);
            print_int(p->pid);
            print_color_full("\n", 0x0F, 0x01);
            print_color_full("State: ", 0x0F, 0x01);
            switch (p->state) {
                case PROC_RUNNING: print_color_full("RUNNING\n", 0x0A, 0x01); break;
                case PROC_SLEEPING: print_color_full("SLEEP\n", 0x0E, 0x01); break;
                case PROC_STOPPED: print_color_full("STOPPED\n", 0x08, 0x01); break;
                default: print_color_full("UNKNOWN\n", 0x0C, 0x01);
            }
            print_color_full("Priority: ", 0x0F, 0x01);
            print_int(p->priority);
            print_color_full("\n", 0x0F, 0x01);
            return 0;
        }
    }
    print_color_full("Process not found\n", 0x0C, 0x01);
    return -1;
}