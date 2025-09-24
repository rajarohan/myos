// window.h - Simple window manager interface
#ifndef WINDOW_H
#define WINDOW_H

#include "graphics.h"
#include <stdint.h>

// Maximum number of windows
#define MAX_WINDOWS 16

// Window states
typedef enum {
    WINDOW_HIDDEN,
    WINDOW_VISIBLE,
    WINDOW_MINIMIZED,
    WINDOW_MAXIMIZED
} window_state_t;

// Window structure
typedef struct window {
    int id;
    char title[64];
    rect_t bounds;          // Window position and size
    rect_t client_area;     // Area inside window decorations
    color_t bg_color;
    window_state_t state;
    int z_order;           // For window layering
    int has_titlebar;
    int has_border;
    int is_active;
    
    // Window content (simple framebuffer)
    uint8_t* content;
    int content_width;
    int content_height;
} window_t;

// Window manager functions
void window_manager_init(void);
int window_create(const char* title, int x, int y, int width, int height, color_t bg_color);
void window_destroy(int window_id);
void window_show(int window_id);
void window_hide(int window_id);
void window_move(int window_id, int x, int y);
void window_resize(int window_id, int width, int height);
void window_set_title(int window_id, const char* title);
void window_set_active(int window_id);

// Window drawing functions
void window_clear(int window_id, color_t color);
void window_draw_pixel(int window_id, int x, int y, color_t color);
void window_draw_rect(int window_id, rect_t rect, color_t color);
void window_fill_rect(int window_id, rect_t rect, color_t color);
void window_draw_text(int window_id, int x, int y, const char* text, color_t fg, color_t bg);

// Window manager drawing
void window_manager_draw(void);
void window_draw_decorations(window_t* window);

// Window management
window_t* window_get_by_id(int window_id);
int window_get_count(void);
void window_bring_to_front(int window_id);

#endif // WINDOW_H