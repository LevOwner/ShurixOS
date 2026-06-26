#include "../boot/multiboot.h"
#include "../ui/menu.h"

void kernel_main(uint32_t magic, multiboot_info_t* mbi) {
    // 1. Обязательная проверка магии загрузчика
    if (magic != 0x2BADB002) {
        while(1) { __asm__ volatile("cli; hlt"); }
    }

    // 2. Инициализируем графику напрямую без лишних условий
    init_graphics(mbi);

    // 3. Сразу запускаем UI
    

    // 4. Если вышли из UI (хотя там вечный цикл) — тушим процессор
    while(1) { __asm__ volatile("cli; hlt"); }
}
