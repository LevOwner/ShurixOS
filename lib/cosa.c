#include "../include/shurix.h"
#include "../fs/hsfs.h"
#include "../ui/menu.h"
#include "../lib/string.h"

#define COSA_MAGIC "COSA"
#define COSA_VERSION 1

typedef struct {
    uint8_t magic[4];
    uint32_t version;
    uint64_t data_size;
} __attribute__((packed)) cosa_header_t;

// Временный буфер для .cosa
static uint8_t cosa_memory[4096];
static uint64_t cosa_mem_size = 0;



int cosa_pack(const void *data, uint64_t size, const char *filename) {
    hsfs_t hsfs;
    hsfs_mount(&hsfs, 0);

    // 1. Создаём буфер для всего файла
    uint8_t *file_buffer = (uint8_t*)0x500000;
    uint64_t buffer_pos = 0;

    // 2. Заголовок
    cosa_header_t header;
    memcpy(header.magic, "COSA", 4);
    header.version = 1;
    header.data_size = size;

    // Копируем заголовок в буфер
    memcpy(file_buffer + buffer_pos, &header, sizeof(cosa_header_t));
    buffer_pos += sizeof(cosa_header_t);

    // 3. Копируем данные в буфер
    memcpy(file_buffer + buffer_pos, data, size);
    buffer_pos += size;

    // 4. Записываем весь буфер в HSFS
    hsfs_write(&hsfs, filename, file_buffer, buffer_pos);

    print("COSA: packed ");
    print_int(size);
    print(" bytes -> ");
    print_int(buffer_pos);
    print(" bytes total\n");
    return 0;
}

int cosa_unpack(const char *filename, void *buffer, uint64_t max_size) {
    hsfs_t hsfs;
    hsfs_mount(&hsfs, 0);

    if (!hsfs_exists(&hsfs, filename)) {
        print("COSA: file not found\n");
        return -1;
    }

    // Читаем весь файл в память
    uint8_t *file_data = (uint8_t*)0x500000;
    int file_size = hsfs_read(&hsfs, filename, file_data, 4096);

    if (file_size < 0) {
        print("COSA: file read failed\n");
        return -1;
    }

    // Заголовок
    cosa_header_t *header = (cosa_header_t*)file_data;
    if (memcmp(header->magic, "COSA", 4) != 0) {
        print("COSA: invalid magic\n");
        return -1;
    }

    // Данные
    uint64_t data_size = header->data_size;
    if (data_size > max_size) data_size = max_size;

    uint8_t *data_ptr = file_data + sizeof(cosa_header_t);
    memcpy(buffer, data_ptr, data_size);

    if (data_size < max_size) {
        ((char*)buffer)[data_size] = '\0';
    }

    print("COSA: unpacked ");
    print_int(data_size);
    print(" bytes\n");
    return data_size;
}