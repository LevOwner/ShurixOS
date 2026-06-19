#ifndef HSFS_H
#define HSFS_H

#include "../include/shurix.h"

#define HSFS_MAGIC "HSF1"
#define HSFS_MAX_FILES 16
#define HSFS_BLOCK_SIZE 4096

typedef struct {
    uint8_t magic[4];
    uint32_t version;
    uint32_t file_count;
    uint64_t total_space;
    uint64_t free_space;
    uint8_t reserved[500];
} __attribute__((packed)) hsfs_super_t;

typedef struct {
    uint8_t used;
    char name[64];
    uint64_t size;
    uint32_t start_block;
    uint32_t blocks;
    uint8_t reserved[56];
} __attribute__((packed)) hsfs_inode_t;

typedef struct {
    hsfs_super_t super;
    hsfs_inode_t inodes[HSFS_MAX_FILES];
    uint32_t start_sector;
    uint8_t mounted;
} hsfs_t;

int hsfs_format(hsfs_t *fs, uint32_t start_sector);
int hsfs_mount(hsfs_t *fs, uint32_t start_sector);
int hsfs_write(hsfs_t *fs, const char *name, void *data, uint64_t size);
int hsfs_read(hsfs_t *fs, const char *name, void *buffer, uint64_t max_size);
int hsfs_delete(hsfs_t *fs, const char *name);
int hsfs_list(hsfs_t *fs);
void hsfs_test(void);
int hsfs_exists(hsfs_t *fs, const char *name);

#endif