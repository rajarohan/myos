# MyOS GUI System Documentation

## Overview

MyOS now includes a basic GUI system with the following components:

### 🖥️ **Graphics Driver (`graphics.c/h`)**
- Basic framebuffer operations (pixel plotting, drawing primitives)
- Color management with RGB support
- Text rendering with bitmap fonts
- Line drawing using Bresenham's algorithm
- Rectangle filling and drawing operations

### 🪟 **Window Manager (`window.c/h`)**
- Multiple window support (up to 16 windows)
- Window decorations (title bars, borders, close buttons)
- Z-order management for window layering
- Window operations: create, destroy, move, resize, show/hide
- Focus management and active window tracking

### 🎮 **GUI Applications (`gui_app.c/h`)**
- Application framework for GUI programs
- Pre-built applications:
  - **File Manager**: Browse and manage files/directories
  - **Text Editor**: Simple text editing interface
  - **Terminal**: GUI terminal emulator

## Features

### **Graphics Capabilities**
- 320x200 resolution in VGA mode 13h
- 256-color palette support
- Basic drawing primitives (pixels, lines, rectangles)
- Bitmap font rendering for text display

### **Window System**
- Overlapping windows with proper layering
- Resizable and moveable windows
- Title bars with window titles
- Window decorations and borders
- Focus indication (active vs inactive windows)

### **Desktop Environment**
- Simple desktop background
- Multiple application windows
- Keyboard navigation and control
- Mode switching between GUI and text console

## Usage

### **Boot Options**
When MyOS starts, you'll see a boot menu:
```
MyOS v1.0 - Boot Menu
====================

Choose startup mode:
1. Text Mode (Shell)
2. GUI Mode (Desktop)

Press 1 or 2:
```

### **Text Mode (Option 1)**
- Traditional command-line interface
- Full shell functionality with file system operations
- All existing commands: `ls`, `mkdir`, `cd`, `create`, `read`, etc.

### **GUI Mode (Option 2)**
- Graphical desktop environment
- Multiple application windows
- File manager, text editor, and terminal applications

### **GUI Controls**
- **ESC**: Return to text mode from GUI
- **1**: Refresh File Manager window
- **2**: Refresh Text Editor window  
- **3**: Refresh Terminal window

## Technical Architecture

### **Graphics System**
```c
// Initialize graphics
graphics_init();

// Basic drawing operations  
graphics_put_pixel(x, y, color);
graphics_draw_line(x1, y1, x2, y2, color);
graphics_fill_rect(rect, color);
graphics_draw_string(x, y, text, fg_color, bg_color);
```

### **Window Management**
```c
// Initialize window manager
window_manager_init();

// Create a window
int window_id = window_create("My App", x, y, width, height, bg_color);

// Draw in window
window_draw_text(window_id, x, y, "Hello World!", fg_color, bg_color);
window_fill_rect(window_id, rect, color);

// Manage windows
window_set_active(window_id);
window_bring_to_front(window_id);
```

### **Application Development**
```c
// Create GUI application
gui_app_t* app = gui_app_create("My App", x, y, width, height);

// Draw application content
gui_app_draw(app);

// Handle events
gui_app_handle_key(app, key);

// Cleanup
gui_app_destroy(app);
```

## Current Limitations

### **Hardware Support**
- Only VGA Mode 13h (320x200, 8-bit color)
- No hardware acceleration
- Limited to basic VGA framebuffer access

### **Input Methods**
- Keyboard input only (no mouse support yet)
- Basic keyboard event handling
- Limited key mapping

### **Graphics Features**
- Simple bitmap fonts only
- No advanced drawing (circles, curves, etc.)
- No image file support
- Limited color palette

## Future Enhancements

### **Planned Features**
1. **Mouse Support**: PS/2 mouse driver and cursor
2. **Higher Resolutions**: VESA VBE support for modern resolutions
3. **Advanced Graphics**: Improved font rendering, image support
4. **More Applications**: Calculator, games, system settings
5. **Improved UI**: Menus, dialogs, buttons, text boxes

### **Technical Improvements**
1. **Memory Management**: Dynamic window content allocation
2. **Event System**: Proper event queue and handling
3. **Graphics Acceleration**: Hardware-accelerated drawing
4. **File System Integration**: Complete GUI file manager
5. **Multi-tasking**: Concurrent GUI applications

## Building with GUI Support

The GUI system is automatically included when building MyOS:

```bash
# Clean and build
make clean
make

# Run with GUI support
make run
```

The GUI components are compiled as part of the standard build process and add minimal overhead to the kernel size.

## Development Notes

### **Adding New Applications**
To create a new GUI application:
1. Create application-specific drawing functions
2. Use the `gui_app_create()` framework
3. Implement custom drawing in `gui_app_draw()`
4. Add event handling in `gui_app_handle_key()`

### **Extending Graphics**
To add new graphics features:
1. Add primitives to `graphics.c`
2. Update `graphics.h` with new function declarations
3. Test thoroughly as graphics errors can crash the system

### **Window System Extensions**
To enhance the window manager:
1. Modify `window_t` structure for new properties
2. Update drawing functions in `window.c`
3. Ensure proper Z-order and focus management

## Troubleshooting

### **Common Issues**
- **Black screen in GUI mode**: Graphics initialization failed
- **Corrupted display**: Memory access issues or wrong framebuffer address
- **No response to keys**: Keyboard driver not properly initialized

### **Debug Tips**
- Use text mode for debugging GUI issues
- Check VGA register settings for mode 13h
- Verify framebuffer address (0xA0000 for VGA mode 13h)

---

This GUI system provides a solid foundation for graphical applications in MyOS while maintaining the existing text-based functionality!