// src/kernel.c - Main kernel with file system
#include <stdint.h>
#include "vga.h"
#include "keyboard.h"
#include "filesystem.h"
#include "shell.h"

void kmain(void) {
    // Initialize VGA display
    vga_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize file system
    fs_init();
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    // This should never be reached, but just in case
    for (;;) {
        __asm__ volatile("hlt");
    }
}
