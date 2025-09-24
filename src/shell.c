// shell.c - Simple command shell implementation
#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "filesystem.h"

static char shell_buffer[SHELL_BUFFER_SIZE];
static char file_buffer[MAX_FILE_SIZE];

// Simple string functions

static int strncmp(const char* str1, const char* str2, int n) {
    while (n > 0 && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}



static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Skip whitespace
static const char* skip_whitespace(const char* str) {
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    return str;
}

// Find next argument in command line
static const char* find_next_arg(const char* str) {
    // Skip current argument
    while (*str && *str != ' ' && *str != '\t') {
        str++;
    }
    // Skip whitespace
    return skip_whitespace(str);
}

void shell_init(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("MyOS File System Shell v1.0\n");
    vga_puts("============================\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Type 'help' for available commands.\n\n");
}

void shell_prompt(void) {
    char current_path[MAX_PATH_LENGTH];
    fs_get_current_path(current_path, MAX_PATH_LENGTH);
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_printf("myos:%s$ ", current_path);
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

int shell_read_line(char* buffer, int max_length) {
    int pos = 0;
    char c;
    
    while (pos < max_length - 1) {
        c = keyboard_getchar();
        
        if (c == 0) {
            continue; // Ignore null characters (key releases, etc.)
        }
        
        if (c == '\n') {
            buffer[pos] = '\0';
            vga_putchar('\n');
            return pos;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                vga_putchar('\b');
            }
        } else if (c >= 32 && c <= 126) { // Printable characters
            buffer[pos] = c;
            pos++;
            vga_putchar(c);
        }
    }
    
    buffer[max_length - 1] = '\0';
    return pos;
}

void shell_execute_command(const char* command) {
    command = skip_whitespace(command);
    
    if (strlen(command) == 0) {
        return;
    }
    
    if (strncmp(command, "help", 4) == 0) {
        cmd_help();
    } else if (strncmp(command, "create", 6) == 0) {
        const char* filename = skip_whitespace(find_next_arg(command));
        if (strlen(filename) > 0) {
            cmd_create(filename);
        } else {
            vga_puts("Usage: create <filename>\n");
        }
    } else if (strncmp(command, "list", 4) == 0 || strncmp(command, "ls", 2) == 0) {
        cmd_list();
    } else if (strncmp(command, "read", 4) == 0 || strncmp(command, "cat", 3) == 0) {
        const char* filename = skip_whitespace(find_next_arg(command));
        if (strlen(filename) > 0) {
            cmd_read(filename);
        } else {
            vga_puts("Usage: read <filename>\n");
        }
    } else if (strncmp(command, "write", 5) == 0 || strncmp(command, "edit", 4) == 0) {
        const char* filename = skip_whitespace(find_next_arg(command));
        if (strlen(filename) > 0) {
            cmd_write(filename);
        } else {
            vga_puts("Usage: write <filename>\n");
        }
    } else if (strncmp(command, "delete", 6) == 0 || strncmp(command, "rm", 2) == 0) {
        const char* filename = skip_whitespace(find_next_arg(command));
        if (strlen(filename) > 0) {
            cmd_delete(filename);
        } else {
            vga_puts("Usage: delete <filename>\n");
        }
    } else if (strncmp(command, "clear", 5) == 0) {
        cmd_clear();
    } else if (strncmp(command, "info", 4) == 0) {
        cmd_info();
    } else if (strncmp(command, "mkdir", 5) == 0) {
        const char* dirname = skip_whitespace(find_next_arg(command));
        if (strlen(dirname) > 0) {
            cmd_mkdir(dirname);
        } else {
            vga_puts("Usage: mkdir <dirname>\n");
        }
    } else if (strncmp(command, "rmdir", 5) == 0) {
        const char* dirname = skip_whitespace(find_next_arg(command));
        if (strlen(dirname) > 0) {
            cmd_rmdir(dirname);
        } else {
            vga_puts("Usage: rmdir <dirname>\n");
        }
    } else if (strncmp(command, "cd", 2) == 0) {
        const char* path = skip_whitespace(find_next_arg(command));
        if (strlen(path) > 0) {
            cmd_cd(path);
        } else {
            // cd with no arguments goes to root
            cmd_cd("/");
        }
    } else if (strncmp(command, "pwd", 3) == 0) {
        cmd_pwd();
    } else {
        vga_printf("Unknown command: %s\n", command);
        vga_puts("Type 'help' for available commands.\n");
    }
}

void cmd_help(void) {
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_puts("Available commands:\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("File Operations:\n");
    vga_puts("  create <file>     - Create a new text file\n");
    vga_puts("  read <file>       - Display file contents\n");
    vga_puts("  cat <file>        - Alias for read\n");
    vga_puts("  write <file>      - Write text to file\n");
    vga_puts("  edit <file>       - Alias for write\n");
    vga_puts("  delete <file>     - Delete a file\n");
    vga_puts("  rm <file>         - Alias for delete\n");
    vga_puts("\nDirectory Operations:\n");
    vga_puts("  mkdir <dir>       - Create a new directory\n");
    vga_puts("  rmdir <dir>       - Remove an empty directory\n");
    vga_puts("  cd <path>         - Change to directory (/, .., dir)\n");
    vga_puts("  pwd               - Show current directory\n");
    vga_puts("  list, ls          - List directory contents\n");
    vga_puts("\nSystem Operations:\n");
    vga_puts("  clear             - Clear screen\n");
    vga_puts("  info              - Show file system info\n");
    vga_puts("  help              - Show this help message\n");
}

void cmd_create(const char* filename) {
    // Extract just the filename (stop at first space)
    char clean_filename[MAX_FILENAME_LENGTH];
    int i = 0;
    while (filename[i] && filename[i] != ' ' && filename[i] != '\t' && i < MAX_FILENAME_LENGTH - 1) {
        clean_filename[i] = filename[i];
        i++;
    }
    clean_filename[i] = '\0';
    
    fs_create_file(clean_filename);
}

void cmd_list(void) {
    fs_list_files();
}

void cmd_read(const char* filename) {
    // Extract just the filename
    char clean_filename[MAX_FILENAME_LENGTH];
    int i = 0;
    while (filename[i] && filename[i] != ' ' && filename[i] != '\t' && i < MAX_FILENAME_LENGTH - 1) {
        clean_filename[i] = filename[i];
        i++;
    }
    clean_filename[i] = '\0';
    
    int size = fs_read_file(clean_filename, file_buffer, MAX_FILE_SIZE);
    if (size > 0) {
        vga_printf("Contents of '%s':\n", clean_filename);
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_puts("--- BEGIN FILE ---\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_puts(file_buffer);
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_puts("\n--- END FILE ---\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

void cmd_write(const char* filename) {
    // Extract just the filename
    char clean_filename[MAX_FILENAME_LENGTH];
    int i = 0;
    while (filename[i] && filename[i] != ' ' && filename[i] != '\t' && i < MAX_FILENAME_LENGTH - 1) {
        clean_filename[i] = filename[i];
        i++;
    }
    clean_filename[i] = '\0';
    
    // Check if file exists
    if (!fs_file_exists(clean_filename)) {
        vga_printf("File '%s' does not exist. Creating it first...\n", clean_filename);
        if (fs_create_file(clean_filename) != 0) {
            return; // Failed to create file
        }
    }
    
    vga_printf("Enter text for file '%s' (press Ctrl+D or empty line to finish):\n", clean_filename);
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    
    int pos = 0;
    char c;
    
    while (pos < MAX_FILE_SIZE - 1) {
        c = keyboard_getchar();
        
        if (c == 0) {
            continue;
        }
        
        // Check for Ctrl+D (ASCII 4) or double newline to end input
        if (c == 4) {
            break;
        }
        
        if (c == '\n') {
            file_buffer[pos++] = c;
            vga_putchar(c);
            // Check if this is an empty line (previous char was also newline)
            if (pos > 1 && file_buffer[pos-2] == '\n') {
                pos--; // Remove the last newline
                break;
            }
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                vga_putchar('\b');
            }
        } else if (c >= 32 && c <= 126) {
            file_buffer[pos++] = c;
            vga_putchar(c);
        }
    }
    
    file_buffer[pos] = '\0';
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    if (pos > 0) {
        fs_write_file(clean_filename, file_buffer, pos);
    } else {
        vga_puts("No text entered.\n");
    }
}

void cmd_delete(const char* filename) {
    // Extract just the filename
    char clean_filename[MAX_FILENAME_LENGTH];
    int i = 0;
    while (filename[i] && filename[i] != ' ' && filename[i] != '\t' && i < MAX_FILENAME_LENGTH - 1) {
        clean_filename[i] = filename[i];
        i++;
    }
    clean_filename[i] = '\0';
    
    fs_delete_file(clean_filename);
}

void cmd_clear(void) {
    vga_clear();
}

void cmd_info(void) {
    fs_print_info();
}

void cmd_mkdir(const char* dirname) {
    // Extract just the directory name
    char clean_dirname[MAX_FILENAME_LENGTH];
    int i = 0;
    while (dirname[i] && dirname[i] != ' ' && dirname[i] != '\t' && i < MAX_FILENAME_LENGTH - 1) {
        clean_dirname[i] = dirname[i];
        i++;
    }
    clean_dirname[i] = '\0';
    
    fs_create_directory(clean_dirname);
}

void cmd_rmdir(const char* dirname) {
    // Extract just the directory name
    char clean_dirname[MAX_FILENAME_LENGTH];
    int i = 0;
    while (dirname[i] && dirname[i] != ' ' && dirname[i] != '\t' && i < MAX_FILENAME_LENGTH - 1) {
        clean_dirname[i] = dirname[i];
        i++;
    }
    clean_dirname[i] = '\0';
    
    fs_remove_directory(clean_dirname);
}

void cmd_cd(const char* path) {
    // Extract just the path
    char clean_path[MAX_PATH_LENGTH];
    int i = 0;
    while (path[i] && path[i] != ' ' && path[i] != '\t' && i < MAX_PATH_LENGTH - 1) {
        clean_path[i] = path[i];
        i++;
    }
    clean_path[i] = '\0';
    
    if (fs_change_directory(clean_path) == 0) {
        char current_path[MAX_PATH_LENGTH];
        fs_get_current_path(current_path, MAX_PATH_LENGTH);
        vga_printf("Changed to directory: %s\n", current_path);
    }
}

void cmd_pwd(void) {
    char current_path[MAX_PATH_LENGTH];
    fs_get_current_path(current_path, MAX_PATH_LENGTH);
    vga_printf("%s\n", current_path);
}

void shell_run(void) {
    while (1) {
        shell_prompt();
        shell_read_line(shell_buffer, SHELL_BUFFER_SIZE);
        shell_execute_command(shell_buffer);
        vga_putchar('\n');
    }
}