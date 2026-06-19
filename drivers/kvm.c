#include "kvm.h"
#include "../ui/menu.h"
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static int kvm_fd = -1;

int kvm_init(void) {
    // Открываем устройство KVM
    kvm_fd = open("/dev/kvm", O_RDWR | O_CLOEXEC);
    if (kvm_fd < 0) {
        print("KVM: failed to open /dev/kvm\n");
        return -1;
    }

    // Проверяем версию KVM
    int version = ioctl(kvm_fd, KVM_GET_API_VERSION, 0);
    if (version < 0 || version != 12) {
        print("KVM: unsupported version\n");
        close(kvm_fd);
        return -1;
    }

    print("KVM: initialized successfully!\n");
    return 0;
}

int kvm_start_container(container_t *c) {
    if (!c) return -1;

    print("KVM: starting container '");
    print(c->name);
    print("' with ");
    print_int(c->ram_mb);
    print(" MB RAM, ");
    print_int(c->cpu_cores);
    print(" CPU cores\n");

    // 1. Создаём виртуальную машину
    int vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0);
    if (vm_fd < 0) {
        print("KVM: failed to create VM\n");
        return -1;
    }

    // 2. Выделяем память для гостя (4KB)
    void *guest_mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (guest_mem == MAP_FAILED) {
        print("KVM: failed to allocate guest memory\n");
        close(vm_fd);
        return -1;
    }

    // 3. Простой гостевой код: HLT (остановка) — мы его перезапишем позже
    unsigned char guest_code[] = {
        0xF4, // hlt
    };
    memcpy(guest_mem, guest_code, sizeof(guest_code));

    // 4. Создаём VCPU
    int vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0);
    if (vcpu_fd < 0) {
        print("KVM: failed to create VCPU\n");
        munmap(guest_mem, 0x1000);
        close(vm_fd);
        return -1;
    }

    // 5. Настраиваем регистры VCPU
    struct kvm_regs regs = {0};
    regs.rip = 0x1000; // Начало выполнения (адрес гостевой памяти)
    if (ioctl(vcpu_fd, KVM_SET_REGS, &regs) < 0) {
        print("KVM: failed to set registers\n");
        close(vcpu_fd);
        close(vm_fd);
        munmap(guest_mem, 0x1000);
        return -1;
    }

    // 6. Запускаем VCPU
    int run_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    struct kvm_run *run = mmap(NULL, run_size, PROT_READ | PROT_WRITE,
                                MAP_SHARED, vcpu_fd, 0);
    if (run == MAP_FAILED) {
        print("KVM: failed to map kvm_run\n");
        close(vcpu_fd);
        close(vm_fd);
        munmap(guest_mem, 0x1000);
        return -1;
    }

    // Выполняем гостевой код
    print("KVM: executing guest code...\n");
    while (1) {
        if (ioctl(vcpu_fd, KVM_RUN, 0) < 0) {
            print("KVM: run failed\n");
            break;
        }
        if (run->exit_reason == KVM_EXIT_HLT) {
            print("KVM: guest halted (HLT)\n");
            break;
        }
        if (run->exit_reason == KVM_EXIT_IO) {
            print("KVM: guest IO (not handled)\n");
            break;
        }
        break;
    }

    // Очистка
    munmap(run, run_size);
    close(vcpu_fd);
    close(vm_fd);
    munmap(guest_mem, 0x1000);

    c->state = 1;
    return 0;
}

int kvm_stop_container(container_t *c) {
    print("KVM: stopping container '");
    print(c->name);
    print("'\n");
    c->state = 0;
    return 0;
}