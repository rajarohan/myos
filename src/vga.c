// vga.c - Enhanced VGA text mode driver implementation
#include "vga.h"

static volatile uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0x0F; // White on black

static uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void vga_init(void) {
    cursor_x = 0;
    cursor_y = 0;
    current_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', current_color);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_set_color(enum vga_color fg, enum vga_color bg) {
    current_color = vga_entry_color(fg, bg);
}

void vga_set_cursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void vga_get_cursor(int* x, int* y) {
    *x = cursor_x;
    *y = cursor_y;
}

static void vga_scroll(void) {
    // Move all lines up by one
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    
    // Clear the bottom line
    for (int x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', current_color);
    }
    
    cursor_y = VGA_HEIGHT - 1;
}

void vga_putchar(char c) {
    switch (c) {
        case '\n':
            cursor_x = 0;
            cursor_y++;
            break;
        case '\r':
            cursor_x = 0;
            break;
        case '\b':
            if (cursor_x > 0) {
                cursor_x--;
                const int index = cursor_y * VGA_WIDTH + cursor_x;
                vga_buffer[index] = vga_entry(' ', current_color);
            }
            break;
        case '\t':
            cursor_x = (cursor_x + 8) & ~(8 - 1);
            break;
        default:
            if (c >= 32) { // Printable character
                const int index = cursor_y * VGA_WIDTH + cursor_x;
                vga_buffer[index] = vga_entry(c, current_color);
                cursor_x++;
            }
            break;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        vga_scroll();
    }
}

void vga_puts(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

// Simple string length function
static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Simple integer to string conversion
static void itoa(int value, char* buffer, int base) {
    char* p = buffer;
    char* p1, *p2;
    int digits = 0;
    
    if (value < 0 && base == 10) {
        *p++ = '-';
        value = -value;
    }
    
    do {
        int remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        digits++;
    } while (value /= base);
    
    *p = 0;
    
    // Reverse string (excluding sign)
    p1 = (buffer[0] == '-') ? buffer + 1 : buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

// Simplified printf implementation for kernel
void vga_printf(const char* format, ...) {
    char buffer[32];
    
    // Get pointer to first argument
    char** args = (char**)&format + 1;
    int arg_count = 0;
    
    for (const char* p = format; *p != '\0'; p++) {
        if (*p != '%') {
            vga_putchar(*p);
            continue;
        }
        
        p++; // Skip '%'
        switch (*p) {
            case 'c':
                vga_putchar((char)(int)args[arg_count++]);
                break;
            case 's':
                vga_puts((char*)args[arg_count++]);
                break;
            case 'd': {
                int num = (int)args[arg_count++];
                itoa(num, buffer, 10);
                vga_puts(buffer);
                break;
            }
            case 'x': {
                int num = (int)args[arg_count++];
                itoa(num, buffer, 16);
                vga_puts(buffer);
                break;
            }
            case '%':
                vga_putchar('%');
                break;
            default:
                vga_putchar('%');
                vga_putchar(*p);
                break;
        }
    }
}