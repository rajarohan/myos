// graphics.h - Graphics driver interface
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

// Graphics mode information
struct graphics_mode {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;          // Bits per pixel
    uint32_t pitch;        // Bytes per line
    uint8_t* framebuffer;  // Framebuffer address
};

// Color structure (RGB)
typedef struct {
    uint8_t r, g, b, a;
} color_t;

// Basic colors
#define COLOR_BLACK     ((color_t){0, 0, 0, 255})
#define COLOR_WHITE     ((color_t){255, 255, 255, 255})
#define COLOR_RED       ((color_t){255, 0, 0, 255})
#define COLOR_GREEN     ((color_t){0, 255, 0, 255})
#define COLOR_BLUE      ((color_t){0, 0, 255, 255})
#define COLOR_YELLOW    ((color_t){255, 255, 0, 255})
#define COLOR_CYAN      ((color_t){0, 255, 255, 255})
#define COLOR_MAGENTA   ((color_t){255, 0, 255, 255})
#define COLOR_GRAY      ((color_t){128, 128, 128, 255})
#define COLOR_LIGHT_GRAY ((color_t){192, 192, 192, 255})

// Rectangle structure
typedef struct {
    int x, y;
    int width, height;
} rect_t;

// Graphics functions
void graphics_init(void);
int graphics_set_mode(uint32_t width, uint32_t height, uint32_t bpp);
struct graphics_mode* graphics_get_mode(void);

// Basic drawing functions
void graphics_put_pixel(int x, int y, color_t color);
color_t graphics_get_pixel(int x, int y);
void graphics_fill_rect(rect_t rect, color_t color);
void graphics_draw_rect(rect_t rect, color_t color);
void graphics_draw_line(int x1, int y1, int x2, int y2, color_t color);
void graphics_clear_screen(color_t color);

// Text rendering in graphics mode
void graphics_draw_char(int x, int y, char c, color_t fg, color_t bg);
void graphics_draw_string(int x, int y, const char* str, color_t fg, color_t bg);

// Utility functions
uint32_t color_to_rgb32(color_t color);
color_t rgb32_to_color(uint32_t rgb);

#endif // GRAPHICS_H