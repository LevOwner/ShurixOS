#include "vm.h"
#include "../ui/menu.h"
#include "../lib/string.h"  // ← это не <string.h>, а твой собственный

static int vm_mode = VM_MODE_KVM;

static uint8_t vmxon_region[4096] __attribute__((aligned(4096)));
static uint8_t vmcs_region[4096] __attribute__((aligned(4096)));

static uint8_t guest_code[] = {
    0xB0, 'X',          // mov al, 'X'
    0xE6, 0xE9,         // out 0xE9, al
    0xF4                // hlt
};

int vm_check_kvm(void) {
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
    return (ecx & (1 << 5)) != 0;
}

void vm_set_mode(int mode) {
    vm_mode = mode;
}

static void enable_vmx(void) {
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 13);
    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4));
}

static uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile ("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

static void vmcs_write(uint32_t field, uint64_t value) {
    __asm__ volatile ("vmwrite %1, %0" : : "r"(field), "r"(value));
}

static uint64_t vmcs_read(uint32_t field) {
    uint64_t value;
    __asm__ volatile ("vmread %1, %0" : "=r"(value) : "r"(field));
    return value;
}

static void print_color_full_char(char c, uint8_t fg, uint8_t bg) {
    uint8_t combined = (bg << 4) | fg;
    vga_putchar(c, combined);
}

static int kvm_init(void) {
    print_color_full("VM: initializing KVM...\n", 0x0F, 0x01);
    if (!vm_check_kvm()) {
        print_color_full("VM: VMX not supported!\n", 0x0C, 0x01);
        return -1;
    }
    print_color_full("VM: VMX supported\n", 0x0A, 0x01);
    enable_vmx();
    print_color_full("VM: VMX enabled\n", 0x0A, 0x01);
    uint64_t vmx_basic = rdmsr(0x480);
    *(uint32_t*)vmxon_region = (uint32_t)vmx_basic;
    __asm__ volatile ("vmxon %0" : : "m"(vmxon_region));
    print_color_full("VM: VMXON success\n", 0x0A, 0x01);
    __asm__ volatile ("vmclear %0" : : "m"(vmcs_region));
    __asm__ volatile ("vmptrld %0" : : "m"(vmcs_region));
    print_color_full("VM: VMCS loaded\n", 0x0A, 0x01);
    return 0;
}

static int kvm_launch(void) {
    print_color_full("VM: launching guest with KVM...\n", 0x0F, 0x01);
    uint32_t *guest_mem = (uint32_t*)0x1000;
    for (int i = 0; i < sizeof(guest_code); i++) {
        ((uint8_t*)guest_mem)[i] = guest_code[i];
    }
    vmcs_write(0x00006000, 0x0000);
    vmcs_write(0x00006002, 0x0000);
    vmcs_write(0x00006004, 0x0000);
    vmcs_write(0x00006006, 0x0000);
    vmcs_write(0x00006008, 0x0000);
    vmcs_write(0x0000600A, 0x0000);
    vmcs_write(0x0000600C, 0x0000);
    vmcs_write(0x0000600E, 0x0000);
    vmcs_write(0x00006800, 0x1000);
    vmcs_write(0x00006802, 0x0000);
    vmcs_write(0x00006804, 0x0000);
    vmcs_write(0x0000680A, 0x00000002);
    uint32_t esp;
    __asm__ volatile ("mov %%esp, %0" : "=r"(esp));
    vmcs_write(0x00006C00, esp);
    vmcs_write(0x00006C02, 0x00000000);
    vmcs_write(0x00004000, (uint32_t)guest_mem);
    vmcs_write(0x00004002, 0);
    vmcs_write(0x00004800, 0x00000000);
    vmcs_write(0x00004802, 0x00000000);
    vmcs_write(0x00004804, 0x00000000);
    vmcs_write(0x00004000, 0);
    print_color_full("VM: VMLAUNCH...\n", 0x0A, 0x01);
    __asm__ volatile ("vmlaunch");
    uint64_t exit_reason = vmcs_read(0x00004402);
    print_color_full("VM: VM-exit, reason: ", 0x0C, 0x01);
    print_int(exit_reason);
    print_color_full("\n", 0x0F, 0x01);
    return 0;
}

// ============================================================
// РЕАЛЬНАЯ ЭМУЛЯЦИЯ (БЕЗ KVM)
// ============================================================

static void emulate_guest(void) {
    print_color_full("VM: emulating guest code...\n", 0x0A, 0x01);

    uint8_t *code = guest_code;
    uint32_t ip = 0;
    uint8_t al = 0;

    while (1) {
        uint8_t op = code[ip++];

        if (op == 0xB0) {          // mov al, imm8
            al = code[ip++];
        } else if (op == 0xE6) {   // out imm8, al
            uint8_t port = code[ip++];
            if (port == 0xE9) {
                print_color_full("VM: guest output: ", 0x0F, 0x01);
                print_color_full_char(al, 0x0E, 0x01);
                print_color_full("\n", 0x0F, 0x01);
            }
        } else if (op == 0xF4) {   // hlt
            print_color_full("VM: guest halted\n", 0x0A, 0x01);
            break;
        } else {
            print_color_full("VM: unknown opcode: ", 0x0C, 0x01);
            print_int(op);
            print_color_full("\n", 0x0F, 0x01);
            break;
        }
    }
}


int vm_init(void) {
    if (vm_mode == VM_MODE_EMU) {
        print_color_full("VM: emulation mode enabled\n", 0x0E, 0x01);
        return 0;
    }
    return kvm_init();
}

int vm_launch(void) {
    if (vm_mode == VM_MODE_EMU) {
        emulate_guest();
        return 0;
    }
    return kvm_launch();
}