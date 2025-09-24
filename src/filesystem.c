// filesystem.c - Simple in-memory file system implementation
#include "filesystem.h"
#include "vga.h"

// Global file system instance
static struct filesystem fs;
static uint8_t filesystem_data[FILESYSTEM_MEMORY_SIZE];

// Simple string functions
static int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

static void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static void memcpy(void* dest, const void* src, uint32_t size) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (uint32_t i = 0; i < size; i++) {
        *d++ = *s++;
    }
}

// Simple integer to string conversion
static void itoa(int value, char* buffer, int base) {
    char* p = buffer;
    char* p1, *p2;
    int digits = 0;
    
    if (value < 0 && base == 10) {
        *p++ = '-';
        value = -value;
    }
    
    do {
        int remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        digits++;
    } while (value /= base);
    
    *p = 0;
    
    // Reverse string (excluding sign)
    p1 = (buffer[0] == '-') ? buffer + 1 : buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

static void memset(void* ptr, int value, uint32_t size) {
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = (uint8_t)value;
    }
}

// Forward declarations for helper functions
static void add_child_to_directory(int parent_index, int child_index);
static void remove_child_from_directory(int parent_index, int child_index);

int fs_init(void) {
    // Initialize file system structure
    memset(&fs, 0, sizeof(struct filesystem));
    fs.data_area = filesystem_data;
    fs.next_data_offset = 0;
    
    // Clear all file entries
    for (int i = 0; i < MAX_FILES; i++) {
        fs.files[i].used = 0;
        fs.files[i].size = 0;
        fs.files[i].data_offset = 0;
        fs.files[i].is_directory = 0;
        fs.files[i].parent_index = -1;
        fs.files[i].first_child_index = -1;
        fs.files[i].next_sibling_index = -1;
        memset(fs.files[i].name, 0, MAX_FILENAME_LENGTH);
    }
    
    // Create root directory
    fs.files[0].used = 1;
    fs.files[0].is_directory = 1;
    fs.files[0].parent_index = -1;
    fs.files[0].first_child_index = -1;
    fs.files[0].next_sibling_index = -1;
    strcpy(fs.files[0].name, "/");
    fs.files[0].size = 0;
    fs.files[0].data_offset = 0;
    
    fs.root_directory = 0;
    fs.current_directory = 0;
    
    vga_puts("File system with directory support initialized successfully.\n");
    return 0;
}

static int find_free_file_entry(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.files[i].used) {
            return i;
        }
    }
    return -1; // No free entries
}

static int find_file_entry(const char* filename) {
    // Look in current directory only
    int child = fs.files[fs.current_directory].first_child_index;
    while (child != -1) {
        if (strcmp(fs.files[child].name, filename) == 0 && !fs.files[child].is_directory) {
            return child;
        }
        child = fs.files[child].next_sibling_index;
    }
    return -1; // File not found
}

int fs_create_file(const char* filename) {
    // Check if file already exists in current directory
    if (find_file_entry(filename) >= 0) {
        vga_printf("Error: File '%s' already exists.\n", filename);
        return -1;
    }
    
    // Check if directory with same name exists
    int child = fs.files[fs.current_directory].first_child_index;
    while (child != -1) {
        if (strcmp(fs.files[child].name, filename) == 0 && fs.files[child].is_directory) {
            vga_printf("Error: Directory '%s' already exists with that name.\n", filename);
            return -1;
        }
        child = fs.files[child].next_sibling_index;
    }
    
    // Check filename length
    if (strlen(filename) >= MAX_FILENAME_LENGTH) {
        vga_puts("Error: Filename too long.\n");
        return -1;
    }
    
    // Find free file entry
    int index = find_free_file_entry();
    if (index < 0) {
        vga_puts("Error: No free file entries available.\n");
        return -1;
    }
    
    // Create the file entry
    strcpy(fs.files[index].name, filename);
    fs.files[index].used = 1;
    fs.files[index].size = 0;
    fs.files[index].data_offset = fs.next_data_offset;
    fs.files[index].is_directory = 0;
    fs.files[index].first_child_index = -1;
    fs.files[index].next_sibling_index = -1;
    
    // Add to current directory
    add_child_to_directory(fs.current_directory, index);
    
    vga_printf("File '%s' created successfully.\n", filename);
    return 0;
}

int fs_write_file(const char* filename, const char* data, uint32_t size) {
    int index = find_file_entry(filename);
    if (index < 0) {
        vga_printf("Error: File '%s' not found.\n", filename);
        return -1;
    }
    
    if (size > MAX_FILE_SIZE) {
        vga_printf("Error: File size too large (max %d bytes).\n", MAX_FILE_SIZE);
        return -1;
    }
    
    // Check if we have enough space in data area
    if (fs.next_data_offset + size > FILESYSTEM_MEMORY_SIZE) {
        vga_puts("Error: Not enough space in file system.\n");
        return -1;
    }
    
    // If file already has data, we'll overwrite it (simple implementation)
    if (fs.files[index].size == 0) {
        fs.files[index].data_offset = fs.next_data_offset;
        fs.next_data_offset += size;
    }
    
    // Write data
    memcpy(fs.data_area + fs.files[index].data_offset, data, size);
    fs.files[index].size = size;
    
    vga_printf("Data written to file '%s' (%d bytes).\n", filename, size);
    return 0;
}

int fs_read_file(const char* filename, char* buffer, uint32_t buffer_size) {
    int index = find_file_entry(filename);
    if (index < 0) {
        vga_printf("Error: File '%s' not found.\n", filename);
        return -1;
    }
    
    if (fs.files[index].size == 0) {
        vga_printf("File '%s' is empty.\n", filename);
        return 0;
    }
    
    uint32_t copy_size = fs.files[index].size;
    if (copy_size > buffer_size - 1) {
        copy_size = buffer_size - 1;
    }
    
    memcpy(buffer, fs.data_area + fs.files[index].data_offset, copy_size);
    buffer[copy_size] = '\0'; // Null-terminate for text files
    
    return copy_size;
}

int fs_delete_file(const char* filename) {
    int index = find_file_entry(filename);
    if (index < 0) {
        vga_printf("Error: File '%s' not found.\n", filename);
        return -1;
    }
    
    // Remove from parent directory
    remove_child_from_directory(fs.current_directory, index);
    
    // Mark file entry as unused
    fs.files[index].used = 0;
    fs.files[index].size = 0;
    memset(fs.files[index].name, 0, MAX_FILENAME_LENGTH);
    
    vga_printf("File '%s' deleted successfully.\n", filename);
    return 0;
}

int fs_list_files(void) {
    char current_path[MAX_PATH_LENGTH];
    char count_str[16];
    fs_get_current_path(current_path, MAX_PATH_LENGTH);
    
    vga_puts("Contents of ");
    vga_puts(current_path);
    vga_puts(":\n");
    vga_puts("Type Name                    Size (bytes)\n");
    vga_puts("----------------------------------------\n");
    
    int count = 0;
    int child = fs.files[fs.current_directory].first_child_index;
    
    while (child != -1) {
        if (fs.files[child].is_directory) {
            vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            vga_puts("DIR  ");
            vga_puts(fs.files[child].name);
            
            // Pad to 20 characters
            int name_len = strlen(fs.files[child].name);
            for (int i = name_len; i < 20; i++) {
                vga_putchar(' ');
            }
            vga_puts(" <DIR>\n");
            vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        } else {
            vga_puts("FILE ");
            vga_puts(fs.files[child].name);
            
            // Pad to 20 characters
            int name_len = strlen(fs.files[child].name);
            for (int i = name_len; i < 20; i++) {
                vga_putchar(' ');
            }
            vga_putchar(' ');
            
            // Convert size to string
            itoa(fs.files[child].size, count_str, 10);
            vga_puts(count_str);
            vga_putchar('\n');
        }
        count++;
        child = fs.files[child].next_sibling_index;
    }
    
    if (count == 0) {
        vga_puts("Directory is empty.\n");
    } else {
        vga_puts("\nTotal: ");
        itoa(count, count_str, 10);
        vga_puts(count_str);
        vga_puts(" items\n");
    }
    
    return count;
}

int fs_file_exists(const char* filename) {
    return find_file_entry(filename) >= 0;
}

uint32_t fs_get_file_size(const char* filename) {
    int index = find_file_entry(filename);
    if (index >= 0) {
        return fs.files[index].size;
    }
    return 0;
}

// Helper function to add child to parent directory
static void add_child_to_directory(int parent_index, int child_index) {
    if (fs.files[parent_index].first_child_index == -1) {
        // First child
        fs.files[parent_index].first_child_index = child_index;
    } else {
        // Find last sibling and add new child
        int sibling = fs.files[parent_index].first_child_index;
        while (fs.files[sibling].next_sibling_index != -1) {
            sibling = fs.files[sibling].next_sibling_index;
        }
        fs.files[sibling].next_sibling_index = child_index;
    }
    fs.files[child_index].parent_index = parent_index;
}

// Helper function to remove child from parent directory
static void remove_child_from_directory(int parent_index, int child_index) {
    if (fs.files[parent_index].first_child_index == child_index) {
        // Removing first child
        fs.files[parent_index].first_child_index = fs.files[child_index].next_sibling_index;
    } else {
        // Find previous sibling
        int sibling = fs.files[parent_index].first_child_index;
        while (sibling != -1 && fs.files[sibling].next_sibling_index != child_index) {
            sibling = fs.files[sibling].next_sibling_index;
        }
        if (sibling != -1) {
            fs.files[sibling].next_sibling_index = fs.files[child_index].next_sibling_index;
        }
    }
}

int fs_create_directory(const char* dirname) {
    // Check if directory already exists
    int child = fs.files[fs.current_directory].first_child_index;
    while (child != -1) {
        if (strcmp(fs.files[child].name, dirname) == 0) {
            vga_printf("Error: Directory '%s' already exists.\n", dirname);
            return -1;
        }
        child = fs.files[child].next_sibling_index;
    }
    
    // Check dirname length
    if (strlen(dirname) >= MAX_FILENAME_LENGTH) {
        vga_puts("Error: Directory name too long.\n");
        return -1;
    }
    
    // Find free file entry
    int index = find_free_file_entry();
    if (index < 0) {
        vga_puts("Error: No free file entries available.\n");
        return -1;
    }
    
    // Create the directory entry
    strcpy(fs.files[index].name, dirname);
    fs.files[index].used = 1;
    fs.files[index].is_directory = 1;
    fs.files[index].size = 0;
    fs.files[index].data_offset = 0;
    fs.files[index].first_child_index = -1;
    fs.files[index].next_sibling_index = -1;
    
    // Add to current directory
    add_child_to_directory(fs.current_directory, index);
    
    vga_printf("Directory '%s' created successfully.\n", dirname);
    return 0;
}

int fs_resolve_path(const char* path) {
    if (path[0] == '/') {
        // Absolute path - start from root
        return fs.root_directory;
    } else if (strcmp(path, ".") == 0) {
        // Current directory
        return fs.current_directory;
    } else if (strcmp(path, "..") == 0) {
        // Parent directory
        int parent = fs.files[fs.current_directory].parent_index;
        return (parent == -1) ? fs.root_directory : parent;
    } else {
        // Relative path - look in current directory
        int child = fs.files[fs.current_directory].first_child_index;
        while (child != -1) {
            if (strcmp(fs.files[child].name, path) == 0 && fs.files[child].is_directory) {
                return child;
            }
            child = fs.files[child].next_sibling_index;
        }
    }
    return -1; // Not found
}

int fs_change_directory(const char* path) {
    int target_dir = fs_resolve_path(path);
    if (target_dir == -1) {
        vga_printf("Error: Directory '%s' not found.\n", path);
        return -1;
    }
    
    fs.current_directory = target_dir;
    return 0;
}

int fs_remove_directory(const char* dirname) {
    // Validate directory name
    if (!dirname || strlen(dirname) == 0) {
        vga_puts("Error: Invalid directory name.\n");
        return -1;
    }
    
    // Find directory in current directory
    int child = fs.files[fs.current_directory].first_child_index;
    while (child != -1) {
        if (strcmp(fs.files[child].name, dirname) == 0) {
            // Check if it's actually a directory
            if (!fs.files[child].is_directory) {
                vga_printf("Error: '%s' is not a directory.\n", dirname);
                return -1;
            }
            
            // Check if directory is empty
            if (fs.files[child].first_child_index != -1) {
                vga_printf("Error: Directory '%s' is not empty.\n", dirname);
                return -1;
            }
            
            // Remove from parent
            remove_child_from_directory(fs.current_directory, child);
            
            // Mark as unused
            fs.files[child].used = 0;
            fs.files[child].is_directory = 0;
            fs.files[child].first_child_index = -1;
            fs.files[child].next_sibling_index = -1;
            memset(fs.files[child].name, 0, MAX_FILENAME_LENGTH);
            
            vga_printf("Directory '%s' removed successfully.\n", dirname);
            return 0;
        }
        child = fs.files[child].next_sibling_index;
    }
    
    vga_printf("Error: Directory '%s' not found.\n", dirname);
    return -1;
}

void fs_get_current_path(char* buffer, int buffer_size) {
    if (fs.current_directory == fs.root_directory) {
        if (buffer_size > 1) {
            buffer[0] = '/';
            buffer[1] = '\0';
        }
        return;
    }
    
    // Build path recursively
    int path_components[MAX_FILES];
    int component_count = 0;
    
    int current = fs.current_directory;
    while (current != fs.root_directory && current != -1) {
        path_components[component_count++] = current;
        current = fs.files[current].parent_index;
    }
    
    // Build path string
    int pos = 0;
    buffer[pos++] = '/';
    
    for (int i = component_count - 1; i >= 0 && pos < buffer_size - 1; i--) {
        int name_len = strlen(fs.files[path_components[i]].name);
        if (pos + name_len + 1 < buffer_size) {
            for (int j = 0; j < name_len; j++) {
                buffer[pos++] = fs.files[path_components[i]].name[j];
            }
            if (i > 0) {
                buffer[pos++] = '/';
            }
        }
    }
    
    buffer[pos] = '\0';
}

void fs_print_info(void) {
    int used_entries = 0;
    int directories = 0;
    int files = 0;
    uint32_t total_size = 0;
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used) {
            used_entries++;
            if (fs.files[i].is_directory) {
                directories++;
            } else {
                files++;
                total_size += fs.files[i].size;
            }
        }
    }
    
    char current_path[MAX_PATH_LENGTH];
    fs_get_current_path(current_path, MAX_PATH_LENGTH);
    
    vga_puts("\nFile System Information:\n");
    vga_printf("Current Directory: %s\n", current_path);
    vga_printf("Total entries: %d/%d\n", used_entries, MAX_FILES);
    vga_printf("Directories: %d, Files: %d\n", directories, files);
    vga_printf("Data used: %d/%d bytes\n", total_size, FILESYSTEM_MEMORY_SIZE);
    vga_printf("Free space: %d bytes\n", FILESYSTEM_MEMORY_SIZE - total_size);
}