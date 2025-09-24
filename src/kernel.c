// src/kernel.c - Main kernel with file system and GUI support
#include <stdint.h>
#include "vga.h"
#include "keyboard.h"
#include "filesystem.h"
#include "shell.h"
#include "graphics.h"
#include "window.h"
#include "gui_app.h"

// Operating system mode
static int gui_mode = 0;

void kmain(void) {
    // Initialize VGA display
    vga_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize file system
    fs_init();
    
    // Show startup menu
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("MyOS v1.0 - Boot Menu\n");
    vga_puts("====================\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Choose startup mode:\n");
    vga_puts("1. Text Mode (Shell)\n");
    vga_puts("2. GUI Mode (Desktop)\n");
    vga_puts("\nPress 1 or 2: ");
    
    // Wait for user choice
    char choice = 0;
    while (choice != '1' && choice != '2') {
        choice = keyboard_getchar();
    }
    
    vga_putchar(choice);
    vga_putchar('\n');
    
    if (choice == '1') {
        // Text mode - run shell
        vga_puts("\nStarting text mode...\n");
        for (int i = 0; i < 1000000; i++) __asm__ volatile("nop"); // Simple delay
        
        shell_init();
        shell_run();
    } else {
        // GUI mode - start desktop
        vga_puts("\nStarting GUI mode...\n");
        for (int i = 0; i < 1000000; i++) __asm__ volatile("nop"); // Simple delay
        
        gui_mode = 1;
        
        // Initialize graphics
        graphics_init();
        
        // Initialize window manager
        window_manager_init();
        
        // Create some demo windows
        gui_app_t* file_manager = file_manager_create();
        gui_app_t* text_editor = text_editor_create();
        gui_app_t* terminal = terminal_create();
        
        // Draw everything
        window_manager_draw();
        
        // Simple GUI event loop
        vga_puts("\nGUI mode started. Press ESC to return to shell.\n");
        
        char key;
        while ((key = keyboard_getchar()) != 27) { // ESC to exit
            // Handle GUI events here
            if (key == '1') {
                // Refresh file manager
                gui_app_destroy(file_manager);
                file_manager = file_manager_create();
                window_manager_draw();
            } else if (key == '2') {
                // Refresh text editor
                gui_app_destroy(text_editor);
                text_editor = text_editor_create();
                window_manager_draw();
            } else if (key == '3') {
                // Refresh terminal
                gui_app_destroy(terminal);
                terminal = terminal_create();
                window_manager_draw();
            }
        }
        
        // Cleanup GUI apps
        gui_app_destroy(file_manager);
        gui_app_destroy(text_editor);
        gui_app_destroy(terminal);
        
        // Return to text mode
        vga_init();
        vga_puts("Returned to text mode.\n");
        shell_init();
        shell_run();
    }
    
    // This should never be reached, but just in case
    for (;;) {
        __asm__ volatile("hlt");
    }
}
