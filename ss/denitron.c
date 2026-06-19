#include "denitron.h"
#include "../ui/menu.h"
#include "../lib/string.h"

#define MAX_PROCESSES 256

typedef struct {
    uint32_t pid;
    char name[32];
    char destination[64];
    char intention[128];
    uint8_t approved;
} denitron_process_t;

static denitron_process_t processes[MAX_PROCESSES];
static uint32_t process_count = 0;
static uint32_t threats_destroyed = 0;

int denitron_validate(const char *name, const char *dest, const char *intent) {
    // Проверка на пустые поля
    if (strlen(dest) == 0 || strlen(intent) == 0) {
        return 0;
    }

    // Проверка на запрещённые намерения
    if (strstr(intent, "delete") || strstr(intent, "format") || strstr(intent, "encrypt")) {
        return 0;
    }

    return 1;
}

void denitron_register(uint32_t pid, const char *name, const char *dest, const char *intent) {
    if (process_count >= MAX_PROCESSES) return;

    processes[process_count].pid = pid;
    strcpy(processes[process_count].name, name);
    strcpy(processes[process_count].destination, dest);
    strcpy(processes[process_count].intention, intent);
    processes[process_count].approved = denitron_validate(name, dest, intent);
    process_count++;
}

void denitron_destroy(uint32_t pid) {
    print_color_full("Denitron: DESTROYING threat (PID: ", 0x0C, 0x01);
    print_int(pid);
    print_color_full(")\n", 0x0C, 0x01);
    threats_destroyed++;
}

void denitron_scan_all(void) {
    print_color_full("Denitron: scanning all processes...\n", 0x0F, 0x01);
    for (uint32_t i = 0; i < process_count; i++) {
        if (processes[i].approved == 0) {
            denitron_destroy(processes[i].pid);
        }
    }
}

void denitron_status(void) {
    print_color_full("=== DENITRON STATUS ===\n", 0x0E, 0x01);
    print_color_full("Processes registered: ", 0x0F, 0x01);
    print_int(process_count);
    print_color_full("\n", 0x0F, 0x01);
    print_color_full("Threats destroyed: ", 0x0F, 0x01);
    print_int(threats_destroyed);
    print_color_full("\n", 0x0F, 0x01);
    print_color_full("Status: ACTIVE\n", 0x0A, 0x01);
}