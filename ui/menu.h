#ifndef MENU_H
#define MENU_H

#include "../include/shurix.h"

// Глобальные переменные
extern int cursor_x;
extern int cursor_y;

void vga_clear_blue(void);
void vga_putchar(char c, uint8_t color);
void print_color_full(const char *s, uint8_t fg_color, uint8_t bg_color);
void print(const char *s);
void print_color(const char *s, uint8_t color);
void print_int(int n);
void print_center(int y, const char *s, uint8_t color);
void vga_delete_char(void);
void menu_main(void);

#endif