#ifndef CFS_H
#define CFS_H

#include "../include/shurix.h"

#define CFS_MAGIC "CFS1"
#define CFS_MAX_FILES 16
#define CFS_BLOCK_SIZE 512

typedef struct {
    uint8_t magic[4];
    uint32_t version;
    uint32_t file_count;
    uint8_t reserved[512 - 12];
} __attribute__((packed)) cfs_super_t;

typedef struct {
    uint8_t used;
    char name[32];
    uint32_t size;
    uint32_t start_block;
    uint32_t blocks;
    uint8_t reserved[56];
} __attribute__((packed)) cfs_inode_t;

typedef struct {
    cfs_super_t super;
    cfs_inode_t inodes[CFS_MAX_FILES];
    uint32_t start_sector;
    uint8_t mounted;
} cfs_t;

int cfs_format(cfs_t *fs, uint32_t start_sector);
int cfs_mount(cfs_t *fs, uint32_t start_sector);
int cfs_write(cfs_t *fs, const char *name, void *data, uint32_t size);
int cfs_read(cfs_t *fs, const char *name, void *buffer, uint32_t max_size);
int cfs_delete(cfs_t *fs, const char *name);
int cfs_list(cfs_t *fs);
void cfs_test(void);

#endif