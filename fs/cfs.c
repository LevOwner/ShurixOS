#include "cfs.h"
#include "../lib/string.h"
#include "../ui/menu.h"

static uint8_t cfs_data[1024 * 512];

int cfs_format(cfs_t *fs, uint32_t start_sector) {
    (void)start_sector;
    fs->start_sector = 0;
    fs->mounted = 1;

    cfs_super_t *s = &fs->super;
    memcpy(s->magic, CFS_MAGIC, 4);
    s->version = 1;
    s->file_count = 0;

    memset(cfs_data, 0, sizeof(cfs_data));
    print_color_full("CFS: formatted (in-memory)\n", 0x0A, 0x01);
    return 0;
}

int cfs_mount(cfs_t *fs, uint32_t start_sector) {
    (void)start_sector;
    fs->start_sector = 0;
    fs->mounted = 1;

    memcpy(&fs->super, cfs_data, sizeof(cfs_super_t));
    if (memcmp(fs->super.magic, CFS_MAGIC, 4) != 0) {
        cfs_format(fs, 0);
    }
    memcpy(fs->inodes, cfs_data + sizeof(cfs_super_t), sizeof(fs->inodes));
    print_color_full("CFS: mounted\n", 0x0A, 0x01);
    return 0;
}

int cfs_write(cfs_t *fs, const char *name, void *data, uint32_t size) {
    if (!fs->mounted) return -1;

    for (int i = 0; i < CFS_MAX_FILES; i++) {
        if (!fs->inodes[i].used) {
            fs->inodes[i].used = 1;
            strcpy(fs->inodes[i].name, name);
            fs->inodes[i].size = size;
            fs->inodes[i].start_block = i;
            fs->inodes[i].blocks = 1;

            uint32_t offset = sizeof(cfs_super_t) + sizeof(cfs_inode_t) * CFS_MAX_FILES;
            memcpy(cfs_data + offset + i * CFS_BLOCK_SIZE, data, size);

            fs->super.file_count++;
            memcpy(cfs_data, &fs->super, sizeof(cfs_super_t));
            memcpy(cfs_data + sizeof(cfs_super_t), fs->inodes, sizeof(fs->inodes));
            return 0;
        }
    }
    print_color_full("CFS: no free inodes!\n", 0x0C, 0x01);
    return -1;
}

int cfs_read(cfs_t *fs, const char *name, void *buffer, uint32_t max_size) {
    if (!fs->mounted) return -1;

    for (int i = 0; i < CFS_MAX_FILES; i++) {
        if (fs->inodes[i].used && strcmp(fs->inodes[i].name, name) == 0) {
            uint32_t offset = sizeof(cfs_super_t) + sizeof(cfs_inode_t) * CFS_MAX_FILES;
            uint32_t size = fs->inodes[i].size;
            if (size > max_size) size = max_size;
            memcpy(buffer, cfs_data + offset + i * CFS_BLOCK_SIZE, size);
            return size;
        }
    }
    return -1;
}

int cfs_delete(cfs_t *fs, const char *name) {
    if (!fs->mounted) return -1;

    for (int i = 0; i < CFS_MAX_FILES; i++) {
        if (fs->inodes[i].used && strcmp(fs->inodes[i].name, name) == 0) {
            fs->inodes[i].used = 0;
            fs->super.file_count--;
            memcpy(cfs_data, &fs->super, sizeof(cfs_super_t));
            memcpy(cfs_data + sizeof(cfs_super_t), fs->inodes, sizeof(fs->inodes));
            return 0;
        }
    }
    return -1;
}

int cfs_list(cfs_t *fs) {
    if (!fs->mounted) return -1;

    print_color_full("=== CFS FILES ===\n", 0x0E, 0x01);
    for (int i = 0; i < CFS_MAX_FILES; i++) {
        if (fs->inodes[i].used) {
            print("  ");
            print(fs->inodes[i].name);
            print(" (");
            print_int(fs->inodes[i].size);
            print(" bytes)\n");
        }
    }
    return 0;
}

void cfs_test(void) {
    cfs_t fs;
    cfs_format(&fs, 0);

    char test_data[] = "Hello from CFS!";
    cfs_write(&fs, "test.txt", test_data, sizeof(test_data));

    char buffer[64] = {0};
    cfs_read(&fs, "test.txt", buffer, sizeof(buffer));
    print_color_full("CFS test read: ", 0x0F, 0x01);
    print(buffer);
    print("\n");

    cfs_list(&fs);
}