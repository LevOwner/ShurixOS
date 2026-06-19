#include "thread.h"
#include "process.h"        // ← ДОБАВЛЯЕМ ЭТУ СТРОКУ
#include "../ui/menu.h"
#include "../lib/string.h"

static thread_t threads[MAX_THREADS];
static uint32_t thread_count = 0;
static uint32_t next_tid = 1;

int thread_create(uint32_t pid, const char *name, uint32_t entry) {
    if (thread_count >= MAX_THREADS) {
        print_color_full("Thread: max threads reached\n", 0x0C, 0x01);
        return -1;
    }

    thread_t *t = &threads[thread_count];
    t->tid = next_tid++;
    t->pid = pid;
    strcpy(t->name, name);
    t->state = PROC_RUNNING;
    t->entry_point = entry;
    t->stack_ptr = 0;
    thread_count++;

    print_color_full("Thread: created ", 0x0A, 0x01);
    print_color_full(name, 0x0F, 0x01);
    print_color_full(" (TID: ", 0x0F, 0x01);
    print_int(t->tid);
    print_color_full(")\n", 0x0F, 0x01);

    return t->tid;
}

int thread_kill(uint32_t tid) {
    for (uint32_t i = 0; i < thread_count; i++) {
        if (threads[i].tid == tid) {
            threads[i].state = PROC_ZOMBIE;
            print_color_full("Thread: killed TID ", 0x0C, 0x01);
            print_int(tid);
            print_color_full("\n", 0x0C, 0x01);
            return 0;
        }
    }
    return -1;
}

int thread_list(uint32_t pid) {
    print_color_full("=== THREAD LIST (PID ", 0x0E, 0x01);
    print_int(pid);
    print_color_full(") ===\n", 0x0E, 0x01);
    print_color_full("TID   STATE    NAME\n", 0x0F, 0x01);

    for (uint32_t i = 0; i < thread_count; i++) {
        if (threads[i].pid != pid) continue;
        if (threads[i].state == PROC_ZOMBIE) continue;

        print_int(threads[i].tid);
        print_color_full("   ", 0x0F, 0x01);
        switch (threads[i].state) {
            case PROC_RUNNING: print_color_full("RUNNING ", 0x0A, 0x01); break;
            case PROC_SLEEPING: print_color_full("SLEEP   ", 0x0E, 0x01); break;
            default: print_color_full("UNKNOWN ", 0x0C, 0x01);
        }
        print_color_full("  ", 0x0F, 0x01);
        print_color_full(threads[i].name, 0x0F, 0x01);
        print_color_full("\n", 0x0F, 0x01);
    }
    return 0;
}