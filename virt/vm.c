#include "../ui/menu.h"
#include "../lib/string.h"

// Прототип функции
void vm_send_interrupt(uint32_t int_num);

// ============================================================
// 1. ВКЛЮЧЕНИЕ VT-x
// ============================================================

static void enable_vmx(void) {
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 13); // CR4.VMXE
    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4));
}

static int check_vmx(void) {
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
    return (ecx & (1 << 5)) != 0;
}

// ============================================================
// 2. VMCS (УПРАВЛЕНИЕ ГОСТЕМ)
// ============================================================

static uint8_t vmxon_region[4096] __attribute__((aligned(4096)));
static uint8_t vmcs_region[4096] __attribute__((aligned(4096)));

static void vmcs_write(uint32_t field, uint64_t value) {
    __asm__ volatile ("vmwrite %1, %0" : : "r"(field), "r"(value));
}

static uint64_t vmcs_read(uint32_t field) {
    uint64_t value;
    __asm__ volatile ("vmread %1, %0" : "=r"(value) : "r"(field));
    return value;
}

// ============================================================
// 3. НАСТРОЙКА ГОСТЯ
// ============================================================

static uint8_t guest_code[] = {
    0xB0, 'X',          // mov al, 'X'
    0xE6, 0xE9,         // out 0xE9, al
    0xF4                // hlt
};

static void setup_guest(void) {
    uint32_t *guest_mem = (uint32_t*)0x1000;
    for (int i = 0; i < sizeof(guest_code); i++) {
        ((uint8_t*)guest_mem)[i] = guest_code[i];
    }

    vmcs_write(0x00006800, 0x1000); // RIP
    vmcs_write(0x00006802, 0x0000); // RSP
    vmcs_write(0x00006804, 0x0000); // RBP
    vmcs_write(0x0000680A, 0x00000002); // RFLAGS
}

// ============================================================
// 4. ПЕРЕХВАТ ПРЕРЫВАНИЙ И ОТПРАВКА В BIOS
// ============================================================

// Вместо:
// __asm__ volatile ("int %0" : : "r"(int_num));

// Используй:
void vm_send_interrupt(uint32_t int_num) {
    switch (int_num) {
        case 0x10: __asm__ volatile ("int $0x10"); break;
        case 0x13: __asm__ volatile ("int $0x13"); break;
        case 0x15: __asm__ volatile ("int $0x15"); break;
        case 0x16: __asm__ volatile ("int $0x16"); break;
        case 0x1A: __asm__ volatile ("int $0x1A"); break;
        default:
            print_color_full("Unknown interrupt: ", 0x0C, 0x01);
            print_int(int_num);
            print_color_full("\n", 0x0C, 0x01);
    }
}

// ============================================================
// 5. ЗАПУСК ГОСТЯ
// ============================================================

int vm_launch(void) {
    print_color_full("VM: initializing...\n", 0x0F, 0x01);

    if (!check_vmx()) {
        print_color_full("VM: VT-x not supported!\n", 0x0C, 0x01);
        return -1;
    }
    print_color_full("VM: VT-x supported\n", 0x0A, 0x01);

    enable_vmx();
    print_color_full("VM: VT-x enabled\n", 0x0A, 0x01);

    // VMXON
    uint64_t vmx_basic;
    __asm__ volatile ("rdmsr" : "=A"(vmx_basic) : "c"(0x480));
    *(uint32_t*)vmxon_region = (uint32_t)vmx_basic;
    __asm__ volatile ("vmxon %0" : : "m"(vmxon_region));
    print_color_full("VM: VMXON success\n", 0x0A, 0x01);

    // VMCS
    __asm__ volatile ("vmclear %0" : : "m"(vmcs_region));
    __asm__ volatile ("vmptrld %0" : : "m"(vmcs_region));
    print_color_full("VM: VMCS loaded\n", 0x0A, 0x01);

    // Настройка гостя
    setup_guest();
    print_color_full("VM: guest configured\n", 0x0A, 0x01);

    // Запуск
    print_color_full("VM: VMLAUNCH...\n", 0x0A, 0x01);
    __asm__ volatile ("vmlaunch");

    // VM-exit
    uint64_t exit_reason = vmcs_read(0x00004402);
    print_color_full("VM: VM-exit, reason: ", 0x0C, 0x01);
    print_int(exit_reason);
    print_color_full("\n", 0x0C, 0x01);

    // Перехват и отправка в BIOS
    if (exit_reason == 0x20) {
        vm_send_interrupt(exit_reason);
    }

    return 0;
}