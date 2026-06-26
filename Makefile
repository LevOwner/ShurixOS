# ============================================================
# SHURIXOS - Makefile
# ============================================================

CC = i686-elf-gcc
ASM = nasm
LD = i686-elf-ld
CFLAGS = -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

# Объектные файлы (все .o)
OBJS = boot.o kernel.o init.o string.o cfs.o hsfs.o container.o keyboard.o vm.o kafos.o menu.o cosa.o crypto.o process.o thread.o

# ============================================================
# ПРАВИЛА
# ============================================================

all: shurix.elf

# Компиляция .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Компиляция .asm → .o
%.o: %.asm
	$(ASM) -f elf32 $< -o $@

# Линковка
shurix.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Очистка
clean:
	rm -f *.o *.elf *.bin *.img

# Запуск
run: shurix.elf
	qemu-system-i386 -kernel shurix.elf -vga std -no-reboot -m 128

.PHONY: all clean run