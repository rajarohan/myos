// graphics.c - Graphics driver implementation
#include "graphics.h"
#include "vga.h"

// Default framebuffer mode (we'll start with a simple mode)
static struct graphics_mode current_mode = {
    .width = 320,
    .height = 200,
    .bpp = 8,
    .pitch = 320,
    .framebuffer = (uint8_t*)0xA0000  // VGA mode 13h framebuffer
};

static int graphics_initialized = 0;

// Simple 8x8 bitmap font (very basic)
static const uint8_t font_8x8[128][8] = {
    // Character 'A' (65)
    [65] = {
        0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00
    },
    // Character 'B' (66)
    [66] = {
        0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00
    },
    // Add more characters as needed...
    // For now, let's add basic ASCII printable characters
    [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    [33] = {0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00}, // !
    [48] = {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // 0
    [49] = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, // 1
    [50] = {0x3C, 0x66, 0x06, 0x1C, 0x30, 0x60, 0x7E, 0x00}, // 2
    [51] = {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00}, // 3
    [52] = {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00}, // 4
    [53] = {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}, // 5
    [67] = {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00}, // C
    [68] = {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00}, // D
    [69] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00}, // E
    [70] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00}, // F
};

void graphics_init(void) {
    // For now, we'll use VGA mode 13h (320x200, 256 colors)
    // This is a simple mode that's well-supported
    graphics_initialized = 1;
    
    // Clear screen to black
    graphics_clear_screen(COLOR_BLACK);
}

int graphics_set_mode(uint32_t width, uint32_t height, uint32_t bpp) {
    // For this simple implementation, we only support 320x200x8
    if (width == 320 && height == 200 && bpp == 8) {
        current_mode.width = width;
        current_mode.height = height;
        current_mode.bpp = bpp;
        current_mode.pitch = width;
        return 1; // Success
    }
    return 0; // Failure
}

struct graphics_mode* graphics_get_mode(void) {
    return &current_mode;
}

void graphics_put_pixel(int x, int y, color_t color) {
    if (!graphics_initialized || x < 0 || y < 0 || 
        x >= (int)current_mode.width || y >= (int)current_mode.height) {
        return;
    }
    
    if (current_mode.bpp == 8) {
        // For 8-bit mode, we'll use a simple color mapping
        uint8_t color_index = (color.r >> 5) << 5 | (color.g >> 5) << 2 | (color.b >> 6);
        current_mode.framebuffer[y * current_mode.pitch + x] = color_index;
    } else if (current_mode.bpp == 32) {
        uint32_t* pixel = (uint32_t*)(current_mode.framebuffer + y * current_mode.pitch + x * 4);
        *pixel = color_to_rgb32(color);
    }
}

color_t graphics_get_pixel(int x, int y) {
    if (!graphics_initialized || x < 0 || y < 0 || 
        x >= (int)current_mode.width || y >= (int)current_mode.height) {
        return COLOR_BLACK;
    }
    
    if (current_mode.bpp == 8) {
        uint8_t color_index = current_mode.framebuffer[y * current_mode.pitch + x];
        // Convert back from color index (simplified)
        color_t color;
        color.r = (color_index >> 5) * 32;
        color.g = ((color_index >> 2) & 0x7) * 32;
        color.b = (color_index & 0x3) * 64;
        color.a = 255;
        return color;
    } else if (current_mode.bpp == 32) {
        uint32_t* pixel = (uint32_t*)(current_mode.framebuffer + y * current_mode.pitch + x * 4);
        return rgb32_to_color(*pixel);
    }
    
    return COLOR_BLACK;
}

void graphics_fill_rect(rect_t rect, color_t color) {
    for (int y = rect.y; y < rect.y + rect.height; y++) {
        for (int x = rect.x; x < rect.x + rect.width; x++) {
            graphics_put_pixel(x, y, color);
        }
    }
}

void graphics_draw_rect(rect_t rect, color_t color) {
    // Draw top and bottom lines
    for (int x = rect.x; x < rect.x + rect.width; x++) {
        graphics_put_pixel(x, rect.y, color);
        graphics_put_pixel(x, rect.y + rect.height - 1, color);
    }
    
    // Draw left and right lines
    for (int y = rect.y; y < rect.y + rect.height; y++) {
        graphics_put_pixel(rect.x, y, color);
        graphics_put_pixel(rect.x + rect.width - 1, y, color);
    }
}

void graphics_draw_line(int x1, int y1, int x2, int y2, color_t color) {
    // Bresenham's line algorithm
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dx_abs = (dx < 0) ? -dx : dx;
    int dy_abs = (dy < 0) ? -dy : dy;
    
    int x_inc = (dx < 0) ? -1 : 1;
    int y_inc = (dy < 0) ? -1 : 1;
    
    int x = x1;
    int y = y1;
    
    graphics_put_pixel(x, y, color);
    
    if (dx_abs > dy_abs) {
        int error = dx_abs / 2;
        while (x != x2) {
            error -= dy_abs;
            if (error < 0) {
                y += y_inc;
                error += dx_abs;
            }
            x += x_inc;
            graphics_put_pixel(x, y, color);
        }
    } else {
        int error = dy_abs / 2;
        while (y != y2) {
            error -= dx_abs;
            if (error < 0) {
                x += x_inc;
                error += dy_abs;
            }
            y += y_inc;
            graphics_put_pixel(x, y, color);
        }
    }
}

void graphics_clear_screen(color_t color) {
    rect_t screen = {0, 0, (int)current_mode.width, (int)current_mode.height};
    graphics_fill_rect(screen, color);
}

void graphics_draw_char(int x, int y, char c, color_t fg, color_t bg) {
    if (c < 0 || c >= 128) return;
    
    const uint8_t* char_data = font_8x8[(int)c];
    
    for (int row = 0; row < 8; row++) {
        uint8_t byte = char_data[row];
        for (int col = 0; col < 8; col++) {
            color_t pixel_color = (byte & (0x80 >> col)) ? fg : bg;
            graphics_put_pixel(x + col, y + row, pixel_color);
        }
    }
}

void graphics_draw_string(int x, int y, const char* str, color_t fg, color_t bg) {
    int current_x = x;
    while (*str) {
        if (*str == '\n') {
            current_x = x;
            y += 8;
        } else {
            graphics_draw_char(current_x, y, *str, fg, bg);
            current_x += 8;
        }
        str++;
    }
}

uint32_t color_to_rgb32(color_t color) {
    return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

color_t rgb32_to_color(uint32_t rgb) {
    color_t color;
    color.a = (rgb >> 24) & 0xFF;
    color.r = (rgb >> 16) & 0xFF;
    color.g = (rgb >> 8) & 0xFF;
    color.b = rgb & 0xFF;
    return color;
}