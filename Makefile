# ============================================================
# SHURIXOS - Makefile (с папками)
# ============================================================

CC = i686-elf-gcc
ASM = nasm
LD = i686-elf-ld
CFLAGS = -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

# Объектные файлы
OBJS = boot/boot.o kernel/kernel.o kernel/init.o lib/string.o fs/cfs.o fs/hsfs.o \
       container/container.o drivers/keyboard.o virt/vm.o drivers/kafos.o ui/menu.o \
       lib/cosa.o ss/crypto.o kernel/process.o kernel/thread.o

all: shurix.elf

# Сборка .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка .asm → .o
%.o: %.asm
	$(ASM) -f elf32 $< -o $@

# Линковка
shurix.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) shurix.elf shurix.bin shurix.img

run: shurix.elf
	qemu-system-i386 -kernel shurix.elf -vga std -no-reboot -m 128

.PHONY: all clean run