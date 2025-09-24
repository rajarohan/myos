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
        d[i] = s[i];
    }
}

static void memset(void* ptr, int value, uint32_t size) {
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = (uint8_t)value;
    }
}

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
        memset(fs.files[i].name, 0, MAX_FILENAME_LENGTH);
    }
    
    vga_puts("File system initialized successfully.\n");
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
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            return i;
        }
    }
    return -1; // File not found
}

int fs_create_file(const char* filename) {
    // Check if file already exists
    if (find_file_entry(filename) >= 0) {
        vga_printf("Error: File '%s' already exists.\n", filename);
        return -1;
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
    
    // Mark file entry as unused (simple deletion)
    fs.files[index].used = 0;
    fs.files[index].size = 0;
    memset(fs.files[index].name, 0, MAX_FILENAME_LENGTH);
    
    vga_printf("File '%s' deleted successfully.\n", filename);
    return 0;
}

int fs_list_files(void) {
    vga_puts("Files in file system:\n");
    vga_puts("Name                    Size (bytes)\n");
    vga_puts("------------------------------------\n");
    
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used) {
            vga_printf("%-20s %d\n", fs.files[i].name, fs.files[i].size);
            count++;
        }
    }
    
    if (count == 0) {
        vga_puts("No files found.\n");
    } else {
        vga_printf("\nTotal: %d files\n", count);
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

void fs_print_info(void) {
    int used_entries = 0;
    uint32_t total_size = 0;
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used) {
            used_entries++;
            total_size += fs.files[i].size;
        }
    }
    
    vga_puts("\nFile System Information:\n");
    vga_printf("Files: %d/%d\n", used_entries, MAX_FILES);
    vga_printf("Data used: %d/%d bytes\n", total_size, FILESYSTEM_MEMORY_SIZE);
    vga_printf("Free space: %d bytes\n", FILESYSTEM_MEMORY_SIZE - total_size);
}