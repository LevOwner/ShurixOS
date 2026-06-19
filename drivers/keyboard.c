#include "keyboard.h"
#include "../include/shurix.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

static const char scancode_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', 8, 9, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', 13, 0, 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x',
    'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
    '0', '.'
};

// Таблица для Shift (заглавные и символы)
static const char scancode_to_shift[] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    '_', '+', 8, 9, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', 13, 0, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X',
    'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
    '0', '.'
};

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

char getchar(void) {
    uint8_t scancode;
    uint8_t shift = 0;
    while (1) {
        while ((inb(KEYBOARD_STATUS_PORT) & 1) == 0);
        scancode = inb(KEYBOARD_DATA_PORT);

        // Отслеживаем Shift (скан-код 0x2A — левый, 0x36 — правый)
        if (scancode == 0x2A || scancode == 0x36) {
            shift = 1;
            continue;
        }
        if (scancode == 0xAA || scancode == 0xB6) {
            shift = 0;
            continue;
        }

        // Extended-коды (стрелки, F-клавиши, Home, End и т.д.)
        if (scancode == 0xE0) {
            while ((inb(KEYBOARD_STATUS_PORT) & 1) == 0);
            scancode = inb(KEYBOARD_DATA_PORT);
            switch (scancode) {
                case 0x48: return 0x48; // ↑
                case 0x50: return 0x50; // ↓
                case 0x4B: return 0x4B; // ←
                case 0x4D: return 0x4D; // →
                case 0x47: return 0x47; // Home
                case 0x4F: return 0x4F; // End
                case 0x49: return 0x49; // Page Up
                case 0x51: return 0x51; // Page Down
                case 0x53: return 0x53; // Delete
                case 0x52: return 0x52; // Insert
                default: continue;
            }
        }

        // Отпускание клавиши
        if (scancode & 0x80) continue;

        // F1–F12
        if (scancode >= 0x3B && scancode <= 0x44) {
            return 0x80 + (scancode - 0x3B);
        }

        // Обычные клавиши (с учётом Shift)
        if (scancode < sizeof(scancode_to_ascii)) {
            if (shift && scancode_to_shift[scancode]) {
                return scancode_to_shift[scancode];
            }
            if (scancode_to_ascii[scancode]) {
                return scancode_to_ascii[scancode];
            }
        }
    }
}