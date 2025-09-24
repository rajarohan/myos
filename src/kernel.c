// src/kernel.c
#include <stdint.h>

// VGA text mode buffer starts at 0xB8000
volatile uint16_t* vga = (uint16_t*)0xB8000;

void kmain(void) {
    const char *msg = "Hello from MyOS kernel!";
    for (int i = 0; msg[i] != 0; i++) {
        vga[i] = (0x0F << 8) | msg[i]; // 0x0F = white text, black background
    }

    // Halt CPU
    for (;;) {
        __asm__ volatile("hlt");
    }
}
