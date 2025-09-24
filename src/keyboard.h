// keyboard.h - Keyboard driver header
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Key codes for common keys
#define KEY_ESC     0x01
#define KEY_ENTER   0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_SPACE   0x39
#define KEY_LSHIFT  0x2A
#define KEY_RSHIFT  0x36

// Function prototypes
void keyboard_init(void);
char keyboard_getchar(void);
int keyboard_available(void);

#endif