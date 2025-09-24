// gui_app.h - Simple GUI application framework
#ifndef GUI_APP_H
#define GUI_APP_H

#include "window.h"
#include "graphics.h"

// GUI Application structure
typedef struct gui_app {
    int window_id;
    char title[64];
    int running;
} gui_app_t;

// GUI application functions
gui_app_t* gui_app_create(const char* title, int x, int y, int width, int height);
void gui_app_destroy(gui_app_t* app);
void gui_app_run(gui_app_t* app);
void gui_app_draw(gui_app_t* app);
void gui_app_handle_key(gui_app_t* app, char key);

// Specific GUI applications
gui_app_t* file_manager_create(void);
gui_app_t* text_editor_create(void);
gui_app_t* terminal_create(void);

#endif // GUI_APP_H