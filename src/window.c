// window.c - Simple window manager implementation
#include "window.h"
#include "graphics.h"
#include "vga.h"

// Window storage
static window_t windows[MAX_WINDOWS];
static int window_count = 0;
static int next_window_id = 1;
static int active_window_id = -1;

// Window manager state
static int wm_initialized = 0;

// Helper functions
static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

void window_manager_init(void) {
    // Initialize all windows
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].id = -1;
        windows[i].state = WINDOW_HIDDEN;
        windows[i].content = 0;
    }
    
    window_count = 0;
    next_window_id = 1;
    active_window_id = -1;
    wm_initialized = 1;
    
    // Clear screen
    graphics_clear_screen(COLOR_BLUE);
}

int window_create(const char* title, int x, int y, int width, int height, color_t bg_color) {
    if (!wm_initialized || window_count >= MAX_WINDOWS) {
        return -1;
    }
    
    // Find empty slot
    int slot = -1;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == -1) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) return -1;
    
    // Initialize window
    window_t* win = &windows[slot];
    win->id = next_window_id++;
    strcpy(win->title, title);
    
    // Set bounds (including decorations)
    win->bounds.x = x;
    win->bounds.y = y;
    win->bounds.width = width;
    win->bounds.height = height;
    
    // Set client area (inside decorations)
    win->has_titlebar = 1;
    win->has_border = 1;
    win->client_area.x = x + 2;
    win->client_area.y = y + 20; // Title bar height
    win->client_area.width = width - 4;
    win->client_area.height = height - 22;
    
    win->bg_color = bg_color;
    win->state = WINDOW_VISIBLE;
    win->z_order = window_count;
    win->is_active = 0;
    
    // Allocate content buffer (simplified - we won't use it for now)
    win->content = 0;
    win->content_width = win->client_area.width;
    win->content_height = win->client_area.height;
    
    window_count++;
    
    // Make this window active
    window_set_active(win->id);
    
    return win->id;
}

void window_destroy(int window_id) {
    window_t* win = window_get_by_id(window_id);
    if (!win) return;
    
    win->id = -1;
    win->state = WINDOW_HIDDEN;
    window_count--;
    
    if (active_window_id == window_id) {
        active_window_id = -1;
    }
}

void window_show(int window_id) {
    window_t* win = window_get_by_id(window_id);
    if (win) {
        win->state = WINDOW_VISIBLE;
    }
}

void window_hide(int window_id) {
    window_t* win = window_get_by_id(window_id);
    if (win) {
        win->state = WINDOW_HIDDEN;
    }
}

void window_move(int window_id, int x, int y) {
    window_t* win = window_get_by_id(window_id);
    if (!win) return;
    
    int dx = x - win->bounds.x;
    int dy = y - win->bounds.y;
    
    win->bounds.x = x;
    win->bounds.y = y;
    win->client_area.x += dx;
    win->client_area.y += dy;
}

void window_resize(int window_id, int width, int height) {
    window_t* win = window_get_by_id(window_id);
    if (!win) return;
    
    win->bounds.width = width;
    win->bounds.height = height;
    win->client_area.width = width - 4;
    win->client_area.height = height - 22;
}

void window_set_title(int window_id, const char* title) {
    window_t* win = window_get_by_id(window_id);
    if (win) {
        strcpy(win->title, title);
    }
}

void window_set_active(int window_id) {
    // Deactivate current active window
    if (active_window_id != -1) {
        window_t* old_active = window_get_by_id(active_window_id);
        if (old_active) {
            old_active->is_active = 0;
        }
    }
    
    // Activate new window
    window_t* win = window_get_by_id(window_id);
    if (win) {
        win->is_active = 1;
        active_window_id = window_id;
        window_bring_to_front(window_id);
    }
}

void window_clear(int window_id, color_t color) {
    window_t* win = window_get_by_id(window_id);
    if (!win || win->state != WINDOW_VISIBLE) return;
    
    graphics_fill_rect(win->client_area, color);
}

void window_draw_pixel(int window_id, int x, int y, color_t color) {
    window_t* win = window_get_by_id(window_id);
    if (!win || win->state != WINDOW_VISIBLE) return;
    
    // Convert to screen coordinates
    int screen_x = win->client_area.x + x;
    int screen_y = win->client_area.y + y;
    
    // Check bounds
    if (screen_x >= win->client_area.x && screen_x < win->client_area.x + win->client_area.width &&
        screen_y >= win->client_area.y && screen_y < win->client_area.y + win->client_area.height) {
        graphics_put_pixel(screen_x, screen_y, color);
    }
}

void window_draw_rect(int window_id, rect_t rect, color_t color) {
    window_t* win = window_get_by_id(window_id);
    if (!win || win->state != WINDOW_VISIBLE) return;
    
    // Convert to screen coordinates
    rect_t screen_rect;
    screen_rect.x = win->client_area.x + rect.x;
    screen_rect.y = win->client_area.y + rect.y;
    screen_rect.width = rect.width;
    screen_rect.height = rect.height;
    
    graphics_draw_rect(screen_rect, color);
}

void window_fill_rect(int window_id, rect_t rect, color_t color) {
    window_t* win = window_get_by_id(window_id);
    if (!win || win->state != WINDOW_VISIBLE) return;
    
    // Convert to screen coordinates
    rect_t screen_rect;
    screen_rect.x = win->client_area.x + rect.x;
    screen_rect.y = win->client_area.y + rect.y;
    screen_rect.width = rect.width;
    screen_rect.height = rect.height;
    
    graphics_fill_rect(screen_rect, color);
}

void window_draw_text(int window_id, int x, int y, const char* text, color_t fg, color_t bg) {
    window_t* win = window_get_by_id(window_id);
    if (!win || win->state != WINDOW_VISIBLE) return;
    
    // Convert to screen coordinates
    int screen_x = win->client_area.x + x;
    int screen_y = win->client_area.y + y;
    
    graphics_draw_string(screen_x, screen_y, text, fg, bg);
}

void window_draw_decorations(window_t* window) {
    if (!window || window->state != WINDOW_VISIBLE) return;
    
    // Draw window border
    if (window->has_border) {
        color_t border_color = window->is_active ? COLOR_WHITE : COLOR_GRAY;
        graphics_draw_rect(window->bounds, border_color);
    }
    
    // Draw title bar
    if (window->has_titlebar) {
        rect_t titlebar = {
            window->bounds.x + 1,
            window->bounds.y + 1,
            window->bounds.width - 2,
            18
        };
        
        color_t titlebar_color = window->is_active ? COLOR_BLUE : COLOR_GRAY;
        graphics_fill_rect(titlebar, titlebar_color);
        
        // Draw title text
        graphics_draw_string(
            titlebar.x + 4,
            titlebar.y + 5,
            window->title,
            COLOR_WHITE,
            titlebar_color
        );
        
        // Draw close button (simple X)
        int close_x = titlebar.x + titlebar.width - 16;
        int close_y = titlebar.y + 2;
        rect_t close_button = {close_x, close_y, 14, 14};
        graphics_fill_rect(close_button, COLOR_RED);
        graphics_draw_string(close_x + 4, close_y + 3, "X", COLOR_WHITE, COLOR_RED);
    }
}

void window_manager_draw(void) {
    if (!wm_initialized) return;
    
    // Clear screen with desktop background
    graphics_clear_screen(COLOR_CYAN);
    
    // Draw desktop
    graphics_draw_string(10, 10, "MyOS Desktop", COLOR_BLACK, COLOR_CYAN);
    
    // Draw all windows in z-order
    for (int z = 0; z < window_count; z++) {
        for (int i = 0; i < MAX_WINDOWS; i++) {
            if (windows[i].id != -1 && windows[i].z_order == z && windows[i].state == WINDOW_VISIBLE) {
                // Clear window background
                graphics_fill_rect(windows[i].client_area, windows[i].bg_color);
                
                // Draw window decorations
                window_draw_decorations(&windows[i]);
            }
        }
    }
}

window_t* window_get_by_id(int window_id) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id == window_id) {
            return &windows[i];
        }
    }
    return 0;
}

int window_get_count(void) {
    return window_count;
}

void window_bring_to_front(int window_id) {
    window_t* win = window_get_by_id(window_id);
    if (!win) return;
    
    int old_z = win->z_order;
    
    // Move all windows with higher z-order down
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].id != -1 && windows[i].z_order > old_z) {
            windows[i].z_order--;
        }
    }
    
    // Move this window to top
    win->z_order = window_count - 1;
}