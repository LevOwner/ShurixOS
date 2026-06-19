#ifndef MENU_H
#define MENU_H

#include "../include/shurix.h"

void print_color_full(const char *s, uint8_t fg_color, uint8_t bg_color);
void vga_clear(void);
void vga_putchar(char c, uint8_t color);
void print(const char *s);                     // ← обычный принт (белый)
void print_color(const char *s, uint8_t color); // ← цветной принт
void print_int(int n);
void print_center(int y, const char *s, uint8_t color);
void menu_main(void);

#endif