#include "hsfs.h"
#include "../lib/string.h"
#include "../ui/menu.h"

#define HSFS_MAX_FILES 16
#define HSFS_BLOCK_SIZE 512

static uint8_t hsfs_data[HSFS_MAX_FILES][HSFS_BLOCK_SIZE];
static char hsfs_names[HSFS_MAX_FILES][32];
static uint8_t hsfs_used[HSFS_MAX_FILES];
static int hsfs_file_count = 0;

int hsfs_format(hsfs_t *fs, uint32_t start_sector) {
    (void)start_sector;
    fs->mounted = 1;
    memset(hsfs_data, 0, sizeof(hsfs_data));
    memset(hsfs_names, 0, sizeof(hsfs_names));
    memset(hsfs_used, 0, sizeof(hsfs_used));
    hsfs_file_count = 0;
    print_color_full("HSFS: formatted\n", 0x0A, 0x01);
    return 0;
}

int hsfs_mount(hsfs_t *fs, uint32_t start_sector) {
    (void)start_sector;
    fs->mounted = 1;
    print_color_full("HSFS: mounted\n", 0x0A, 0x01);
    return 0;
}

int hsfs_write(hsfs_t *fs, const char *name, void *data, uint64_t size) {
    if (!fs->mounted) return -1;
    if (size > HSFS_BLOCK_SIZE) {
        print_color_full("HSFS: file too large\n", 0x0C, 0x01);
        return -1;
    }

    for (int i = 0; i < HSFS_MAX_FILES; i++) {
        if (!hsfs_used[i]) {
            hsfs_used[i] = 1;
            strcpy(hsfs_names[i], name);
            memcpy(hsfs_data[i], data, (size_t)size);
            hsfs_file_count++;
            print_color_full("HSFS: wrote ", 0x0A, 0x01);
            print_int(size);
            print_color_full(" bytes to ", 0x0A, 0x01);
            print(name);
            print("\n");
            return 0;
        }
    }
    print_color_full("HSFS: no free slots\n", 0x0C, 0x01);
    return -1;
}

int hsfs_read(hsfs_t *fs, const char *name, void *buffer, uint64_t max_size) {
    if (!fs->mounted) return -1;

    for (int i = 0; i < HSFS_MAX_FILES; i++) {
        if (hsfs_used[i] && strcmp(hsfs_names[i], name) == 0) {
            uint64_t size = HSFS_BLOCK_SIZE;
            if (size > max_size) size = max_size;
            memcpy(buffer, hsfs_data[i], (size_t)size);
            if (size < max_size) {
                ((char*)buffer)[size] = '\0';
            }
            print_color_full("HSFS: read ", 0x0A, 0x01);
            print_int(size);
            print_color_full(" bytes from ", 0x0A, 0x01);
            print(name);
            print("\n");
            return (int)size;
        }
    }
    print_color_full("HSFS: file not found: ", 0x0C, 0x01);
    print(name);
    print("\n");
    return -1;
}

int hsfs_delete(hsfs_t *fs, const char *name) {
    if (!fs->mounted) return -1;

    for (int i = 0; i < HSFS_MAX_FILES; i++) {
        if (hsfs_used[i] && strcmp(hsfs_names[i], name) == 0) {
            hsfs_used[i] = 0;
            hsfs_file_count--;
            print_color_full("HSFS: deleted ", 0x0A, 0x01);
            print(name);
            print("\n");
            return 0;
        }
    }
    return -1;
}

int hsfs_list(hsfs_t *fs) {
    if (!fs->mounted) return -1;

    print_color_full("=== HSFS FILES ===\n", 0x0E, 0x01);
    int count = 0;
    for (int i = 0; i < HSFS_MAX_FILES; i++) {
        if (hsfs_used[i]) {
            print("  ");
            print(hsfs_names[i]);
            print(" (");
            print_int(i);
            print(" bytes)\n");
            count++;
        }
    }
    if (count == 0) {
        print("  (empty)\n");
    }
    return 0;
}

int hsfs_exists(hsfs_t *fs, const char *name) {
    if (!fs->mounted) return 0;
    for (int i = 0; i < HSFS_MAX_FILES; i++) {
        if (hsfs_used[i] && strcmp(hsfs_names[i], name) == 0) {
            return 1;
        }
    }
    return 0;
}