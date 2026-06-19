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
#include "../kernel/process.h"
#include "../kernel/thread.h"
#include "../ss/denitron.h"

#define RAM_SIZE_128MB          (128 * 1024 * 1024)
#define KERNEL_PHYS_MAP_OFFSET  0xC0000000

void shurix_init(void) {
    // ============================================================
    // 1. БАЗОВЫЕ СИСТЕМЫ
    // ============================================================
    print_color_full("SHURIXOS: initializing...\n", 0x0F, 0x01);

    cfs_t cfs;
    hsfs_t hsfs;
    cfs_format(&cfs, 0);
    hsfs_format(&hsfs, 0);
    print_color_full("SHURIXOS: filesystems ready\n", 0x0A, 0x01);

    // ============================================================
    // 2. БЕЗОПАСНОСТЬ (CRYPTO)
    // ============================================================
    print_color_full("SHURIXOS: initializing security...\n", 0x0F, 0x01);

    print_color_full("Enter disk password: ", 0x0F, 0x01);
    char password[32];
    int i = 0;
    while (1) {
        char c = getchar();
        if (c == 13) { password[i] = '\0'; break; }
        if (c == 8) {
            if (i > 0) {
                i--;
                vga_putchar(8, 0x0F);
                vga_putchar(' ', 0x0F);
                vga_putchar(8, 0x0F);
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

    // ============================================================
    // 3. ПРОЦЕССЫ И ПОТОКИ
    // ============================================================

    process_create("kernel", 0, 10);
    process_create("init", 0, 5);
    process_create("shurix", 0, 8);

    thread_create(1, "kernel_thread", 0);
    thread_create(2, "init_thread", 0);
    thread_create(3, "shurix_thread", 0);


    


    // ============================================================
    // 4. КОНТЕЙНЕРЫ
    // ============================================================
    container_create("Ubuntu", 2048, 2);
    container_create("Windows", 4096, 4);
    container_set_default(0);
    print_color_full("Containers: ready\n", 0x0A, 0x01);

    // ============================================================
    // 5. ВИРТУАЛИЗАЦИЯ (KVM / EMU)
    // ============================================================
    if (vm_check_kvm()) {
        print_color_full("KVM: available\n", 0x0A, 0x01);
        vm_set_mode(VM_MODE_KVM);
    } else {
        print_color_full("KVM: not available, using emulation\n", 0x0E, 0x01);
        vm_set_mode(VM_MODE_EMU);
    }
    vm_init();

    // ============================================================
    // 6. МЕНЮ
    // ============================================================
    print_color_full("SHURIXOS: ready\n", 0x0A, 0x01);
    

    while (1) {
        __asm__ volatile ("hlt");
    }
}