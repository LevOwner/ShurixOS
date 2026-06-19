#include "../ui/menu.h"
#include "../container/container.h"
#include "../fs/cfs.h"
#include "../fs/hsfs.h"
#include "../lib/cosa.h"
#include "../lib/string.h"

void kernel_main() {
    cfs_t cfs;
    hsfs_t hsfs;
    cfs_format(&cfs, 0);
    hsfs_format(&hsfs, 0);

    // ============================================================
    // ТЕСТ HSFS (без .cosa)
    // ============================================================
    char original[] = "SHURIX TEST 123!";
    hsfs_write(&hsfs, "test.txt", original, sizeof(original));

    char buffer[128] = {0};
    int size = hsfs_read(&hsfs, "test.txt", buffer, sizeof(buffer));

    print("Original: ");
    print(original);
    print("\n");

    print("Read: ");
    print(buffer);
    print("\n");

    if (size > 0) {
        print("HEX original: ");
        for (int i = 0; i < sizeof(original) && i < 16; i++) {
            print_int((unsigned char)original[i]);
            print(" ");
        }
        print("\n");

        print("HEX read: ");
        for (int i = 0; i < size && i < 16; i++) {
            print_int((unsigned char)buffer[i]);
            print(" ");
        }
        print("\n");
    }

    // ============================================================
    // ТЕСТ .COSA (с HSFS)
    // ============================================================
    char test_data[] = "Hello SHURIX!";
    cosa_pack(test_data, sizeof(test_data), "test.cosa");

    char cosa_buffer[128] = {0};
    int result = cosa_unpack("test.cosa", cosa_buffer, sizeof(cosa_buffer));

    print("COSA result: ");
    print_int(result);
    print("\n");

    if (result > 0) {
        print("COSA HEX: ");
        for (int i = 0; i < result && i < 16; i++) {
            print_int((unsigned char)cosa_buffer[i]);
            print(" ");
        }
        print("\n");

        print("COSA string: ");
        print(cosa_buffer);
        print("\n");
    } else {
        print("COSA unpack failed!\n");
    }

    // Проверяем, что файл test.cosa существует в HSFS
hsfs_list(&hsfs);

// Читаем test.cosa напрямую (без распаковки)
uint8_t raw_data[256] = {0};
int raw_size = hsfs_read(&hsfs, "test.cosa", raw_data, sizeof(raw_data));
print("Raw test.cosa size: ");
print_int(raw_size);
print("\n");

print("Raw HEX: ");
for (int i = 0; i < raw_size && i < 16; i++) {
    print_int(raw_data[i]);
    print(" ");
}
print("\n");

    // ============================================================
    // КОНТЕЙНЕРЫ
    // ============================================================
    container_create("Ubuntu", 2048, 2);
    container_create("Windows", 4096, 4);
    container_set_default(0);

    // ============================================================
    // МЕНЮ
    // ============================================================
    menu_main();

    while (1) {}
}