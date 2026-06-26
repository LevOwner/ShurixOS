# ============================================================
# SHURIXOS - Makefile
# ============================================================

CC = i686-elf-gcc
ASM = nasm
LD = i686-elf-ld
CFLAGS = -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld
OBJS = boot.o kernel.o init.o string.o cfs.o hsfs.o container.o keyboard.o vm.o kafos.o menu.o cosa.o crypto.o process.o thread.o

# ============================================================
# ЦЕЛИ
# ============================================================

all: shurix.elf

# Сборка объектных файлов из C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка объектных файлов из ассемблера
%.o: %.asm
	$(ASM) -f elf32 $< -o $@

# Линковка
shurix.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Очистка
clean:
	rm -f *.o *.elf *.bin *.img

# Запуск в QEMU
run: shurix.elf
	qemu-system-i386 -kernel shurix.elf -vga std -no-reboot -m 128

# ISO для загрузки на реальном железе
iso: shurix.elf
	mkdir -p iso/boot/grub
	cp shurix.elf iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o shurix.iso iso/

# Генерация образа для флешки
img: shurix.elf
	dd if=/dev/zero of=shurix.img bs=512 count=2880
	dd if=shurix.elf of=shurix.img bs=512 conv=notrunc

.PHONY: all clean run iso img