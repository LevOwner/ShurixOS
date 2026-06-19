#include "../lib/string.h"
#include "../ui/menu.h"

// 32-битное смещение физической памяти ядра (типичное для x86 архитектур)
#define KERNEL_PHYS_MAP_OFFSET  0xC0000000
#define PAGE_SIZE               4096

// Внешняя функция вашей ОС для вывода логов
extern void kernel_log(const char* fmt, ...);

/**
 * Автономный встроенный сканер физической памяти для 32-битной SHURIXOS.
 * 
 * @param ram_size_bytes - Общий объем RAM (для 32-бит достаточно unsigned int)
 * @param signature      - Указатель на сигнатуру, которую нужно найти
 * @param sig_len        - Длина сигнатуры (тип size_t из вашей библиотеки)
 */
size_t denitron_ray(unsigned int ram_size_bytes, const unsigned char* signature, size_t sig_len) {
    

    if (sig_len == 0 || sig_len > PAGE_SIZE || signature == 0) {
        print("ERORR: Scan not ready");
        return 0;
    }

    // Постраничный перебор всей физической памяти системы
    for (unsigned int phys_page = 0; phys_page < ram_size_bytes; phys_page += PAGE_SIZE) {
        
        // Преобразуем физический адрес в виртуальный адрес ядра (32-битный указатель)
        unsigned char* page_data = (unsigned char*)(phys_page + KERNEL_PHYS_MAP_OFFSET);

        // Безопасный предел внутри страницы, чтобы memcmp не вышел за границы
        size_t limit = PAGE_SIZE - sig_len;
        
        for (size_t offset = 0; offset < limit; offset++) {
            
            // Вызов вашей функции memcmp для побайтового сравнения памяти
            if (memcmp(&page_data[offset], signature, sig_len) == 0) {
                
                unsigned int exact_phys_addr = phys_page + offset;
                
                
                // Вызов вашей функции memset для зануления кода угрозы
                memset(&page_data[offset], 0, sig_len);
                
                
                
                // Пропускаем уже проверенный и очищенный участок памяти
                offset += (sig_len - 1);
            }
        }
    }
    
}
void denitron_scan_process_list(unsigned int ram_size_bytes, const char* proc_name) {
    size_t name_len = strlen(proc_name);
    if (name_len == 0 || name_len > 32) return;

    // ОПТИМИЗАЦИЯ 1: Пропускаем первый мегабайт (0x0 - 0x100000). 
    // Там находятся таблицы прерываний BIOS, видеопамять VGA и MMIO, 
    // чтение которых вызывает аппаратное зависание или сбой (Page Fault).
    unsigned int start_page = 1024 * 1024; 
    
    // ОПТИМИЗАЦИЯ 2: Ограничиваем сканирование верхним пределом выделенной RAM
    unsigned int end_page = ram_size_bytes;

    for (unsigned int phys_page = start_page; phys_page < end_page; phys_page += PAGE_SIZE) {
        unsigned char* page_data = (unsigned char*)(phys_page + KERNEL_PHYS_MAP_OFFSET);
        
        // ОПТИМИЗАЦИЯ 3: Вместо побайтового перебора всей страницы (4096 раз)
        // мы проверяем только выровненные участки памяти (например, каждые 16 или 32 байта).
        // Структуры процессов в Си всегда выравниваются компилятором (Structure Alignment).
        for (size_t offset = 0; offset < (PAGE_SIZE - name_len); offset += 16) {
            
            if (memcmp(&page_data[offset], proc_name, name_len) == 0) {
                
                // Проверяем, что строка заканчивается нулевым символом, 
                // чтобы не путать с кусками кода
                if (page_data[offset + name_len] == 0) {
                    print_color_full("Found process in RAM: [", 0x0F, 0x01);
                    print_color_full(proc_name, 0x0A, 0x01);
                    print_color_full("]\n", 0x0F, 0x01);
                    
                    // Как только процесс найден на этой странице, сразу уходим на следующую,
                    // чтобы не крутиться в тяжелом цикле бесконечно
                    goto next_page;
                }
            }
        }
        
    next_page:
        continue;
    }
}