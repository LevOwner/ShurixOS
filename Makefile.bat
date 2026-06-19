@echo off
chcp 1251 >nul
cls

echo ==============================
echo   SHURIXOS - ПОЛНАЯ СБОРКА
echo ==============================
echo.

echo [1] Сборка загрузчика...
nasm -f elf32 boot/boot.asm -o boot.o
if errorlevel 1 goto error

echo [2] Компиляция lib/string.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c lib/string.c -o string.o -Iinclude
if errorlevel 1 goto error

echo [3] Компиляция fs/cfs.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c fs/cfs.c -o cfs.o -Iinclude
if errorlevel 1 goto error

echo [4] Компиляция fs/hsfs.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c fs/hsfs.c -o hsfs.o -Iinclude
if errorlevel 1 goto error

echo [5] Компиляция container/container.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c container/container.c -o container.o -Iinclude
if errorlevel 1 goto error

echo [6] Компиляция drivers/keyboard.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c drivers/keyboard.c -o keyboard.o -Iinclude
if errorlevel 1 goto error

echo [7] Компиляция virt/vm.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c virt/vm.c -o vm.o -Iinclude
if errorlevel 1 goto error

echo [8] Компиляция drivers/kafos.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c drivers/kafos.c -o kafos.o -Iinclude
if errorlevel 1 goto error

echo [9] Компиляция ui/menu.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c ui/menu.c -o menu.o -Iinclude
if errorlevel 1 goto error

echo [10] Компиляция lib/cosa.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c lib/cosa.c -o cosa.o -Iinclude
if errorlevel 1 goto error

echo [11] Компиляция kernel/kernel.c...
i686-elf-gcc -ffreestanding -m32 -nostdlib -nostdinc -fno-pie -fno-stack-protector -c kernel/kernel.c -o kernel.o -Iinclude
if errorlevel 1 goto error

echo [12] Линковка ядра...
i686-elf-ld -m elf_i386 -T linker.ld -o shurix.elf boot.o kernel.o string.o cfs.o hsfs.o container.o menu.o keyboard.o vm.o kafos.o cosa.o
if errorlevel 1 goto error

echo.
echo ==============================
echo   ГОТОВО! shurix.elf создан.
echo   Запуск в QEMU...
echo ==============================
echo.

echo [13] Запуск в QEMU...
qemu-system-i386 -kernel shurix.elf -vga std -no-reboot

:error
if errorlevel 1 (
    echo.
    echo ==============================
    echo   ОШИБКА СБОРКИ!
    echo ==============================
    pause
    exit /b 1
)