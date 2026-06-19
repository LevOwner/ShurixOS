#include "../ui/menu.h"
#include "../container/container.h"
#include "../fs/cfs.h"
#include "../fs/hsfs.h"
#include "../lib/cosa.h"
#include "../lib/string.h"
#include "../virt/vm.h"
#include "../drivers/kafos.h"
#include "../drivers/keyboard.h"
#include "../ss/crypto.h"
#include "../ss/denitron.h"

// Внешние переменные из menu.c (для управления курсором)
extern int cursor_x;
extern int cursor_y;

// Функция удаления символа (из menu.c)
extern void vga_delete_char(void);

void kernel_main() {
    // ============================================================
    // 1. ИНИЦИАЛИЗАЦИЯ ФАЙЛОВЫХ СИСТЕМ
    // ============================================================
    cfs_t cfs;
    hsfs_t hsfs;
    cfs_format(&cfs, 0);
    hsfs_format(&hsfs, 0);

    // ============================================================
    // 2. ИНИЦИАЛИЗАЦИЯ БЕЗОПАСНОСТИ (CRYPTO + DENITRON)
    // ============================================================
    print_color_full("SHURIXOS: initializing security...\n", 0x0F, 0x01);

    // Запрос пароля для шифрования диска (с поддержкой Backspace)
    print_color_full("Enter disk password: ", 0x0F, 0x01);
    char password[32];
    int i = 0;
    while (1) {
        char c = getchar();

        if (c == 13) { // Enter
            password[i] = '\0';
            break;
        }

        if (c == 8) { // Backspace
            if (i > 0) {
                i--;
                vga_delete_char();
            }
            continue;
        }

        if (i < 31) {
            password[i++] = c;
            vga_putchar('*', 0x0F);
        }
    }
    vga_putchar('\n', 0x0F);

    crypto_derive_key(password);
    print_color_full("Crypto: AES-256 key derived\n", 0x0A, 0x01);

    // Инициализация Denitron (агенты безопасности)
    denitron_register(0, "kernel", "system", "core");
    denitron_register(1, "init", "system", "boot");
    denitron_register(2, "shurix", "system", "hypervisor");
    print_color_full("Denitron: security agents active\n", 0x0A, 0x01);

    // ============================================================
    // 3. ТЕСТОВЫЙ СКАНИРОВАНИЕ DENITRON
    // ============================================================
    denitron_scan_all();
    denitron_status();

    // ============================================================
    // 4. ТЕСТ .COSA (УПАКОВКА / РАСПАКОВКА)
    // ============================================================
    char test_data[] = "Hello SHURIX!";
    cosa_pack(test_data, sizeof(test_data), "test.cosa");

    char buffer[128] = {0};
    int result = cosa_unpack("test.cosa", buffer, sizeof(buffer));

    print_color_full("COSA test: ", 0x0F, 0x01);
    if (result > 0) {
        print_color_full(buffer, 0x0A, 0x01);
    } else {
        print_color_full("FAILED\n", 0x0C, 0x01);
    }
    print_color_full("\n", 0x0F, 0x01);

    // ============================================================
    // 5. ВИРТУАЛИЗАЦИЯ (KVM / EMU)
    // ============================================================
    if (vm_check_kvm()) {
        print_color_full("KVM available\n", 0x0A, 0x01);
        vm_set_mode(VM_MODE_KVM);
    } else {
        print_color_full("KVM not available, using emulation\n", 0x0E, 0x01);
        vm_set_mode(VM_MODE_EMU);
    }
    vm_init();

    // ============================================================
    // 6. КОНТЕЙНЕРЫ
    // ============================================================
    container_create("Ubuntu", 2048, 2);
    container_create("Windows", 4096, 4);
    container_set_default(0);

    // ============================================================
    // 7. МЕНЮ
    // ============================================================
    

    while (1) {
        __asm__ volatile ("hlt");
    }
}