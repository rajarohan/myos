// gui_app.c - Simple GUI application framework
#include "gui_app.h"
#include "filesystem.h"
#include "vga.h"

// Helper functions
static void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

gui_app_t* gui_app_create(const char* title, int x, int y, int width, int height) {
    static gui_app_t app; // Static allocation for simplicity
    
    strcpy(app.title, title);
    app.window_id = window_create(title, x, y, width, height, COLOR_WHITE);
    app.running = 1;
    
    return &app;
}

void gui_app_destroy(gui_app_t* app) {
    if (app) {
        window_destroy(app->window_id);
        app->running = 0;
    }
}

void gui_app_run(gui_app_t* app) {
    if (!app) return;
    
    while (app->running) {
        gui_app_draw(app);
        // In a real implementation, we'd handle events here
        // For now, just draw once and exit
        break;
    }
}

void gui_app_draw(gui_app_t* app) {
    if (!app) return;
    
    // Clear window
    window_clear(app->window_id, COLOR_WHITE);
    
    // Draw some basic content
    window_draw_text(app->window_id, 10, 10, "Hello GUI World!", COLOR_BLACK, COLOR_WHITE);
}

void gui_app_handle_key(gui_app_t* app, char key) {
    if (!app) return;
    
    if (key == 27) { // ESC key
        app->running = 0;
    }
}

// File Manager Application
gui_app_t* file_manager_create(void) {
    gui_app_t* app = gui_app_create("File Manager", 50, 50, 300, 200);
    
    if (app) {
        // Clear window and draw file manager content
        window_clear(app->window_id, COLOR_LIGHT_GRAY);
        
        // Draw title
        window_draw_text(app->window_id, 10, 10, "File Manager", COLOR_BLACK, COLOR_LIGHT_GRAY);
        window_draw_text(app->window_id, 10, 25, "Files and Directories:", COLOR_BLACK, COLOR_LIGHT_GRAY);
        
        // Draw file list (simplified)
        int y = 45;
        char current_path[64];
        fs_get_current_path(current_path, 64);
        
        // Show current directory
        window_draw_text(app->window_id, 10, y, "Current: ", COLOR_BLACK, COLOR_LIGHT_GRAY);
        window_draw_text(app->window_id, 70, y, current_path, COLOR_BLUE, COLOR_LIGHT_GRAY);
        y += 20;
        
        // List some files (we'd integrate with the real filesystem here)
        window_draw_text(app->window_id, 10, y, "[DIR]  Documents", COLOR_BLUE, COLOR_LIGHT_GRAY);
        y += 15;
        window_draw_text(app->window_id, 10, y, "[FILE] readme.txt", COLOR_BLACK, COLOR_LIGHT_GRAY);
        y += 15;
        window_draw_text(app->window_id, 10, y, "[FILE] hello.txt", COLOR_BLACK, COLOR_LIGHT_GRAY);
        
        // Draw instructions
        y += 30;
        window_draw_text(app->window_id, 10, y, "Press ESC to close", COLOR_GRAY, COLOR_LIGHT_GRAY);
    }
    
    return app;
}

// Text Editor Application
gui_app_t* text_editor_create(void) {
    gui_app_t* app = gui_app_create("Text Editor", 100, 80, 280, 180);
    
    if (app) {
        // Clear window and draw text editor content
        window_clear(app->window_id, COLOR_WHITE);
        
        // Draw title bar content
        window_draw_text(app->window_id, 10, 10, "Simple Text Editor", COLOR_BLACK, COLOR_WHITE);
        
        // Draw text area border
        rect_t text_area = {5, 30, 270, 120};
        window_draw_rect(app->window_id, text_area, COLOR_BLACK);
        
        // Draw some sample text
        window_draw_text(app->window_id, 10, 35, "Welcome to MyOS Text Editor!", COLOR_BLACK, COLOR_WHITE);
        window_draw_text(app->window_id, 10, 50, "", COLOR_BLACK, COLOR_WHITE);
        window_draw_text(app->window_id, 10, 65, "Type your text here...", COLOR_GRAY, COLOR_WHITE);
        
        // Draw cursor (simple block)
        rect_t cursor = {10, 80, 8, 12};
        window_fill_rect(app->window_id, cursor, COLOR_BLACK);
        
        // Draw status bar
        window_draw_text(app->window_id, 10, 155, "Press ESC to close", COLOR_GRAY, COLOR_WHITE);
    }
    
    return app;
}

// Terminal Application  
gui_app_t* terminal_create(void) {
    gui_app_t* app = gui_app_create("Terminal", 150, 110, 320, 200);
    
    if (app) {
        // Clear window with black background (like a terminal)
        window_clear(app->window_id, COLOR_BLACK);
        
        // Draw terminal content
        window_draw_text(app->window_id, 5, 10, "MyOS Terminal v1.0", COLOR_GREEN, COLOR_BLACK);
        window_draw_text(app->window_id, 5, 25, "Connected to MyOS kernel", COLOR_WHITE, COLOR_BLACK);
        window_draw_text(app->window_id, 5, 40, "", COLOR_WHITE, COLOR_BLACK);
        
        // Show current directory prompt
        char current_path[64];
        fs_get_current_path(current_path, 64);
        
        window_draw_text(app->window_id, 5, 55, "myos:", COLOR_GREEN, COLOR_BLACK);
        window_draw_text(app->window_id, 40, 55, current_path, COLOR_CYAN, COLOR_BLACK);
        window_draw_text(app->window_id, 5, 70, "$ ls", COLOR_WHITE, COLOR_BLACK);
        
        // Show some sample output
        window_draw_text(app->window_id, 5, 85, "Documents/", COLOR_BLUE, COLOR_BLACK);
        window_draw_text(app->window_id, 5, 100, "hello.txt", COLOR_WHITE, COLOR_BLACK);
        window_draw_text(app->window_id, 5, 115, "readme.txt", COLOR_WHITE, COLOR_BLACK);
        
        window_draw_text(app->window_id, 5, 130, "myos:", COLOR_GREEN, COLOR_BLACK);
        window_draw_text(app->window_id, 40, 130, current_path, COLOR_CYAN, COLOR_BLACK);
        window_draw_text(app->window_id, 5, 145, "$ _", COLOR_WHITE, COLOR_BLACK);
        
        // Draw cursor
        rect_t cursor = {15, 145, 8, 12};
        window_fill_rect(app->window_id, cursor, COLOR_WHITE);
        
        window_draw_text(app->window_id, 5, 170, "Press ESC to close", COLOR_GRAY, COLOR_BLACK);
    }
    
    return app;
}