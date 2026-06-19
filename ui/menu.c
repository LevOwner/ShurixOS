#include "menu.h"
#include "../container/container.h"
#include "../drivers/keyboard.h"
#include "../virt/vm.h"
#include "../drivers/kafos.h"

#define VGA_COLS 80
#define VGA_ROWS 25
static uint16_t *vga = (uint16_t*)0xB8000;

// Глобальные переменные (без static)
int cursor_x = 0;
int cursor_y = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void vga_clear_blue(void) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        vga[i] = (0x17 << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_delete_char(void) {
    if (cursor_x > 0) {
        cursor_x--;
        vga_putchar(' ', 0x0F);
        cursor_x--;
    }
}


void vga_putchar(char c, uint8_t color) {
    if (c == '\n') { cursor_x = 0; cursor_y++; return; }
    vga[cursor_y * VGA_COLS + cursor_x] = (color << 8) | c;
    cursor_x++;
    if (cursor_x >= VGA_COLS) { cursor_x = 0; cursor_y++; }
    if (cursor_y >= VGA_ROWS) {
        for (int row = 0; row < VGA_ROWS - 1; row++) {
            for (int col = 0; col < VGA_COLS; col++) {
                vga[row * VGA_COLS + col] = vga[(row + 1) * VGA_COLS + col];
            }
        }
        cursor_y = VGA_ROWS - 1;
    }
    uint16_t pos = cursor_y * VGA_COLS + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void print_color_full(const char *s, uint8_t fg_color, uint8_t bg_color) {
    uint8_t combined = (bg_color << 4) | fg_color;
    while (*s) vga_putchar(*s++, combined);
}

void print(const char *s) {
    print_color_full(s, 0x0F, 0x01);
}

void print_color(const char *s, uint8_t color) {
    while (*s) vga_putchar(*s++, color);
}

void print_int(int n) {
    char buf[16];
    int i = 0;
    if (n == 0) { vga_putchar('0', 0x1F); return; }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i--) vga_putchar(buf[i], 0x1F);
}

void draw_box(void) {
    cursor_x = 0; cursor_y = 8;
    vga_putchar(0xC9, 0x07);
    for (int i = 0; i < 78; i++) vga_putchar(0xCD, 0x07);
    vga_putchar(0xBB, 0x07);

    for (int i = 0; i < 11; i++) {
        cursor_x = 0; cursor_y = 9 + i;
        vga_putchar(0xBA, 0x07);
        for (int j = 0; j < 78; j++) vga_putchar(' ', 0x17);
        vga_putchar(0xBA, 0x07);
    }

    cursor_x = 0; cursor_y = 20;
    vga_putchar(0xC8, 0x07);
    for (int i = 0; i < 78; i++) vga_putchar(0xCD, 0x07);
    vga_putchar(0xBC, 0x07);
}

void container_manager_ui(void) {
    while (1) {
        vga_clear_blue();

        print_color_full("=== CONTAINER MANAGER ===\n", 0x0E, 0x01);
        print_color_full("\n", 0x0F, 0x01);

        print_color_full("Containers:\n", 0x0F, 0x01);
        for (int i = 0; i < container_count && i < 8; i++) {
            print("  ");
            print_int(i + 1);
            print(". ");
            print(containers[i].name);
            print(" [");
            print(containers[i].state ? "RUN" : "STOP");
            print("]");
            if (containers[i].is_default) {
                print_color_full("  DEFAULT", 0x0E, 0x01);
            }
            print("\n");
        }
        if (container_count == 0) {
            print("  (no containers)\n");
        }

        print_color_full("\nActions:\n", 0x0F, 0x01);
        print("  1. Create container\n");
        print("  2. Delete container\n");
        print("  3. Set default\n");
        print("  4. Start container (VM)\n");
        print("  5. Stop container\n");
        print("  6. Show status\n");
        print("  7. Back\n");
        print("\n");
        print("Enter choice: ");

        char key = getchar();

        if (key == '1') {
            print("\nContainer name: ");
            char name[32];
            int i = 0;
            while (1) {
                char c = getchar();
                if (c == 13) { name[i] = '\0'; break; }
                if (c == 8 && i > 0) { i--; continue; }
                if (i < 31) { name[i++] = c; vga_putchar(c, 0x1F); }
            }
            print("\nRAM (MB): ");
            int ram = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { ram = ram * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            print("\nCPU cores: ");
            int cpu = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { cpu = cpu * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            if (container_create(name, ram, cpu) == 0) {
                print("\nContainer created!\n");
            } else {
                print("\nCreation failed!\n");
            }
            print("\nPress any key...");
            getchar();
        } else if (key == '2') {
            print("\nEnter container number: ");
            int idx = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { idx = idx * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            if (container_delete(idx - 1) == 0) {
                print("\nContainer deleted!\n");
            } else {
                print("\nInvalid index!\n");
            }
            print("\nPress any key...");
            getchar();
        } else if (key == '3') {
            print("\nEnter container number: ");
            int idx = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { idx = idx * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            if (container_set_default(idx - 1) == 0) {
                print("\nDefault set!\n");
            } else {
                print("\nInvalid index!\n");
            }
            print("\nPress any key...");
            getchar();
        } else if (key == '4') {
            print("\nEnter container number to start: ");
            int idx = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { idx = idx * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            if (idx > 0 && idx <= container_count) {
                if (containers[idx-1].state == 0) {
                    print("\nStarting container via VM...\n");
                    kafos_command_t cmd;
                    kafos_response_t resp;
                    cmd.cmd = 1;
                    kafos_send(&cmd, &resp);
                    containers[idx-1].state = 1;
                    print_color_full("Container started!\n", 0x0A, 0x01);
                } else {
                    print("Container already running!\n");
                }
            } else {
                print("Invalid index!\n");
            }
            print("\nPress any key...");
            getchar();
        } else if (key == '5') {
            print("\nEnter container number to stop: ");
            int idx = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { idx = idx * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            if (idx > 0 && idx <= container_count) {
                if (containers[idx-1].state == 1) {
                    containers[idx-1].state = 0;
                    print("Container stopped!\n");
                } else {
                    print("Container already stopped!\n");
                }
            } else {
                print("Invalid index!\n");
            }
            print("\nPress any key...");
            getchar();
        } else if (key == '6') {
            print("\nEnter container number: ");
            int idx = 0;
            while (1) {
                char c = getchar();
                if (c == 13) break;
                if (c >= '0' && c <= '9') { idx = idx * 10 + (c - '0'); vga_putchar(c, 0x1F); }
            }
            container_print_status(idx - 1);
            print("\nPress any key...");
            getchar();
        } else if (key == '7' || key == 27) {
            break;
        }
    }
}

void menu_main(void) {
    vga_clear_blue();

    print_color_full("Container manager v1.0\n", 0x0E, 0x01);
    print_color_full("\n", 0x0F, 0x01);
    print_color_full("        For freedom of devices,\n", 0x07, 0x01);
    print_color_full("        For easier work with the OS,\n", 0x07, 0x01);
    print_color_full("        For user priority in system control,\n", 0x07, 0x01);
    print_color_full("        All for you ShurixOS\n", 0x07, 0x01);
    print_color_full("\n", 0x0F, 0x01);

    draw_box();

    int selected = 0;
    int items = 4;
    char *menu_items[] = {
        "* Manage Container",
        "  Storage and Drive",
        "  Security and recovery",
        "  Save and exit"
    };

    while (1) {
        for (int i = 0; i < items; i++) {
            cursor_x = 2;
            cursor_y = 10 + i * 2;
            for (int j = 0; j < 76; j++) vga_putchar(' ', 0x17);
            cursor_x = 2;
            cursor_y = 10 + i * 2;

            if (i == selected) {
                print_color_full("-> ", 0x0E, 0x01);
            } else {
                print_color_full("   ", 0x0F, 0x01);
            }
            print_color_full(menu_items[i], 0x0F, 0x01);
        }

        char key = getchar();

        if (key == 'w' || key == 'W' || key == 0x48) {
            if (selected > 0) selected--;
        } else if (key == 's' || key == 'S' || key == 0x50) {
            if (selected < items - 1) selected++;
        } else if (key == 13) {
            if (selected == 0) {
                container_manager_ui();
                vga_clear_blue();
                print_color_full("Container manager v1.0\n", 0x0E, 0x01);
                print_color_full("\n", 0x0F, 0x01);
                print_color_full("        For freedom of devices,\n", 0x07, 0x01);
                print_color_full("        For easier work with the OS,\n", 0x07, 0x01);
                print_color_full("        For user priority in system control,\n", 0x07, 0x01);
                print_color_full("        All for you ShurixOS\n", 0x07, 0x01);
                print_color_full("\n", 0x0F, 0x01);
                draw_box();
            } else if (selected == 1) {
                print_color_full("\n\nStorage and Drive selected\n", 0x0A, 0x01);
                print("\nPress any key...");
                getchar();
            } else if (selected == 2) {
                print_color_full("\n\nSecurity and recovery selected\n", 0x0A, 0x01);
                print("\nPress any key...");
                getchar();
            } else if (selected == 3) {
                print_color_full("\n\nSave and exit\n", 0x0A, 0x01);
                break;
            }
        } else if (key == 27) {
            break;
        }
    }
}